// 2024/04/23 14:37:09 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "config.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/systick.h"
#include "Hardware/hrd_Iap.h"
#include <gd32e23x.h>


void HAL::Init()
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOF);

    HAL_SPI::Init();
}
