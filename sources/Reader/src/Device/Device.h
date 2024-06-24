// 2022/04/27 11:48:09 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Utils/String.h"


namespace Device
{
    void Init();

    void Update();

    bool UpdateTasks();

    // Окончена инициализация. Работаем
    bool IsRunning();

    namespace StartState
    {
        // Нужно делать сброс на зводские настройки
        // Включатеся замыканием TX и SND
        bool NeedReset();

        // Нужен режим режим минимального WG - когда по WG передаётся лишь GUID карты
        // Включается замыканием RX и SND
        bool NeedMinimalWG();

        // Режим автономного считывателя
        bool NeedOffline();
    }

    namespace Info
    {
        namespace Hardware
        {
            String<> Version_ASCII_HEX();           // Аппаратная версия
            String<> DateManufactured_ASCII();      // Дата производства
            uint64 MacUint64();
            String<> SerialNumber_ASCII_HEX();      // Серийный номер изделия
            uint   SerialNumber_UINT();             // Серийный номер изделия

            uint   Vendor();
            uint8  ManufacturedModelNumber();
            uint8  ManufacturedVersion();
        }

        namespace Loader
        {
            String<> Version_ASCII_HEX();           // Версия загрузчика
        }

        namespace Firmware
        {
            String<> TypeID_ASCII();                // Идентификатор типа изделия
            String<> Version_ASCII_HEX();           // Версия программы
            String<> Version_ASCII_DEC();           // Версия программы
            uint     Version_UINT();                // Версия программы
            String<> Info_ASCII();
        }
    }
}
