
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
 * ����� ����� ���� SPI
 *
 * ���� ��� ������ ���� ���������, � ������ CS �������������
 * \param size ���������� ���� ��� ������
 * \param tx ��������� �� ������ � ������������� �������, ���� 0, �� ����� �������� 0xFF
 * \param rx ��������� �� ����� ��� ����������� ������, ���� 0, �� ����������� ������ �� ������������
 *
 * \return 1 � ������ ������, ����� 0
 */
template<uint32_t s>int SPI_Bus<s>::swap( uint16_t size, const uint8_t *tx, uint8_t *rx )
{
 return TSpiLpcBus::swapBytes( (LPC_SSP_TypeDef*)s,size,tx,rx);
}


