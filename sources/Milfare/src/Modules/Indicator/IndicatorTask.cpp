// 2023/12/21 10:45:50 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/Indicator/Indicator.h"


/*
    Зажечь чёрный цвет
    69 CMD

    00 Reader number
    00 LED Number

    01 Control Code : Cancel any temporary operation and display this LED's permanent state immediately
    01 ON time      : 100 ms
    00 OFF time     : 0
    01 ON color     : 1
    02 OFF color    : 2
    00 Timer LSB
    00 Timer MSB

    01 Control Code :
    01              : ON time
    00              : OFF time
    00              : ON color
    00              : OFF color
*/


namespace Indicator
{
    namespace TaskOSDP
    {
        static Color perm_color_off(0, 0.0f);
    }
}


void Indicator::TaskOSDP::Set(
    TemporaryControlCode::E,
    uint /*temp_time_on*/,
    uint /*temp_time_off*/,
    Color & /*temp_color_on*/,
    Color & /*temp_color_off*/,
    uint /*temp_timer*/,

    PermanentControlCode::E,
    uint /*perm_time_on*/,
    uint /*perm_time_off*/,
    Color &_perm_color_on,
    Color &_perm_color_off)
{
    perm_color_off = _perm_color_off;

    TurnOn(_perm_color_on, 0, false);

    TurnOn(perm_color_off, 0, false);
}


void Indicator::TaskOSDP::FirePermanentColor()
{
    TurnOn(perm_color_off, 0, false);
}
