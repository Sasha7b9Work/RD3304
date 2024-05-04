/*! \file booter_aes.h
 *
 * \brief Заголовочный файл для реализации алгоритма кодирования
 *
*/
/*! \addtogroup group_aes
 * @{
*/ 
#ifndef BOOTER_AES_H
 #define BOOTER_AES_H
 
 #include <stdint.h>
 
 void aesDecrypt( uint8_t *buffer, uint8_t *chainBlock );
 
#endif
/*! @} */
