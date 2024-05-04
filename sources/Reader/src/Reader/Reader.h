// 2023/09/05 12:49:31 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Reader/Card.h"


// Чем управляются сигналы светодиодов, громкоговорителя - внутренним состоянием
// (Internal) или внешним воздействием (External)
namespace ModeSignals
{
    bool IsInternal();

    bool IsExternal();
}


// Это режим, в котором считыватель лишь выдаёт управляющие сигналы на замок и сирену, основываясь на
// битовой карте, занесённой в память.
namespace ModeOffline
{
    void Update();
    void Enable();
    bool IsEnabled();

    // Открыть замок на количество секунд, указанное в настройках
    void OpenTheLock();

    // Включить сигнал тревоги на количестов секнуд, указанное в настройках
    void EnableAlarm();

    uint MaxNumCards();
}


namespace ModeWG
{
    void EnableOnlyGUID();

    bool IsNormal();
};


struct ModeReader
{
    enum E
    {
        WG,
        UART,
        Read,
        Write,
        Count
    };

    static void Set(E m) { current = m; }

    static E Current() { return current; }

    static pchar Name(E);

    static bool IsRead() { return current == Read; }

    // Это режим "Read", дополненный функциями записи на карты
    static bool IsWrite() { return current == Write; }

    static bool IsWG() { return current == WG; }

    static bool IsUART() { return current == UART; }

    // Выводить ли дополнительные сообщения
    static bool IsExtended() { return IsRead() || IsWrite(); }

    // Находимся в нормальном режиме - определение мастер- и пользовательских карт
    static bool IsNormal() { return IsWG() || IsUART(); }

private:
    static E current;
};


struct TypeAuth
{
    TypeAuth(bool _enabled = true, uint64 _password = 0) :
        enabled(_enabled),
        password(_password)
    {
    }

    bool   enabled;     // Если true - подключаемся по паролю. Иначе - без
    uint64 password;

    bool operator==(const TypeAuth &rhs)
    {
        if (enabled != rhs.enabled)
        {
            return false;
        }

        if (!enabled)
        {
            return true;
        }

        return password == rhs.password;
    }

    bool operator!=(const TypeAuth &rhs)
    {
        return !(*this == rhs);
    }
};


namespace Reader
{
    void Init();

    void Update();

    void SetAuth(const TypeAuth &);

    TypeAuth GetAuth();
}
