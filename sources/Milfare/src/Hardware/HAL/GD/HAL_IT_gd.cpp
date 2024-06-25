// 2024/02/28 15:46:34 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "system.h"
#include "systick.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"


#ifdef __cplusplus
extern "C" {
#endif


void NMI_Handler(void)
{
}


__attribute__((noreturn)) void HardFault_Handler(void)
{
    int i = 0;
    /* if Hard Fault exception occurs, go to infinite loop */
    while (1) //-V776
    {
        i++;
    }
}


__attribute__((noreturn)) void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while (1) {
    }
}


__attribute__((noreturn)) void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while (1) {
    }
}


__attribute__((noreturn)) void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while (1) {
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


void SysTick_Handler(void)
{
    delay_decrement();

    Timer::counter_ms++;
}


void TIMER13_IRQHandler(void)
{
    if (TIMER_DMAINTEN(TIMER13 & TIMER_INT_FLAG_UP) == RESET)
    {
        return;
    }

    if ((TIMER_INTF(TIMER13) & TIMER_INT_FLAG_UP) != RESET)
    {
//        timer_interrupt_flag_clear(TIMER13, TIMER_INT_FLAG_UP);
        TIMER_INTF(TIMER13) = (~(uint32_t)TIMER_INT_FLAG_UP);
    }
}


// Channel2 - I2S0_TX
void DMA_Channel1_2_IRQHandler(void)
{
    if (RESET != dma_interrupt_flag_get(DMA_CH2, DMA_INT_FLAG_HTF))
    {
        dma_interrupt_flag_clear(DMA_CH2, DMA_INT_FLAG_G);
    }

    if (RESET != dma_interrupt_flag_get(DMA_CH2, DMA_INT_FLAG_FTF))
    {
        dma_interrupt_flag_clear(DMA_CH2, DMA_INT_FLAG_G);
    }
}


#ifdef __cplusplus
}
#endif
