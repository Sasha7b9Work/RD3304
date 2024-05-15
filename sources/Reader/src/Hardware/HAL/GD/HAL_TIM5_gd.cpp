// 2024/04/03 22:36:57 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include <gd32e23x.h>


void HAL_TIM5::Init()
{
//    timer_deinit(TIMER5);

    /*
    timer_parameter_struct is;
    timer_struct_para_init(&is);

    is.prescaler = (uint16)(SystemCoreClock / 1000000 * 1 / 2 - 1);     // 1 ìêñ
    is.alignedmode = TIMER_COUNTER_EDGE;
    is.counterdirection = TIMER_COUNTER_UP;
    is.period = 65535U;
    is.clockdivision = TIMER_CKDIV_DIV1;
    timer_init(TIMER5, &is);
    */

    TIMER_PSC(TIMER5) = (uint16)(SystemCoreClock / 500000 * 1 / 2 - 1);     // 1 ìêñ
    TIMER_CAR(TIMER5) = (uint)65535U;
    TIMER_SWEVG(TIMER5) |= (uint)TIMER_SWEVG_UPG;
}


void HAL_TIM5::Start()
{
//    timer_disable(TIMER5);
    TIMER_CTL0(TIMER5) &= ~(uint)TIMER_CTL0_CEN;

//    timer_counter_value_config(TIMER5, 0);
    TIMER_CNT(TIMER5) = (uint)0;

//    timer_enable(TIMER5);
    TIMER_CTL0(TIMER5) |= (uint)TIMER_CTL0_CEN;
}


uint HAL_TIM5::ElapsedUS()
{
//    return timer_counter_read(TIMER5);
    return TIMER_CNT(TIMER5);
}


void HAL_TIM5::Test()
{
    Start();

    static const uint dT_ms = 5;

    volatile uint end_time = TIME_MS + dT_ms;

//    int counter = 0;
    while (TIME_MS < end_time)
    {
//        HAL_USART::UART::TransmitF("wait");
//        counter++;
    }

    HAL_USART::UART::TransmitF("%u ms = %u us", dT_ms, ElapsedUS());
}
