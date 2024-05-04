// 2023/06/08 11:58:09 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Utils/String.h"


struct ParseBuffer
{
    char data[32];

    int GetLength() const;
};


namespace SU
{
    char *GetWord(pchar data, int num, ParseBuffer *);

    // � pointer_before ������������ ����� ������� �������
    char *GetString(pchar data, int num, char *out, pchar *first_symbol = nullptr);

    // ���������� ��������� �� ����� num
    pchar PointerWord(pchar data, int num);
    String<> ItoA(uint value, int base);
    bool AtoInt(pchar, int *value);
    bool AtoUInt(pchar, uint *value);
    bool AtoUInt64(pchar, uint64 *value);

    // ���������� ��������� �� ������, ������������ � num_slash-�� ����� (������� ��� ���������) � ����� ������.
    // ���� ������� ������ ����, ���������� ��� ������ �������
    pchar GetFromSlash(pchar, int num_slash);

    inline uint Hash(uint hash, char byte)
    {
        return (uint8)byte + (hash << 6) + (hash << 16) - hash;
    }

    inline uint CalculateHash(const void *buffer, int size)
    {
        uint crc = 0;

        const uint8 *data = (const uint8 *)buffer;

        for (int i = 0; i < size; i++)
        {
            crc = Hash(crc, (char)*data++);
        }

        return crc;
    }
}
