// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Hardware/Timer.h"
#include "Modules/Indicator/Indicator.h"
#include "Utils/Math.h"
#include "system.h"
#include <cstring>
#include <cstdio>

/*
    TIM1  Beeper
    TIM2  Спящий режим
    TIM3  Beeper
*/

/*
    ***Разделяемые ресурсы ***
    SPI1 и I2C1 в данном контроллере не могут использоваться одновременно из - за бага МК.
    SPI1 используют :
        -CLRC6630
        - Микросхема памяти
    I2C1 используют :
        -Индикатор LP5012 в плате 771
        - LIS2DH12
*/


namespace HAL
{
    bool bus_is_busy = false;

    static void SystemClock_Config();
}


float HAL::ControlVoltage()
{
    return 8.5f;
}


void HAL::Init()
{
    HAL_Init();

    SystemClock_Config();

    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

    __HAL_AFIO_REMAP_SWJ_NOJTAG();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    __HAL_RCC_DMA1_CLK_ENABLE();        // Понадобится для чтения памяти

    pinLG.Init();
    pinLR.Init();
    pinSND.Init();

    HAL_ADC::Init();

    HAL_I2C::Init();

    HAL_SPI::Init();
}


void HAL::SystemClock_Config()
{
    RCC_OscInitTypeDef RCC_OscInitStruct =
    {
        RCC_OSCILLATORTYPE_HSI,     // OscillatorType
        0,                          // HSEState
        0,                          // HSEPredivValue
        0,                          // LSEState
        RCC_HSI_ON,                 // HSIState
        RCC_HSICALIBRATION_DEFAULT, // HSICalibrationValue
        0,                          // LSIState
        {
            RCC_PLL_ON,             // PLLState
            RCC_PLLSOURCE_HSI_DIV2, // PLLSource
            RCC_PLL_MUL15           // PLLMUL
        }
    };

    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitTypeDef RCC_ClkInitStruct =
    {
        RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2,      // ClockType
        RCC_SYSCLKSOURCE_PLLCLK,    // SYSCLKSource
        RCC_SYSCLK_DIV1,            // AHBCLKDivider
        RCC_HCLK_DIV2,              // APB1CLKDivider
        RCC_HCLK_DIV1               // APB2CLKDivider
    };

    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

    RCC_PeriphCLKInitTypeDef PeriphClkInit =
    {
        RCC_PERIPHCLK_ADC,  // PeriphClockSelection
        0,                  // RTCClockSelection
        RCC_ADCPCLK2_DIV6,  // AdcClockSelection
        0                   // UsbClockSelection
    };

    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
}


String<> HAL::GetUID()
{
    uint8 bytes[12];

    std::memcpy(bytes, (void *)UID_BASE, 12); //-V566

    return String<>("%08X", Math::CalculateHash((uint)bytes, 12));
}


bool HAL::IsDebugBoard()
{
    if (GetUID() == "1FFC3AE3")
    {
        return true;
    }

    return false;
}


void HAL::ErrorHandler()
{

}
