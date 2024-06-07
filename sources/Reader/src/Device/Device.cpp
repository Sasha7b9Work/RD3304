// 2022/04/27 11:48:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device/Device.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Modules/Player/Player.h"
#include "Modules/Indicator/Indicator.h"
#include "Modules/LIS2DH12/LIS2DH12.h"
#include "Modules/Memory/Memory.h"
#include "Hardware/Power.h"
#include "Nodes/OSDP/OSDP.h"
#include "Reader/Reader.h"
#include "Modules/Sensor/TTP223.h"
#include "Modules/PlayerSoft/PlayerSoft.h"
#include "Nodes/Upgrader.h"
#include "system.h"
#include <cstdlib>
#include <limits.h>


namespace Device
{
    static bool is_running = false;

    bool IsRunning()
    {
        return is_running;
    }
}


void Device::Init()
{
    HAL::Init();

    SettingsMaster::Load();

    Timer::Init();

    TimeMeterMS meter;

    meter.WaitFor(200);

    Memory::Erase::FirmwareIfNeed();

    Player::Init();

    Indicator::Init();

    LIS2DH12::Init();

    Reader::Init();

#ifdef MCU_GD

    TTP223::Init();

#endif

    Player::Play(TypeSound::Beep);

    if (StartState::NeedMinimalWG())                // Режим "минимального WG" - передаём только GUID
    {
        ModeWG::EnableOnlyGUID();
    }
    else if (StartState::NeedReset())               // Делаем сброс
    {
        SettingsMaster::ResetToFactory();
    }
    else if (gset.IsEnabledOSDP())                  // Включаем OSDP
    {
        OSDP::Init();
    }
    else if (gset.IsOfflineModeAldowed() && StartState::NeedOffline())             // Считыватель будет работать автономно
    {
        ModeOffline::Enable();
    }
    else                                            // Обычный запуск
    {
        for (int i = 0; i < 3; i++)
        {
            Indicator::Blink(Color(0xFFFFFF, 1.0f), Color(0, 0), 500, false);
        }
    }

//    ModeReader::Set(ModeReader::UART);

//    LOG_WRITE_TRACE("Mode UART is enabled");

    while (Device::UpdateTasks())
    {
    }

    Power::Init();

    is_running = true;
}


void Device::Update()
{
    if (!HAL::IsDebugBoard())
    {
//        Power::EnterSleepMode();
    }

    LIS2DH12::Update();         // Акселерометр

    if (Upgrader::IsCompleted())
    {
        Reader::Update();
    }

    ModeOffline::Update();      // Обработка замка и тревоги

    HAL_ADC::Update();          // Обрабаытваем пониженное напряжение

    UpdateTasks();
}


bool Device::UpdateTasks()
{
    volatile bool beeper = Player::Update();

    volatile bool indicator = Indicator::Update();

    volatile bool usart = HAL_USART::Update();

    return beeper || indicator || usart;
}
