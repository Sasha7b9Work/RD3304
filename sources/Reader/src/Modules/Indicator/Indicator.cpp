// 2023/11/18 16:21:26 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/Indicator/Indicator.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include "Nodes/OSDP/OSDP.h"
#include "Device/Device.h"
#include "Reader/Reader.h"
#include "Hardware/Power.h"
#include "Nodes/Upgrader.h"
#include "Modules/LIS2DH12/LIS2DH12.h"
#ifdef TYPE_BOARD_771
    #include "Modules/Indicator/LP5012/LP5012.h"
#else
    #include "Modules/Indicator/WS2812B/WS2812B.h"
#endif
#include <cstring>
#include <cmath>


ModeIndicator::E ModeIndicator::current = ModeIndicator::External;


namespace Indicator
{
    static bool lg_and_lr = false;      // Это если lr и lg поданы
    static bool prev_lg = false;        // Если true - нужно зажигать
    static bool prev_lr = false;        // Если true - нужно зажигать


    static void FireInputsInEnergySavingMode(bool lg, bool lr);

    // Цвет, посланнный в LED в последнюю засылку
    static ColorLED last_color;

    Color FiredColor()
    {
        return last_color.ToColor();
    }

    static ColorLED TransformColor(const ColorLED &);

    static void FireLED(const ColorLED &color);

    static ColorLED operator*(const ColorLED &color, uint mul)
    {
        ColorLED result = color;

        for (int i = 0; i < 3; i++)
        {
            result.chan_rel[i] *= (float)mul;
        }

        return result;
    }

    static ColorLED operator+(const ColorLED &color1, const ColorLED &color2)
    {
        ColorLED result = color1;

        for (int i = 0; i < 3; i++)
        {
            result.chan_rel[i] += color2.chan_rel[i];
        }

        return result;
    }

    static bool operator!=(const ColorLED &color1, const ColorLED &color2)
    {
        return std::memcmp(&color1, &color2, sizeof(ColorLED)) != 0; //-V1014
    }


    struct Task
    { //-V802
        void Init(const Color &color, uint time)
        {
            first_step = true;
            color_end = ColorLED::FromColor(color);
            time_run = time;
        }

        void RunStep()
        {
            if (first_step)
            {
                is_finished = false;

                prev_time = TIME_MS;

                first_step = false;

                color_start = last_color;
                time_start = TIME_MS;
                color_step = CalculateStep(last_color, color_end, time_run);
            }

            if (TIME_MS - prev_time < 33)
            {
                return;
            }

            prev_time = TIME_MS;

            uint time = TIME_MS - time_start;

            ColorLED color = color_start + color_step * time;

            if (time >= time_run)
            {
                color = color_end;
            }

            FireLED(color);

            if (TIME_MS >= time_start + time_run)
            {
                is_finished = true;
            }
        }

        bool IsFinished() const
        {
            return is_finished;
        }

        // \todo для экономии места нужно уменьшить размер переменных
        bool     first_step;
        ColorLED color_start;       // С этого цвета начинаем
        ColorLED color_end;         // Таким цвет должен быть после отработки задания
        ColorLED color_step;        // На столько изменяется цвет за одну миллисекунду
        uint     time_start;        // Время начала выполнения задания
        uint     time_run;          // Время выполнения задания. Оно закончится в time_start + time_run
        uint     prev_time;         // Время предыдущего шага. Нужно для ограничения fps
        bool     is_finished;       // Если true - анимация закончена

    private:

        ColorLED CalculateStep(const ColorLED &start, const ColorLED &end, uint time)
        {
            ColorLED step;

            if (time == 0)
            {
                for (int i = 0; i < 3; i++)
                {
                    step.chan_rel[i] = end.chan_rel[i] - start.chan_rel[i];
                }
            }
            else
            {
                for (int i = 0; i < 3; i++)
                {
                    step.chan_rel[i] = (end.chan_rel[i] - start.chan_rel[i]) / (float)time;
                }
            }

            return step;
        }
    };


    namespace Tasks
    {
        static const int8 MAX_TASKS = 10;
        static Task tasks[MAX_TASKS];
        static int8 num_tasks = 0;

        static bool IsEmpty()
        {
            return (num_tasks == 0);
        }

        static void RunStep()
        {
            if (!IsEmpty())
            {
                tasks[0].RunStep();

                if (tasks[0].IsFinished())
                {
                    num_tasks--;
                    std::memmove(tasks, tasks + 1, sizeof(Task) * (uint)num_tasks);
                }
            }
        }

        static void Clear()
        {
            num_tasks = 0;
        }

        static void AppendNew(const Color &color, uint time)
        {
            if (num_tasks < MAX_TASKS)
            {
                tasks[num_tasks++].Init(color, time);
            }
//            else
//            {
//                LOG_ERROR("Too many led tasks");
//            }
        }
    }
}


void Indicator::Init()
{
#ifdef TYPE_BOARD_771
    LP5012::Init();
#else
    WS2812B::Init();
#endif

#ifdef MCU_GD
#else
    HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);

    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
#endif
}


void Indicator::FireInputsInEnergySavingMode(bool lg, bool lr)
{
    static bool _prev_lr = false;    // Если true, то в прошлый раз зажигали красный.
    static uint time_prev_lr = 0;   // Время зажигания предыдущего красного.

    if (lg && !lr)
    {
        TurnOn(gset.ColorGreen(), TIME_RISE_LEDS, false);
    }
    else
    {
        TurnOff(TIME_RISE_LEDS, false);

        if (lr && !lg)                      // Пришёл "красный"
        {
            if (!_prev_lr)                   // Если ранее был не "красный"
            {
                _prev_lr = true;             // Сохраняем факт и время прихода
                time_prev_lr = TIME_MS;
            }
        }
        else                                // В любом случае кроме красного и зелёного
        {
            if (_prev_lr && (TIME_MS < time_prev_lr + 2000))     // Если был красный и времени прошло менее 2000 мс
            {
                uint time = TIME_MS - time_prev_lr;

                TurnOn(gset.ColorRed(), time / 2, false);
                TurnOff(time / 2, false);
            }

            _prev_lr = false;
        }
    }
}


void Indicator::FireInputs(bool lg, bool lr)
{
    prev_lg = lg;
    prev_lr = lr;

    if (!ModeOffline::IsEnabled() && !OSDP::IsEnabled())
    {
        if (Power::InEnergySavingMode())
        {
            FireInputsInEnergySavingMode(lg, lr);
        }
        else
        {
            if (lg && lr)
            {
                TurnOn(Color(0xFFFFFF, 1.0f), TIME_RISE_LEDS, false);
            }
            else if (lg)
            {
                TurnOn(gset.ColorGreen(), TIME_RISE_LEDS, false);
            }
            else if (lr)
            {
                TurnOn(gset.ColorRed(), TIME_RISE_LEDS, false);
            }
            else
            {
                TurnOff(TIME_RISE_LEDS, false);
            }
        }
    }
}


bool Indicator::IsRunning()
{
    return !Tasks::IsEmpty();
}


bool Indicator::IsFired()
{
    return (last_color != ColorLED(0, 0, 0));
}


void Indicator::TurnOn(const Color &color, uint time, bool stopped)
{
    if (stopped)
    {
        Tasks::Clear();
    }

    Tasks::AppendNew(color, time);
}


void Indicator::TurnOff(uint time, bool stopped)
{
    TurnOn(Color(0, 0.0f), time, stopped);
}


void Indicator::Blink(const Color &color1, const Color &color2, uint time, bool stopped)
{
    TurnOn(color1, time / 2, stopped);

    TurnOn(color2, time / 2, false);
}


void Indicator::FireLED(const ColorLED &color)
{
    if (color != last_color)
    {
#ifdef TYPE_BOARD_771
        LP5012::Fire(-1, TransformColor(color));
#else
        WS2812B::Fire(-1, TransformColor(color));
#endif

        last_color = color;
    }
}


ColorLED Indicator::TransformColor(const ColorLED &color)
{
    return color;

//    static int8 first = 1;
//
//    static uint8 koeff[256];
//
//    if (first)
//    {
//        first = 0;
//
//        for (int x = 0; x < 256; x++)
//        {
//            const float pow = 100.0f;
//
//            float y = std::powf(pow, (float)x / 255.0f) - 1.0f;
//
//            y /= (std::powf(pow, 1.0f) - 1.0f);
//
//            koeff[x] = (uint8)((y * 255.0f) + 0.49f);
//        }
//    }
//
//    return ColorLED
//    (
//        koeff[color.Red()],
//        koeff[color.Green()],
//        koeff[color.Blue()]
//    );
}


bool Indicator::Update()
{
    if (Upgrader::IsCompleted() && ModeIndicator::IsExternal() && Device::IsRunning() && !ModeOffline::IsEnabled() && !OSDP::IsEnabled() && LIS2DH12::IsExist())
    {
        static TimeMeterMS meter_lg_lr;

        static bool prev_state_low_energy = true;  // Здесь будет true, если в предыдущем кадре было пониженное питание

        bool lg = pinLG.IsLow();
        bool lr = pinLR.IsLow();

        if (lg != prev_lg || lr != prev_lr ||
            (prev_state_low_energy && !Power::InEnergySavingMode()))    // Если вышли из режима пониженного питания
        {
            if (lg && lr)
            {
                if (!lg_and_lr)
                {
                    lg_and_lr = true;

                    meter_lg_lr.Reset();
                }

                if (meter_lg_lr.ElapsedMS() > 200)
                {
                    FireInputs(lg, lr);

                    prev_lg = lg; //-V547
                    prev_lr = lr; //-V547
                }
            }
            else
            {
                FireInputs(lg, lr);

                prev_lg = lg;
                prev_lr = lr;

                lg_and_lr = false;
            }
        }

        prev_state_low_energy = Power::InEnergySavingMode();
    }

    Tasks::RunStep();

    return IsRunning();
}
