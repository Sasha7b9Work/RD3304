// 2023/06/09 22:33:24 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/CLRC66303HN/Password.h"
#include "Settings/Settings.h"
#include "Utils/StringUtils.h"
#include <cstdio>


namespace Password
{
    static uint current = 0;
}


uint Password::Current()
{
    return current;
}


String<> Password::CurrentText()
{
    if (Current() == (uint)-1)
    {
        return String<>("null");
    }

    char buffer[32];

    std::sprintf(buffer, "%s %s %s %s",
        SU::ItoA((uint)((Current() >> 24) & 0xFF), 16).c_str(),
        SU::ItoA((uint)((Current() >> 16) & 0xFF), 16).c_str(),
        SU::ItoA((uint)((Current() >> 8) & 0xFF), 16).c_str(),
        SU::ItoA((uint)((Current()) & 0xFF), 16).c_str()
    );

    return String<>(buffer);
}


void Password::SetInternal()
{
    current = Settings::PSWD::Get();
}


void Password::Null()
{
    current = (uint)(-1);
}


void Password::Set(uint password)
{
    current = password;
}
