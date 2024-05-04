// 2024/01/13 22:51:24 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"


void HAL_ROM::ErasePage(uint address)
{
    (void)address;

    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef itd =
    {
        FLASH_TYPEERASE_PAGES,
        0,                              // Banks
        address,
        1                               // Number pages
    };

    uint error = 0;

    if (HAL_FLASHEx_Erase(&itd, &error) != HAL_OK)
    {
//        LOG_ERROR("Can not erase page %d", address);
    }

    HAL_FLASH_Lock();
}


void HAL_ROM::WriteBuffer(uint address, void *buffer, int size)
{
    (void)address;
    (void)buffer;
    (void)size;

    HAL_FLASH_Unlock();

    uint *data = (uint *)buffer;

    for (int i = 0; i < size; i += 4)
    {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, *data) != HAL_OK)
        {
//            LOG_ERROR("Can not write data to %u", address);
            break;
        }
        address += 4;
        data++;
    }

    HAL_FLASH_Lock();
}


void HAL_ROM::ReadBuffer(uint address, void *out, int size)
{
    std::memcpy(out, (void *)address, (uint)size);
}


uint8 HAL_ROM::ReadUInt8(uint address)
{
    uint8 *pointer = (uint8 *)address;

    return *pointer;
}


uint HAL_ROM::ReadUInt(uint address)
{
    uint *pointer = (uint *)address;

    return *pointer;
}
