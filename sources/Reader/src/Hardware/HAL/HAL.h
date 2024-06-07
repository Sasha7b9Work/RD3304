// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Modules/CLRC66303HN/CommandsCLRC663.h"
#include "Settings/Settings.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Utils/String.h"


#define PRIORITY_USART 0, 0
#define PRIORITY_SOUND 1, 0
#define PRIORITY_DMA   2, 0
#define PRIORITY_SLEEP 3, 0


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

    String<> GetUID();

    bool IsDebugBoard();

    // Если true - то 665-я плата
    bool Is765();

    // Возвращает пороговое напряжение
    float ControlVoltage();

    void ErrorHandler();

    extern bool bus_is_busy;
}


namespace HAL_ADC
{
    void Init();

    void Update();

    float GetVoltage();
}


namespace HAL_FLASH
{
    void LoadAntennaConfiguration106();

    void LoadProtocol();
}


namespace HAL_SPI
{
    void Init();

    void WriteByte(DirectionSPI::E, uint8);
    void WriteBuffer(DirectionSPI::E, const void *buffer, int size);
    void WriteRead(DirectionSPI::E, const void *out, void *in, int size);

    void Test();

#ifdef MCU_GD

#else
    extern void *handle;       // SPI_HandleTypeDef

    void WriteReadDMA(DirectionSPI::E, void *buffer_tx, void *buffer_rx, int size, void (callback_on_complete)());

#endif
}


namespace HAL_I2C
{
    void Init(); 

    bool Read(uint8 id, uint8 reg_addr, uint8 *reg_data, uint8 len);
    void Read8(uint8 id, uint8 reg_addr, uint8 *data);

    bool Write(uint8 id, uint8 reg_addr, const uint8 *reg_data, uint8 len);
    void Write8(uint8 id, uint8 reg_addr, uint8 data);
}


namespace HAL_ROM
{
    static const uint SIZE_PAGE = 1024;

    static const uint ADDRESS_BASE       = 0x08000000;
    static const uint ADDRESS_START      = 0x08001000;

#ifdef MCU_GD
    static const uint ADDRESS_SECTOR_ACCESS_CARDS = ADDRESS_BASE + 62 * 1024;
    static const uint ADDRESS_SECTOR_SETTINGS = ADDRESS_BASE + 63 * 1024;
#else
    static const uint ADDRESS_SECTOR_ACCESS_CARDS = ADDRESS_BASE + 126 * 1024;;
    static const uint ADDRESS_SECTOR_SETTINGS = ADDRESS_BASE + 127 * 1024;;
#endif


    void Init();

    void Load(SettingsMaster &);

    void Save(const SettingsMaster &);

    // Адрес первого байта страницы
    void ErasePage(uint address);

    void WriteBuffer(uint address, void *data, int size);

    void ReadBuffer(uint address, void *out, int size);

    uint8 ReadUInt8(uint address);

    uint ReadUInt(uint address);
}


namespace HAL_USART
{
    void Init();

    bool Update();

    extern void *handle;       // UART_HandleTypeDef

    // Включить/выключить фильтрацию принимаемых символов - в текстовом режиме некоторые символы отбрасываются
    void EnableFiltering(bool);

    namespace WG26
    {
        void Transmit(uint8, uint8, uint8);
    }

    namespace UART
    {
        void Transmit(const void *, int);

        void TransmitF(pchar format, ...);
    }

    namespace OSDP
    {
        void Transmit(const void *, int);

        void TransmitByte(uint8);
    }
    
    namespace Data
    {
        void Append(uint8);
    }
}


#ifdef MCU_GD

// Отсчёт микросекунд
namespace HAL_TIM5
{
    void Init();

    void Start();

    uint ElapsedUS();
}


// \todo Предполагалось, что по этому таймеру нужно подгружать звуковые буфера
namespace HAL_TIM13
{
    void Init();

    void _Start(uint ms);

    void Stop();
}


namespace HAL_I2S
{
    void Init();

    // Запуск. Передаём адрес начала буфера
    void Start(int16 *, int size);

    void Stop();
}

#endif


#ifdef __cplusplus
extern "C" {
#endif 
    void NMI_Handler(void);
    void HardFault_Handler(void);
    void MemManage_Handler(void);
    void BusFault_Handler(void);
    void UsageFault_Handler(void);
    void SVC_Handler(void);
    void DebugMon_Handler(void);
    void PendSV_Handler(void);
    void SysTick_Handler(void);
#ifdef MCU_GD
    void USART1_IRQHandler(void);
    void TIMER13_IRQHandler(void);
    void DMA_Channel1_2_IRQHandler(void);
#else
    void USART2_IRQHandler(void);
    void DMA1_Channel2_IRQHandler(void);
    void DMA1_Channel3_IRQHandler(void);
    void TIM2_IRQHandler(void);
    void TIM3_IRQHandler(void);
#endif
#ifdef __cplusplus
}
#endif
