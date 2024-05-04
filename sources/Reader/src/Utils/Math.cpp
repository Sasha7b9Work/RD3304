// 2023/10/02 13:29:10 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Utils/Math.h"


const float Math::PI = 3.14159265358979323846f;


bool Math::ArraysEqueals(uint8 *array1, uint8 *array2, uint8 *array3, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (*array1 != *array2)
        {
            return false;
        }

        if (*array1 != *array3)
        {
            return false;
        }

        array1++;
        array2++;
        array3++;
    }

    return true;
}


namespace Math
{
    static uint16 nCrcTblValid = 0;
    // preset: CRC Table not initialized
    static uint16 cCrcTable[256];
    // CRC table - working copy

    // generate the table for POLY == 0x1012
    static int fCrcTblInit(uint16 *pTbl)
    {
        int ii, jj;
        uint16 ww;

        for (ii = 0; ii < 256; ii++)
        {
            ww = (uint16)(ii << 8);
            for (jj = 0; jj < 8; jj++)
            {
                if (ww & 0x8000)
                {
                    ww = (uint16)((ww << 1) ^ 0x1021);
                }
                else
                {
                    ww = (uint16)(ww << 1);
                }
            }
            pTbl[ii] = ww;
        }
        return 1;
    }
}


uint16 Math::CalculateCRC(const void *buffer, int size)
{
    const uint8 *data = (const uint8 *)buffer;

    uint16 nCrc;
    int ii;

    if (nCrcTblValid == 0)
    {
        nCrcTblValid = (uint16)fCrcTblInit(&cCrcTable[0]);
    }
    for (ii = 0, nCrc = 0x1D0F; ii < size; ii++)
    {
        nCrc = (uint16)((nCrc << 8) ^ cCrcTable[((nCrc >> 8) ^ data[ii]) & 0xFF]);
    }
    return nCrc;
}
