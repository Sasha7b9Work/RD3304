// 2023/09/18 09:16:51 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Reader/Card.h"


namespace Event
{
    // Вывести список возможных событий
    void WriteList();

    // В поле считывателя обнаружена карта
    void CardDetected();

    // Карта аутентифицирована или не аутентицфицирована
    void CardReadOK(const UID &, uint number, pchar password_string);

    void CardReadFAIL(const UID &, pchar password_string);

    // Карта удалена
    void CardRemove();
}
