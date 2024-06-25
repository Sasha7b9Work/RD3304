// 2022/08/05 20:41:56 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


/*
*   ����-������
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

    // ����� buffer ������ ���� ���� ����, ������� ����� ��������� ���� ����������. ����� ���������
    // �� ����� ������������ � ������ �������������
    void ReadBufferFast(uint address, void *buffer, int size);

#ifdef MCU_GD
#else
    // !!! �������� !!! ������� ������ 5 ���� ����� buffer. ���������� ������� ������ �� ��������� ����� �������
    // � ��������������� � callback_on_complete
    void ReadBufferFastDMA(uint address, void *buffer, int size, void (*callback_on_complete)());
#endif

    int WriteBufferRelible512(uint address, const void *buffer, int size, int number_attempts = 10);

    void WriteInt16(uint address, int16 value);

    int16 ReadInt16(uint address);

    void WriteInt(uint address, int value);

    int ReadInt(uint address);

    // ������� ���� � �������
    int Capasity();

    int GetBlocksCount();

    namespace Erase
    {
        // ������� ���� �������� 4�
        void Block(int num_block);

        void Full();

        void Firmware();

        // �������, ���� ���-�� ��������
        void FirmwareIfNeed();

        void Sounds();
    }

    namespace Test
    {
        bool Run();

        // ���� �������� ������. ���������� ���������� ���� � �������
        float SpeedWrite();

        // ���� �������� ������. ���������� ���������� ���� � �������
        float SpeedRead();
    }
}
