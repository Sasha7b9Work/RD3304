// 2023/10/06 08:16:00 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Settings/Settings.h"


struct SamplingRateKHZ
{
    enum E
    {
        _8 = 8,
        _16 = 16,
        _32 = 32
    };
};


namespace Player
{
    static const SamplingRateKHZ::E sampling_rate = SamplingRateKHZ::_32;

    void Init();

    bool Update();

    // Проигрывает звук, 
    void Play(TypeSound::E);

    void Stop();

    // 3 - максимальный уровень громкости
    void PlayFromMemory(uint8 num_sound_in_memory, uint8 volume);

    bool IsPlaying();

#ifdef MCU_GD

    // Вызывается по завершении передачи половины буфера
    void CallbackOnHalfTransmit();

    // Вызывается по завершении передачи полного буфера
    void CallbackOnFullTransmit();

    void CallbackInTimer13();

#else

    void CallbackOnTimer();

    extern void *handleTIM;

#endif

    namespace Task
    {
        // Звук будет звучать time_on ms, затем молчать time_off ms, и так count раз.
        // Если count == 0, то звук будет звучать, пока не придёт другая команда
        void Set(TypeSound::E, uint time_on, uint time_off, int count);
    }
}
