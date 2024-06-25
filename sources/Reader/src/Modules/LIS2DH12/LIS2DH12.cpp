// 2022/6/24 15:52:26 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/LIS2DH12/LIS2DH12.h"
#include "Modules/LIS2DH12/LIS2DH12_reg.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Hardware/Timer.h"
#include "Modules/Player/Player.h"
#include "Nodes/Upgrader.h"
#include "Modules/Indicator/Indicator.h"
#include "Modules/Player/Player.h"
#include "Device/Device.h"
#include "Nodes/OSDP/OSDP.h"
#include "Utils/Averager.h"
#include "Utils/FiltrMiddleOf3.h"
#include "Utils/Math.h"
#include "system.h"
#include <cstring>
#include <cstdio>


namespace LIS2DH12 
{
    static const uint8 ADDRESS = 0x19 << 1;

    static StructDataRaw raw_temp;

    static Averager<StructDataRaw, 1> raw_acce_x;       // /
    static Averager<StructDataRaw, 1> raw_acce_y;       // | текущие значени€ акселерометра
    static Averager<StructDataRaw, 1> raw_acce_z;       // /

    static Averager<StructDataRaw, 1> *raw_1 = nullptr;     // «десь будет наименьша€ измеренна€ ось
    static Averager<StructDataRaw, 1> *raw_2 = nullptr;     // «десь будет втора€ по величине измеренна€ ось

    static bool is_exist = true;

    bool IsExist()
    {
        return is_exist;
    }

    static bool Write(uint8 reg, uint8 data)
    {
        return HAL_I2C::Write(ADDRESS, reg, &data, 1);
    }

    static uint8 Read(uint8 reg)
    {
        uint8 result = 0;
        HAL_I2C::Read(ADDRESS, reg, &result, 1);
        return result;
    }

    // —торож
    namespace Watcher
    {
        static StructDataRaw start_x;       // /
        static StructDataRaw start_y;       // | Ёто стартовые значени€ - с ними будем сравнивать
        static StructDataRaw start_z;       // /

        static StructDataRaw *start_1 = nullptr;    // ”казатель на минимальную измеренную ось
        static StructDataRaw *start_2 = nullptr;    // ”казатель на вторую по величине измеренную ось

        static bool is_init = false;

        static bool is_alarmed = false;                 // ѕоложение изменилось. √удим
        static TimeMeterMS meter_duration_alarm;        // «десь - врем€, которое прошло с начала последней тревоги
        static TimeMeterMS meter_after_disable_alarm;   // —только времени прошло с отключени€ последней тревоги


        static void Init()
        {
            is_init = true;

            start_x = raw_acce_x.Get();
            start_y = raw_acce_y.Get();
            start_z = raw_acce_z.Get();

            float x = start_x.ToAccelearation();
            float y = start_y.ToAccelearation();
            float z = start_z.ToAccelearation();

            if (x <= y && x <= z)
            {
                start_1 = &start_x;
                raw_1 = &raw_acce_x;

                start_2 = (y <= z) ? &start_y : &start_z;
                raw_2 = (y <= z) ? &raw_acce_y : &raw_acce_z;
            }
            else if (y <= x && y <= z)
            {
                start_1 = &start_y;
                raw_1 = &raw_acce_y;

                start_2 = (x <= z) ? &start_x : &start_z;
                raw_2 = (x <= z) ? &raw_acce_x : &raw_acce_z;
            }
            else
            {
                start_1 = &start_z;
                raw_1 = &raw_acce_z;

                start_2 = (x <= y) ? &start_x : &start_y;
                raw_2 = (x <= y) ? &raw_acce_x : &raw_acce_y;
            }
        }

        static void Update()
        {
            if (meter_after_disable_alarm.ElapsedMS() < 5000)
            {
                return;
            }

            if (ModeReader::IsExtended())
            {
                return;
            }

            if (!gset.IsEnabledAntibreak())
            {
                return;
            }

            const float delta = gset.GetAntibreakSens();

            float angle = 0.0f;

            if (raw_1)
            {
                bool result = Math::AngleBetweenVectors2D(start_1->ToAccelearation(), start_2->ToAccelearation(),
                    raw_1->Get().ToAccelearation(), raw_2->Get().ToAccelearation(), &angle);

                static FiltrMiddleOf3<float> middle_angle;

                if (result)
                {
                    angle = middle_angle.Push(angle);
                }
                else
                {
                    if (!is_alarmed)
                    {
                        return;
                    }
                }

                if (is_alarmed)
                {
                    Indicator::Blink(Color(0xFFFFFFFF, 1.0f), Color(0x00000000, 0.0f), 1, true);

                    Player::PlayFromMemory(gset.Melody(TypeSound::Beep), 3);

                    while (Indicator::IsRunning())
                    {
                        Device::UpdateTasks();
                    }

                    if (meter_duration_alarm.ElapsedMS() > 30 * 1000)
                    {
                        is_alarmed = false;
                        is_init = false;
                        meter_after_disable_alarm.Reset();
                        raw_1 = nullptr;
                    }

                    return;
                }
            }

            if (!is_init)
            {
                Init();

                return;
            }

            if (angle > delta)
            {
                is_alarmed = true;
                meter_duration_alarm.Reset();

                uint number = gset.GetAntibreakNumber();

                if (OSDP::IsEnabled())
                {
                    OSDP::AntibreakAlarm();
                }
                else if (ModeReader::IsWG())
                {
                    BitSet32 bs;
                    bs.word = number;

                    HAL_USART::WG26::Transmit(bs.bytes[2], bs.bytes[1], bs.bytes[0]);
                }
            }
        }
    }
}


bool LIS2DH12::IsAlarmed()
{
    return Watcher::is_alarmed;
}


void LIS2DH12::Init()
{
    pinIRQ_SNS.Init();

    uint8 data = 0;
    _SET_BIT(data, 4);                              // I1_ZYXDA = 1 - разрешаем прерывани€ INT1 по полученным измерени€м
    Write(LIS2DH12_CTRL_REG3, data);

    // Enable Block Data Update.
    data = Read(LIS2DH12_CTRL_REG4);
    data |= (1 << 7);                               // BDU = 1
    Write(LIS2DH12_CTRL_REG4, data);

    // Set Output Data Rate to 1Hz.
    HAL_I2C::Read(ADDRESS, LIS2DH12_CTRL_REG1, &data, 1);
    data |= (1 << 4);                               // ODR = 0b0001, 1Hz
    HAL_I2C::Write(ADDRESS, LIS2DH12_CTRL_REG1, &data, 1);
    
 // Set full scale to 2g.
 // lis2dh12_full_scale_set(&dev_ctx, LIS2DH12_2g);   Ёто значение по умолчанию

    // Enable temperature sensor.
    data = 0xC0;
    HAL_I2C::Write(ADDRESS, LIS2DH12_TEMP_CFG_REG, &data, 1);     // TEMP_EN = 0b11

    // Set device in continuous mode with 12 bit resol.
    if (!HAL_I2C::Read(ADDRESS, LIS2DH12_CTRL_REG4, &data, 1))
    {
        is_exist = false;
    }
    data |= (1 << 3);                                           // HR = 1, (LPen = 0 - High resolution mode)
    if (!Write(LIS2DH12_CTRL_REG4, data))
    {
        is_exist = false;
    }
}


void LIS2DH12::Update()
{
    if (!is_exist || Player::IsPlaying())
    {
        return;
    }

    if (pinIRQ_SNS.IsHi())                                      // ZYXDA
    {
        static FiltrMiddleOf3<int16> middleX;
        static FiltrMiddleOf3<int16> middleY;
        static FiltrMiddleOf3<int16> middleZ;

        if (Read(LIS2DH12_STATUS_REG) & (1 << 3))
        {
//            static float prev_x = 0.0f;
//            static float prev_y = 0.0f;
//            static float prev_z = 0.0f;

//            float x = 0.0f;
//            float y = 0.0f;
//            float z = 0.0f;

            StructDataRaw data;

            ////////////////////////////////////////

            data.lo = Read(LIS2DH12_OUT_X_L);
            data.hi = Read(LIS2DH12_OUT_X_H);

            data.raw = middleX.Push(data.raw);

            raw_acce_x.Push(data);

//            x = data.ToAccelearation();

            ////////////////////////////////////////

            data.lo = Read(LIS2DH12_OUT_Y_L);
            data.hi = Read(LIS2DH12_OUT_Y_H);

            data.raw = middleY.Push(data.raw);

            raw_acce_y.Push(data);

//            y = data.ToAccelearation();

            ///////////////////////////////////////

            data.lo = Read(LIS2DH12_OUT_Z_L);
            data.hi = Read(LIS2DH12_OUT_Z_H);

            data.raw = middleZ.Push(data.raw);

            raw_acce_z.Push(data);

//            z = data.ToAccelearation();

            //////////////////////////////////////

//            float dx = std::fabsf(x - prev_x);
//            float dy = std::fabsf(y - prev_y);
//            float dz = std::fabsf(z - prev_z);
//
//            if (dx != 0.0f || dy != 0.0f || dz != 0.0f)
//            {
//                LOG_WRITE_TRACE("%5.3f %5.3f %5.3f abs = %5.3f, angle = %f",
//                    (double)dx, (double)dy, (double)dz,
//                    (double)std::sqrtf(dx * dx + dy * dy + dz * dz), 
//                    (double)Math::AngleBetweenVectors(x, y, z, prev_x, prev_y, prev_z));
//            }
//            else
//            {
//                LOG_WRITE_TRACE("All zeros");
//            }
//
//            prev_x = x;
//            prev_y = y;
//            prev_z = z;

            Watcher::Update();
        }
    }

    if (Read(LIS2DH12_STATUS_REG_AUX) & (1 << 2))       // TDA
    {
        raw_temp.lo = Read(LIS2DH12_OUT_TEMP_L);
        raw_temp.hi = Read(LIS2DH12_OUT_TEMP_H);
    }
}


StructDataRaw LIS2DH12::GetRawTemperature()
{
    return raw_temp;
}
