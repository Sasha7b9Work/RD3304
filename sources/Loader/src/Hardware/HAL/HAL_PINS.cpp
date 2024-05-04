// 2024/02/28 15:12:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "config.h"
#include "Hardware/HAL/HAL_PINS.h"
#include <gd32e23x.h>


// RS485
PinOutputPP         pinTXD1(Port::_A, GPIO_PIN_9, Pulling::Up);         // 19 def

// Картридер
PinOutputPP         pinENN(Port::_A, GPIO_PIN_6, Pulling::Up);          // 12 def

// Акселерометр
PinOutputPP         pinLOCK(Port::_A, GPIO_PIN_2, Pulling::Up);         //  8 def
PinOutputPP         pinALARM(Port::_A, GPIO_PIN_3, Pulling::Up);        //  9 def



Pin::Pin(Port::E _port, uint16 _pin, PinMode::E _mode, Pulling::E _pull) :
    pin(_pin),
    mode(_mode)
{
    uint gpios[Port::Count] =
    {
        GPIOA,
        GPIOB,
        GPIOC,
        GPIOC,
        GPIOC,
        GPIOF
    };

    gpio = gpios[_port];

    uint pulls[Pulling::Count] =
    {
        GPIO_PUPD_PULLUP,
        GPIO_PUPD_PULLDOWN,
        GPIO_PUPD_NONE
    };

    pull = pulls[_pull];
}


void Pin::Init() const
{
    if (mode == PinMode::Input)
    {
        gpio_mode_set(gpio, GPIO_MODE_INPUT, pull, pin);
    }
    else if (mode == PinMode::OutputPP)
    {
        gpio_mode_set(gpio, GPIO_MODE_OUTPUT, pull, pin);
        gpio_output_options_set(gpio, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, pin);
    }
    else if (mode == PinMode::AF_PP)
    {
        gpio_mode_set(gpio, GPIO_MODE_AF, pull, pin);
    }
}


void Pin::ToHi() const
{
    GPIO_BOP(gpio) = (uint)pin;
}


void Pin::ToLow() const
{
    GPIO_BC(gpio) = (uint)pin;
}


void Pin::Set(bool hi) const
{
    if (hi)
    {
        ToHi();
    }
    else
    {
        ToLow();
    }
}


bool Pin::IsHi() const
{
    return gpio_input_bit_get(gpio, pin) == SET;
}
