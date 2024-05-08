#ifndef _IAP_H_
#define _IAP_H_

#include <stdint.h>

uint8_t hrd_Iap_EraseSectors( uint32_t start_addr, uint32_t end_addr);
uint8_t hrd_Iap_FlashWrite  ( uint32_t flash_addr, uint32_t ram_addr, uint32_t len);

#endif /*_IAP_H_*/
