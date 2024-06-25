// 2023/09/21 14:50:49 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once

/*
*   Здесь звуковые и световые реакции считывателя на разные события
*/


namespace Signals
{
    void GoodUserCard(bool sound);

    void BadUserCard(bool sound);

    void GoodMasterCard();

    void BadMasterCard();
}
