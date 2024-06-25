// 2022/6/18 10:53:59 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Hardware/Timer.h"
#include "system.h"


#ifdef TYPE_BOARD_777

    PinAnalog          pinVIN(Port::_A, GPIO_PIN_0);

    PinInput           pinSND(Port::_A, GPIO_PIN_11, Pulling::None);
    PinInput           pinRXD2(Port::_A, GPIO_PIN_3, Pulling::None);
    PinInputAntiBounce pinLR(Port::_A, GPIO_PIN_10, Pulling::None);

    PinInput           pinMISO(Port::_B, GPIO_PIN_4, Pulling::Up);

    PinAF_PP           pinTXD2(Port::_A, GPIO_PIN_2);
    PinAF_PP           pinSCK(Port::_B, GPIO_PIN_3);
    PinAF_PP           pinMOSI(Port::_B, GPIO_PIN_5);

    PinOutputPP        pinTXD1(Port::_A, GPIO_PIN_9, Pulling::Up);
    PinOutputPP        pinENN(Port::_A, GPIO_PIN_6, Pulling::Up);

    PinOutputPP        pinLOCK(Port::_A, GPIO_PIN_2, Pulling::Up);
    PinOutputPP        pinALARM(Port::_A, GPIO_PIN_3, Pulling::Up);

    Pin                pinBEEP(Port::_A, GPIO_PIN_7, PinMode::AF_PP, Pulling::Up);
    Pin                pinBEEN(Port::_A, GPIO_PIN_8, PinMode::AF_PP, Pulling::Up);
    PinInputAntiBounce pinLG(Port::_B, GPIO_PIN_0, Pulling::None);
    PinInput           pinIRQ_SNS(Port::_D, GPIO_PIN_0, Pulling::Down);
    PinInput           pinIRQ_TRX(Port::_A, GPIO_PIN_4, Pulling::Down);
    PinOutputPP        pinSTRX(Port::_A, GPIO_PIN_15, Pulling::Up);
    PinOutputPP        pinSRAM(Port::_A, GPIO_PIN_12, Pulling::Up);

#endif

#ifdef TYPE_BOARD_771

    PinAnalog          pinVIN(Port::_A, GPIO_PIN_0);

    PinInput           pinSND(Port::_A, GPIO_PIN_11, Pulling::None);
    PinInput           pinRXD2(Port::_A, GPIO_PIN_3, Pulling::None);
    PinInputAntiBounce pinLR(Port::_A, GPIO_PIN_10, Pulling::None);

    PinInput           pinMISO(Port::_B, GPIO_PIN_4, Pulling::Up);

    PinAF_PP           pinTXD2(Port::_A, GPIO_PIN_2);
    PinAF_PP           pinSCK(Port::_B, GPIO_PIN_3);
    PinAF_PP           pinMOSI(Port::_B, GPIO_PIN_5);

    PinOutputPP        pinTXD1(Port::_A, GPIO_PIN_9, Pulling::Up);
    PinOutputPP        pinENN(Port::_A, GPIO_PIN_6, Pulling::Up);

    PinOutputPP        pinLOCK(Port::_A, GPIO_PIN_2, Pulling::Up);
    PinOutputPP        pinALARM(Port::_A, GPIO_PIN_3, Pulling::Up);

    Pin                pinBEEP(Port::_B, GPIO_PIN_0, PinMode::OutputPP, Pulling::Down);
    Pin                pinBEEN(Port::_B, GPIO_PIN_1, PinMode::OutputPP, Pulling::Down);
    PinInputAntiBounce pinLG(Port::_A, GPIO_PIN_8, Pulling::None);
    PinInput           pinIRQ_SNS(Port::_A, GPIO_PIN_5, Pulling::Down);
    PinInput           pinIRQ_TRX(Port::_D, GPIO_PIN_0, Pulling::Down);
    PinOutputPP        pinSTRX(Port::_A, GPIO_PIN_12, Pulling::Up);
    PinOutputPP        pinSRAM(Port::_A, GPIO_PIN_15, Pulling::Up);
    PinOutputPP        pinENP(Port::_B, GPIO_PIN_2, Pulling::Down);

#endif


Pin::Pin(Port::E _port, uint16 _pin, PinMode::E _mode, Pulling::E _pull) :
    pin(_pin)
{
    GPIO_TypeDef *gpios[Port::Count] =
    {
        GPIOA,
        GPIOB,
        GPIOC,
        GPIOD,
        GPIOE
    };

    gpio = gpios[_port];

    uint16 modes[PinMode::Count] =
    {
        GPIO_MODE_INPUT,
        GPIO_MODE_OUTPUT_PP,
        GPIO_MODE_AF_PP,
        GPIO_MODE_AF_OD,
        GPIO_MODE_ANALOG
    };

    mode = modes[_mode];

    uint16 pullings[Pulling::Count] =
    {
        GPIO_PULLUP,
        GPIO_PULLDOWN,
        GPIO_NOPULL
    };

    pull = pullings[_pull];
}



void Pin::Init() const
{
    GPIO_InitTypeDef is =
    {
        pin,
        mode,
        pull,
        GPIO_SPEED_HIGH
    };

    HAL_GPIO_Init((GPIO_TypeDef *)gpio, &is);

    if (pull == GPIO_PULLDOWN)
    {
        ((GPIO_TypeDef *)gpio)->ODR &= ~pin;
    }
    else
    {
        ((GPIO_TypeDef *)gpio)->ODR |= pin;
    }

    if ((GPIO_TypeDef *)gpio == GPIOD && (pin == GPIO_PIN_0 || pin == GPIO_PIN_1))
    {
        if (mode == GPIO_MODE_INPUT || mode == GPIO_MODE_OUTPUT_OD || mode == GPIO_MODE_OUTPUT_PP)
        {
            __HAL_AFIO_REMAP_PD01_ENABLE();
        }
    }
}


void Pin::ToLow() const
{
    HAL_GPIO_WritePin((GPIO_TypeDef *)gpio, pin, GPIO_PIN_RESET);
}


void Pin::ToHi() const
{
    HAL_GPIO_WritePin((GPIO_TypeDef *)gpio, pin, GPIO_PIN_SET);
}


void Pin::Set(bool hi) const
{
    HAL_GPIO_WritePin((GPIO_TypeDef *)gpio, pin, hi ? GPIO_PIN_SET : GPIO_PIN_RESET);
}


bool Pin::IsHi() const
{
    return HAL_GPIO_ReadPin((GPIO_TypeDef *)gpio, pin) == GPIO_PIN_SET;
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
