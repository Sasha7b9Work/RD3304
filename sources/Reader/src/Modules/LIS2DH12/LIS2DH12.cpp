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
#include "system.h"
#include <cstring>
#include <cstdio>


namespace LIS2DH12 
{
    static const uint8 ADDRESS = 0x19 << 1;

    static StructDataRaw raw_temp;

    static Averager<StructDataRaw, 4> raw_acce_x;
    static Averager<StructDataRaw, 4> raw_acce_y;
    static Averager<StructDataRaw, 4> raw_acce_z;

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

    // Сторож
    namespace Watcher
    {
        static float start_x = 0.0f;
        static float start_y = 0.0f;
        static float start_z = 0.0f;

        static bool is_init = false;

        static bool is_alarmed = false;         // Положение изменилось. Гудим
        static uint time_disable_alarm = 0;     // В это время нужно выключить тревогу


        static void Init(float x, float y, float z)
        {
            is_init = true;

            start_x = x;
            start_y = y;
            start_z = z;
        }

        static void Update(float x, float y, float z)
        {
            if (ModeReader::IsExtended())
            {
                return;
            }

            if (!gset.IsEnabledAntibreak())
            {
                return;
            }

            if (is_alarmed)
            {
                Indicator::Blink(Color(0xFFFFFFFF, 1.0f), Color(0x00000000, 0.0f), 1, true);

                Player::PlayFromMemory(gset.Melody(TypeSound::Beep), 3);

                while (Indicator::IsRunning())
                {
                    Device::UpdateTasks();
                }

                if (TIME_MS > time_disable_alarm)
                {
                    is_alarmed = false;
                    is_init = false;
                }

                return;
            }

            if (!is_init)
            {
                if (TIME_MS < 5000)
                {
                    return;
                }

                Init(x, y, z);
            }

            const float delta = gset.GetAntibreakSens();

            float dx = x - start_x;
            float dy = y - start_y;
            float dz = z - start_z;

            if(std::sqrtf(dx * dx + dy * dy + dz * dz) > delta)
            {
                is_alarmed = true;
                time_disable_alarm = TIME_MS + 30 * 1000;

                uint number = gset.GetAntibreakNumber();

                if (OSDP::IsEnabled())
                {
                    OSDP::AntibreakAlarm();
                }
                else if (ModeReader::IsWG())
                {
                    HAL_USART::WG26::Transmit((uint8)(number & 0xFF), (uint8)((number >> 8) & 0xFF), (uint8)((number >> 16) & 0xFF));
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
    _SET_BIT(data, 4);                              // I1_ZYXDA = 1 - разрешаем прерывания INT1 по полученным измерениям
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
 // lis2dh12_full_scale_set(&dev_ctx, LIS2DH12_2g);   Это значение по умолчанию

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
        if (Read(LIS2DH12_STATUS_REG) & (1 << 3))
        {
            StructDataRaw data;

            data.lo = Read(LIS2DH12_OUT_X_L);
            data.hi = Read(LIS2DH12_OUT_X_H);

            raw_acce_x.Push(data);

            data.lo = Read(LIS2DH12_OUT_Y_L);
            data.hi = Read(LIS2DH12_OUT_Y_H);

            raw_acce_y.Push(data);

            data.lo = Read(LIS2DH12_OUT_Z_L);
            data.hi = Read(LIS2DH12_OUT_Z_H);

            raw_acce_z.Push(data);
        }
    }

    if (Read(LIS2DH12_STATUS_REG_AUX) & (1 << 2))       // TDA
    {
        raw_temp.lo = Read(LIS2DH12_OUT_TEMP_L);
        raw_temp.hi = Read(LIS2DH12_OUT_TEMP_H);
    }

    Watcher::Update(raw_acce_x.Get().ToAccelearation(), raw_acce_y.Get().ToAccelearation(), raw_acce_z.Get().ToAccelearation());
}


StructDataRaw LIS2DH12::GetRawTemperature()
{
    return raw_temp;
}


float LIS2DH12::GetAccelerationX()
{
    return raw_acce_x.Get().ToAccelearation();
}


float LIS2DH12::GetAccelerationY()
{
    return raw_acce_y.Get().ToAccelearation();
}


float LIS2DH12::GetAccelerationZ()
{
    return raw_acce_z.Get().ToAccelearation();
}
