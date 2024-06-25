// 2024/04/16 08:39:12 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace PlayerSoft
{
    void _Play(float freq, uint time);

    void Update();

    bool IsRunning();

    void CallbackOnHalfTransmit();

    void CallbackOnFullTransmit();
}
