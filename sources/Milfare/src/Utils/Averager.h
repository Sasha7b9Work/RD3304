// 2022/11/06 19:23:41 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include <cstring>


template<class T, int size_buffer>
class Averager //-V730
{
public:
    void Push(T value)
    {
        if (num_elements == size_buffer)
        {
            std::memmove(buffer, buffer + 1, sizeof(T) * (size_buffer - 1));
            num_elements--;
        }
        buffer[num_elements++] = value;
    }

    T Get()
    {
        int64 sum = 0;

        for (int i = 0; i < num_elements; i++)
        {
            sum += (int64)buffer[i].raw;
        }

        return StructDataRaw((int16)(sum / (int64)num_elements));
    }

private:
    T buffer[size_buffer];
    int num_elements = 0;
};
