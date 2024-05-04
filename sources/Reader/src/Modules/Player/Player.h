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

    // ����������� ����, 
    void Play(TypeSound::E);

    void Stop();

    // 3 - ������������ ������� ���������
    void PlayFromMemory(uint8 num_sound_in_memory, uint8 volume);

    bool IsPlaying();

#ifdef MCU_GD

    // ���������� �� ���������� �������� �������� ������
    void CallbackOnHalfTransmit();

    // ���������� �� ���������� �������� ������� ������
    void CallbackOnFullTransmit();

    void CallbackInTimer13();

#else

    void CallbackOnTimer();

    extern void *handleTIM;

#endif

    namespace Task
    {
        // ���� ����� ������� time_on ms, ����� ������� time_off ms, � ��� count ���.
        // ���� count == 0, �� ���� ����� �������, ���� �� ����� ������ �������
        void Set(TypeSound::E, uint time_on, uint time_off, int count);
    }
}
