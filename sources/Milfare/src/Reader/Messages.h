// 2023/09/18 12:17:01 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Message
{
    // Завершает символами \r\n
    void Send(pchar);

    void Send(int count_tab, pchar);

    // Не завершает симовлами \r\n
    void SendRAW(pchar);

    void SendTAB(int = 1);

    void SendFormat(char *format, ...);

    void OK();

    void NotImplemented();
}
