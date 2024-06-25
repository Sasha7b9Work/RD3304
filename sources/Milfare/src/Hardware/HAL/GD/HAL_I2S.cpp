// 2024/03/26 15:34:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Modules/Player/Player.h"
#include <gd32e23x.h>


namespace HAL_I2S
{
    /*
    *   DMA : I2S0_TX : Channel2
    */
    static PinOutputPP pinSD(Port::_A, GPIO_PIN_8, Pulling::None);
}


void HAL_I2S::Init()
{
    pinSD.Init();

    pinSD.ToLow();

    // configure I2S0 GPIO: I2S0_WS/PA4, I2S0_CK/PA5, I2S0_SD/PA7
    gpio_af_set(GPIOA, GPIO_AF_0, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7);

//    spi_i2s_deinit(SPI0);
    rcu_periph_reset_enable(RCU_SPI0RST);
    rcu_periph_reset_disable(RCU_SPI0RST);
    i2s_init(SPI0, I2S_MODE_MASTERTX, I2S_STD_PHILLIPS, I2S_CKPL_HIGH);
    i2s_psc_config(SPI0, I2S_AUDIOSAMPLE_32K, I2S_FRAMEFORMAT_DT16B_CH16B, I2S_MCKOUT_DISABLE);

    dma_parameter_struct is;
    dma_struct_para_init(&is);

    dma_deinit(DMA_CH2);

    dma_deinit(DMA_CH2);
    is.periph_addr = (uint)&SPI_DATA(SPI0);
    is.memory_addr = (uint)0;
    is.direction = DMA_MEMORY_TO_PERIPHERAL;
    is.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    is.memory_width = DMA_MEMORY_WIDTH_16BIT;
    is.priority = DMA_PRIORITY_LOW;
    is.number = 1024;
    is.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    is.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init(DMA_CH2, &is);
}


void HAL_I2S::Start(int16 *buffer, int size)
{
    pinSD.ToHi();

    DMA_CHMADDR(DMA_CH2) = (uint)buffer;

    DMA_CHCNT(DMA_CH2) = (uint)(size) & DMA_CHANNEL_CNT_MASK;

    dma_channel_enable(DMA_CH2);

    dma_circulation_enable(DMA_CH2);

    dma_interrupt_enable(DMA_CH2, DMA_INT_HTF);
    dma_interrupt_enable(DMA_CH2, DMA_INT_FTF);

    spi_dma_enable(SPI0, SPI_DMA_TRANSMIT);

    i2s_enable(SPI0);
}


void HAL_I2S::Stop()
{
    pinSD.ToLow();

    dma_interrupt_disable(DMA_CH2, DMA_INT_HTF);
    dma_interrupt_disable(DMA_CH2, DMA_INT_FTF);

    i2s_disable(SPI0);

    spi_dma_disable(SPI0, SPI_DMA_TRANSMIT);

    dma_channel_disable(DMA_CH2);
}
