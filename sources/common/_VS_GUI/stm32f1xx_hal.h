// 2024/01/12 21:41:52 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#define  GPIO_MODE_INPUT                        0x00000000u
#define  GPIO_MODE_OUTPUT_PP                    0x00000001u
#define  GPIO_MODE_AF_PP                        0x00000002u
#define  GPIO_MODE_AF_OD                        0x00000012u 

#define  GPIO_PULLUP        0x00000001u
#define  GPIO_NOPULL        0x00000000u


#define GPIO_PIN_0                 ((uint16_t)0x0001)  /* Pin 0 selected    */
#define GPIO_PIN_1                 ((uint16_t)0x0002)  /* Pin 1 selected    */
#define GPIO_PIN_2                 ((uint16_t)0x0004)  /* Pin 2 selected    */
#define GPIO_PIN_3                 ((uint16_t)0x0008)  /* Pin 3 selected    */
#define GPIO_PIN_4                 ((uint16_t)0x0010)  /* Pin 4 selected    */
#define GPIO_PIN_5                 ((uint16_t)0x0020)  /* Pin 5 selected    */
#define GPIO_PIN_6                 ((uint16_t)0x0040)  /* Pin 6 selected    */
#define GPIO_PIN_7                 ((uint16_t)0x0080)  /* Pin 7 selected    */
#define GPIO_PIN_8                 ((uint16_t)0x0100)  /* Pin 8 selected    */
#define GPIO_PIN_9                 ((uint16_t)0x0200)  /* Pin 9 selected    */
#define GPIO_PIN_10                ((uint16_t)0x0400)  /* Pin 10 selected   */
#define GPIO_PIN_11                ((uint16_t)0x0800)  /* Pin 11 selected   */
#define GPIO_PIN_12                ((uint16_t)0x1000)  /* Pin 12 selected   */
#define GPIO_PIN_13                ((uint16_t)0x2000)  /* Pin 13 selected   */
#define GPIO_PIN_14                ((uint16_t)0x4000)  /* Pin 14 selected   */
#define GPIO_PIN_15                ((uint16_t)0x8000)  /* Pin 15 selected   */
#define GPIO_PIN_All               ((uint16_t)0xFFFF)  /* All pins selected */

#define GPIO_PULLDOWN 0


typedef struct
{
    uint CRL;
    uint CRH;
    uint IDR;
    uint ODR;
    uint BSRR;
    uint BRR;
    uint LCKR;
} GPIO_TypeDef;


extern GPIO_TypeDef sGPIOA;
extern GPIO_TypeDef sGPIOB;
extern GPIO_TypeDef sGPIOC;
extern GPIO_TypeDef sGPIOD;
extern GPIO_TypeDef sGPIOE;

#define GPIOA &sGPIOA
#define GPIOB &sGPIOB
#define GPIOC &sGPIOC
#define GPIOD &sGPIOD
#define GPIOE &sGPIOE

struct SPI_HandleTypeDef
{

};


typedef struct
{
    uint Pin;       /*!< Specifies the GPIO pins to be configured.
                             This parameter can be any value of @ref GPIO_pins_define */

    uint Mode;      /*!< Specifies the operating mode for the selected pins.
                             This parameter can be a value of @ref GPIO_mode_define */

    uint Pull;      /*!< Specifies the Pull-up or Pull-Down activation for the selected pins.
                             This parameter can be a value of @ref GPIO_pull_define */

    uint Speed;     /*!< Specifies the speed for the selected pins.
                             This parameter can be a value of @ref GPIO_speed_define */
} GPIO_InitTypeDef;


#define  GPIO_SPEED_LOW     0
#define  GPIO_SPEED_MEDIUM  1
#define  GPIO_SPEED_FAST    2
#define  GPIO_SPEED_HIGH    3

#define GPIO_MODE_OUTPUT_OD 0

#define __HAL_AFIO_REMAP_PD01_ENABLE()

#define HAL_NVIC_SetPriority(x, y, z)
#define HAL_NVIC_EnableIRQ(x)

#define GPIO_PIN_RESET 0U
#define GPIO_PIN_SET   0U

void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16 GPIO_Pin, uint PinState);
void HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init);
uint HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16 GPIO_Pin);

inline void HAL_NVIC_SystemReset() { }