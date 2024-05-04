// 2022/6/10 9:08:19 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/Timer.h"
#include "Modules/Player/Player.h"
#include "system.h"
#include "Hardware/HAL/GD/systick.h"


namespace Timer
{
    uint counter_ms = 0;
}


TimeMeterMS TimeMeterMS::global;


void Timer::Init()
{
#ifndef WIN32
    #ifdef MCU_GD
    #else
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    #endif
#endif
}

uint Timer::CurrentTime()
{
#ifdef MCU_GD
    return counter_ms;
#else
    return HAL_GetTick();
#endif
}


void Timer::Delay(uint timeMS)
{
#ifdef MCU_GD
    delay_1ms(timeMS);
#else
    HAL_Delay(timeMS);
#endif
}


TimeMeterMS::TimeMeterMS()
{
    Reset();
}


void TimeMeterMS::Reset()
{
    time_reset = TIME_MS;
    time_pause = 0;
}


void TimeMeterMS::WaitFor(uint ms)
{
    while (ElapsedMS() < ms)
    {
//        Player::Update();
    }
}


void TimeMeterMS::Pause()
{
    time_pause = TIME_MS;
}


void TimeMeterMS::Continue()
{
    time_reset += (TIME_MS - time_pause);
}


uint TimeMeterMS::ElapsedMS() const
{
    return TIME_MS - time_reset;
}
