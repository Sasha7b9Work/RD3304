  
#include "stm32f10x.h"
#include "config.h"
#include "sflash.h"

//#define PCLK 12000000
#define SPI_FREQ 2000000

#define GPIO_SET_VAL( port_num, pin_num, val ) port_num->MASKED_ACCESS[(1<<(pin_num))] = (val<<(pin_num))

#define SPI SPI1
static void setCS( int s )
{
#if PCB_771
 if( s )
   GPIOA->BRR = (1<<15);
 else 
   GPIOA->BSRR = (1<<15);
 }
#endif
#if PCB_777
 if( s )
   GPIOA->BRR = (1<<12);
 else 
   GPIOA->BSRR = (1<<12);
 }
#endif
void spi_init( ) 
{
/*
 // Используемые пины уже должны быть сконфигурированы !!!
 // настройка пинов для spi

// Настройка пинов для SerialFlash
 LPC_IOCON->PIO3_1  = 0x00;  //SSEL как GPIO
 LPC_GPIO3->DIR |= (1<<1);
 GPIO_SET_VAL( LPC_GPIO3, 1, 1 );
 
 (LPC_SSP0)->CR0   = 7; //8bit frame //((0x07 << DSS) | (FRF_SPI << FRF) | (0x00 << CPOL) | (0x00 << CPHA) |//SPI 8bit
                    //((((ssp_pclk / ssp_clk) / 2 - 1) & 0xFF) << SCR));
 
 (LPC_SSP0)->CPSR  = 0x02;
 //(LPC_SSP0)->IMSC  = 0x00;  //Отключение прерываний


 (LPC_SSP0)->CR1   = 1<<1;  
 // низкоприоритетный обработчик 
// NVIC_SetPriority( SSP0_IRQn, OS_INTERRUPT_PRIORITY );  // уровень прерывания, контролируемый RTOS
*/
 #define SPI_HI_CLK 18000000
 #define DVH (PCLK/SPI_HI_CLK)
 
  #if DVH <= 2
   #define BRVH 0
 #elif DVH <= 4
   #define BRVH 1
 #elif DVH <= 8
   #define BRVH 2
 #elif DVH <= 16
   #define BRVH 2
 #elif DVH <= 32
   #define BRVH 4
 #elif  DVH <= 64
   #define BRVH 5
 #elif DVH <= 128
   #define BRVH 6 
 #elif DVH <= 256
   #define BRVH 7
 #endif


 SPI->CR2 = 0;// SPI_CR2_RXDMAEN;// | SPI_CR2_TXDMAEN /*|SPI_CR2_TXEIE*/;
 
 SPI->CR1=0;
 SPI->CR1= SPI_CR1_SSM
           |SPI_CR1_SSI
           |SPI_CR1_MSTR
           | (1 <<3)  //Fpclk/4
//           |SPI_CR1_CPOL
//           |SPI_CR1_CPHA
           ;
 SPI->CR1 |= SPI_CR1_SPE;
           ;
}

//#define SSPSR_BSY (1UL<<4)
//#define SSPSR_TNF (1UL<<1)
//#define SSPSR_RNE (1UL<<2)

uint8_t SSPTransfer( uint8_t snd )
{
 volatile int r = SPI->SR;
 while( !(SPI->SR & SPI_SR_TXE) )
   ;
 SPI->DR = snd;  // Помещаем данные на передачу (запускаем передачу)
 
 for(;;)
   {
    r = SPI->SR;
    if( r & SPI_SR_RXNE )
      break;
   }
 
// while( !(SPI->SR & SPI_SR_RXNE) ) // Ждем появления данных в буфере приёма
//   ;
 // Считываем принятые данные и возвращаем их  
 snd = SPI->DR;
 return snd;
/*
 // Ждем освобождения места в буфера передачи
 while ( (LPC_SSP0->SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF );
 // Помещаем данные на передачу (запускаем передачу)
 LPC_SSP0->DR = snd;
 // Ждем появления данных в буфере приёма
 while ( (LPC_SSP0->SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE );
 // Считываем принятые данные и возвращаем их
 return LPC_SSP0->DR;
 */
 
}

void sflash_read( uint32_t addr, uint32_t size, uint8_t *r_buf )
{
 setCS( 1 );
 
 SSPTransfer( 0x03 );
 SSPTransfer( addr>>16 );
 SSPTransfer( addr>>8 );
 SSPTransfer( addr );
 
 while( size-- )
   {
    *r_buf = SSPTransfer( 0xFF );
    r_buf++;
   }  
// sflash_transmit( b, 4 );

// sflash_transmit( r_buf, 8 );
 setCS( 0 );
}

uint32_t sflash_id( )
{
 uint32_t res;
 uint8_t *r_buf = (uint8_t*)&res;
 setCS( 1 );
 
 *r_buf++ = SSPTransfer( 0x9F );
 *r_buf++ = SSPTransfer( 0xFF );
 *r_buf++ = SSPTransfer( 0xFF );
 *r_buf++ = SSPTransfer( 0xFF );
 setCS( 0 );
 return res;
}
