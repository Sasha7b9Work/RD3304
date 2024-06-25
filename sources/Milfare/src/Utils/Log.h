// 2023/08/31 13:22:50 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include <Hardware/HAL/HAL.h>


//#define LOG_TRACE()          Log::TraceLine(__FILE__, __LINE__)
//#define LOG_WRITE(...)       Log::Write(__VA_ARGS__)
#define _LOG_WRITE_TRACE(...)
//#define LOG_ERROR(...)       Log::Error(__FILE__, __LINE__, __VA_ARGS__)


namespace Log
{
    void TraceLine(char *, int);

    void Error(char *file, int line, char *format, ...);

    void WriteTrace(char *file, int line, char *format, ...);

    void Write(char *format, ...);
}


namespace HAL_Status
{
    pchar Name(int);
};
