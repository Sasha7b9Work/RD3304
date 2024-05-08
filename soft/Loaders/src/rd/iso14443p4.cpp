#include <string.h>
#include "iso14443p4.h"

//#include "../Hardware/CLRC663/hrd_CLRC663.h"


/*
 * 
 * \param tx_buf передаваемые данные, при этом первый байт(с индексом 0) будет использован под BCP протокола T=CL
 * \return количество принятых байт
 */
/*int TIso14443p4_Card::ISO14443p4_Transceive( uint8_t *tx_buf, int tx_byte_num ) const
{
 tx_buf[0] = 0x02 |( (this->blockNumber++) & 0x01 ); // В нулевой бит PCB заносим младший бит счетчика, а первый бит должен быть единицей
 this->lastCommand = tx_buf[1];
 return this->answerLength = Maf->transceive( true, tx_buf, tx_byte_num*8, this->answer, sizeof(this->answer) ); 
}
*/

/*!
 * Передача в протоколе T=CL
 * 
 * \param tx_buf передаваемые данные, при этом первый байт(с индексом 0) будет использован под BCP протокола T=CL
 * \return если не отрицательное число(бит 31 равен 0), то младший байт(биты 7..0) это количество принятых байт в ответ
 *         если число отрицатльеное(бит 31 равен 1), то в процессе приема были ошибки
 *
 *    Байты:   3          2          1          0   
 *         7654 3210  7654 3210  7654 3210  7654 3210 
 *         Sxxx xxEC  xxxx xxxx  bbbb bbbb  RRRR RRRR
 *
 *         S: признак ошибки
 *           S = 0 ошибок не было 
 *           S = 1 были ошибки
 *         E: признак ошибки приема данных
 *           ессли 1 , то была ошибка
 *         C: признак колиизии
 *           если 1 то bbbbbbbbb номер первого бита где была обнаружена коллизия
 *
 *         RRRRRRRR: количество принятых в ответ байт
 *
 */
int TIso14443p4_Card::transceiveTCL( uint8_t *tx_buf, int tx_byte_num ) const
{
 tx_buf[0] = 0x02 |( (this->blockNumber++) & 0x01 ); // В нулевой бит PCB заносим младший бит счетчика, а первый бит должен быть единицей
 this->lastCommand = tx_buf[1];                      // запрминаем первый байт команды
 int r = Maf->transceive( true, tx_buf, tx_byte_num*8, this->answer, sizeof(this->answer) ); 
 this->answerLength = r & 0xFF;
 return r;
}

/*
 \return если >0 то в this->answer[5] лежа тhistorical_bytes 
 */
int TIso14443p4_Card::ISO14443p4_RATS( /*uint8_t historical_bytes[7]*/ ) const
{
 uint8_t tx_buf[4];
  
 memset( &this->answer[5], 0, 7);
 
 tx_buf[0] = 0xE0;
 tx_buf[1] = 0x50;  // FSDI = 5, CID = 0
 
 //int rx_len = Clrc663.transceive( true, tx_buf, 2*8,rx_buf, sizeof(rx_buf)); // передача 7байт 
 //this->lastCommand = tx_buf[0];
 int rx_len = TIso14443p3_Card::transceive( true, tx_buf, 2*8); 
 //int rx_len = card->answerLength = Clrc663.transceive( true, tx_buf, 2*8,card->answer, sizeof(card->answer)); // передача 7байт 
/* if( rx_len )
   {//! \todo проверка на ошибку
    memcpy( historical_bytes, &this->answer[5],7 );
   }*/
 return rx_len;
}

/////////////////////////////////////

/*

int ISO14443p4_RATS( const iso14443p4_Card *card, uint8_t historical_bytes[7] )
{
 uint8_t tx_buf[4];
 //uint8_t rx_buf[16];
 
 memset( historical_bytes, 0, 7);
 
 
 tx_buf[0] = 0xE0;
 tx_buf[1] = 0x50;  // FSDI = 5, CID = 0
 
 //int rx_len = Clrc663.transceive( true, tx_buf, 2*8,rx_buf, sizeof(rx_buf)); // передача 7байт 
 card->lastCommand = tx_buf[0];
 int rx_len = card->answerLength = Clrc663.transceive( true, tx_buf, 2*8,card->answer, sizeof(card->answer)); // передача 7байт 
 if( rx_len )
   {//! \todo проверка на ошибку
    memcpy( historical_bytes, &card->answer[5],7 );
   }
 return rx_len;
}

*/
