
#include "Update.h"
#include "config.h"
#include "booter_aes.h"
#include "booter_crc.h"
#include "Utils.h"
#include "fmw.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/hrd_Iap.h"

#include <stdint.h>
#include <string.h>

#include <gd32e23x.h>

/*inline uint32_t ntoh( const uint8_t *p)
{
 return (((uint32_t)p[0])<<24)|(((uint32_t)p[1])<<16)|(((uint32_t)p[2])<<8)|(((uint32_t)p[3])<<0);
}*/

static uint8_t buffer[1024 + 16];

/*! Начальное значение chain blok */
static uint8_t const ChainBlock[16] =
{
 0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF
};

/*! \brief Расшифровка блока сектора
 *
 * Расшифровка блока сектора. Внутри блока находится и контрольная сумма. \n
 * Размер сетора 1024.
 * \param blk указатель на буфер с шифрованными данными
 * \return контрольная сумма из блока
*/
static uint32_t DecipherSector(uint8_t *blk)
{
    int j;
    uint8_t *dst;
    uint32_t crc;
    uint8_t chain[16];
    memcpy(chain, ChainBlock, 16);
    dst = blk;
    for (j = 0; j < (1024 + 16) / 16; j++)
    {
        aesDecrypt(dst, chain);
        dst += 16;
    }
    crc = ntohl(*((uint32_t *)blk)); // первые 16 байт это заголовок контрольной суммой и со случайными данными 
    dst = blk + 16;
    for (j = 0; j < 1024; j++) // сдвиг данных на место заголовка 
    {
        *blk++ = *dst++;
    }
    return crc;
}

void doUpdate( /*const uint8_t *fileHeader */)
{
    hrd_Iap_EraseSectors();

    uint32_t blo_Addr;

    //long coded_len = NTOH(&fileHeader[6*4]);
    long coded_len = get_file_coded_len();
    blo_Addr = UPDATE_START_ADDR + 32; // смещение на размер заголовка

    uint32_t flash_addr = 1024 * 4 + 0x8000000;

    while (coded_len > 0)
    {
        HAL_SPI::ReadBuffer(DirectionSPI::Memory, blo_Addr, buffer, 1024 + 16); // читаем блок
        // дешифруем блок
        uint32_t crc_from_block = DecipherSector(buffer);
        // проверка контрольной суммы
        uint32_t crc = ProcessCRC(&buffer[0], 1024, 0);
        if (crc != crc_from_block)
        {
            break;
        }
        else
        {
            hrd_Iap_FlashWrite(flash_addr, (uint32_t)buffer, 1024);
        }
        blo_Addr += 1024 + 16;
        coded_len -= 1024 + 16;
        flash_addr += 1024;
    }
}
