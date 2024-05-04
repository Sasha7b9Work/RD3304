// 2022/6/19 6:32:00 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/Player/Player.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Modules/Indicator/Indicator.h"
#include "Nodes/OSDP/OSDP.h"
#include "Device/Device.h"
#include "Hardware/Power.h"
#include "Modules/Player/STM/Sounds_stm.h"
#include <stm32f1xx_hal.h>


/*
1 - й вариант      AF       REMAP
BEEP  14 - PA7   TIM3_CH2  TIM1_CH1N
BEEN  20 - PA8   TIM1_CH1

2 - й вариант
BEEP  15 - PB0   TIM3_CH3  TIM1_CH2N
BEEN  16 - PB1   TIM3_CH4  TIM1_CH3N
*/


namespace Player
{
#ifdef TYPE_BOARD_777
    #define CHAN_BEEN TIM_CHANNEL_2
    #define CHAN_BEEP TIM_CHANNEL_1
#else
    #define CHAN_BEEN TIM_CHANNEL_3
    #define CHAN_BEEP TIM_CHANNEL_4
#endif

    // Период в тиках
    // APB1 timers clock = 60000000Гц
    // 8000 - частота дискретизации
    // 4 - на один отсчёт столько раз вызывается прерывание
    const int period_TIM = 60000000 / 8000 / 4;

    // Этот таймер заводится для окончания импульса ШИМ
    static TIM_HandleTypeDef handleTIM3 =
    {
        TIM3,
        {
            0,
            TIM_COUNTERMODE_UP,
            (uint)(period_TIM - 1),
            TIM_CLOCKDIVISION_DIV1,
            0,
            TIM_AUTORELOAD_PRELOAD_DISABLE
        },
        HAL_TIM_ACTIVE_CHANNEL_CLEARED,
        { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr },
        HAL_UNLOCKED,
        HAL_TIM_STATE_RESET,
        { HAL_TIM_CHANNEL_STATE_RESET },
        { HAL_TIM_CHANNEL_STATE_RESET },
        HAL_DMA_BURST_STATE_RESET
    };

    static TIM_OC_InitTypeDef sConfigOC = { 0, 0, 0, 0, 0, 0, 0 };

    void *handleTIM = &handleTIM3;

    static float volume = 1.0f;                 // Уровень громкости

    static int CalculateBEEN(int sample);       // Это будет основной таймер TIM3
    static int CalculateBEEP(int sample);       // Это будет ведомый таймер TIM1

    static void ConfigCH_BEEN(int);
    static void ConfigCH_BEEP(int);

#ifdef TYPE_BOARD_777

    static TIM_HandleTypeDef handleTIM1 =
    {
        TIM1,
        {
            0,
            TIM_COUNTERMODE_UP,
            period_TIM - 1,
            TIM_CLOCKDIVISION_DIV1,
            0,
            TIM_AUTORELOAD_PRELOAD_DISABLE
        },
        HAL_TIM_ACTIVE_CHANNEL_CLEARED,
        { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr },
        HAL_UNLOCKED,
        HAL_TIM_STATE_RESET,
        { HAL_TIM_CHANNEL_STATE_RESET },
        { HAL_TIM_CHANNEL_STATE_RESET },
        HAL_DMA_BURST_STATE_RESET
    };
#endif

    static TIM_HandleTypeDef *handleBEEN = &handleTIM3;

#ifdef TYPE_BOARD_777
    static TIM_HandleTypeDef *handleBEEP = &handleTIM1;
#else
    static TIM_HandleTypeDef *handleBEEP = &handleTIM3;
#endif
}


void Player::Init()
{
    __HAL_RCC_TIM3_CLK_ENABLE();

    HAL_NVIC_SetPriority(TIM3_IRQn, PRIORITY_SOUND);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);

    TIM_ClockConfigTypeDef sClockSourceConfig =
    {
        TIM_CLOCKSOURCE_INTERNAL,
        0,
        0,
        0
    };

    TIM_MasterConfigTypeDef sMasterConfig =
    {
        TIM_TRGO_ENABLE,
        TIM_MASTERSLAVEMODE_ENABLE
    };

    if (HAL_TIM_Base_Init(&handleTIM3) != HAL_OK)
    {
        HAL::ErrorHandler();
    }

    if (HAL_TIM_ConfigClockSource(&handleTIM3, &sClockSourceConfig) != HAL_OK)
    {
        HAL::ErrorHandler();
    }

    if (HAL_TIM_PWM_Init(&handleTIM3) != HAL_OK)
    {
        HAL::ErrorHandler();
    }

    if (HAL_TIMEx_MasterConfigSynchronization(&handleTIM3, &sMasterConfig) != HAL_OK)
    {
        HAL::ErrorHandler();
    }

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    __HAL_TIM_DISABLE_OCxPRELOAD(&handleTIM3, CHAN_BEEN);
    __HAL_TIM_DISABLE_OCxPRELOAD(&handleTIM3, CHAN_BEEP);

    GPIO_InitTypeDef GPIO_InitStruct = { 0, 0, 0, 0 };
    __HAL_RCC_GPIOB_CLK_ENABLE();
#ifdef TYPE_BOARD_777
    GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8;
#else
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
#endif
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
#ifdef TYPE_BOARD_777
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#else
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif


#ifdef TYPE_BOARD_777

    __HAL_RCC_TIM1_CLK_ENABLE();

    if (HAL_TIM_Base_Init(&handleTIM1) != HAL_OK)
    {
        HAL::ErrorHandler();
    }

    if (HAL_TIM_ConfigClockSource(&handleTIM1, &sClockSourceConfig) != HAL_OK)
    {
        HAL::ErrorHandler();
    }

    if (HAL_TIM_PWM_Init(&handleTIM1) != HAL_OK)
    {
        HAL::ErrorHandler();
    }

    TIM_SlaveConfigTypeDef slave_conf =
    {
        TIM_SLAVEMODE_TRIGGER,
        TIM_TS_ITR2,
        TIM_TRIGGERPOLARITY_RISING,
        TIM_TRIGGERPRESCALER_DIV1,
        0
    };

    if (HAL_TIM_SlaveConfigSynchro(&handleTIM1, &slave_conf) != HAL_OK)
    {
        HAL::ErrorHandler();
    }

    __HAL_TIM_DISABLE_OCxPRELOAD(&handleTIM1, CHAN_BEEN);
    __HAL_TIM_DISABLE_OCxPRELOAD(&handleTIM1, CHAN_BEEP);

#endif
}


void Player::ConfigCH_BEEN(int pulse)
{
    sConfigOC.Pulse = (uint)pulse;

    if (HAL_TIM_PWM_ConfigChannel(handleBEEN, &sConfigOC, CHAN_BEEN) != HAL_OK)
    {
        HAL::ErrorHandler();
    }

    __HAL_TIM_DISABLE_OCxPRELOAD(handleBEEN, CHAN_BEEN);
}


void Player::ConfigCH_BEEP(int pulse)
{
    sConfigOC.Pulse = (uint)pulse;

    if (HAL_TIM_PWM_ConfigChannel(handleBEEP, &sConfigOC, CHAN_BEEP) != HAL_OK)
    {
        HAL::ErrorHandler();
    }

    __HAL_TIM_DISABLE_OCxPRELOAD(handleBEEP, CHAN_BEEP);
}


void Player::Play(TypeSound::E type)
{
    if (Power::InEnergySavingMode())
    {
        return;
    }

    PlayFromMemory(gset.Melody(type), gset.Volume(type));
}


void Player::PlayFromMemory(uint8 num_sound, uint8 _volume)
{
    if (IsPlaying())
    {
        return;
    }

    if (_volume == 0)       { volume = 0.0f;        }
    else if (_volume == 1)  { volume = 1.0f / 3.0f; }
    else if (_volume == 2)  { volume = 2.0f / 3.0f; }
    else                    { volume = 1.0f;        }

    if (HAL::IsDebugBoard())
    {
        volume *= 0.05f;
    }

    ConfigCH_BEEN(0);

    ConfigCH_BEEP(0);

    if (HAL_TIM_PWM_Start(handleBEEN, CHAN_BEEN) != HAL_OK)
    {
        HAL::ErrorHandler();
    }

    if (HAL_TIM_PWM_Start(handleBEEP, CHAN_BEEP) != HAL_OK)
    {
        HAL::ErrorHandler();
    }

    Sound::Start(num_sound);

    HAL_TIM_Base_Start_IT(handleBEEN);
}


void Player::Stop()
{
    HAL_TIM_PWM_Stop(handleBEEN, CHAN_BEEN);

    HAL_TIM_PWM_Stop(handleBEEP, CHAN_BEEP);

    HAL_TIM_Base_Stop_IT(&handleTIM3);

    Sound::Stop();
}


int Player::CalculateBEEN(int sample)
{
    float result = 0.0f;

    if (sample < 0)
    {
        result = -(float)sample / 2047.0f * (float)period_TIM;
    }

    return (int)(result * volume);
}


int Player::CalculateBEEP(int sample)
{
    float result = 0.0f;

    if (sample > 0)
    {
        result = (float)sample / 2047.0f * (float)period_TIM;
    }

    return (int)(result * volume);
}


void Player::CallbackOnTimer()
{
    static int8 tick = 0;
    static int16 sample = 0;
    static uint been = 0;
    static uint beep = 0;

    if (Sound::IsPlaying())
    {
        if (tick == 0)
        {
            sample = Sound::NextSample();
        }
        else if (tick == 1)
        {
            been = (uint)CalculateBEEN(sample);
        }
        else if (tick == 2)
        {
            beep = (uint)CalculateBEEP(sample);
        }
        else
        {
#ifdef TYPE_BOARD_777
            TIM3->CCR2 = been;
            TIM1->CCR1 = beep;
#else
            TIM3->CCR3 = been;
            TIM3->CCR4 = beep;
#endif
        }

        tick = (tick + 1) & 0x03;       // if(tick == 4) tick = 0
    }
    else
    {
        tick = 0;

        Stop();
    }
}


bool Player::Update()
{
    if (!ModeOffline::IsEnabled() && Device::IsRunning() && !OSDP::IsEnabled())
    {
        if (pinSND.IsLow())
        {
            Player::Play(TypeSound::Beep);
        }
        else
        {
            static bool prev_lr = false;    // Предыдущие
            static bool prev_lg = false;    // состояния

            bool lr = pinLR.IsLow();
            bool lg = pinLG.IsLow();

            if (lr != prev_lr || lg != prev_lg)
            {
                if (lg && (lg != prev_lg))
                {
                    Player::Play(TypeSound::Green);
                }
                else if (lr && (lr != prev_lr))
                {
                    Player::Play(TypeSound::Red);
                }

                prev_lr = lr;
                prev_lg = lg;
            }
        }
    }

    return Sound::Update();
}


bool Player::IsPlaying()
{
    return Sound::IsPlaying();
}
