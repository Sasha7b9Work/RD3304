// 2023/08/31 13:22:41 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Utils/Log.h"
#include "Hardware/HAL/HAL.h"
#include "Nodes/Upgrader.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>


namespace Log
{
    static int counter = 0;
}


void Log::TraceLine(char *file, int line)
{
    if (!Upgrader::IsCompleted())
    {
        return;
    }

    HAL_USART::UART::TransmitF("Trace : %s : %d", file, line);
}


void Log::Error(char *file, int line, char *format, ...)
{
    if (!Upgrader::IsCompleted())
    {
        return;
    }

    char message[256];

    std::va_list args;
    va_start(args, format);
    std::vsprintf(message, format, args);
    va_end(args);

    char buffer[512];

    std::sprintf(buffer, "!!! ERROR !!! %d : %s     %s:%d", counter++, message, file, line);

    HAL_USART::UART::TransmitF(buffer);
}


void Log::Write(char *format, ...)
{
    if (!Upgrader::IsCompleted())
    {
        return;
    }

    char message[256];

    std::va_list args;
    va_start(args, format);
    std::vsprintf(message, format, args);
    va_end(args);

    HAL_USART::UART::TransmitF(message);
}


void Log::WriteTrace(char *file, int line, char *format, ...)
{
    if (!Upgrader::IsCompleted())
    {
        return;
    }

    char message[256];

    std::va_list args;
    va_start(args, format);
    std::vsprintf(message, format, args);
    va_end(args);

    char buffer[512];

    std::sprintf(buffer, "%d : %s     %s:%d", counter++, message, file, line);

    HAL_USART::UART::TransmitF(buffer);
}


pchar HAL_Status::Name(int status)
{
    static const pchar names[4] =
    {
        "HAL_OK",
        "HAL_ERROR",
        "HAL_BUSY",
        "HAL_TIMEOUT"
    };

    if (status < 4)
    {
        return names[status];
    }

    return "Undefined status";
}
