// 2022/6/18 10:53:35 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "system.h"


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
    bool IsLow() const { return !IsHi(); }
private:
    uint   pull;
    uint16 pin;
#ifdef MCU_GD
    uint       gpio;
    PinMode::E mode;
#else
    void   *gpio;
    uint16  mode;
#endif
};


struct PinInput : public Pin
{
    PinInput(Port::E _port, uint16 _pin, Pulling::E _pull) : Pin(_port, _pin, PinMode::Input, _pull) {}
};


// Антидребезговый пин
struct PinInputAntiBounce : public PinInput
{
    PinInputAntiBounce(Port::E _port, uint16 _pin, Pulling::E _pull) :
        PinInput(_port, _pin, _pull),
        prev_state(false),
        time_next_read(0)
    { }

    bool IsHi(bool anti_bounce = true);
    bool IsLow(bool anti_bounce = true);

private:
    static const uint time_delay = 50;  // Время антидребезга контакта
    bool prev_state;                    // В таком состоянии находился пин в предыдущее чтение
    uint time_next_read;                // Не ранее этого времени можно проверять состояние входа
};


struct PinOutputPP : public Pin
{
    PinOutputPP(Port::E _port, uint16 _pin, Pulling::E _pull) : Pin(_port, _pin, PinMode::OutputPP, _pull) {}
};


struct PinAF_PP : public Pin
{
    PinAF_PP(Port::E _port, uint16 _pin) : Pin(_port, _pin, PinMode::AF_PP, Pulling::Up) {}
};


struct PinAF_OD : public Pin
{
    PinAF_OD(Port::E _port, uint16 _pin) : Pin(_port, _pin, PinMode::AF_OD, Pulling::Up) {}
};


struct PinAnalog : public Pin
{
    PinAnalog(Port::E _port, uint16 _pin) : Pin(_port, _pin, PinMode::Analog, Pulling::None) {}
};


extern PinInput           pinIRQ_SNS;   // Акселерометр

extern PinOutputPP        pinTXD1;      // USART2 Приём/передача

extern PinInputAntiBounce pinLG;
extern PinInputAntiBounce pinLR;
extern PinInput           pinSND;

extern PinOutputPP        pinLOCK;
extern PinOutputPP        pinALARM;

extern PinOutputPP        pinENN;       // CLRC663
extern PinInput           pinIRQ_TRX;   // CLRC633

#ifndef MCU_GD

    extern PinAF_PP           pinTXD2;      // USART2
    extern PinInput           pinRXD2;      // USART2

    extern Pin                pinBEEP;
    extern Pin                pinBEEN;

    extern PinAnalog          pinVIN;

    extern PinInput           pinMISO;      // SPI1
    extern PinAF_PP           pinSCK;       // SPI1
    extern PinAF_PP           pinMOSI;      // SPI1
    extern PinOutputPP        pinSTRX;      // SPI1 CS Reader
    extern PinOutputPP        pinSRAM;      // SPI1 CS SRAM

    #ifdef TYPE_BOARD_771

        extern PinOutputPP      pinENP;     // LP5012

    #endif

#endif
