// 2023/09/05 12:49:31 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Reader/Card.h"


// ��� ����������� ������� �����������, ���������������� - ���������� ����������
// (Internal) ��� ������� ������������ (External)
namespace ModeSignals
{
    bool IsInternal();

    bool IsExternal();
}


// ��� �����, � ������� ����������� ���� ����� ����������� ������� �� ����� � ������, ����������� ��
// ������� �����, ��������� � ������.
namespace ModeOffline
{
    void Update();
    void Enable();
    bool IsEnabled();

    // ������� ����� �� ���������� ������, ��������� � ����������
    void OpenTheLock();

    // �������� ������ ������� �� ���������� ������, ��������� � ����������
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

    // ��� ����� "Read", ����������� ��������� ������ �� �����
    static bool IsWrite() { return current == Write; }

    static bool IsWG() { return current == WG; }

    static bool IsUART() { return current == UART; }

    // �������� �� �������������� ���������
    static bool IsExtended() { return IsRead() || IsWrite(); }

    // ��������� � ���������� ������ - ����������� ������- � ���������������� ����
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

    bool   enabled;     // ���� true - ������������ �� ������. ����� - ���
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
