// 2024/01/12 14:48:25 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace AccessCards
{
    // Установить доступ для карт
    void Set(pchar);

    // Возвращает true, если у карты с номером number есть доступ
    bool Access(uint number);
}
