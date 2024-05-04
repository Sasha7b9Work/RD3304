// 2023/06/08 12:00:44 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Utils/String.h"
#include "Utils/StringUtils.h"
#include <cstring>
#include <cstdlib>
#include <climits>


int ParseBuffer::GetLength() const
{
    return (int)std::strlen(data);
}


char *SU::GetString(const char *data, int num, char *out, pchar *first_symbol)
{
    if (num == 1)
    {
        out[0] = '\0';

        uint len = std::strlen(data);

        for (uint i = 0; i < len; i++)
        {
            if (data[i] == ' ' || data[i] == 0)
            {
                out[i] = '\0';
                return out;
            }

            out[i] = data[i];
        }

        if (first_symbol)
        {
            *first_symbol = data;
        }

        return out;
    }

    num--;

    out[0] = 0;

    uint size = std::strlen(data);

    uint pos_start = 0;

    for (uint i = 0; i < size; i++)
    {
        if (i == size - 1)
        {
            return out;
        }
        if (data[i] == ' ')
        {
            num--;

            if (num == 0)
            {
                pos_start = i + 1;
                break;
            }
        }
    }

    uint pos_end = pos_start;

    for (uint i = pos_end; i <= size; i++)
    {
        if (data[i] == ' ' || data[i] == '\0')
        {
            pos_end = i;
            break;
        }
    }

    if (pos_end > pos_start)
    {
        int index = 0;

        if (first_symbol)
        {
            *first_symbol = &data[pos_start];
        }

        for (uint i = pos_start; i < pos_end; i++)
        {
            out[index++] = data[i];
        }

        out[index] = 0;
    }

    return out;
}


char *SU::GetWord(const char *data, int num, ParseBuffer *out)
{
    return GetString(data, num, out->data);
}


pchar SU::PointerWord(pchar data, int num)
{
    if (num == 0 || num == 1)
    {
        return data;
    }

    num--;

    while (*data)
    {
        if (*data == ' ')
        {
            num--;

            if (num == 0)
            {
                return data + 1;
            }
        }

        data++;
    }

    return data;
}


String<> SU::ItoA(uint value, int base)
{
    char buffer[32] = { '\0' };

    int i = 30;
    for (; value && i; --i, value /= (uint)base)
    {
        buffer[i] = "0123456789ABCDEF"[value % (uint)base];
    }

    return String<>(&buffer[i + 1]);
}


bool SU::AtoInt(pchar str, int *value)
{
    char *end;

    int r = std::strtol(str, &end, 10);

    if ((end != (str + std::strlen(str))) || r == LONG_MAX || r == LONG_MIN)
    {
        return false;
    }

    *value = r;

    return true;
}


bool SU::AtoUInt(pchar str, uint *value)
{
    char *end;

    uint r = (uint)std::strtoull(str, &end, 10);

    if ((end != (str + std::strlen(str))) || r == ULONG_MAX)
    {
        return false;
    }

    *value = r;

    return true;
}


bool SU::AtoUInt64(pchar str, uint64 *value)
{
    char *end;

    uint64 r = std::strtoull(str, &end, 10);

    if ((end != (str + std::strlen(str))) || r == ULLONG_MAX)
    {
        return false;
    }

    *value = r;

    return true;
}


pchar SU::GetFromSlash(pchar string, int num_slash)
{
    pchar pointer = string + std::strlen(string);

    if (num_slash <= 0)
    {
        return string;
    }

    while (pointer > string)
    {
        if (*pointer == '\\' || *pointer == '/')
        {
            num_slash--;

            if (num_slash == 0)
            {
                return pointer + 1;
            }
        }

        pointer--;
    }

    return string;
}
