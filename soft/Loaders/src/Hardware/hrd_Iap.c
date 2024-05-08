#include "hrd_Iap.h"
//#include "../config.h"
#include "stm32f10x.h"

//#define FREQ_CCLK  48000000

/* Delay definition */   
#define EraseTimeout             ((u32)0x00000FFF)
#define ProgramTimeout           ((u32)0x0000000F)


/* FLASH Keys */
#define RDP_Key                  ((u16)0x00A5)
#define FLASH_KEY1               ((u32)0x45670123)
#define FLASH_KEY2               ((u32)0xCDEF89AB)

static int wait_for_last_operation()
{
 /* Wait for last operation to be completed */
 int Timeout = 1000000;
 /* Wait for a Flash operation to complete or a TIMEOUT to occur */
 while((FLASH->SR & FLASH_SR_BSY) )
   {
    if( Timeout ==0 )
      return 0;
    Timeout--;
   }
 return 1;  
}

static void flash_unlock()
{
if( FLASH->CR & FLASH_CR_LOCK )
  {// Authorize the FPEC Access
   FLASH->KEYR = FLASH_KEY1;
   FLASH->KEYR = FLASH_KEY2;
  } 
}

/* erase flash sectors from start_addr to end_addr */
// прерывания должны быть зарпрещены заранее
uint8_t hrd_Iap_EraseSectors( uint32_t start_addr, uint32_t end_addr)
{
 uint32_t Page_Address = start_addr;
 
 flash_unlock();
 // Wait for last operation to be completed 
 while( Page_Address < end_addr)
   {     
    if(wait_for_last_operation())
      { 
       // if the previous operation is completed, proceed to erase the page 
       FLASH->CR |= FLASH_CR_PER;
       FLASH->AR = Page_Address; 
       FLASH->CR|= FLASH_CR_STRT;
       // Wait for last operation to be completed
       wait_for_last_operation();
       // if the erase operation is completed, disable the PER Bit
       FLASH->CR &= ~FLASH_CR_PER;
      }
    Page_Address += 1024;
      
  }  
 return 0;
}

/* writes down flash sector                         *
 *   flash_addr - address to begin write            *
 *   ram_addr   - source data address               *
 *   len        - data size (256, 512, 1024, 4096)  */
 // прерывания должны быть зарпрещены заранее
uint8_t hrd_Iap_FlashWrite( uint32_t flash_addr, uint32_t ram_addr, uint32_t len)
{
 flash_unlock();
 
 uint16_t *f_a = (uint16_t *)flash_addr;
 uint16_t *r_a = (uint16_t *)ram_addr;
 
 while(len)
   {
    FLASH->CR |= FLASH_CR_PG;
    *f_a = *r_a;
    wait_for_last_operation();
    
    f_a++;
    r_a++;
    len-=2;
   } 
 FLASH->CR &= ~FLASH_CR_PG;
 return 0;
}
