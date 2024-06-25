// 2022/6/10 9:08:02 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Power.h"
#include "Modules/Player/Player.h"
#include <stm32f1xx_hal.h>


#ifdef __cplusplus
extern "C" {
#endif


void NMI_Handler(void)
{
}


__attribute__((noreturn)) void HardFault_Handler(void)
{
//    static volatile pchar file = nullptr;
//    static volatile int line = 0;
//    static volatile int index = 0;

    while (true)
    {
//        index = index;
//        file = file;
//        line = line;
    }
}


__attribute__((noreturn)) void MemManage_Handler(void)
{
    while (1)
    {
    }
}
    

__attribute__((noreturn)) void BusFault_Handler(void)
{
    while (1)
    {
    }
}


__attribute__((noreturn)) void UsageFault_Handler(void)
{
    while (1)
    {
    }
}


void SVC_Handler(void)
{

}


void DebugMon_Handler(void)
{

}


void PendSV_Handler(void)
{

}


void USART2_IRQHandler(void)
{
    HAL_UART_IRQHandler((UART_HandleTypeDef *)HAL_USART::handle);
}


void SysTick_Handler(void)
{
    HAL_IncTick();
}


void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler((TIM_HandleTypeDef *)Power::handleTIM);
}


void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler((TIM_HandleTypeDef *)Player::handleTIM);
}


// SPI1 RX
void DMA1_Channel2_IRQHandler(void)
{
    SPI_HandleTypeDef *handle = (SPI_HandleTypeDef *)HAL_SPI::handle;

    HAL_DMA_IRQHandler(handle->hdmarx);
}


// SPI1 TX
void DMA1_Channel3_IRQHandler(void)
{
    SPI_HandleTypeDef *handle = (SPI_HandleTypeDef *)HAL_SPI::handle;

    HAL_DMA_IRQHandler(handle->hdmatx);
}


#ifdef __cplusplus
}
#endif
