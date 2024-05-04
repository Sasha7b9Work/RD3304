// 2022/6/30 23:43:06 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Power
{
    void Init();

    void EnterSleepMode();

    bool InEnergySavingMode();

    extern void *handleTIM;   // TIM_HandleTypeDef
}
