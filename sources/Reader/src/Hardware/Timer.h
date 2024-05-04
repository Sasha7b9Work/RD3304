// 2022/6/7 9:06:54 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Hardware/HAL/HAL.h"


#ifdef MCU_GD
    #define TIME_MS Timer::counter_ms
#else
    #define TIME_MS Timer::CurrentTime()
#endif


namespace Timer
{
    void Init();

    uint CurrentTime();

    void Delay(uint timeMS);

    extern uint counter_ms;
}



// Структура для отсчёта времени
struct TimeMeterMS
{
    TimeMeterMS();

    // Установить момент отсчёта
    void Reset();

    void Pause();

    void Continue();

    // Ождиать ms миллисекунд с момента последнего Reset()
    void WaitFor(uint ms);

    // Столько миллисекунд прошло с момента вызова Reset()
    uint ElapsedMS() const;

    static TimeMeterMS global;

private:

    uint time_reset;        // От этого времени отсчитывается ElapsedTime()
    uint time_pause;        // В этот момент поставили на паузу
};


// !!! WARNING !!! Эта структура может существовать в единственном экземпляре в каждый момент времени,
// т.к. задействует глобальный аппаратный ресурс
// !!! WARNING !!! Максимальный интервал - 65535 мкс
struct TimeMeterUS
{
    TimeMeterUS()
    {
        Reset();
    }

    void Reset()
    {
#ifndef WIN32
    #ifdef MCU_GD
        HAL_TIM5::Start();
    #else
        DWT->CYCCNT = 0U;
    #endif
#endif
    }

    // Ожидать us микросекунд с момента последнего Reset()
    void WaitFor(uint us)
    {
        (void)us;

#ifndef WIN32

    #ifdef MCU_GD
        while (HAL_TIM5::ElapsedUS() < us) { }
    #else
        uint end_count = us * 60;

        while (DWT->CYCCNT < end_count) { }
    #endif

#endif
    }

    uint ElapsedUS()
    {
#ifdef WIN32
        return 0;
#else
    #ifdef MCU_GD
        return HAL_TIM5::ElapsedUS();
    #else
        return DWT->CYCCNT / 60;
    #endif
#endif
    }
};
