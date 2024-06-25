// 2022/6/15 22:18:18 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/Indicator/WS2812B/WS2812B.h"
#include "Hardware/Timer.h"
#include "Nodes/Communicator.h" 
#include "Hardware/HAL/HAL.h"
#include "Modules/Player/Player.h"
#include "system.h"


#ifndef TYPE_BOARD_771


namespace HAL_USART
{
    namespace Data
    {
        void Append(uint8);
    }
}


namespace WS2812B
{
    void Fire(uint colors[4]);

#ifdef MCU_GD

    static const uint PORT_DLED = GPIOB;
    static const uint16 PIN_DLED = GPIO_PIN_1;

#endif

    namespace DLED
    {
        static void Init()
        {
#ifdef MCU_GD
            gpio_mode_set(PORT_DLED, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, PIN_DLED);
            gpio_output_options_set(PORT_DLED, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, PIN_DLED);
#else
            GPIO_InitTypeDef is =
            {
                GPIO_PIN_1,
                GPIO_MODE_OUTPUT_PP,
                GPIO_PULLUP,
                GPIO_SPEED_HIGH
            };

            HAL_GPIO_Init(GPIOB, &is);

            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
#endif
        }

        // Записать полную последовательность бит в четыре индикатора
        // 96 бит = 4 индикатора по 24 бита
        // data представляет собой массив количества тактов, в которые нужно передёргивать из 0 в 1 и обратно.
        // data[0] - начало положительного импульса 0-го бита
        // data[1] - окончание положительного импульса 0-го бита
        // data[2], data[3] - --//-- 1-го бита
        // data[4], data[5] - --//-- 2-го бита
        // В массиве не 96 * 2 элементов
        // 24 - бит на индикатор, 2 - интервалов на бит, 4 - количество индикаторов, +4 - после 24 бит каждого индикатора идёт завершающий ноль
        static void WriteFullSequency(const uint data[24 * 2 * 4 + 4]);

        // Записать управляющую последовательность для одного индикатора. Здесь на 1 элемент больше, чем нужно переключений - завершается нулём
        static void WriteOneIndicator(const uint data[24 * 2 + 1]);
    }
}


void WS2812B::Init()
{
    DLED::Init();
}


void WS2812B::Fire(int num_led, const ColorLED &_color)
{
    static uint colors[4] =
    {
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF
    };

    uint color = (uint)((_color.Green() << 16) | (_color.Red() << 8) | _color.Blue());

    if (num_led == -1)
    {
        if (color != colors[0] || color != colors[1] || color != colors[2] || color != colors[3])
        {
            colors[0] = colors[1] = colors[2] = colors[3] = color;

            Fire(colors);
        }
    }
    else
    {
        if (colors[num_led] != color)
        {
            colors[num_led] = color;

            Fire(colors);
        }
    }
}


void WS2812B::Fire(uint colors[4])
{
    static const uint dT0H = 3;    // В значениях программного счётчика  2...4  1 единица - 133 ns //-V525
    static const uint dT0L = 5;    // В значениях программного счётчика  5...
    static const uint dT1H = 5;    // В значениях программного счётчика
    static const uint dT1L = 4;    // В значениях программного счётчика

    /*
    * Цвет хранится в формате GRB
    */

    // Записать текущий счётчик времени в массив и увеличить его на delta
#define WRITE_TIME(delta)   *time++ = delta;

    uint bits[24 * 2 * 4 + 4];
    uint *time = &bits[0];

    for (int indicator = 0; indicator < 4; indicator++)
    {
        uint col = colors[indicator] << 8;

        for (int i = 0; i < 24; i++)
        {
            if (col & 0x80000000)       // единица
            {
                WRITE_TIME(dT1H)
                WRITE_TIME(dT1L)
            }
            else                        // ноль
            {
                WRITE_TIME(dT0H)
                WRITE_TIME(dT0L)
            }

            col <<= 1;
        }

        *time++ = 0;
    }

    DLED::WriteFullSequency(bits);
}


#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#else
    #ifndef WIN32
        #pragma O3      // Это нельзя убирать. А то длительности импульсов будут больше и работать не будет
    #endif
#endif


#define DISABLE_IRQ()   __disable_irq(); __DSB(); __ISB()
#define ENABLE_IRQ()    __enable_irq()


void WS2812B::DLED::WriteFullSequency(const uint data[24 * 2 * 4 + 4])
{
    if (Player::IsPlaying())
    {
        for (int i = 0; i < 4; i++)
        {
            __disable_irq();

            WriteOneIndicator(data + i * (24 * 2 + 1));     // 1 - это завершающий ноль. На каждый индикатор идёт 49 элементов массива, а не 48

            ENABLE_IRQ();
        }
    }
    else
    {
        DISABLE_IRQ()

            for (int i = 0; i < 4; i++)
            {
#ifdef MCU_GD
                if((USART_REG_VAL(USART1, USART_INT_FLAG_RBNE) & BIT(USART_BIT_POS(USART_INT_FLAG_RBNE))) &&
                    (USART_REG_VAL2(USART1, USART_INT_FLAG_RBNE) & BIT(USART_BIT_POS2(USART_INT_FLAG_RBNE))))
                {
                    HAL_USART::Data::Append((uint8)usart_data_receive(USART1));
                }
#else
                if (USART2->SR & USART_SR_RXNE) { HAL_USART::Data::Append((uint8)USART2->DR); }
#endif

                WriteOneIndicator(data + i * (24 * 2 + 1));         // 1 - это завершающий ноль. На каждый индикатор идёт 49 элементов массива, а не 48

#ifdef MCU_GD
                if ((USART_REG_VAL(USART1, USART_INT_FLAG_RBNE) & BIT(USART_BIT_POS(USART_INT_FLAG_RBNE))) &&
                    (USART_REG_VAL2(USART1, USART_INT_FLAG_RBNE) & BIT(USART_BIT_POS2(USART_INT_FLAG_RBNE))))
                {
                    HAL_USART::Data::Append((uint8)usart_data_receive(USART1));
                }
#else
                if (USART2->SR & USART_SR_RXNE) { HAL_USART::Data::Append((uint8)USART2->DR); }
#endif
            }

        ENABLE_IRQ();
    }
}

#ifdef WIN32
    #pragma warning(push, 0)
    #pragma warning disable
#endif


void WS2812B::DLED::WriteOneIndicator(const uint data[24 * 2 + 1])
{
    (void)data;

#ifdef MCU_GD
    #define PIN_SET     GPIO_BOP(GPIOB) = (uint32_t)GPIO_PIN_1
    #define PIN_RESET   GPIO_BC(GPIOB) = (uint32_t)GPIO_PIN_1
#else
    #define PIN_SET     GPIOB->BSRR = GPIO_PIN_1
    #define PIN_RESET   GPIOB->BSRR = (uint)(GPIO_PIN_1 << 16)
#endif

#define WRITE_BIT   counter = *data++;      \
                    PIN_SET;                \
                    while(--counter) {}     \
                    counter = *data++;      \
                    PIN_RESET;              \
                    while(--counter) {}

    volatile uint counter = 0;

    WRITE_BIT   // 1 //-V760
    WRITE_BIT   // 2
    WRITE_BIT   // 3
    WRITE_BIT   // 4
    WRITE_BIT   // 5
    WRITE_BIT   // 6
    WRITE_BIT   // 7
    WRITE_BIT   // 8
    WRITE_BIT   // 9
    WRITE_BIT   // 10
    WRITE_BIT   // 11
    WRITE_BIT   // 12
    WRITE_BIT   // 13
    WRITE_BIT   // 14
    WRITE_BIT   // 15
    WRITE_BIT   // 16
    WRITE_BIT   // 17
    WRITE_BIT   // 18
    WRITE_BIT   // 19
    WRITE_BIT   // 20
    WRITE_BIT   // 21
    WRITE_BIT   // 22
    WRITE_BIT   // 23
    WRITE_BIT   // 24
}

#ifdef WIN32
    #pragma warning pop
#endif


#endif
