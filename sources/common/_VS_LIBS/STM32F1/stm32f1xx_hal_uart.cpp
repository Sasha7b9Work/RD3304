// 2022/04/27 14:59:31 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include <stm32f1xx_hal.h>


HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef * /*huart*/)
{
    return HAL_OK;
}


HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef * /*huart*/, uint8_t * /*pData*/, uint16_t /*Size*/, uint32_t /*Timeout*/)
{
    return HAL_OK;
}