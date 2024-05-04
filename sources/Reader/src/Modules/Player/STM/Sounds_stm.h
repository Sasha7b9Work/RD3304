// 2023/10/05 21:44:42 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Modules/Memory/Memory.h"


namespace Sound
{
    bool Update();

    void Start(uint8 _num_sound);

    void Stop();

    // ¬озвращает true, если прогирывание не закончено
    bool IsPlaying();

    int16 NextSample();
};
