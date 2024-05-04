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

    // В pointer_before возвращается адрес первого символа
    char *GetString(pchar data, int num, char *out, pchar *first_symbol = nullptr);

    // Возвращает указатель на слово num
    pchar PointerWord(pchar data, int num);
    String<> ItoA(uint value, int base);
    bool AtoInt(pchar, int *value);
    bool AtoUInt(pchar, uint *value);
    bool AtoUInt64(pchar, uint64 *value);

    // Возвращает указатель на строку, начинающуюся с num_slash-го слэша (прямого или обратного) с конца строки.
    // Если столько слешей нету, возвращает всю строку целиком
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
