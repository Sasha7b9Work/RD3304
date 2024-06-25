// 2023/10/16 10:59:45 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Nodes/Communicator.h"


class RingBufferU8
{
public:
    RingBufferU8() { Clear(); }
    static const int SIZE = 512;
    void Clear();
    void Append(uint8 byte);
    bool GetData(BufferUSART &);
    int GetElementCount() const;
private:
    uint8 buffer[SIZE];
    int index_in;
    int index_out;
};


template <class T, int capacity>
class RingBuffer
{
public:
    RingBuffer() : num_elements(0), index_in(0), index_out(0) { }

    void Append(T elem)
    {
        if (IsFull())
        {
            return;
        }

        buffer[index_in] = elem;
        index_in++;

        if (index_in == capacity)
        {
            index_in = 0;
        }

        num_elements++;
    }

    T Pop()
    {
        if (GetElementCount() == 0)
        {
            return T(0);
        }

        num_elements--;

        T result = buffer[index_out++];

        if (index_out == capacity)
        {
            index_out = 0;
        }

        return result;
    }

    int GetElementCount() const
    {
        return num_elements;
    }

    bool IsFull() const
    {
        return (GetElementCount() == capacity);
    }

private:
    T buffer[capacity];
    int num_elements;
    int index_in;
    int index_out;
};
