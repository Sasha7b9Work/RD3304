/*! \file booter_crc.h
 *
 * \brief Заголовочный файл для функций расчета контрольной суммы по алгоритму CRC32
*/ 
/*! \addtogroup group_crc32 
 *
 * @{
 */
#ifndef BOOTER_CRC_H
 #define BOOTER_CRC_H
  #include <stdint.h>
  
 // void Init_CRC32( void );
  
  uint32_t ProcessCRC( uint8_t* pData, int nLen, uint32_t crc );
//  uint32_t StepCRC( uint8_t b, uint32_t crc );
  void Init_CRC32( void );
  extern uint32_t CrcTable[256];
 
#endif
/*! @} */

