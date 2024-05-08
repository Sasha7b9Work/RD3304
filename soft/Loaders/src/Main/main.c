

/*
 Загрузчик должен запускаться только через аппаратныя рестарт, т.к. 
 расчитан на состояние аппаратуры после рестарта.
*/
#include "stm32f10x.h"

#include <string.h>
#include <stdint.h>
#include "../config.h"
//#include "iap.h"
#include "../booter_aes.h"
#include "../booter_crc.h"

#include "../fmw.h"

#include "../sflash.h"
#include "../Update.h"

#include "../Hardware/hrd_RCC.h"
#include "../Hardware/hrd_Led.h"

///////////////////////////////////////////////////////
void hrd_ConfigurePins()
{
 ///////////////////////////////////////////////////////
 // настраиваем пины
 #define FINPUT    0x4UL  // вход без подтяжек
 #define PULLINPUT 0x8UL  // вход c подтяжками
 #define ALTOUT    0xBUL  // альтернативный выход 50МГц
 #define OUT10     0x1UL  // выход на 10МГц
 #define ANIN      0x00UL // аналоговый режим
 
// GPIOB->BSRR = 1<<0; // сигнал, разрешающий выходы 

 AFIO->MAPR |= AFIO_MAPR_SPI1_REMAP;
 
 GPIOA->BSRR = (1<<15)|(1<<12);
 
 GPIOA->CRL= 0
            |(FINPUT << 0)    // PA0 
            |(FINPUT<< 4)    //Eth PA1 In    Floating input (reset state)
            |(FINPUT<< 8)    // PA2 UART2_TX Eth PA2 MDIO  InOut AF output push-pull highspeed (50 MHz)
            |(FINPUT<<12)    // PA3 UART2 RX
            |(FINPUT  <<16)    // PA4 DAC
            |(FINPUT<<20)    // PA5 SPI1 SCK
            |(FINPUT<<24)    // PA6 SPI1 MISO
            |(FINPUT<<28)    // PA7 SPI1 MOSI
            ;

            
 GPIOA->CRH =(FINPUT<< 0)     // PA8 
            |(FINPUT<< 4)    // PA9   UART1_TX     терминал  UART1 PA.9 output alternate
            |(FINPUT<< 8)    // PA10  UART1_RX     терминал
            |(FINPUT<<12)    // PA11 
#if PCB_777
            |(OUT10 <<16)    // PA12 
#else
            |(OUT10 <<16)    // PA12 
#endif
            |(0x0UL<<20)     // PA13 SWDIO ?
            |(0x0UL<<24)     // PA14 SWDCLK ?
//#if PCB_771
//            |(OUT10<<28)     // PA15 SPI1 NSS
//#else
//            |(OUT10<<28)     //
//#endif
            ;

 // порт B
 GPIOB->CRL=(FINPUT<< 0)    // PB0 #OE_SR разрешение выходов сдвиговых регшистров
           |(FINPUT<< 4)   // PB1 DE5
           |(FINPUT<< 8)   // PB2 
           |(ALTOUT<<12)  // PB3 SPI1(remap) SPI_SCK альтернативный выход
           |(FINPUT<<16)  // PB4 SPI1(remap) SPI_MISO вход
           |(ALTOUT<<20)  // PB5 SPI1((remap)SPI_MOSI альтернативный выход
           |(FINPUT<<24)   // PB6 
           |(FINPUT<<28)   // PB7 NSS_E
           ;
 GPIOB->CRH=(FINPUT<<0)     // PB8 
           |(FINPUT<<4)     // PB9 DE3
           |(FINPUT<<8)     // PB10 TX3
           |(FINPUT<<12)   //Eth PB11 E_TXEN RX3
           |(FINPUT<<16)   //Eth PB12 E_TXD0
           |(FINPUT<<20)   //Eth PB13 E_TXD1 NSS3
           |(FINPUT<<24)   // PB14 DIO1
           |(FINPUT<<28)   // PB15 DIO2
           ;
 
 //GPIOC->CRL = (ANIN<<0)          // PC0   T_BAT
 //            |(ALTOUT<<4 )      // PC1 Eth PC1 MDIOUT    Out   AF output push-pull highspeed (50 MHz)
 //            |(ANIN<<8 )        // PC2 измерение питания
 //            |(OUT10<<12 )      // PC3 сброс phy/mediateck ****
 //            |(PULLINPUT<<16 )     // PC4  dio10
 //            |(PULLINPUT<<20 )     // PC5  dio9
 //            |(PULLINPUT<<24 )      // PC6 DIO3
 //            |(PULLINPUT<<28 )     // PC7 DIO4
 //            ;
 //GPIOC->CRH = ( PULLINPUT<<0)      // PC8   DIO5
 //            |( PULLINPUT<<4)      // PC9   DIO6
 //            |( ALTOUT <<8 )    // PC10 UART4_TX1
 //            |( FINPUT<<12 )    // PC11 UART4_RX1
 //            |( ALTOUT<<16 )    // PC12 UART5_TX
 //            |(0x1<<29)         // PC13 
 //            ;
 //GPIOD->CRL = (ALTOUT<<8)       // PD2   UART5 RX
 //             ;
 
 
}

/* Main Program */
int main (void) 
{
 // направление для LED (red green blue)
// LPC_GPIO1->DIR = (1<<2)|(1<<1)|(1<<4);
 __disable_irq(); // отключаем прерывания на время инициализации
 
 
 // Настролйка тактовой частоты и подключение переферии
 hrd_RCC_Init();

 AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE; //JTAG-DP Disabled and SW-DP Enabled
 hrd_ConfigurePins();

 redOff();
 
 SysTick->CTRL = (1<<2)|(1<<0); // Control and Status Register, cpuclk, start(0)

  uint32_t magicNum = *MAGIC_NUM_ADDR;  
 // магическое число от основной программы (если она спровоцировала рестарт) 
 // *((uint32_t*)0x10000000) = 0;
 *MAGIC_NUM_ADDR = 0;

 spi_init( ) ;  // инициализация SPI
 
 Init_CRC32();

 volatile uint32_t sfid;
 sfid = sflash_id( );
 //sflash_read( UPDATE_START_ADDR, 8*4, fileHeader );
 // читаем заголовок файла
 read_header();
 
 //((void(*)(void))(*((unsigned long*)(MAIN_PROG_FIRST_ADDR + 4))))();
 // проверка наличия файла образа в Serial flash
// if( *(uint32_t*)fileHeader == 0x30314643 ) // проверка сигнатуры файла


 if( check_header( ) )
   {
    uint32_t fileProgramVersion;
    //fileProgramVersion = NTOH( &fileHeader[12] );
    fileProgramVersion = get_file_program_version( );
    // сверить версию программы в файле с той что находится в ROM
    uint32_t romProgramVersion = *((uint32_t*) (MAIN_PROG_FIRST_ADDR +516));
    if( fileProgramVersion != romProgramVersion )
      {// версии не совпали, теперь проверяем файл образа
       //uint32_t progSig = *((uint32_t*)&fileHeader[8]);
       uint32_t progSig = get_file_program_sig();
       if( progSig == PROGRAM_SIGNATURE )
         {
          if( check_file() )
            {// контрольная сумма рбраза верная, можно прошивать
             greenOn();
             doUpdate( /*fileHeader*/ );
             greenOff();
            }
         }
       
      }
   }
 
 if( magicNum != 0x12345678 ) // в ОЗУ нет признака принудительной отмены запуска основной программы
   { 
    if( CheckMainProgControlSum() || magicNum==0xFEDCBA98 ) //сходится контрольная сумма или в ОЗУ есть признак разрешающий запуск с неверной контрольной суммой
      {
      __enable_irq(); // отключаем прерывания на время инициализации
         { // передача управления на стартовый адрес, прерывания все еще запрещены
          // стартовый адрес это второе слово от начала области основной программы
          ((void(*)(void))(*((unsigned long*)(MAIN_PROG_FIRST_ADDR + 4))))();
         }
      }
   } 
 
 while (1)                                // Loop forever 
  {// тут можно выдать сообщение в UART
  // sflash_read( UPDATE_START_ADDR, 1024+16, buffer )         ;
  }
}

