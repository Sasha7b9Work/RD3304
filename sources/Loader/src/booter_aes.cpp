/*! \file booter_aes.c
 *
 * \brief Реализация алгоритма кодирования
 * 
 * \page page_aes Алгоритм кодирования.
 *
 */
 /*! \defgroup group_aes Модуль реализации алгоритма кодирования
 
 @{
*/
 
 #include "config.h"
 
#ifndef BOOTER_AES_C
 #define BOOTER_AES_C
 #include <stdint.h>
 
 /* ****************************** Алгоритм кодирования *********************************************** */
 /*! \brief Полином для алгоритма кодирования
  *
  *Полином для алгоритма кодирования младшая 8битовая часть от (x^8+x^4+x^3+x+1), т.е. (x^4+x^3+x+1). 
 */
 #define BPOLY 0x1b   
 
 /*! \brief Размер блока для алгоритма кодирования 
  *
  * Размер блока для алгоритма кодирования. Зависит от алгоритма кодирования. Не изменять. 
 */
 #define BLOCKSIZE 16 

 /*! \brief Длина ключа в битах 
  *
  * Длина ключа в битах. Зависит от алгоритма кодирования. Не изменять. 
 */
 #define KEYBITS   128
 /*! \brief Длина ключа в байтах 
  *
  * Длина ключа в байтах. Зависит от алгоритма кодирования. Не изменять. 
 */
 #define KEYLENGTH 16

 /*! \brief Количество повторений кодирования блока  
  * 
  * Количество повторений кодирования блока. Зависит от алгоритма кодирования. Не изменять. 
 */
 #define ROUNDS    10

 /*! \brief Длина расширенного ключа  
  *
  * Длина расширенного ключа. 
 */
 #define EXPANDED_KEY_SIZE (BLOCKSIZE * (ROUNDS+1))

 /*! \brief Расширенный ключ, полученный из 16 байтного ключа стандартным способом, принятым в AES
  *
  * Готовится отдельно и подставляется в программу
 */
 const uint8_t ExKey[ EXPANDED_KEY_SIZE ] =
 {
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x63, 0x61, 0x60, 0x67, 0x66, 0x67, 0x67, 0x6F, 0x6F, 0x67, 0x67, 0x6F, 0x6F, 0x67, 0x67, 0x6F,
  0xE4, 0xE4, 0xC8, 0xCF, 0x82, 0x83, 0xAF, 0xA0, 0xED, 0xE4, 0xC8, 0xCF, 0x82, 0x83, 0xAF, 0xA0,
  0x0C, 0x9D, 0x28, 0xDC, 0x8E, 0x1E, 0x87, 0x7C, 0x63, 0xFA, 0x4F, 0xB3, 0xE1, 0x79, 0xE0, 0x13,
  0xB2, 0x7C, 0x55, 0x24, 0x3C, 0x62, 0xD2, 0x58, 0x5F, 0x98, 0x9D, 0xEB, 0xBE, 0xE1, 0x7D, 0xF8,
  0x5A, 0x83, 0x14, 0x8A, 0x66, 0xE1, 0xC6, 0xD2, 0x39, 0x79, 0x5B, 0x39, 0x87, 0x98, 0x26, 0xC1,
  0x3C, 0x74, 0x6C, 0x9D, 0x5A, 0x95, 0xAA, 0x4F, 0x63, 0xEC, 0xF1, 0x76, 0xE4, 0x74, 0xD7, 0xB7,
  0xEE, 0x7A, 0xC5, 0xF4, 0xB4, 0xEF, 0x6F, 0xBB, 0xD7, 0x03, 0x9E, 0xCD, 0x33, 0x77, 0x49, 0x7A,
  0x9B, 0x41, 0x1F, 0x37, 0x2F, 0xAE, 0x70, 0x8C, 0xF8, 0xAD, 0xEE, 0x41, 0xCB, 0xDA, 0xA7, 0x3B,
  0xD7, 0x1D, 0xFD, 0x28, 0xF8, 0xB3, 0x8D, 0xA4, 0x00, 0x1E, 0x63, 0xE5, 0xCB, 0xC4, 0xC4, 0xDE,
  0xFD, 0x01, 0xE0, 0x37, 0x05, 0xB2, 0x6D, 0x93, 0x05, 0xAC, 0x0E, 0x76, 0xCE, 0x68, 0xCA, 0xA8
 };

 /*! \brief Таблица обратных перестановок для алгоритма AES
  * 
 * Таблица обратных перестановок для алгоритма AES. Посчитана заранее и размещена в ПЗУ для экономии ОЗУ.
 */
 const uint8_t sBoxInv[ 256 ] =
 {
  0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38,
  0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
  0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87,
  0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
  0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D,
  0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
  0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2,
  0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
  0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16,
  0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92,
  0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA,
  0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
  0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A,
  0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
  0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02,
  0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
  0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA,
  0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
  0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85,
  0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
  0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89,
  0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
  0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20,
  0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
  0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31,
  0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
  0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D,
  0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF,
  0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0,
  0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
  0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26,
  0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D
 };

 /*! Операция XOR над массивом байт
  *
  * \param bytes1 указатель на первыйы массив байт
  * \param bytes2 указатель на второй массив байт
  * \param count количество байт для обработки, кратно 4 
 */
 void XORBytes( uint8_t *bytes1, const uint8_t *bytes2, uint8_t count )
 {
   do
     {
      *bytes1 ^= *bytes2; // Add in GF(2), ie. XOR. *
      bytes1++;
      bytes2++;
     }while( --count );
 }

 /*! \brief Копирование байт из одного массива в другой
  *
  * Копирование байт из одного массива в другой. Количество копируемых байт должно быть кратно 4.
  * \param  to указатель на массива  приемник
  * \param from указатель на массив источник
  * \param count количество байт для работы, должно быть кратно 4 
 */
 void CopyBytes( uint8_t *to, const uint8_t *from, uint8_t count )
 {
   /* оптимизирвано под 32р процессор */
   uint32_t *t, *f, c;
   t = (uint32_t*)to;
   f = (uint32_t*)from;
   c = count;
   do
     {
      *t++ = *f++;
      c -= 4;
     }while( c );
 }

 /* ************************** Функции для только ДЕкодирования *********************** */

 /*! Обратная подстановка байт и XOR с ключем
  *
  * \param bytes указатель на обрабатываемый массив 
  * \param key ключ
  * \param count количество байт для работы
 */
 void InvSubBytesAndXOR( uint8_t *bytes, const uint8_t *key, uint8_t count )
 {
   do
     {
      *bytes = sBoxInv[ *bytes ] ^ *key; /* Inverse substitute every byte in state and add key. */
      bytes++;
      key++;
     }while( --count );
 }

 /*! Обратный сдвиг колонок
  *
  * \param state указатель на "состояние"
 */
 void InvShiftRows( uint8_t *state )
 {
   uint8_t temp;
   /* Note: State is arranged column by column. */
   /* Cycle second row right one time.          */
   temp = state[ 1 + 3*4 ];
   state[ 1 + 3*4 ] = state[ 1 + 2*4 ];
   state[ 1 + 2*4 ] = state[ 1 + 1*4 ];
   state[ 1 + 1*4 ] = state[ 1 + 0*4 ];
   state[ 1 + 0*4 ] = temp;
   /* Cycle third row right two times. */
   temp = state[ 2 + 0*4 ];
   state[ 2 + 0*4 ] = state[ 2 + 2*4 ];
   state[ 2 + 2*4 ] = temp;
   temp = state[ 2 + 1*4 ];
   state[ 2 + 1*4 ] = state[ 2 + 3*4 ];
   state[ 2 + 3*4 ] = temp;
   /* Cycle fourth row right three times, ie. left once. */
   temp = state[ 3 + 0*4 ];
   state[ 3 + 0*4 ] = state[ 3 + 1*4 ];
   state[ 3 + 1*4 ] = state[ 3 + 2*4 ];
   state[ 3 + 2*4 ] = state[ 3 + 3*4 ];
   state[ 3 + 3*4 ] = temp;
 }

 /*! Обратное перемешивание колонки
  *
  * \param column указатель на колонки
 */
 void InvMixColumn( uint8_t *column )
 {
   uint8_t r0, r1, r2, r3;
   r0 = column[ 1 ] ^ column[ 2 ] ^ column[ 3 ];
   r1 = column[ 0 ] ^ column[ 2 ] ^ column[ 3 ];
   r2 = column[ 0 ] ^ column[ 1 ] ^ column[ 3 ];
   r3 = column[ 0 ] ^ column[ 1 ] ^ column[ 2 ];
   column[ 0 ] = (column[ 0 ] << 1) ^ (column[ 0 ] & 0x80 ? BPOLY : 0);
   column[ 1 ] = (column[ 1 ] << 1) ^ (column[ 1 ] & 0x80 ? BPOLY : 0);
   column[ 2 ] = (column[ 2 ] << 1) ^ (column[ 2 ] & 0x80 ? BPOLY : 0);
   column[ 3 ] = (column[ 3 ] << 1) ^ (column[ 3 ] & 0x80 ? BPOLY : 0);
   r0 ^= column[ 0 ] ^ column[ 1 ];
   r1 ^= column[ 1 ] ^ column[ 2 ];
   r2 ^= column[ 2 ] ^ column[ 3 ];
   r3 ^= column[ 0 ] ^ column[ 3 ];
   column[ 0 ] = (column[ 0 ] << 1) ^ (column[ 0 ] & 0x80 ? BPOLY : 0);
   column[ 1 ] = (column[ 1 ] << 1) ^ (column[ 1 ] & 0x80 ? BPOLY : 0);
   column[ 2 ] = (column[ 2 ] << 1) ^ (column[ 2 ] & 0x80 ? BPOLY : 0);
   column[ 3 ] = (column[ 3 ] << 1) ^ (column[ 3 ] & 0x80 ? BPOLY : 0);
   r0 ^= column[ 0 ] ^ column[ 2 ];
   r1 ^= column[ 1 ] ^ column[ 3 ];
   r2 ^= column[ 0 ] ^ column[ 2 ];
   r3 ^= column[ 1 ] ^ column[ 3 ];
   column[ 0 ] = (column[ 0 ] << 1) ^ (column[ 0 ] & 0x80 ? BPOLY : 0);
   column[ 1 ] = (column[ 1 ] << 1) ^ (column[ 1 ] & 0x80 ? BPOLY : 0);
   column[ 2 ] = (column[ 2 ] << 1) ^ (column[ 2 ] & 0x80 ? BPOLY : 0);
   column[ 3 ] = (column[ 3 ] << 1) ^ (column[ 3 ] & 0x80 ? BPOLY : 0);
   column[ 0 ] ^= column[ 1 ] ^ column[ 2 ] ^ column[ 3 ];
   r0 ^= column[ 0 ];
   r1 ^= column[ 0 ];
   r2 ^= column[ 0 ];
   r3 ^= column[ 0 ];
   column[ 0 ] = r0;
   column[ 1 ] = r1;
   column[ 2 ] = r2;
   column[ 3 ] = r3;
 }

 /*! Обратное перемешивание колонок
  *
  * \param state указатель на "состояние"
 */
 void InvMixColumns( uint8_t *state )
 {
   InvMixColumn( state + 0*4 );
   InvMixColumn( state + 1*4 );
   InvMixColumn( state + 2*4 );
   InvMixColumn( state + 3*4 );
 }

 /*! Кодирование блока
  *
  * \param block указатель на блок
  * \param expandedKey указатель на расширенный ключ
 */ 
 void InvCipher( uint8_t *block, const uint8_t *expandedKey )
 {  
   uint8_t round = ROUNDS-1;
   expandedKey += BLOCKSIZE * ROUNDS;
   XORBytes( block, expandedKey, 16 );
   expandedKey -= BLOCKSIZE;
   do
     {
      InvShiftRows( block );
      InvSubBytesAndXOR( block, expandedKey, 16 );

      expandedKey -= BLOCKSIZE;
      InvMixColumns( block );
     } while( --round );

   InvShiftRows( block );
   InvSubBytesAndXOR( block, expandedKey, 16 );
 }

 /*! \brief Раскодирование блока с учетом ChainBlock
  *
  * Раскодирование блока с учетом ChainBlock. ChainBlock остается от обработки предыдущего блока
  * \param buffer указатель на блок
  * \param chainBlock указатель на ChainBlock
 */
 void aesDecrypt( unsigned char *buffer, unsigned char *chainBlock )
 {
   uint8_t temp[ BLOCKSIZE ];
   CopyBytes( temp, buffer, BLOCKSIZE );
   InvCipher( buffer,ExKey );
   XORBytes( buffer, chainBlock, BLOCKSIZE );
   CopyBytes( chainBlock, temp, BLOCKSIZE );
 }
 
#endif
/*! @} */
