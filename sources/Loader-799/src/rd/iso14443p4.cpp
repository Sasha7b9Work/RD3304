#include <string.h>
#include "iso14443p4.h"

//#include "../Hardware/CLRC663/hrd_CLRC663.h"


/*
 * 
 * \param tx_buf ������������ ������, ��� ���� ������ ����(� �������� 0) ����� ����������� ��� BCP ��������� T=CL
 * \return ���������� �������� ����
 */
/*int TIso14443p4_Card::ISO14443p4_Transceive( uint8_t *tx_buf, int tx_byte_num ) const
{
 tx_buf[0] = 0x02 |( (this->blockNumber++) & 0x01 ); // � ������� ��� PCB ������� ������� ��� ��������, � ������ ��� ������ ���� ��������
 this->lastCommand = tx_buf[1];
 return this->answerLength = Maf->transceive( true, tx_buf, tx_byte_num*8, this->answer, sizeof(this->answer) ); 
}
*/

/*!
 * �������� � ��������� T=CL
 * 
 * \param tx_buf ������������ ������, ��� ���� ������ ����(� �������� 0) ����� ����������� ��� BCP ��������� T=CL
 * \return ���� �� ������������� �����(��� 31 ����� 0), �� ������� ����(���� 7..0) ��� ���������� �������� ���� � �����
 *         ���� ����� �������������(��� 31 ����� 1), �� � �������� ������ ���� ������
 *
 *    �����:   3          2          1          0   
 *         7654 3210  7654 3210  7654 3210  7654 3210 
 *         Sxxx xxEC  xxxx xxxx  bbbb bbbb  RRRR RRRR
 *
 *         S: ������� ������
 *           S = 0 ������ �� ���� 
 *           S = 1 ���� ������
 *         E: ������� ������ ������ ������
 *           ����� 1 , �� ���� ������
 *         C: ������� ��������
 *           ���� 1 �� bbbbbbbbb ����� ������� ���� ��� ���� ���������� ��������
 *
 *         RRRRRRRR: ���������� �������� � ����� ����
 *
 */
int TIso14443p4_Card::transceiveTCL( uint8_t *tx_buf, int tx_byte_num ) const
{
 tx_buf[0] = 0x02 |( (this->blockNumber++) & 0x01 ); // � ������� ��� PCB ������� ������� ��� ��������, � ������ ��� ������ ���� ��������
 this->lastCommand = tx_buf[1];                      // ���������� ������ ���� �������
 int r = Maf->transceive( true, tx_buf, tx_byte_num*8, this->answer, sizeof(this->answer) ); 
 this->answerLength = r & 0xFF;
 return r;
}

/*
 \return ���� >0 �� � this->answer[5] ���� �historical_bytes 
 */
int TIso14443p4_Card::ISO14443p4_RATS( /*uint8_t historical_bytes[7]*/ ) const
{
 uint8_t tx_buf[4];
  
 memset( &this->answer[5], 0, 7);
 
 tx_buf[0] = 0xE0;
 tx_buf[1] = 0x50;  // FSDI = 5, CID = 0
 
 //int rx_len = Clrc663.transceive( true, tx_buf, 2*8,rx_buf, sizeof(rx_buf)); // �������� 7���� 
 //this->lastCommand = tx_buf[0];
 int rx_len = TIso14443p3_Card::transceive( true, tx_buf, 2*8); 
 //int rx_len = card->answerLength = Clrc663.transceive( true, tx_buf, 2*8,card->answer, sizeof(card->answer)); // �������� 7���� 
/* if( rx_len )
   {//! \todo �������� �� ������
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
 
 //int rx_len = Clrc663.transceive( true, tx_buf, 2*8,rx_buf, sizeof(rx_buf)); // �������� 7���� 
 card->lastCommand = tx_buf[0];
 int rx_len = card->answerLength = Clrc663.transceive( true, tx_buf, 2*8,card->answer, sizeof(card->answer)); // �������� 7���� 
 if( rx_len )
   {//! \todo �������� �� ������
    memcpy( historical_bytes, &card->answer[5],7 );
   }
 return rx_len;
}

*/
