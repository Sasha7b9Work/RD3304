// 2022/6/7 9:06:54 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Hardware/HAL/HAL.h"


#ifdef MCU_GD
    #define TIME_MS Timer::counter_ms
#else
    #define TIME_MS Timer::CurrentTime()
#endif


namespace Timer
{
    void Init();

    uint CurrentTime();

    void Delay(uint timeMS);

    extern uint counter_ms;
}



// ��������� ��� ������� �������
struct TimeMeterMS
{
    TimeMeterMS();

    // ���������� ������ �������
    void Reset();

    void Pause();

    void Continue();

    // ������� ms ����������� � ������� ���������� Reset()
    void WaitFor(uint ms);

    // ������� ����������� ������ � ������� ������ Reset()
    uint ElapsedMS() const;

    static TimeMeterMS global;

private:

    uint time_reset;        // �� ����� ������� ������������� ElapsedTime()
    uint time_pause;        // � ���� ������ ��������� �� �����
};


// !!! WARNING !!! ��� ��������� ����� ������������ � ������������ ���������� � ������ ������ �������,
// �.�. ����������� ���������� ���������� ������
// !!! WARNING !!! ������������ �������� - 65535 ���
struct TimeMeterUS
{
    TimeMeterUS()
    {
        Reset();
    }

    void Reset()
    {
#ifndef WIN32
    #ifdef MCU_GD
        HAL_TIM5::Start();
    #else
        DWT->CYCCNT = 0U;
    #endif
#endif
    }

    // ������� us ����������� � ������� ���������� Reset()
    void WaitFor(uint us)
    {
        (void)us;

#ifndef WIN32

    #ifdef MCU_GD
        while (HAL_TIM5::ElapsedUS() < us) { }
    #else
        uint end_count = us * 60;

        while (DWT->CYCCNT < end_count) { }
    #endif

#endif
    }

    uint ElapsedUS()
    {
#ifdef WIN32
        return 0;
#else
    #ifdef MCU_GD
        return HAL_TIM5::ElapsedUS();
    #else
        return DWT->CYCCNT / 60;
    #endif
#endif
    }
};
