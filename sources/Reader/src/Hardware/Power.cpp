// 2022/6/30 23:43:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/Power.h"
#include "Modules/Player/Player.h"
#include "Modules/Player/Player.h"
#include "system.h"


namespace Power
{
#ifdef MCU_GD
#else
    static TIM_HandleTypeDef handle =
    {
        TIM2,
        {
            (uint)(60000 / 17 - 1),         // prescaler, 50мс = 20Гц
            TIM_COUNTERMODE_UP,
            999,                            // period
            TIM_CLOCKDIVISION_DIV1,
            0,                              // RepetitionCounter
            TIM_AUTORELOAD_PRELOAD_DISABLE  // AutoReloadPreload
        },
        HAL_TIM_ACTIVE_CHANNEL_CLEARED,
        { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr },
        HAL_UNLOCKED,
        HAL_TIM_STATE_RESET,
        { HAL_TIM_CHANNEL_STATE_RESET },
        { HAL_TIM_CHANNEL_STATE_RESET },
        HAL_DMA_BURST_STATE_RESET
    };

    void *handleTIM = &handle;
#endif
}


void Power::Init()
{
#ifdef MCU_GD
#else
    __HAL_RCC_TIM2_CLK_ENABLE();

    HAL_NVIC_SetPriority(TIM2_IRQn, PRIORITY_SLEEP);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);

    HAL_TIM_Base_Init(&handle);

    HAL_TIM_Base_Start_IT(&handle);
#endif
}


void Power::EnterSleepMode()
{
#ifdef MCU_GD
#else
    HAL_SuspendTick();

    HAL_PWR_DisableSleepOnExit();

    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);   // Здесь процессор останавливается

    HAL_ResumeTick();                                                   // А здесь возобнавляет работу после любого прерывания
#endif
}


bool Power::InEnergySavingMode()
{
    return HAL_ADC::GetVoltage() < HAL::ControlVoltage();
}

#ifdef MCU_GD
#else
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) //-V2009
{
    if (htim == (TIM_HandleTypeDef *)Player::handleTIM)
    {
        Player::CallbackOnTimer();
    }
}
#endif
