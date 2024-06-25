// 2022/6/10 9:12:45 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <stm32f1xx_hal.h>


namespace HAL_SPI
{
    namespace CS
    {
        static void Hi(DirectionSPI::E dir)
        {
            (dir == DirectionSPI::Memory) ? pinSRAM.ToHi() : pinSTRX.ToHi();
        }

        static void Low(DirectionSPI::E dir)
        {
            (dir == DirectionSPI::Memory) ? pinSRAM.ToLow() : pinSTRX.ToLow();
        }

        static void Init()
        {
            pinSTRX.Init();
            pinSRAM.Init();

            Hi(DirectionSPI::Memory);
            Hi(DirectionSPI::Reader);
        }
    }

    static void (*callbackOnTransfer)() = nullptr;

    static DirectionSPI::E dirDMA = DirectionSPI::Count;

    static DMA_HandleTypeDef hDMA_TX =
    {
        DMA1_Channel3,
        {
            DMA_MEMORY_TO_PERIPH,
            DMA_PINC_DISABLE,
            DMA_MINC_ENABLE,
            DMA_PDATAALIGN_BYTE,
            DMA_MDATAALIGN_BYTE,
            DMA_NORMAL,
            DMA_PRIORITY_LOW
        },
        HAL_UNLOCKED,
        HAL_DMA_STATE_RESET,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        0,
        nullptr,
        0
    };

    static DMA_HandleTypeDef hDMA_RX =
    {
        DMA1_Channel2,
        {
            DMA_PERIPH_TO_MEMORY,
            DMA_PINC_DISABLE,
            DMA_MINC_ENABLE,
            DMA_PDATAALIGN_BYTE,
            DMA_MDATAALIGN_BYTE,
            DMA_NORMAL,
            DMA_PRIORITY_LOW
        },
        HAL_UNLOCKED,
        HAL_DMA_STATE_RESET,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        0,
        nullptr,
        0
    };

#ifdef __cplusplus
    extern "C" {
#endif

        void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *)
        {
            CS::Hi(dirDMA);

            HAL_NVIC_DisableIRQ(DMA1_Channel2_IRQn);

            HAL_NVIC_DisableIRQ(DMA1_Channel3_IRQn);

            HAL_DMA_DeInit(&hDMA_TX);

            HAL_DMA_DeInit(&hDMA_RX);

            callbackOnTransfer();
        }

#ifdef __cplusplus
    }
#endif

    static SPI_HandleTypeDef handleSPI1;

    void *handle = &handleSPI1;
}


void HAL_SPI::Init()
{
    __HAL_RCC_SPI1_CLK_ENABLE();

    CS::Init();

    pinSCK.Init();
    pinMOSI.Init();
    pinMISO.Init();

    __HAL_AFIO_REMAP_SPI1_ENABLE();

    handleSPI1.Instance = SPI1;
    handleSPI1.Init.Mode = SPI_MODE_MASTER;
    handleSPI1.Init.Direction = SPI_DIRECTION_2LINES;
    handleSPI1.Init.DataSize = SPI_DATASIZE_8BIT;
    handleSPI1.Init.CLKPolarity = SPI_POLARITY_HIGH;
    handleSPI1.Init.CLKPhase = SPI_PHASE_2EDGE;
    handleSPI1.Init.NSS = SPI_NSS_SOFT;
    handleSPI1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    handleSPI1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    handleSPI1.Init.TIMode = SPI_TIMODE_DISABLE;
    handleSPI1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    handleSPI1.Init.CRCPolynomial = 10;

    HAL_SPI_Init(&handleSPI1);
}


void HAL_SPI::WriteBuffer(DirectionSPI::E dir, const void *buffer, int size)
{
    CS::Low(dir);

    HAL_SPI_Transmit(&handleSPI1, (uint8 *)buffer, (uint16)size, 100);

    CS::Hi(dir);
}


void HAL_SPI::WriteByte(DirectionSPI::E dir, uint8 byte)
{
    CS::Low(dir);

    HAL_SPI_Transmit(&handleSPI1, &byte, 1, 100);

    CS::Hi(dir);
}


void HAL_SPI::WriteRead(DirectionSPI::E dir, const void *out, void *in, int size)
{
    CS::Low(dir);

    HAL_SPI_TransmitReceive(&handleSPI1, (uint8 *)out, (uint8 *)in, (uint16)size, 100);

    CS::Hi(dir);
}


void HAL_SPI::WriteReadDMA(DirectionSPI::E dir, void *buffer_tx, void *buffer_rx, int size, void (callback_on_complete)())
{
    dirDMA = dir;

    callbackOnTransfer = callback_on_complete;

    // SPI1 RX - Channel 2
    // SPI1 TX - Channel 3

    HAL_DMA_Init(&hDMA_TX);

    HAL_DMA_Init(&hDMA_RX);

    __HAL_LINKDMA(&handleSPI1, hdmatx, hDMA_TX);

    __HAL_LINKDMA(&handleSPI1, hdmarx, hDMA_RX);

    HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, PRIORITY_DMA);

    HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, PRIORITY_DMA);

    HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);

    HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

    CS::Low(dir);

    HAL_SPI_TransmitReceive_DMA(&handleSPI1, (uint8 *)buffer_tx, (uint8 *)buffer_rx, (uint16)size);
}
