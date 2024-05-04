#include "config.h"
#include "Utils.h"

uint32_t ntohl( uint32_t a )
{
    uint8_t byte0 = (uint8_t)a;
    uint8_t byte1 = (uint8_t)(a >> 8);
    uint8_t byte2 = (uint8_t)(a >> 16);
    uint8_t byte3 = (uint8_t)(a >> 24);

    return (uint32_t)((byte0 << 24) | (byte1 << 16) | (byte2 << 8) | (byte3));
}
