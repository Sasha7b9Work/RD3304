// 2024/03/26 08:24:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/GD/systick.h"
#include "Utils/Math.h"
#include "Modules/LIS2DH12/LIS2DH12.h"


/*
    Ресурсы

    +-----+-----------+-----------+-----------+
    |     |    777    |     771   |   799     |
    +-----+-----------+-----------+-----------+
    |     |   First   |   Second  |   Third   |
    +-----+-----------+-----------+-----------+
    | A4  | + IRQ TRX |   WNG     |    WS     | + I2S0_WS
    | A5  | + -       | + IRQ SNS |   BCLK    | + I2S0_CK
    | A7  | + BEEP    | + -       |   DIN     | + I2S0_SD
    | A8  | + BEEN    | + LG      |    SD     | + I2S0
    | A12 | + SRAM    | + STRX    |   SRAM    |
    | A15 | + STRX    | + SRAM    |   STRX    |
    | B0  | + LG      | + BEEP    |   SND     |
    | B1  | + DLED    | + BEEN    |   DLED    |
    | B2  | + 0V      | + ENP     |  IRQ_TRX  |
    | D0  | + IRQ SNS | + IRQ TRX |     -     |
    +-----+-----------+-----------+-----------+
    | A1  | + -       |   SKEY    |    KB     |
    | D1  | + -       |   IRQ KEY |     -     |
    +-----+-----------+-----------+-----------+
    | A0  | + VIN     | + VIN     |   V_DC    |
    | A2  | + TXD2    | + TXD2    |   TXD2    |
    | A3  | + RXD2    | + RXD2    |   RXD2    |
    | A6  | + ENN     | + ENN     |   ENN     |
    | A9  | + TXD1    | + TXD1    |   TXD1    |
    | A10 | + LR      | + LR      |   LG      |
    | A11 | + SND     | + SND     |   LR      |
    | A13 | + SWDIO   | + SWDIO   |     -     |
    | A14 | + SWCLK   | + SWCLK   |     -     |
    | B3  | + SCK     | + SCK     |   SCK     |
    | B4  | + MISO    | + MISO    |   MISO    |
    | B5  | + MOSI    | + MOSI    |   MOSI    |
    | B6  | + SCL     | + SCL     |   SCL     |
    | B7  | + SDA     | + SDA     |   SDA     |
    +-----+-----------+-----------+-----------+
    | PF0 |           |           |  IRQ_SNS  |
    +-----+-----------+-----------+-----------+


    SPI0                Память, считыватель
            PB3 SPI0_SCK    AF_0
            PB4 SPI0_MISO   AF_0
            PB5 SPI0_MOSI   AF_0
            SPI0_RX         DMA CH1
            SPI0_TX         DMA CH2

    I2S0                Динамик
            PA5 I2S0_CK     AF_0
            PA4 I2S0_WS     AF_0
            PA7 I2S0_SD     AF_0
            I2S0_RX         DMA CH1
            I2S0_TX         DMA_CH2

    I2C0                LIS2DH12
            PB6 I2C0_SCL    AF_1
            PB7 I2C0_SDA    AF_1

    USART1
*/


void HAL::Init()
{
    systick_config();

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOF);

    rcu_periph_clock_enable(RCU_DMA);           // I2S0 для звука
    nvic_irq_enable(DMA_Channel1_2_IRQn, 0);    // I2S0 для звука

    rcu_periph_clock_enable(RCU_SPI0);          // Для звука

    rcu_periph_clock_enable(RCU_USART1);        // RS-485
    nvic_irq_enable(USART1_IRQn, 0);

    rcu_periph_clock_enable(RCU_ADC);           // Измерение напряжения
//    rcu_adc_clock_config(RCU_ADCCK_APB2_DIV6);
    RCU_CFG0 &= ~RCU_CFG0_ADCPSC;
    RCU_CFG2 &= ~(RCU_CFG2_ADCSEL | RCU_CFG2_IRC28MDIV | RCU_CFG2_ADCPSC2);
    RCU_CFG0 |= RCU_ADC_CKAPB2_DIV6;
    RCU_CFG2 |= RCU_CFG2_ADCSEL;

    rcu_periph_clock_enable(RCU_I2C0);          // Акселерометр

    rcu_periph_clock_enable(RCU_TIMER5);        // Счётчик микросекунд TimeMeterMS

    rcu_periph_clock_enable(RCU_TIMER13);       // Для подгрузки данных при воспроизведении звука
    nvic_irq_enable(TIMER13_IRQn, 2);

    HAL_I2S::Init();

    HAL_ADC::Init();

    HAL_I2C::Init();

    HAL_SPI::Init();

    HAL_USART::Init();

    HAL_TIM5::Init();

    HAL_TIM13::Init();
}


bool HAL::IsDebugBoard()
{
    static bool first = true;
    static bool result = false;

    if (first)
    {
        first = false;

        String<> uid = GetUID();

        result = (uid == "6D0A6F3A" || uid == "140D6D00");
    }

    return result;
}


bool HAL::Is765()
{
    return !LIS2DH12::IsExist();
}


float HAL::ControlVoltage()
{
    return Is765() ? 4.0f : 8.5f;
}


String<> HAL::GetUID()
{
#define UID_BASE 0x1FFFF7AC

    uint8 bytes[12];

    std::memcpy(bytes, (void *)UID_BASE, 12); //-V566

    return String<>("%08X", Math::CalculateHash((uint)bytes, 12));
}
