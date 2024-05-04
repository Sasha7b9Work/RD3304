// 2022/6/18 10:53:35 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include <gd32e23x.h>


struct Port
{
    enum E
    {
        _A,
        _B,
        _C,
        _D,
        _E,
        _F,
        Count
    };
};

struct PinMode
{
    enum E
    {
        Input,
        OutputPP,
        AF_PP,
        AF_OD,
        Analog,
        Count
    };
};

struct Pulling
{
    enum E
    {
        Up,
        Down,
        None,
        Count
    };
};

struct Pin
{ //-V802
    Pin(Port::E _port, uint16 _pin, PinMode::E _mode, Pulling::E _pull);
    void Init() const;
    void ToLow() const;
    void ToHi() const;
    void Set(bool) const;
    bool IsHi() const;
private:
    uint       pull;
    uint16     pin;
    uint       gpio;
    PinMode::E mode;
};


struct PinOutputPP : public Pin
{
    PinOutputPP(Port::E _port, uint16 _pin, Pulling::E _pull) : Pin(_port, _pin, PinMode::OutputPP, _pull) {}
};


extern PinOutputPP        pinTXD1;      // USART2 Приём/передача

extern PinOutputPP        pinLOCK;
extern PinOutputPP        pinALARM;

extern PinOutputPP        pinENN;       // CLRC663

