// 2024/03/26 08:24:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/GD/systick.h"
#include "Utils/Math.h"


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

//    HAL_I2C::Init();

    HAL_SPI::Init();

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


String<> HAL::GetUID()
{
#define UID_BASE 0x1FFFF7AC

    uint8 bytes[12];

    std::memcpy(bytes, (void *)UID_BASE, 12); //-V566

    return String<>("%08X", Math::CalculateHash((uint)bytes, 12));
}
