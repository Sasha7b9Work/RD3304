// 2022/7/3 11:16:31 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Modules/CLRC66303HN/CommandsCLRC663.h"
#include "Modules/CLRC66303HN/CLRC66303_def.h"
#include "Reader/Reader.h"


namespace CLRC66303HN
{
    void Init();

    bool UpdateNormalMode();

    // new_auth true говорит о том, что нужно выводить сообщение, даже если пароль не изменился
    bool UpdateExtendedMode(const TypeAuth &, bool new_auth);
}
