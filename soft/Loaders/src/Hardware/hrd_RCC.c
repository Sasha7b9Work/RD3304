#include "stm32f10x.h"
#include "../config.h"
#include "hrd_RCC.h"

 // частота ядра
 #define SYS_CLK (56000000)

void hrd_RCC_Init()
{
 // Disable all interrupts 
 RCC->CIR = 0x00000000;

 ////////////////////////////////////////////////////
  //Устанавливаем 2 цикла ожидания для Flash
 // 1 = частота ядра у нас будет 24 MHz < SYSCLK <= 48 MHz
 // 2 = частота ядра у нас будет 48 MHz < SYSCLK <= 72 MHz
 FLASH->ACR |= 0
               // FLASH_ACR_PRFTBE  // разрешаем префетчер ВКЛЮЧЕНИЕ ПРЕФЕТЧЕРА ВОЗМОЖНО ТОЛЬКО НА НИЗКОЙ СКРОСТИ
                                  // поэтому включаем его до перехода на PLL
 #if SYS_CLK <=24000000
                |(0x00<<0)  // на низкой частоте латентность не нужна
 #elif SYS_CLK <=48000000
                |(0x01<<0)
 #else                
                |(0x02<<0)  // на высоких частотах максимальная латентность
 #endif                
;

 //uint32_t tmpreg = 0;
 uint32_t tmpreg = RCC->CFGR;

 //#define CFGR_PLL_Mask             ((u32)0xFFC0FFFF)

 // Clear PLLSRC, PLLXTPRE and PLLMUL[3:0] bits
 //tmpreg &= CFGR_PLL_Mask;

 // Set the PLL configuration bits
 // умножитель на 12 -> 4MH*12=48MH
 
 /*
 #define MUL_VALUE (SYS_CLK/4000000)

 #if MUL_VALUE >= 16 
  #define RCC_PLLMul ( ((MUL_VALUE-2)&0xF)<<18) |(1UL<<27)
 #else 
  #define RCC_PLLMul ( (MUL_VALUE-2)<<18)
 #endif
 
 tmpreg |= RCC_PLLMul
         |(3UL<<14)   // предделитель для ADC на 8
 #if PCLK1_DIV==2 
         |RCC_CFGR_PPRE1_2 // предделитель на PCLK1
 #endif
 ;

 // Store the new value
 RCC->CFGR = tmpreg;

 // запуск PLL
 RCC->CR |= 1UL<<24;
 // ждем запуска PLL
 while( !(RCC->CR & RCC_CR_PLLRDY))
    ;
 //переключаемся на PLL
 RCC->CFGR |= (2UL<<0);

 PWR->CR |= PWR_CR_DBP; // для доступа к регистрам, связанным с RTC
*/
 // подключаем тактировку на периферийные узлы
 RCC->AHBENR = RCC_AHBENR_DMA1EN
//              |RCC_AHBENR_DMA2EN
              |RCC_AHBENR_SRAMEN  // SRAM clock in sleep mode
              |RCC_AHBENR_FLITFEN // FLITF clock during Sleep mode
               ;

 RCC->APB2ENR |= 0
              | RCC_APB2ENR_AFIOEN     // AFIO
              | RCC_APB2ENR_USART1EN   // UART1
              | RCC_APB2ENR_TIM1EN     // TIM1
              | RCC_APB2ENR_ADC1EN     // ADC1 
              | RCC_APB2ENR_IOPAEN     // PORTA
              | RCC_APB2ENR_IOPBEN     // PORTB
              | RCC_APB2ENR_IOPCEN     // PORTC
              | RCC_APB2ENR_IOPDEN     // PORTD
              | RCC_APB2ENR_SPI1EN     // SPI1
              ;

 RCC->APB1ENR |=RCC_APB1ENR_USART2EN   // UART2
//              | RCC_APB1ENR_USART3EN   // UART3
              //| RCC_APB1ENR_DACEN
//              | RCC_APB1ENR_SPI2EN     // SPI2
//              | RCC_APB1ENR_I2C1EN   // I2C1
//              | RCC_APB1ENR_TIM2EN     // TIM2
//              | RCC_APB1ENR_TIM3EN     // TIM3
//              | RCC_APB1ENR_TIM4EN     // TIM4
//              | RCC_APB1ENR_PWREN      // Power interface clock enable
//              | RCC_APB1ENR_BKPEN      // Backup interface clock enable
//              | RCC_APB1ENR_SPI3EN     // SPI3
//              | RCC_APB1ENR_UART4EN    // UART4
//              | RCC_APB1ENR_UART5EN    // UART5
//              | RCC_APB1ENR_TIM6EN     // TIM6 
//              | RCC_APB1ENR_TIM7EN
//              | RCC_APB1ENR_TIM5EN
              ;
              
 //RCC->BDCR = 0// RCC_BDCR_RTCEN 
 //            |RCC_BDCR_LSEON        // External Low Speed oscillator enable
 //            |RCC_BDCR_RTCSEL_LSE
 //            //|RCC_BDCR_RTCSEL_LSI
 //            ;
 
 //AFIO->MAPR |= AFIO_MAPR_MII_RMII_SEL; // Select RMII

// #if ETH_ENABLE
 // тактирвование MAC включим после после выбора RMII
// RCC->AHBENR  |= RCC_AHBENR_DMA1EN 
//              |  RCC_AHBENR_DMA2EN

//              |  RCC_AHBENR_ETHMACEN
//              |  RCC_AHBENR_ETHMACTXEN
//              |  RCC_AHBENR_ETHMACRXEN
// #endif
                ;
// RCC->CSR |= RCC_CSR_LSION;

}  
