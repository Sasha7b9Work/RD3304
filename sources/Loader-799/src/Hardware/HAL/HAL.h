// 2024/04/23 14:36:50 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct DirectionSPI
{
    enum E
    {
        Memory,         // Работа с флеш-памятью
        Reader,         // Работа с карт-ридером
        Count
    };
};


namespace HAL
{
    void Init();
}


namespace HAL_SPI
{
    void Init();

    void WriteRead(DirectionSPI::E dir, const void *out, void *in, int size);

    void WriteByte(DirectionSPI::E dir, uint8 byte);

    void WriteBuffer(DirectionSPI::E dir, const void *buffer, int size);

    void ReadBuffer(DirectionSPI::E dir, uint address, uint8 *buffer, uint size);

    uint ReadID();
}

#ifdef __cplusplus
extern "C" {
#endif 

/* function declarations */
/* this function handles NMI exception */
void NMI_Handler(void);
/* this function handles HardFault exception */
void HardFault_Handler(void);
/* this function handles SVC exception */
void SVC_Handler(void);
/* this function handles PendSV exception */
void PendSV_Handler(void);

void FMC_IRQHandler(void);

#ifdef __cplusplus
}
#endif
