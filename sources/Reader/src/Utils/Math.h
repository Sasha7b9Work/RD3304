// 2023/10/02 13:29:03 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Math
{
    extern const float PI;

    bool ArraysEqueals(uint8 *array1, uint8 *array2, uint8 *array3, int size);

    inline uint Hash(uint hash, char byte)
    {
        return (uint8)byte + (hash << 6) + (hash << 16) - hash;
    }

    inline uint CalculateHash(uint address, int size)
    {
        uint crc = 0;

        uint8 *data = (uint8 *)address;

        for (int i = 0; i < size; i++)
        {
            crc = Math::Hash(crc, (char)*data++);
        }

        return crc;
    }

    uint16 CalculateCRC(const void *, int);
}
