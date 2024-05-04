// 2023/06/10 12:17:46 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include <cstring>
#include "system.h"


void HAL_ROM::Load(SettingsReader &settings)
{
    SettingsReader set;

    ReadBuffer(ADDRESS_SECTOR_SETTINGS, &set, set.Size());

    uint hash = set.Hash();
    uint new_hash = set.CalculateHash();

    if (hash == new_hash)
    {
        settings = set;
    }
}


void HAL_ROM::Save(const SettingsReader &settings)
{
    ErasePage(ADDRESS_SECTOR_SETTINGS);

    WriteBuffer(ADDRESS_SECTOR_SETTINGS, (void *)&settings, sizeof(settings));
}
