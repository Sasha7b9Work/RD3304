// 2022/08/06 09:15:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Modules/Indicator/LP5012/LP5012.h"
#include "Modules/Indicator/ColorLED.h"


namespace LP5012
{
    namespace Driver
    {
        struct Bank
        {
            enum E
            {
                A,
                B,
                C,
                Count
            };
        };

        struct LED
        {
            enum E
            {
                _0,
                _1,
                _2,
                _3,
                Count
            };
        };

        struct Out
        {
            enum E
            {
                _0,
                _1,
                _2,
                _3,
                _4,
                _5,
                _6,
                _7,
                _8,
                _9,
                _10,
                _11,
                Count
            };
        };

        struct Address
        {
            enum E
            {
                DEVICE_CONFIG_0  = 0x00,
                DEVICE_CONFIG_1  = 0x01,
                LED_CONFIG_0     = 0x02,
                BANK_BRIGHTNESS  = 0x03,
                BANK_A_COLOR     = 0x04,
                BANK_B_COLOR     = 0x05,
                BANK_C_COLOR     = 0x06,
                LED_0_BRIGHTNESS = 0x07,
                LED_1_BRIGHTNESS = 0x08,
                LED_2_BRIGHTNESS = 0x09,
                LED_3_BRIGHTNESS = 0x0A,
                OUT_0_COLOR      = 0x0B,
                OUT_1_COLOR      = 0x0C,
                OUT_2_COLOR      = 0x0D,
                OUT_3_COLOR      = 0x0E,
                OUT_4_COLOR      = 0x0F,
                OUT_5_COLOR      = 0x10,
                OUT_6_COLOR      = 0x11,
                OUT_7_COLOR      = 0x12,
                OUT_8_COLOR      = 0x13,
                OUT_9_COLOR      = 0x14,
                OUT_10_COLOR     = 0x15,
                OUT_11_COLOR     = 0x16,
                RESET            = 0x17,
                Count
            };
        };

        namespace Reg
        {
            struct RegisterLP5012
            {
                // _device может быть 0 или 1
                RegisterLP5012(int _device, Address::E _address) : address(_address), value(0), dev_id((uint8)(_device + 0x14)) {}

                void Write(uint8);
                void Write();
                uint8 Read();
                void SetBit(int bit);
                void ClearBit(int bit);

            protected:

                const Address::E address;
                uint8            value;
                const uint8      dev_id;
            };

            struct Config0 : public RegisterLP5012 { Config0(int _device) : RegisterLP5012(_device, Address::DEVICE_CONFIG_0) {} };

            struct Config1 : public RegisterLP5012 { Config1(int _device) : RegisterLP5012(_device, Address::DEVICE_CONFIG_1) {} };

            struct LedConfig0 : public RegisterLP5012 { LedConfig0(int _device) : RegisterLP5012(_device, Address::LED_CONFIG_0) {} };

            struct BankBrightness : public RegisterLP5012 { BankBrightness(int _device) : RegisterLP5012(_device, Address::BANK_BRIGHTNESS) {} };

            struct BrightnessLED : public RegisterLP5012 { BrightnessLED(int _device, LED::E led) : RegisterLP5012(_device, (Address::E)(0x07 + led)) {} };

            struct ColorOut : public RegisterLP5012 { ColorOut(int _device, Out::E out) : RegisterLP5012(_device, (Address::E)(0x0B + out)) {} };

            struct Reset : public RegisterLP5012 { Reset(int _device) : RegisterLP5012(_device, Address::RESET) {} };
        };

        struct ColorValue
        {
            ColorValue(float _value) : value(_value) {}
            float value;
            operator uint8() { return (uint8)(value * 0xFF); }
        };

        void Init()
        {
            for (int device = 0; device < 2; device++)
            {
                Reg::Reset(device).Write(0xFF);             // Сброс

                Reg::Config0(device).Write(0xFF);           // Включаем

                Reg::Config1 config1(device);
                config1.Read();
                config1.SetBit(1);                          // Output maximum current = 25.5/35 mA
//                config1.ClearBit(5);                        // Log_Scale_EN
                config1.Write();

                Reg::LedConfig0(device).Write(0x00);        // Независимое управление свеодиодами
            }
        }

        // Параметры [0...1.0]
        void Fire(int vh, const ColorLED &color)
        {
            HAL::bus_is_busy = true;

            static const int devices[8] =       {       0,       0,       1,       1,       0,       0,        1,       1 };

            static const LED::E leds[8] =       { LED::_1, LED::_2, LED::_1, LED::_2, LED::_0, LED::_3,  LED::_0, LED::_3 };

            static const Out::E outs_red[8] =   { Out::_3, Out::_6, Out::_3, Out::_6, Out::_0, Out::_9,  Out::_0, Out::_9 };
            static const Out::E outs_green[8] = { Out::_4, Out::_7, Out::_4, Out::_7, Out::_1, Out::_10, Out::_1, Out::_10 };
            static const Out::E outs_blue[8] =  { Out::_5, Out::_8, Out::_5, Out::_8, Out::_2, Out::_11, Out::_2, Out::_11 };

            int dev = devices[vh];

            Reg::BrightnessLED(dev, leds[vh]).Write(255);                   // Это нужно сделать при инициализации один раз

            Reg::ColorOut(dev, outs_red[vh]).Write(color.Red());

            Reg::ColorOut(dev, outs_green[vh]).Write(color.Green());

            Reg::ColorOut(dev, outs_blue[vh]).Write(color.Blue());

            HAL::bus_is_busy = false;
        }
    }
} 


void LP5012::Driver::Reg::RegisterLP5012::Write(uint8 data)
{
    value = data;

    HAL_I2C::Write8(dev_id, (uint8)address, data);
}


void LP5012::Driver::Reg::RegisterLP5012::Write()
{
    Write(value);
}


uint8 LP5012::Driver::Reg::RegisterLP5012::Read()
{
    HAL_I2C::Read8(dev_id, (uint8)address, &value);

    return value;
}


void LP5012::Driver::Reg::RegisterLP5012::SetBit(int bit)
{
    value |= (1 << bit);
}


void LP5012::Driver::Reg::RegisterLP5012::ClearBit(int bit)
{
    value &= (~(1 << bit));
}
