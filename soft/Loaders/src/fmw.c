
#include "config.h"
#include "fmw.h"
#include "booter_crc.h"
#include "utils.h"
#include "sflash.h"


///////////////////////////////////////////////
#define DATA_BLOCK_OFFSET 512

//! Сюда считаем заголовок файла образа программы
uint8_t fileHeader[ 32 ];

static uint8_t buffer[1024+16];

uint32_t countProgramCRC()
{
 uint8_t *p;
 uint32_t crc, sz;
 p = (uint8_t*)MAIN_PROG_FIRST_ADDR;  // адрес с которого начинается основная программа 
 sz = *((uint32_t*)(MAIN_PROG_FIRST_ADDR+DATA_BLOCK_OFFSET+2*4)); // размер программы 
 if( sz > 0xFFFFFUL )
   {
    return 0;       // слишком большая длина программы
   }
 crc = ProcessCRC( p, DATA_BLOCK_OFFSET+3*4, 0 );                   /* контрольная сумма (4 байта) хранится по смещению 19 слов относительно начала программы и в расчете не участвует */
 crc = ProcessCRC( p+DATA_BLOCK_OFFSET+5*4, sz-(DATA_BLOCK_OFFSET+5*4), crc );       /* так же не участвует дата прошивки */
 return crc;
}

uint8_t CheckMainProgControlSum( void )
{
 uint32_t crc = countProgramCRC();
 volatile uint32_t fcrc = *((uint32_t*)(MAIN_PROG_FIRST_ADDR+512+3*4) ); //  сохраненная контрольная сумма 
 if( (crc == fcrc) || fcrc == 0x12345678 ) // отладочная контрольная сумма
   {
    return 1;
   }
 return 0;
}

int check_file()
{
 uint8_t *p;
 //sflash_read( UPDATE_START_ADDR, 8*4, buffer ); // читаем заголовок файла
 //p = &buffer[6*4];
 p = &fileHeader[6*4];
 
 uint32_t coded_len = NTOH(p);//(((uint32_t)p[0])<<24)|(((uint32_t)p[1])<<16)|(((uint32_t)p[2])<<8)|(((uint32_t)p[3])<<0);
 long file_size = coded_len+32; //длина файла без контрольной суммы
 
 #define STEP   1024
 
 uint32_t crc = 0;
 long len = file_size;
 uint32_t addr = UPDATE_START_ADDR;
 int a =STEP;
 while( len>0 )
   {
    sflash_read( addr, STEP, buffer ); // читаем
    if( a >len )
      a= len;  
    crc= ProcessCRC( buffer, a, crc );
    len-=a;
    addr+=STEP;
   }
 sflash_read( UPDATE_START_ADDR+file_size, 4, buffer ); // читаем контрольную сумму
 if( NTOH( buffer ) == crc )
   return 1;  
 else 
   return 0;  
}


void read_header()
{ // читаем заголовок файла
 sflash_read( UPDATE_START_ADDR, 8*4, fileHeader ); // читаем заголовок файла
}

int check_header( void )
{
 if( *(uint32_t*)fileHeader == 0x30314643 ) // проверка сигнатуры файла
   return 1;
 return 0;  
}

// Возвращает сигнатуру программы из ROM
uint32_t get_file_program_sig( void )
{
 return NTOH( &fileHeader[8] );
} 

uint32_t get_file_program_version( void )
{
 return NTOH( &fileHeader[12] );
} 

uint32_t get_file_coded_len( void )
{
 return NTOH( &fileHeader[6*4] );
} 
