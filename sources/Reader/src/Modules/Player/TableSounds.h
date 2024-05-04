// 2024/03/26 16:02:20 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Modules/Memory/Memory.h"


// “еблица описани€ хранени€ звуков во внешней пам€ти
namespace TableSounds
{
    // ѕо этому адресу расположена таблица в пам€ти
    inline uint *Begin() { return (uint *)ADDRESS_SOUNDS; }; //-V566

    // –азмер таблицы
    inline uint Size() { return NUMBER_SOUDNS * sizeof(int); }

    inline int SizeSound(int num_sound)
    {
        return Memory::ReadInt((uint)Begin() + (uint)num_sound * 4U);
    }

    inline int CountSamples(int num_sound) { return SizeSound(num_sound) / 2; }

    inline uint AddressSound(int num_sound)
    {
        return (num_sound == 0) ? ((uint)Begin() + Size()) : (AddressSound(num_sound - 1) + (uint)SizeSound(num_sound - 1));
    }
}
