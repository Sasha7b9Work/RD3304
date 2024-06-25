// 2023/06/09 22:32:04 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Utils/String.h"


// ������ ��� ��������������
namespace Password
{
    // ���������� ������, ������������ ��� ��������������. (-1) - ������ �� ������������
    uint Current();

    String<> CurrentText();

    // � �������� ������ ��� �������������� ����� �������������� ���������� ������ - ��������� ���� �������������
    void SetInternal();

    // ��������� ��������������
    void Null();

    // ������ ���������� ������
    void Set(uint);
}
