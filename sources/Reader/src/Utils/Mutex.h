// 2023/06/08 10:58:15 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


class Mutex
{
public:

    Mutex() : busy(false) { }

    void Try()
    {
        busy = true;
    }

    void Release()
    {
        busy = false;
    }

    bool IsBusy() const
    {
        return busy;
    }

private:

    bool busy;
};
