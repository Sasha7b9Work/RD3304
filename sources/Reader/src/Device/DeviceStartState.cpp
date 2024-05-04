// 2023/12/24 16:50:02 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device/Device.h"
#include "Hardware/Timer.h"
#include "Modules/Indicator/Indicator.h"
#include "Settings/Settings.h"


namespace Device
{
    namespace StartState
    {
        static const uint TIME_WAIT = 1;

        // Проверка на замыкание между pin_write и pin_read
        static bool FuncVerifyShortCuit(PinOutputPP &pin_write, PinInput &pin_read, bool invert)
        {
            pin_write.Init();
            pin_write.ToHi();

            Timer::Delay(TIME_WAIT);

            if (invert)
            {
                if (pin_read.IsHi())
                {
                    return false;
                }
            }
            else
            {
                if (!pin_read.IsHi())
                {
                    return false;
                }
            }

            pin_write.ToLow();

            Timer::Delay(TIME_WAIT);

            return invert ? pin_read.IsHi() : pin_read.IsLow();
        }
    }
}


bool Device::StartState::NeedReset()
{
    PinOutputPP pinWrite(Port::_A, GPIO_PIN_2, Pulling::Down);      // На этот пин будем подавать воздействие

    pinTXD1.Init();                 // Это для того, чтобы пропустить pTX на выход

    pinTXD1.ToLow();

    bool result = FuncVerifyShortCuit(pinWrite, pinSND, false);

    HAL_USART::Init();

    return result;
}


bool Device::StartState::NeedMinimalWG()
{
    PinOutputPP pinWrite(Port::_A, GPIO_PIN_2, Pulling::Down);      // На этот пин будем подавать воздействие

    pinTXD1.Init();                 // Это для того, чтобы пропустить pTX на выход

    pinTXD1.ToLow();

    bool result = FuncVerifyShortCuit(pinWrite, pinSND, true);

    HAL_USART::Init();

    return result;
}


bool Device::StartState::NeedOffline()
{
    PinOutputPP pinWrite(Port::_A, GPIO_PIN_2, Pulling::Down);      // На этот пин будем подавать воздействие

    pinTXD1.Init();                 // Это для того, чтобы пропустить pTX на выход

    pinTXD1.ToLow();

    bool result = FuncVerifyShortCuit(pinWrite, pinLG, false);

    if (!result)
    {
        HAL_USART::Init();
    }

    return result;
}
