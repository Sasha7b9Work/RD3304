// 2024/02/28 15:12:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Hardware/Timer.h"
#include "system.h"


// Выводы
PinInputAntiBounce  pinLG(Port::_A, GPIO_PIN_10, Pulling::None);        // 20 def
PinInputAntiBounce  pinLR(Port::_A, GPIO_PIN_11, Pulling::None);        // 21 def
PinInput            pinSND(Port::_B, GPIO_PIN_0, Pulling::None);        // 14 def

// RS485
PinOutputPP         pinTXD1(Port::_A, GPIO_PIN_9, Pulling::Up);         // 19 def

// Картридер
PinInput            pinIRQ_TRX(Port::_B, GPIO_PIN_2, Pulling::Down);    // 16 
PinOutputPP         pinENN(Port::_A, GPIO_PIN_6, Pulling::Up);          // 12 def

// Акселерометр
PinInput            pinIRQ_SNS(Port::_F, GPIO_PIN_0, Pulling::Down);    //  2 def

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


bool PinInputAntiBounce::IsHi(bool anti_bounce)
{
    if (anti_bounce)
    {
        if (TIME_MS < time_next_read)
        {
            return prev_state;
        }

        bool state = PinInput::IsHi();

        if (state != prev_state)
        {
            prev_state = state;
            time_next_read = TIME_MS + time_delay;
        }

        return state;
    }

    return PinInput::IsHi();
}


bool PinInputAntiBounce::IsLow(bool anti_bounce)
{
    return !IsHi(anti_bounce);
}
