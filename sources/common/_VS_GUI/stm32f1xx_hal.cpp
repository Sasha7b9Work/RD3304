// 2024/01/13 00:17:16 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "stm32f1xx_hal.h"


GPIO_TypeDef sGPIOA;
GPIO_TypeDef sGPIOB;
GPIO_TypeDef sGPIOD;


void HAL_GPIO_WritePin(GPIO_TypeDef * /*GPIOx*/, uint16 /*GPIO_Pin*/, uint /*PinState*/)
{

}

void HAL_GPIO_Init(GPIO_TypeDef * /*GPIOx*/, GPIO_InitTypeDef * /*GPIO_Init*/)
{

}

uint HAL_GPIO_ReadPin(GPIO_TypeDef * /*GPIOx*/, uint16 /*GPIO_Pin*/)
{
    return 0;
}
