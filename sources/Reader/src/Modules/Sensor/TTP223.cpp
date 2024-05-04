// 2024/03/25 14:26:49 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/Sensor/TTP223.h"
#include "Modules/CLRC66303HN/CommandsCLRC663.h"
#include "Hardware/HAL/HAL.h"
#include <cstring>
#include <system.h>


void TTP223::Init()
{
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_1);
}


void TTP223::Update()
{
    static bool pressed = false;

    bool state = gpio_input_bit_get(GPIOA, GPIO_PIN_1) != RESET;

    if (pressed != state)
    {
        pressed = state;

        if (state)
        {
            HAL_USART::UART::TransmitF("12:34:56:78:9A:BC:DE\r\n");
        }
        else
        {
            HAL_USART::UART::TransmitF("DE:BC:9A:78:56:34:12\r\n");
        }
    }
}
