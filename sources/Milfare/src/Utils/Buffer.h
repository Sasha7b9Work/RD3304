#pragma once
#include <cstring>


template <class T, int size_buffer>
class Buffer
{
public:

    Buffer() : size(0) {}  //-V730

    bool IsFull() const
    {
        return Size() == size_buffer;
    }

    // Удвалить первые n байт
    void RemoveFirst(int n)
    {
        if (n >= size)
        {
            Clear();
        }
        else
        {
            size -= n;

            T temp[size_buffer];

            std::memcpy(temp, buffer, size_buffer * sizeof(T));

            std::memset(buffer, 0xFF, size_buffer * sizeof(T));

            std::memmove(buffer, temp + n, (uint)size * sizeof(T));
        }
    }

    const T *Data() const { return buffer; }

    pchar DataChar() const { return (pchar)Data(); }

    // Возвращает количество элементов в буфере
    int Size() const
    {
        return size;
    }

    bool Append(T elem)
    {
        if (size == size_buffer)
        {
            return false;
        }

        buffer[size++] = elem;

        return true;
    }

    bool Append(const void *data, int _size)
    {
        if (Size() + _size > size_buffer)
        {
//            LOG_WRITE_TRACE("!!! ERROR !!! Override buffer");
            return false;
        }

        std::memcpy(&buffer[size], data, (uint)_size * sizeof(T));
        size += _size;

        return true;
    }

    void Clear()
    {
        size = 0;

        std::memset(buffer, 0xFF, size_buffer * sizeof(T));
    }

    // Возвращает позицию первого элемента последовательности data в buffer, если таковая имеется. Иначе : -1.
    int Position(const void *data, int num_bytes) const
    {
        if (num_bytes > size)
        {
            return -1;
        }

        for (int i = 0; i <= size - num_bytes; i++)
        {
            void *pointer = (void *)&buffer[i];

            if (std::memcmp(pointer, data, (uint)num_bytes) == 0)
            {
                return i;
            }
        }

        return -1;
    }

    const T &operator[](int i) const
    {
        if (i >= 0 && i < Size())
        {
            return buffer[i];
        }

        static T null(0);

        return null;
    }

    T &operator[](int i)
    {
        if (i >= 0 && i < Size())
        {
            return buffer[i];
        }

        static T null(0);

        return null;
    }

protected:

    int size;

    T buffer[size_buffer];
};


class Buffer512 : public Buffer<uint8, 512>
{
public:
    Buffer512() : Buffer<uint8, 512>() { }
};


template<int size_buffer>
class StackBuffer
{
public:
    uint8 *DataU8()
    {
        return buffer;
    }
private:
    uint8 buffer[size_buffer];
};
