// 2022/6/10 9:12:45 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <gd32e23x.h>


namespace HAL_SPI
{
    // ѕосле окончани€ операции по DMA нужно поднимать в верхий уровень данное направление
    static DirectionSPI::E dirDMA = DirectionSPI::Count;

    static void (*callbackOnTransfer)() = nullptr;

    namespace CS
    {
        static uint PORT_CS = GPIOA;
        static const uint16 PIN_CS_MEMORY = GPIO_PIN_12;
        static const uint16 PIN_CS_READER = GPIO_PIN_15;

        static const uint16 pins[DirectionSPI::Count] = { PIN_CS_MEMORY, PIN_CS_READER };

        static void ToHi(DirectionSPI::E dir)
        {
//            gpio_bit_set(PORT_CS, pins[dir]);
            GPIO_BOP(PORT_CS) = (uint32_t)pins[dir];
        }

        static void ToLow(DirectionSPI::E dir)
        {
//            gpio_bit_reset(PORT_CS, pins[dir]);
            GPIO_BC(PORT_CS) = (uint32_t)pins[dir];
        }

        static void Init()
        {
            gpio_mode_set(PORT_CS, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, PIN_CS_MEMORY);
            gpio_output_options_set(PORT_CS, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, PIN_CS_MEMORY);

            gpio_mode_set(PORT_CS, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, PIN_CS_READER);
            gpio_output_options_set(PORT_CS, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, PIN_CS_READER);

            ToHi(DirectionSPI::Memory);
            ToHi(DirectionSPI::Reader);
        }
    }
}


void HAL_SPI::Init()
{
    rcu_periph_clock_enable(RCU_SPI0);

    //                                SCK         MISO         MOSI
    gpio_af_set(GPIOB, GPIO_AF_0, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);

    CS::Init();

    spi_parameter_struct spi_struct;

    spi_i2s_deinit(SPI0);

    spi_struct_para_init(&spi_struct);

    spi_struct.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    spi_struct.device_mode = SPI_MASTER;
    spi_struct.frame_size = SPI_FRAMESIZE_8BIT;
    spi_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_struct.nss = SPI_NSS_SOFT;
    spi_struct.prescale = SPI_PSC_64;
    spi_struct.endian = SPI_ENDIAN_MSB;

    spi_init(SPI0, &spi_struct);

    spi_enable(SPI0);
}


void HAL_SPI::WriteBuffer(DirectionSPI::E dir, const void *buffer, int size)
{
    CS::ToLow(dir);

    const uint8 *data = (const uint8 *)buffer;
    spi_i2s_data_receive(SPI0);

    while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE)) { }

    for (int i = 0; i < size; i++)
    {
        spi_i2s_data_transmit(SPI0, *data++);

        while (RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE)) {}

        spi_i2s_data_receive(SPI0);
    }

    CS::ToHi(dir);
}


void HAL_SPI::WriteByte(DirectionSPI::E dir, uint8 byte)
{
    CS::ToLow(dir);

    spi_i2s_data_receive(SPI0);

    while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE)) { }

    spi_i2s_data_transmit(SPI0, byte);

    while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE)) { }

    spi_i2s_data_receive(SPI0);

    CS::ToHi(dir);
}


void HAL_SPI::WriteRead(DirectionSPI::E dir, const void *out, void *in, int size)
{
    CS::ToLow(dir);

    const uint8 *d_out = (const uint8 *)out;
    uint8 *d_in = (uint8 *)in;

    *d_in = (uint8)spi_i2s_data_receive(SPI0);

    while (RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE)) {}

    for (int i = 0; i < size; i++)
    {
        spi_i2s_data_transmit(SPI0, *d_out++);

        while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE)) { }

        *d_in++ = (uint8)spi_i2s_data_receive(SPI0);
    }

    CS::ToHi(dir);
}


void HAL_SPI::WriteReadDMA(DirectionSPI::E dir, void *buffer_tx, void *buffer_rx, int size, void (callback_on_complete)())
{
    dirDMA = dir;

    callbackOnTransfer = callback_on_complete;

    // Write

    dma_deinit(DMA_CH2);

    dma_parameter_struct is =
    {
        (uint)&SPI_DATA(SPI0),          // periph_addr
        DMA_PERIPHERAL_WIDTH_8BIT,      // periph_width;    transfer data size of peripheral
        (uint)buffer_tx,                // memory_addr;     memory base address
        DMA_MEMORY_WIDTH_8BIT,          // memory_width;    transfer data size of memory
        (uint)size,                     // number;          channel transfer number
        DMA_PRIORITY_LOW,               // priority;        channel priority level
        DMA_PERIPH_INCREASE_DISABLE,    // periph_inc;      peripheral increasing mode
        DMA_MEMORY_INCREASE_ENABLE,     // memory_inc;      memory increasing mode
        DMA_MEMORY_TO_PERIPHERAL        // direction;       channel data transfer direction
    };

    dma_init(DMA_CH2, &is);
    dma_circulation_disable(DMA_CH2);
    dma_memory_to_memory_disable(DMA_CH2);

    // Read

    dma_deinit(DMA_CH1);

    is.memory_addr = (uint)buffer_rx;
    is.direction = DMA_PERIPHERAL_TO_MEMORY;

    dma_init(DMA_CH1, &is);
    dma_circulation_disable(DMA_CH1);
    dma_memory_to_memory_disable(DMA_CH2);

    dma_channel_enable(DMA_CH2);
    dma_channel_enable(DMA_CH1);

    CS::ToLow(dir);

    spi_dma_enable(SPI0, SPI_DMA_TRANSMIT);
    spi_dma_enable(SPI0, SPI_DMA_RECEIVE);

    dma_interrupt_enable(DMA_CH1, DMA_INT_FTF);         // ѕосле приЄма всех байт сработает прерывание
}


void HAL_SPI::CallbackOnCompleteDMA()
{
    dma_interrupt_disable(DMA_CH1, DMA_INT_FTF);

    CS::ToHi(dirDMA);

    callbackOnTransfer();
}
