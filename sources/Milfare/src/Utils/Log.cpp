// 2024/6/26 7:21:22 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Utils/Log.h"
#include <cstdio>


void Log::WriteTrace(char *file, int line, char *format, ...)
{
    char message[256];

    std::va_list args;
    va_start(args, format);
    std::vsprintf(message, format, args);
    va_end(args);

//    char buffer[512];

//    std::sprintf(buffer, "%s     %s:%d", message, file, line);

    std::strcpy(message, message);
}
