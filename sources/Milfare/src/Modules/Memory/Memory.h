// 2022/08/05 20:41:56 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


/*
*   Флеш-память
*/


template<int capacity>
struct MemoryBuffer
{
    uint8 GetCapacity() const { return capacity; }
    uint8 buffer[capacity];
};


namespace Memory
{
    const int SIZE_BLOCK = 256;

    int SizeBlock();

    uint AddressBlock(int num_block);

    void WriteBuffer(uint address, const void *buffer, int size);

    void ReadBuffer(uint address, void *buffer, int size);

    // Перед buffer должно быть пять байт, которые будут испорчены этой процедурой. После отработки
    // их нужно восстановить в случае необходимости
    void ReadBufferFast(uint address, void *buffer, int size);

#ifdef MCU_GD
#else
    // !!! ВНИМАНИЕ !!! Функция портит 5 байт перед buffer. Вызывающая фукнция должна их сохранять перед вызовом
    // и восстанавливать в callback_on_complete
    void ReadBufferFastDMA(uint address, void *buffer, int size, void (*callback_on_complete)());
#endif

    int WriteBufferRelible512(uint address, const void *buffer, int size, int number_attempts = 10);

    void WriteInt16(uint address, int16 value);

    int16 ReadInt16(uint address);

    void WriteInt(uint address, int value);

    int ReadInt(uint address);

    // Сколько байт в наличии
    int Capasity();

    int GetBlocksCount();

    namespace Erase
    {
        // Стирает блок размером 4к
        void Block(int num_block);

        void Full();

        void Firmware();

        // Стереть, если что-то записано
        void FirmwareIfNeed();

        void Sounds();
    }

    namespace Test
    {
        bool Run();

        // Тест скорости записи. Возвращает количество байт в секунду
        float SpeedWrite();

        // Тест скорости чтения. Возвращает количество байт в секунду
        float SpeedRead();
    }
}
