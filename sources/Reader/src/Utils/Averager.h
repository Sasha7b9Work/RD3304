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

    T Pop(int index)
    {
        return buffer[index];
    }

    T Get()
    {
        uint64 sum = 0;

        for (int i = 0; i < num_elements; i++)
        {
            sum += buffer[i].raw;
        }

        return StructDataRaw((int16)(sum / num_elements));
    }
    int NumElements() const { return num_elements; }
    void Reset() { num_elements = 0; }
private:
    T buffer[size_buffer];
    int num_elements = 0;
};
