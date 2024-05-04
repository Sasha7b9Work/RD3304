#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>

#define NTOH(blk) ntohl(*((uint32_t*)(blk)) )

uint32_t ntohl( uint32_t a );

#endif
