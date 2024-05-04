// 2023/7/3 14:43:18 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#define DEBUG_POINT         { Debug::file = __FILE__, Debug::line = __LINE__; }
//#define DEBUG_POINT_INT     { Debug::file_int = __FILE__, Debug::line_int = __LINE__; }


namespace Debug
{
    extern pchar file;
    extern int line;

    extern int index;

    extern pchar file_int;
    extern int line_int;
}
