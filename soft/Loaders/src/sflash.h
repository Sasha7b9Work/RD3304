#ifndef _SFLAH_H_
#define _SFLAH_H_

#include <stdint.h>
void spi_init( void ) ;

void sflash_read( uint32_t addr, uint32_t size, uint8_t *buf );
uint32_t sflash_id( void );

#endif
