// 2022/08/05 20:41:28 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/Indicator/LP5012/LP5012.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"

#ifdef TYPE_BOARD_771

namespace LP5012
{
    struct VH
    {
        enum E
        {
            _1,
            _2,
            _3,
            _4,
            _5,
            _6,
            _7,
            _8,
            Count
        };
    };
}


void LP5012::Init()
{
    pinENP.Init();

    pinENP.ToHi();

    Timer::Delay(100);

    Driver::Init();
}


void LP5012::Fire(int num_led, const ColorLED &color)
{
    if (num_led < 0)
    {
        for (int i = 0; i < VH::Count; i++)
        {
            Driver::Fire(i, color);
        }
    }
    else
    {
        Driver::Fire(num_led, color);
    }
}


#endif
