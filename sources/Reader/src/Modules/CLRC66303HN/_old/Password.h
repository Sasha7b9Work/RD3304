// 2023/06/09 22:32:04 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Utils/String.h"


// Пароль для аутентификации
namespace Password
{
    // Возвращает пароль, используемый для аутентификации. (-1) - пароль не используется
    uint Current();

    String<> CurrentText();

    // В качестве пароля дла аутентификации будет использоваться внутренний пароль - заводской либо установленный
    void SetInternal();

    // Отключить аутентификацию
    void Null();

    // Задать конкретный пароль
    void Set(uint);
}
