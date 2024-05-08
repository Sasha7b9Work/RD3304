
#include "LPC13xx.h"
#include "hrd_Globals.h"
#include "hrd_SPI.h"
#include "../scmRTOS/Common/scmRTOS.h"

spi_type spiBus;

extern "C"
__irq void SSP_IRQHandler( void )
{
 spiBus.IRQHandler(LPC_SSP0);
}

/////////////////////////////////////////////////////////


/*****************************************************************************/
/*!
 * ќбмен через шину SPI
 *
 * Ўина уже должна быть захвачена, и нужный CS активизирован
 * \param size количество байт дл€ обмена
 * \param tx указатель на массив с передаваемыми байтами, если 0, то будут переданы 0xFF
 * \param rx указатель на буфер под принимаемые данные, если 0, то принимаемые данные не сохран€юЄтс€
 *
 * \return 1 в случае успеха, иначе 0
 */
template<uint32_t s>int SPI_Bus<s>::swap( uint16_t size, const uint8_t *tx, uint8_t *rx )
{
 return TSpiLpcBus::swapBytes( (LPC_SSP_TypeDef*)s,size,tx,rx);
}


