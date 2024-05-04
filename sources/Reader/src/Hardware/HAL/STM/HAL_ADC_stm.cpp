// 2022/6/17 0:37:41 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Modules/Indicator/Indicator.h"
#include "Device/Device.h"
#include "Hardware/Power.h"
#include <stm32f1xx_hal.h>


namespace HAL_ADC
{
    static ADC_HandleTypeDef handle;
}


void HAL_ADC::Init()
{
    __HAL_RCC_ADC1_CLK_ENABLE();

    pinVIN.Init();

    handle.Instance = ADC1;
    handle.Init.ScanConvMode = ADC_SCAN_DISABLE;
    handle.Init.ContinuousConvMode = DISABLE;
    handle.Init.DiscontinuousConvMode = DISABLE;
    handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    handle.Init.NbrOfConversion = 1;

    HAL_ADC_Init(&handle);

    ADC_ChannelConfTypeDef sConfig =
    {
        ADC_CHANNEL_0,
        ADC_REGULAR_RANK_1,
        ADC_SAMPLETIME_71CYCLES_5
    };

    HAL_ADC_ConfigChannel(&handle, &sConfig);

    HAL_ADCEx_Calibration_Start(&handle);
}


float HAL_ADC::GetVoltage()
{
    // Время следующей проверки
    static uint time_next = 0;
    static float value = 12.0f;

    if (TIME_MS > time_next)
    {
        time_next += 1000;

        HAL_ADCEx_Calibration_Start(&handle);

        HAL_ADC_Start(&handle);

        HAL_ADC_PollForConversion(&handle, 10);

        float value_adc = (float)HAL_ADC_GetValue(&handle);

#ifdef TYPE_BOARD_771
        value = 36.3f / (float)0xFFF * value_adc;
#endif

#ifdef TYPE_BOARD_777

        value = 0.5f + 33.0f / (float)0xFFF * value_adc;
#endif

        HAL_ADC_Stop(&handle);
    }

    return value;
}


void HAL_ADC::Update()
{
    if (!Power::InEnergySavingMode())
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
