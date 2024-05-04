// 2023/09/18 12:16:49 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Reader/Messages.h"
#include "Hardware/HAL/HAL.h"
#include <cstring>
#include <cctype>
#include <cstdio>
#include <cstdarg>
#include <cstdlib>


void Message::Send(pchar message)
{
    HAL_USART::UART::TransmitF(message);
}


void Message::Send(int count_tab, pchar message)
{
    SendTAB(count_tab);
    Send(message);
}


void Message::SendRAW(pchar message)
{
    HAL_USART::UART::Transmit(message, (int)std::strlen(message));
}


void Message::SendTAB(int count)
{
    for (int i = 0; i < count; i++)
    {
        char *message = "    ";

        HAL_USART::UART::Transmit(message, (int)std::strlen(message));
    }
}


void Message::SendFormat(char *format, ...)
{
    char message[256];
    std::va_list args;
    va_start(args, format);
    vsprintf(message, format, args);
    va_end(args);

    char buffer[300];

    std::sprintf(buffer, "%s\r\n", message);

    HAL_USART::UART::Transmit(buffer, (int)std::strlen(buffer));
}


void Message::OK()
{
    Send("#OK");
}


void Message::NotImplemented()
{
    HAL_USART::UART::TransmitF("NOT IMPLEMENTED");
}
