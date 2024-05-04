// 2023/11/18 16:21:10 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct ModeIndicator
{
    enum E
    {
        External,       // � ���� ������ ��������� ���������� �� �������� LG, LR. ����� �� ���������
        Internal        // � ���� ����� ��������� �� ��������� ��������� LG, LR
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

    // ���������� true, ���� ���� ������������� �������
    bool IsRunning();

    // ���������� true, ���� ������������� ���� - �� ����
    bool IsFired();

    // time - �����, �� ������� ����� ��������� ���������
    // stopped - �������� �� ������� ������
    void TurnOn(const Color &, uint time, bool stopped);

    // time - ����� �� ������ ��������� �� ����� ����������
    // stopped - ��������� �� ������� ������
    void Blink(const Color &color1, const Color &color2, uint time, bool stopped);

    // time - �����, �� ������� ����� ���������� ����������
    // stopped - ��������� �� ������� ������
    void TurnOff(uint time, bool stopped);

    // ������ � ������������ � ����������� ����� LG, LR
    void FireInputs(bool lg, bool lr);

    // ����� � ������ ������
    Color FiredColor();

    namespace TaskOSDP
    {
        struct TemporaryControlCode
        {
            enum E
            {
                DoNotAlter,         // �� �������� ��������� ���������
                CancelAndDisplay,   // �������� ����� ��������� �������� � ���������� ���������� ���������
                SetAndStart         // ���������� ��������� ���������, ��� �������, � ��������� ������ ��������� ������� ����������
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

        // ������ ����, ������������� � �������� ����������� ��������� ��������
        void FirePermanentColor();
    }
}
