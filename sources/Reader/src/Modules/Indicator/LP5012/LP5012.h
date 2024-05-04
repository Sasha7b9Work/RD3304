// 2022/08/05 20:42:06 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Modules/Indicator/Indicator.h"
#include "Modules/Indicator/ColorLED.h"

#ifdef WIN32
    #pragma warning(push)
    #pragma warning(disable : 4505)
#endif

/*
*  Драйвер светодиодов
*/

#ifdef TYPE_BOARD_771

namespace LP5012
{
    void Init();

    // -1 - один цвет для всех
    void Fire(int num_led, const ColorLED &);

    inline int NumLeds() { return 8; }

    namespace Driver
    {
        void Init();

        // Параметры [0...1.0]
        void Fire(int num_vdd, const ColorLED &);
    }
}

#endif


#ifdef WIN32
#pragma warning(pop)
#endif
