// 2024/02/11 10:50:32 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"


namespace HAL_SPI
{
    namespace CS
    {
        static PinOutputPP pinSRAM(Port::_A, GPIO_PIN_12, Pulling::Up);
        static PinOutputPP pinSTRX(Port::_A, GPIO_PIN_15, Pulling::Up);

        static const PinOutputPP *pins[DirectionSPI::Count] = { &pinSRAM, &pinSTRX };

        static void ToHi(DirectionSPI::E dir)
        {
            pins[dir]->ToHi();
        }

        static void ToLow(DirectionSPI::E dir)
        {
            pins[dir]->ToLow();
        }

        static void Init()
        {
            pinSRAM.Init();
            pinSTRX.Init();

            ToHi(DirectionSPI::Memory);
            ToHi(DirectionSPI::Reader);
        }
    }

    namespace SCK
    {
        static const uint PORT_SCK = GPIOB;
        static const uint16 PIN_SCK = GPIO_PIN_3;

//        void ToHi()
//        {
////            gpio_bit_set(PORT_SCK, PIN_SCK);
//            GPIO_BOP(PORT_SCK) = (uint32_t)PIN_SCK;
//        }

#define SCK_TO_HI GPIO_BOP(SCK::PORT_SCK) = (uint32_t)SCK::PIN_SCK

//        void ToLow()
//        {
////            gpio_bit_reset(PORT_SCK, PIN_SCK);
//            GPIO_BC(PORT_SCK) = (uint32_t)PIN_SCK;
//        }

#define SCK_TO_LOW GPIO_BC(SCK::PORT_SCK) = (uint32_t)SCK::PIN_SCK

        static void Init()
        {
            gpio_mode_set(PORT_SCK, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, PIN_SCK);
            gpio_output_options_set(PORT_SCK, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, PIN_SCK);

            SCK_TO_HI;
        }
    }

    namespace MOSI
    {
        static const uint PORT_MOSI = GPIOB;
        static const uint16 PIN_MOSI = GPIO_PIN_5;

//        void ToHi()
//        {
////            gpio_bit_set(MOSI::PORT_MOSI, MOSI::PIN_MOSI);
//            GPIO_BOP(MOSI::PORT_MOSI) = (uint32_t)MOSI::PIN_MOSI;
//        }

#define MOSI_TO_HI GPIO_BOP(MOSI::PORT_MOSI) = (uint32_t)MOSI::PIN_MOSI

//        void ToLow()
//        {
////            gpio_bit_reset(MOSI::PORT_MOSI, MOSI::PIN_MOSI);
//            GPIO_BC(MOSI::PORT_MOSI) = (uint32_t)MOSI::PIN_MOSI;
//        }

#define MOSI_TO_LOW GPIO_BC(MOSI::PORT_MOSI) = (uint32_t)MOSI::PIN_MOSI

        static void SetForHiBit(uint8 byte)
        {
            if (byte & 0x80)
            {
                MOSI_TO_HI;
            }
            else
            {
                MOSI_TO_LOW;
            }
        }

        static void Init()
        {
            gpio_mode_set(PORT_MOSI, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, PIN_MOSI);
            gpio_output_options_set(PORT_MOSI, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, PIN_MOSI);

            SetForHiBit(0xff);
        }
    }

    namespace MISO
    {
        static const uint PORT_MISO = GPIOB;
        static const uint16 PIN_MISO = GPIO_PIN_4;

        static void Init()
        {
            gpio_mode_set(PORT_MISO, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, PIN_MISO);
        }

//        bool State()
//        {
////            return gpio_input_bit_get(PORT_MISO, PIN_MISO) == SET;
//            return (GPIO_ISTAT(MISO::PORT_MISO) & (MISO::PIN_MISO)) != RESET;
//        }

#define MISO_IS_HI ((GPIO_ISTAT(MISO::PORT_MISO) & (MISO::PIN_MISO)) != RESET)
    }

    static void WriteByte(uint8);

    static uint8 WriteReadByte(uint8);
}

void HAL_SPI::Init()
{
    CS::Init();

    SCK::Init();

    MOSI::Init();

    MISO::Init();
}


uint8 HAL_SPI::WriteReadByte(uint8 byte)
{
    uint8 result = 0;

    for (int i = 0; i < 8; i++)
    {
        SCK_TO_LOW;

        MOSI::SetForHiBit(byte);

        byte <<= 1;

        SCK_TO_HI;

        result <<= 1;

        if (MISO_IS_HI)
        {
            result |= 0x01;
        }
    }

    return result;
}


void HAL_SPI::WriteRead(DirectionSPI::E dir, const void *out, void *in, int size)
{
    CS::ToLow(dir);

    const  uint8 *write = (const uint8 *)out;
    uint8 *read = (uint8 *)in;

    for (int i = 0; i < size; i++)
    {
        *read++ = WriteReadByte(*write++);
    }

    CS::ToHi(dir);
}


void HAL_SPI::WriteByte(DirectionSPI::E dir, uint8 byte)
{
    WriteBuffer(dir, &byte, 1);
}


void HAL_SPI::WriteByte(uint8 byte)
{
    for (int i = 0; i < 8; i++)
    {
        SCK_TO_LOW;

        MOSI::SetForHiBit(byte);

        SCK_TO_HI;

        byte <<= 1;
    }
}


void HAL_SPI::LOW::WriteByte(uint8 byte)
{
    HAL_SPI::WriteByte(byte);
}


void HAL_SPI::LOW::CS::ToLow()
{
    HAL_SPI::CS::ToLow(DirectionSPI::Memory);
}


void HAL_SPI::LOW::CS::ToHi()
{
    HAL_SPI::CS::ToHi(DirectionSPI::Memory);
}


void HAL_SPI::WriteBuffer(DirectionSPI::E dir, const void *buffer, int size)
{
    CS::ToLow(dir);

    const uint8 *data = (const uint8 *)buffer;

    for (int i = 0; i < size; i++)
    {
        WriteByte(*data++);
    }

    CS::ToHi(dir);
}


void HAL_SPI::Test()
{
    const uint time_start = TIME_MS;

    const int num_bytes = 1000000;

    for (int i = 0; i < num_bytes; i++)
    {
        WriteReadByte(0);
    }

    const uint time = TIME_MS - time_start;
}
