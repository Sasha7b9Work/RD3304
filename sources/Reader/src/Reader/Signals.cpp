// 2023/09/21 14:50:38 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Reader/Signals.h"
#include "Device/Device.h"
#include "Modules/Indicator/Indicator.h"
#include "Settings/Settings.h"
#include "Reader/Reader.h"
#include "Modules/Player/Player.h"


namespace Signals
{
    static uint Time(bool is_master)
    {
        return ModeReader::IsNormal() ? (is_master ? 500U : 100U) : 10U;
    }
}


void Signals::GoodUserCard(bool sound)
{
    Indicator::Blink(Color(0, 0.0f), gset.ColorGreen(), TIME_RISE_LEDS * 2, true);
    Indicator::Blink(gset.ColorGreen(), gset.ColorGreen(), 500, false);
    Indicator::Blink(Color(0, 0.0f), gset.ColorRed(), TIME_RISE_LEDS * 2, false);

   if (sound)
    {
        Player::Play(TypeSound::Green);
    }
}


void Signals::BadUserCard(bool sound)
{
    Indicator::Blink(Color(0, 0.0f), gset.ColorRed(), TIME_RISE_LEDS * 2, true);
    Indicator::Blink(Color(0, 0.0f), gset.ColorRed(), TIME_RISE_LEDS * 2, false);

    if (sound)
    {
        Player::Play(TypeSound::Red);
    }
}


void Signals::GoodMasterCard()
{
    for (int i = 0; i < 3; i++)
    {
        Indicator::Blink(Color(0xFFFFFF, 1.0f), Color(0, 0.0f), Time(true), false);
    }

    while (Device::UpdateTasks())
    {
    }
}


void Signals::BadMasterCard()
{
    for (int i = 0; i < 3; i++)
    {
        Indicator::Blink(Color(0xFF0000, 1.0f), Color(0, 0.0f), Time(true), false);
    }

    while (Device::UpdateTasks())
    {
    }
}
