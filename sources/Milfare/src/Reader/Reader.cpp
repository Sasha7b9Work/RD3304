// 2023/09/05 12:49:45 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Reader/Reader.h"
#include "Modules/CLRC66303HN/CLRC66303HN.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Power.h"
#include "Modules/Indicator/Indicator.h"
#include "Modules/Player/Player.h"
#include "Nodes/Upgrader.h"


ModeReader::E ModeReader::current = ModeReader::WG;


namespace ModeOffline
{
    enum E
    {
        Enabled,
        Disabled
    };

    static E current_mode = Disabled;
    static uint time_lock = 0;          // Время открытия замка
    static uint time_alarm = 0;         // Время включения сирены
}


namespace ModeWG
{
    enum E
    {
        Normal,     // Пересылаем номер карты
        OnlyGUID    // Пересылаем GUID карты
    };

    static E current_mode = Normal;

    bool IsNormal()
    {
        return current_mode == Normal;
    }

    void EnableOnlyGUID()
    {
        current_mode = OnlyGUID;

        for (int i = 0; i < 3; i++)
        {
            Indicator::Blink(Color(0xFF0000, 1.0f), Color(0, 0), 500, false);
        }
    }
}


namespace Reader
{
    static TypeAuth type_auth(false, 0);

    static bool new_auth = false;
}


void Reader::Init()
{
    CLRC66303HN::Init();
}


void Reader::Update()
{
    if (Player::IsPlaying())
    {
//        return;
    }

    static TimeMeterMS meter_update;

    static bool prev_detected = false;              // true, если в предыдущем цикле детектирована карта

    if (meter_update.ElapsedMS() < (prev_detected ? 500U : 50U))
    {
        return;
    }

    meter_update.Reset();

    if (ModeReader::IsNormal())
    {
        static TimeMeterMS meter;

        if (meter.ElapsedMS() >= ProtectionBruteForce::TimeWait())
        {
            meter.Reset();

            prev_detected = CLRC66303HN::UpdateNormalMode();
        }
    }
    else
    {
        prev_detected = CLRC66303HN::UpdateExtendedMode(type_auth, new_auth);
    }

    new_auth = false;
}


void Reader::SetAuth(const TypeAuth &type)
{
    type_auth = type;
    new_auth = true;
}


TypeAuth Reader::GetAuth()
{
    return type_auth;
}


pchar ModeReader::Name(E v)
{
    static const pchar names[Count] =
    {
        "WEIGAND",
        "UART",
        "READ",
        "WRITE"
    };

    return names[v];
}


void ModeOffline::Enable()
{
    current_mode = Enabled;

    pinTXD1.Init();
    pinTXD1.ToLow();

    pinLOCK.Init();         // D0 Замок
    pinLOCK.ToLow();
    
    pinALARM.Init();
    pinALARM.ToLow();

    for (int i = 0; i < 3; i++)
    {
        Indicator::Blink(Color(0x00FF00, 1.0f), Color(0, 0), 500, false);
    }

    Indicator::TurnOn(gset.ColorRed(), TIME_RISE_LEDS, false);
}


void ModeOffline::Update()
{
    if (!IsEnabled())
    {
        return;
    }

    if (time_alarm > 0)
    {
        if (TIME_MS - time_alarm >= gset.TimeAlarm())
        {
            time_alarm = 0;
        }
    }

    if (time_lock > 0)
    {
        if (TIME_MS - time_lock >= gset.TimeLock())
        {
            time_lock = 0;

            pinLOCK.ToLow();
        }
    }
}


bool ModeOffline::IsEnabled()
{
    return (current_mode == Enabled);
}


void ModeOffline::OpenTheLock()
{
    if (!IsEnabled() || gset.TimeLock() == 0)
    {
        return;
    }

    time_lock = TIME_MS;

    pinLOCK.ToHi();
}


void ModeOffline::EnableAlarm()
{
    if (!IsEnabled() || gset.TimeAlarm() == 0)
    {
        return;
    }

    time_alarm = TIME_MS;
}


uint ModeOffline::MaxNumCards()
{
    return 600;
}


bool ModeSignals::IsInternal()
{
    return ModeOffline::IsEnabled();
}


bool ModeSignals::IsExternal()
{
    return !IsInternal();
}
