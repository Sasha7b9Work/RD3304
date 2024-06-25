// 2023/11/18 16:21:10 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct ModeIndicator
{
    enum E
    {
        External,       // ¬ этом режиме индикатор загараетс€ по сигналам LG, LR. –ежим по умолчанию
        Internal        // ¬ этом режме индикатор не провер€ет состо€ние LG, LR
    };

    static void Set(ModeIndicator::E mode)
    {
        current = mode;
    }

    static bool IsExternal()
    {
        return (current == External);
    }

private:

    static E current;
};


namespace Indicator
{
    void Init();

    bool Update();

    // ¬озвращает true, если есть невыполненные задани€
    bool IsRunning();

    // ¬озвращает true, если установленный цвет - не ноль
    bool IsFired();

    // time - врем€, за которое нужно выполнить включение
    // stopped - отмен€ть ли текущие задачи
    void TurnOn(const Color &, uint time, bool stopped);

    // time - врем€ от начала зажигани€ до конца выключени€
    // stopped - прерывать ли текущие задачи
    void Blink(const Color &color1, const Color &color2, uint time, bool stopped);

    // time - врем€, за которое нужно произвести выключение
    // stopped - прерывать ли текущие задачи
    void TurnOff(uint time, bool stopped);

    // «ажечь в соответствии с состо€ни€ми пинов LG, LR
    void FireInputs(bool lg, bool lr);

    // √орит в данный момент
    Color FiredColor();

    namespace TaskOSDP
    {
        struct TemporaryControlCode
        {
            enum E
            {
                DoNotAlter,         // Ќе измен€ть временные настройки
                CancelAndDisplay,   // ќтменить любую временную операцию и немедленно отобразить состо€ние
                SetAndStart         // ”становить временное состо€ние, как указано, и запустить таймер обратного отсчета немедленно
            };
        };

        struct PermanentControlCode
        {
            enum E
            {
                DoNotAlter,     // NOP - do not alter this LED's permanent settings
                SetAsGiven,     // Set the permanent state as given
            };
        };

        void Set(
            TemporaryControlCode::E,    // The mode to enter temporarily
            uint temp_time_on,          // The ON duration of the flash, in units of 100 ms
            uint temp_time_off,         // The OFF duration of the flash, in units of 100 ms
            Color &temp_color_on,       // The color to set during the ON time
            Color &temp_color_off,      // The color to set during the OFF time
            uint temp_timer,
            PermanentControlCode::E,    // The mode to return to after the timer expires
            uint perm_time_on,          // The ON duration of the flash, in units of 100 ms
            uint perm_time_off,         // The OFF duration of the flash, in units of 100 ms
            Color &perm_color_on,       // The color to set during the ON time
            Color &perm_color_off       // The color to set during the OFF time
        );

        // «ажечь цвет, установленный в качестве посто€нного последней командой
        void FirePermanentColor();
    }
}
