// 2024/02/11 10:45:37 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Hardware/Power.h"
#include "Modules/Indicator/Indicator.h"
#include "Device/Device.h"
#include "Modules/Player/Player.h"
#include "Modules/LIS2DH12/LIS2DH12.h"


void HAL_ADC::Init()
{
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0);

//    adc_special_function_config(ADC_CONTINUOUS_MODE, ENABLE);
    ADC_CTL1 |= ADC_CONTINUOUS_MODE;

//    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_NONE);
    ADC_CTL1 &= ~((uint32_t)ADC_CTL1_ETSRC);
    ADC_CTL1 |= (uint32_t)ADC_EXTTRIG_REGULAR_NONE;

//    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    ADC_CTL1 &= ~((uint32_t)ADC_CTL1_DAL);

//    adc_channel_length_config(ADC_REGULAR_CHANNEL, 1U);
    /* configure the length of regular channel group */
    ADC_RSQ0 &= ~((uint32_t)ADC_RSQ0_RL);
    ADC_RSQ0 |= RSQ0_RL((uint32_t)(1U - 1U));

//    adc_regular_channel_config(0U, ADC_CHANNEL_0, ADC_SAMPLETIME_55POINT5);
    uint32_t rsq, sampt;
    rsq = ADC_RSQ2;
    rsq &= ~((uint32_t)(ADC_RSQX_RSQN << (5U * 0U)));
    rsq |= ((uint32_t)ADC_CHANNEL_0 << (5U * 0U));
    ADC_RSQ2 = rsq;
    sampt = ADC_SAMPT1;
    sampt &= ~((uint32_t)(ADC_SAMPTX_SPTN << (3U * ADC_CHANNEL_0)));
    sampt |= (uint32_t)(ADC_SAMPLETIME_55POINT5 << (3U * ADC_CHANNEL_0));
    ADC_SAMPT1 = sampt;

//    adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);
        /* external trigger enable for regular channel */
//    if (RESET != (channel_group & ADC_REGULAR_CHANNEL)) {
        ADC_CTL1 |= ADC_CTL1_ETERC;
//    }
//    /* external trigger enable for inserted channel */
//    if (RESET != (channel_group & ADC_INSERTED_CHANNEL)) {
//        ADC_CTL1 |= ADC_CTL1_ETEIC;
//    }


//    adc_resolution_config(ADC_RESOLUTION_12B);
    ADC_CTL0 &= ~((uint32_t)ADC_CTL0_DRES);
    ADC_CTL0 |= (uint32_t)ADC_RESOLUTION_12B;

//    adc_enable();
//    if (RESET == (ADC_CTL1 & ADC_CTL1_ADCON)) {
        ADC_CTL1 |= (uint32_t)ADC_CTL1_ADCON;
//    }

    Timer::Delay(1);

//    adc_calibration_enable();

    /* reset the selected ADC calibration register */
    ADC_CTL1 |= (uint32_t)ADC_CTL1_RSTCLB;
    /* check the RSTCLB bit state */
    while ((ADC_CTL1 & ADC_CTL1_RSTCLB)) {
    }

    /* enable ADC calibration process */
    ADC_CTL1 |= ADC_CTL1_CLB;
    /* check the CLB bit state */
    while ((ADC_CTL1 & ADC_CTL1_CLB)) {
    }
}


void HAL_ADC::Update()
{
    if (Player::IsPlaying() || !Power::IsFailure())
    {
        return;
    }

    static uint time_last_blink = 0;        // Время последней вспышки

    if (TIME_MS > time_last_blink + 5000 && !Indicator::IsRunning())
    {
        time_last_blink = TIME_MS;

        Indicator::Blink(Color(0xFFFFFFFF, 1.0f), Color(0x00000000, 0.0f), 1, false);

        while (Indicator::IsRunning())
        {
            Device::UpdateTasks();
        }
    }
}


float HAL_ADC::GetVoltage()
{
    // Время следующей проверки
    static uint time_next = 0;
    static float value = 12.0f;

    if (TIME_MS > time_next)
    {
        time_next += 1000;

//        adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
        ADC_CTL1 |= ADC_CTL1_SWRCST;

//        while (SET != adc_flag_get(ADC_FLAG_EOC)) {}
        while ((ADC_STAT & ADC_FLAG_EOC) == 0) { }

        value = LIS2DH12::IsExist() ? (33.0f / (float)0xFFF * (float)ADC_RDATA) : ((float)ADC_RDATA * 2.0f * 3.3f / 4095.0f);
    }

    return value;
}
