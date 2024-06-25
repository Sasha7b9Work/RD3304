// 2024/06/06 09:06:35 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include <cstring>

/*
*   Фильтрует входную последовательность путём выыбора из трёх последних значений
*   среднего
*/


template<class T>
struct FiltrMiddleOf3
{
    T Push(T elem)
    {
        std::memmove(&buffer[0], &buffer[1], sizeof(T) * 2);

        buffer[2] = elem;

        return Middleof3(buffer[0], buffer[1], buffer[2]);

    }
private:
    T buffer[3] = { T(0), T(0), T(0) };

    T Middleof3(T a, T b, T c)
    {
        if ((a <= b) && (a <= c))
        {
            return (b <= c) ? b : c;
        }
        else if ((b <= a) && (b <= c))
        {
            return (a <= c) ? a : c;
        }
        return (a <= b) ? a : b;
    }
};
