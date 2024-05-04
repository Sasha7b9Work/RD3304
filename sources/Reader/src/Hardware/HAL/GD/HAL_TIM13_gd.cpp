// 2024/04/05 14:04:37 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <gd32e23x.h>


void HAL_TIM13::Init()
{
}


void HAL_TIM13::Start(uint ms)
{
//    timer_deinit(TIMER13);

//    is.prescaler = (uint16)(SystemCoreClock / 1000 * ms / 2 - 1);
//    is.alignedmode = TIMER_COUNTER_EDGE;
//    is.counterdirection = TIMER_COUNTER_UP;
//    is.period = 1;
//    is.clockdivision = TIMER_CKDIV_DIV1;
//
//    if (ms > 1 && ms <= 10)
//    {
//        is.prescaler = (uint16)(SystemCoreClock / 100 * ms / 2 - 1);
//        is.period = 9;
//    }
//    else if (ms > 10 && ms <= 100)
//    {
//        is.prescaler = (uint16)(SystemCoreClock / 10 * ms / 2 - 1);
//        is.period = 99;
//    }
//    else
//    {
//        is.prescaler = (uint16)(SystemCoreClock / 1 * ms / 2 - 1);
//        is.period = 999;
//    }
//
//    timer_init(TIMER13, &is);

    uint16 prescaler = (uint16)(SystemCoreClock / 100 * ms / 2 - 1);
    uint period = 9;

    if (ms > 100)
    {
        prescaler = (uint16)(SystemCoreClock / 1 * ms / 2 - 1);
        period = 999;
    }
    else if (ms > 10)
    {
        prescaler = (uint16)(SystemCoreClock / 10 * ms / 2 - 1);
        period = 99;
    }

    TIMER_PSC(TIMER13) = prescaler;
    TIMER_CAR(TIMER13) = period;
    TIMER_CTL0(TIMER13) &= ~(uint)TIMER_CTL0_CKDIV;
    TIMER_CTL0(TIMER13) |= (uint)TIMER_CKDIV_DIV1;
    TIMER_SWEVG(TIMER13) |= (uint)TIMER_SWEVG_UPG;

//    timer_interrupt_flag_clear(TIMER13, TIMER_INT_FLAG_UP);
    TIMER_INTF(TIMER13) = (~(uint)TIMER_INT_FLAG_UP);

//    timer_interrupt_enable(TIMER13, TIMER_INT_UP);
    TIMER_DMAINTEN(TIMER13) |= (uint)TIMER_INT_UP;

//    timer_enable(TIMER13);
    TIMER_CTL0(TIMER13) |= (uint)TIMER_CTL0_CEN;
}


void HAL_TIM13::Stop()
{
//    timer_disable(TIMER13);
    TIMER_CTL0(TIMER13) &= ~(uint)TIMER_CTL0_CEN;

//    timer_interrupt_disable(TIMER13, TIMER_INT_UP);
    TIMER_DMAINTEN(TIMER13) &= (~(uint)TIMER_INT_UP);
}
