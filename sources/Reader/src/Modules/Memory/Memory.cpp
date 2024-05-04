// 2022/08/05 20:41:49 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/Memory/Memory.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Utils/Buffer.h"
#include <cstdlib>
#include <cstring>


//                             page
#define PROGRAM_PAGE     0x02  /* 14 */
#define WRITE_DISABLE    0x04  /* 18 */
#define READ_STATUS_1    0x05  /* 22 */
#define WRITE_ENABLE     0x06  /* 17 */
#define READ_ARRAY       0x0B  /*  8 */
#define BLOCK_ERASE_4KB  0x20  /* 16 */
#define SECTOR_EASE_64KB 0xD8  /* 16 */


namespace Memory
{
    struct Page256
    {
        // В конструктор передаётся любой адрес, принадлежащий странице
        Page256(uint address)
        {
            while (address % SIZE_BLOCK)
            {
                address--;
            }

            start = address;
        }

        uint Begin() const
        {
            return start;
        }

        uint End() const
        {
            return Begin() + SIZE_BLOCK;
        }

    public:
        uint start;
    };

    namespace MEM_SPI
    {
        static void WaitRelease()
        {
            static const uint8 out[2] = { READ_STATUS_1, 0 };
            uint8 in[2];

            TimeMeterMS meter;

            while (meter.ElapsedMS() < 1000)
            {
                HAL_SPI::WriteRead(DirectionSPI::Memory, out, in, 2);

                if ((in[1] & 0x01) == 0)
                {
                    return;
                }
            }
            
            return;
        }

        static void WriteBuffer(uint8 *buffer, int size)
        {
            WaitRelease();

            HAL_SPI::WriteByte(DirectionSPI::Memory, WRITE_ENABLE);

            WaitRelease();

            HAL_SPI::WriteBuffer(DirectionSPI::Memory, buffer, size);
        }
    }

    // Записывает uint8, а затем младшие 3 байта из второго значения
    static void Write32bit(uint8, uint);

    // Записывает столько данных, сколько можно записать за одну транзакцию.
    // Возвращает количество записанных байт
    static int WritePage(uint address, const uint8 *buffer, int size);

    static int ReadPage(uint address, uint8 *buffer, int size);

    static int SizeSector();

    static int GetSectorsCount();

    static void EraseSector(int num_sector);

    void WriteUint8(uint address, uint8 value);
    uint8 ReadUint8(uint address);
}


void Memory::Erase::Block(int num_block)
{
    Write32bit(BLOCK_ERASE_4KB, AddressBlock(num_block));
}


uint Memory::AddressBlock(int num_block)
{
    return (uint)(num_block * SizeBlock());
}


void Memory::EraseSector(int num_sector)
{
    Write32bit(SECTOR_EASE_64KB, (uint)(num_sector * SizeSector()));
}


void Memory::WriteInt16(uint address, int16 value)
{
    uint8 data[6] =
    {
        PROGRAM_PAGE,
        (uint8)(address >> 16),
        (uint8)(address >> 8),
        (uint8)(address),
        (uint8)value,
        (uint8)(value >> 8)
    };

    MEM_SPI::WriteBuffer(data, 6);
}


void Memory::WriteInt(uint address, int value)
{
    uint8 data[8] =
    {
        PROGRAM_PAGE,
        (uint8)(address >> 16),
        (uint8)(address >> 8),
        (uint8)(address),
        (uint8)(value),
        (uint8)(value >> 8),
        (uint8)(value >> 16),
        (uint8)(value >> 24)
    };

    MEM_SPI::WriteBuffer(data, 8);
}


int16 Memory::ReadInt16(uint address)
{
    uint8 out[7] =
    {
        READ_ARRAY,
        (uint8)(address >> 16),
        (uint8)(address >> 8),
        (uint8)address,
        0,
        0,
        0
    };

    uint8 in[7];

    MEM_SPI::WaitRelease();

    HAL_SPI::WriteRead(DirectionSPI::Memory, out, in, 7);

    return (int16)(in[5] + (in[6] << 8));
}


int Memory::ReadInt(uint address)
{
    uint8 out[9] =
    {
        READ_ARRAY,
        (uint8)(address >> 16),
        (uint8)(address >> 8),
        (uint8)(address),
        0,
        0,
        0,
        0,
        0
    };

    uint8 in[9];

    MEM_SPI::WaitRelease();

    HAL_SPI::WriteRead(DirectionSPI::Memory, out, in, 9);

    BitSet32 bs;

    std::memcpy(&bs.word, in + 5, 4);

    return (int)bs.word;
}


void Memory::Write32bit(uint8 command, uint bits24)
{
    uint8 data[4] =
    {
        command,
        (uint8)(bits24 >> 16),
        (uint8)(bits24 >> 8),
        (uint8)(bits24)
    };

    MEM_SPI::WriteBuffer(data, 4);
}


void Memory::WriteUint8(uint address, uint8 value)
{
    uint8 data[5] =
    {
        PROGRAM_PAGE,
        (uint8)(address >> 16),
        (uint8)(address >> 8),
        (uint8)(address),
        (uint8)value
    };

    MEM_SPI::WriteBuffer(data, 5);
}


void Memory::WriteBuffer(uint address, const void *_buffer, int size)
{
    const uint8 *buffer = (const uint8 *)_buffer;

    while (size)
    {
        int written_bytes = WritePage(address, buffer, size);

        address += (uint)written_bytes;
        buffer += written_bytes;
        size -= written_bytes;
    }
}


int Memory::WriteBufferRelible512(uint address, const void *buffer, int size, int number_attempts)
{
    StackBuffer <512>data;

    for (int i = 0; i < number_attempts; i++)
    {
        WriteBuffer(address, buffer, size);

        ReadBuffer(address, data.DataU8(), size);

        if (std::memcmp(data.DataU8(), buffer, (uint)size) == 0)
        {
            return i + 1;
        }
    }

    return number_attempts;
}


int Memory::WritePage(uint address, const uint8 *buffer, int size)
{
    uint8 data[260] =
    {
        PROGRAM_PAGE,
        (uint8)(address >> 16),
        (uint8)(address >> 8),
        (uint8)(address)
    };

    Page256 page(address);

    if ((address + (uint)size) > page.End())
    {
        size = (int)(page.End() - address);
    }

    std::memcpy(data + 4, buffer, (uint)size);

    MEM_SPI::WriteBuffer(data, 4 + size);

    return size;
}


uint8 Memory::ReadUint8(uint address)
{
    uint8 out[6] =
    {
        READ_ARRAY,
        (uint8)(address >> 16),
        (uint8)(address >> 8),
        (uint8)address,
        0,
        0
    };

    uint8 in[6];

    MEM_SPI::WaitRelease();

    HAL_SPI::WriteRead(DirectionSPI::Memory, out, in, 6);

    return in[5];
}


int Memory::ReadPage(uint address, uint8 *buffer, int size)
{
    uint8 data[261] =
    {
        READ_ARRAY,
        (uint8)(address >> 16),
        (uint8)(address >> 8),
        (uint8)(address),
        0
    };

    uint8 in[261];

    Page256 page(address);

    if ((address + (uint)size) > page.End())
    {
        size = (int)(page.End() - address);
    }

    MEM_SPI::WaitRelease();

    HAL_SPI::WriteRead(DirectionSPI::Memory, data, in, 5 + size);

    std::memcpy(buffer, in + 5, (uint)size);

    return size;
}


void Memory::ReadBuffer(uint address, void *_buffer, int size)
{
    uint8 *buffer = (uint8 *)_buffer;

    while (size)
    {
        uint readed_bytes = (uint)ReadPage(address, buffer, size);

        address += readed_bytes;
        buffer += readed_bytes;
        size -= readed_bytes;
    }
}


void Memory::ReadBufferFast(uint address, void *buffer, int size)
{
    uint8 * const pointer = (uint8 *)buffer - 5;

    pointer[0] = READ_ARRAY;
    pointer[1] = (uint8)(address >> 16);
    pointer[2] = (uint8)(address >> 8);
    pointer[3] = (uint8)(address);
    pointer[4] = 0;

    MEM_SPI::WaitRelease();

    HAL_SPI::WriteRead(DirectionSPI::Memory, pointer, pointer, size + 5);
}


#ifdef MCU_GD
#else
void Memory::ReadBufferFastDMA(uint address, void *buffer, int size, void (*callback_on_complete)())
{
    uint8 * pointer = (uint8 *)buffer;

    pointer -= 5;

    pointer[0] = READ_ARRAY;
    pointer[1] = (uint8)(address >> 16);
    pointer[2] = (uint8)(address >> 8);
    pointer[3] = (uint8)(address);
    pointer[4] = 0;

    MEM_SPI::WaitRelease();

    HAL_SPI::WriteReadDMA(DirectionSPI::Memory, pointer, pointer, size + 5, callback_on_complete);
}
#endif


bool Memory::Test::Run()
{
    bool result = true;

    for (int i = 0; i < 3; i++)
    {
        Erase::Block(i);

        uint address = (uint)(i * 4 * 1024);

        const int NUM_BYTES = 250;

        uint8 buffer_out[NUM_BYTES + 5];
        uint8 buffer_in[NUM_BYTES + 5];

        for (int j = 0; j < NUM_BYTES; j++)
        {
            buffer_out[j] = (uint8)std::rand();
        }

        Memory::WriteBufferRelible512(address, buffer_out, NUM_BYTES);

        Memory::ReadBufferFast(address, buffer_in + 5, NUM_BYTES);

        if (std::memcmp(buffer_in + 5, buffer_out, NUM_BYTES) != 0)
        {
            result = false;
            break;
        }
    }

    return result;
}


float Memory::Test::SpeedWrite()
{
    const int SIZE = 256;
    const int COUNTER = 100;

    uint8 buffer[SIZE];

    uint time_start = TIME_MS;

    for (int i = 0; i < COUNTER; i++)
    {
        WriteBuffer(0, buffer, SIZE);
    }

    uint time = TIME_MS - time_start;

    return (float)(SIZE * COUNTER) / (float)time * 1000.0f;
}


float Memory::Test::SpeedRead()
{
    const int SIZE = 256;
    const int COUNTER = 100;

    uint8 buffer[SIZE];

    uint time_start = TIME_MS;

    for (int i = 0; i < COUNTER; i++)
    {
        ReadBuffer(0, buffer, SIZE);
    }

    uint time = TIME_MS - time_start;

    return (float)(SIZE * COUNTER) / (float)time * 1000.0f;
}


int Memory::SizeBlock()
{
    return 4 * 1024;
}


int Memory::Capasity()
{
    return 512 * 1024;
}


int Memory::SizeSector()
{
    return 64 * 1024;
}


int Memory::GetBlocksCount()
{
    return (int)(Capasity() / SizeBlock());
}


int Memory::GetSectorsCount()
{
    return (int)(Capasity() / SizeSector());
}


void Memory::Erase::Full()
{
    for (int i = 0; i < GetSectorsCount(); i++)
    {
        EraseSector(i);
    }
}


void Memory::Erase::Firmware()
{
    int num_blocks = SIZE_FIRMWARE_STORAGE / Memory::SizeBlock();

    for (int i = 0; i < num_blocks; i++)
    {
        Memory::Erase::Block(i);
    }
}


void Memory::Erase::Sounds()
{
    int start_block = ADDRESS_SOUNDS / Memory::SizeBlock();

    int end_block = start_block + MEMORY_FOR_SOUNDS / Memory::SizeBlock();

    for (int i = start_block; i < end_block; i++)
    {
        Memory::Erase::Block(i);
    }
}
