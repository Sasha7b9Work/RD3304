// 2024/01/12 14:48:46 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Settings/AccessCards.h"
#include "Hardware/HAL/HAL.h"
#include "Reader/Reader.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>


namespace AccessCards
{
    // Структура описывает место хранения бита доступа карты
    struct StructCard
    {
        StructCard(uint addr, uint _bit) : address((uint8 *)addr), bit((int)_bit)  { }
        uint8 *address;     // Адрес байта, где хранится искомый бит
        int bit;            // Номер байта в бите
    };

    static StructCard FindPlaceForCard(uint num_card);
}


void AccessCards::Set(pchar bits)
{
    int index = 0;
    uint8 buffer[1024];

    std::memset(buffer, 0, 1024);

    pchar pointer = bits;

    while (*pointer)
    {
        char data[32];

        char byte1 = *pointer++;
        char byte2 = *pointer++;

        std::sprintf(data, "%c%c", byte1, byte2);

        buffer[index++] = (uint8)std::strtoull(data, nullptr, 16);
    }

    HAL_ROM::ErasePage(HAL_ROM::ADDRESS_SECTOR_ACCESS_CARDS);

    HAL_ROM::WriteBuffer(HAL_ROM::ADDRESS_SECTOR_ACCESS_CARDS, buffer, 1024);
}


bool AccessCards::Access(uint number)
{
    if (number > ModeOffline::MaxNumCards())
    {
        return false;
    }

    StructCard card = FindPlaceForCard(number);

    uint8 byte = HAL_ROM::ReadUInt8((uint)card.address);

    return (byte & (1 << card.bit)) != 0;
}


AccessCards::StructCard AccessCards::FindPlaceForCard(uint num_card)
{
    uint index = (uint)(num_card / 8);

    uint bit = num_card - index * 8;

    return StructCard(HAL_ROM::ADDRESS_SECTOR_ACCESS_CARDS + index, bit);
}
