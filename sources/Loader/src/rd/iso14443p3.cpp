#include <stdint.h>
#include "Hardware/hrd_Globals.h"
#include "iso14443p3.h"

#include "scmRTOS/Common/scmRTOS.h"

//#include "Hardware/CLRC663/hrd_CLRC663.h"
//#include "Hardware/CLRC663/hrd_CLRC663_Registers.h"
#include "Main/Utils.h"


/*! ����� ����� ��������
 */
void TIso14443p3_Card::reset( ) const
{
 Maf->fieldOff();   // ��������� ����
 
 OS::sleep(TICKS_PER_SEC/50);   // ������� 5100us 
 
 Maf->fieldOn();    //�������� ����
 
 OS::sleep(TICKS_PER_SEC/50);   // ������� 5100us
}

/*! �������� ������� � ����� � ����� ������
 *
 * \param[in] crc_enable �������� CRC
 * \param[in] tx ��������� �� ������ ������������ ����
 * \param[in] tx_bit_num ���������� ����������� ��� !!!
 *  \return ���� �� ������������� �����(��� 31 ����� 0), �� ������� ����(���� 7..0) ��� ���������� �������� ���� � �����
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
 *
 */
int TIso14443p3_Card::transceive( bool crc_enable, const uint8_t *tx, int tx_bit_num ) const
{ 
 this->lastCommand = tx[0];
 int ret = Maf->transceive( crc_enable, tx, tx_bit_num, this->answer, sizeof(this->answer) );
 
 this->answerLength = ret & 0xFF;   
 return ret;
}

/*!
 * �������� ������� Halt
 */
void TIso14443p3_Card::doHalt(  ) const
{ 
 uint8_t fifodata[2];

 fifodata[0] = ISO14443_HALT_COMMAND >> 8;
 fifodata[1] = ISO14443_HALT_COMMAND & 0xFF;
 
 uint8_t rx_buf[2];
 
 Maf->transceive(true, fifodata, 2*8, rx_buf, 2 );
 //! \todo �������� ����������
 OS::sleep(TICKS_PER_SEC/20);
}

/*!
 * ��������� ����� � ��������� READY, ����� ������� �� ������� REQA ��� WUPA.
 * ������� ��� ������ ���� �������� � ����� ������ Reset, ����� ������ �� �����
 * \param cmd[in] ��� ������� REQA ��� WUPA
 * \return 0 ���� ��� ������ ��� 1 � this->atq �������� ATQ ���������� �� �����
 */
int TIso14443p3_Card::req( uint8_t cmd ) const
{
 uint8_t ret =0;
 uint8_t fifodata[2];
 fifodata[0] = cmd; 

 int rx_len = this->transceive( false, fifodata, 7 ); // �������� 7 ���, ������� WUPA ��� REQA ������ 7 ���
 if( rx_len == 2 )
   {
    this->atq[0]=this->answer[0];   // ����c�� ����� � ���������
    this->atq[1]=this->answer[1];
    ret = 1;
   }
 this->lastError = ret;  
 return ret;  
} 

/*!
 * ��������� ����� � ��������� READY, ����� ������� �� ������� REQA.
 * ������� ��� ������ ���� �������� � ����� ������ Reset, ����� ������ �� �����
 * \return 0 ���� ��� ������ ��� 1 � this->atq �������� ATQ ���������� �� �����
 */
int TIso14443p3_Card::doREQA( ) const
{
 return this->req( ISO14443_REQA_COMMAND );
}

/*!
 * ��������� ����� � ��������� READY, ����� ������� �� ������� WUPA.
 * ������� ��� ������ ���� �������� � ����� ������ Reset, ����� ������ �� �����
 * \return 0 ���� ��� ������ ��� 1 � this->atq �������� ATQ ���������� �� �����
 */
int TIso14443p3_Card::doWUPA(  ) const
{
 return this->req( ISO14443_WUPA_COMMAND );
}

//cmd = 92h ��� 95h
int TIso14443p3_Card::anticollisionStep( uint8_t cmd )
{
 uint8_t fifodata[2];

 fifodata[0] = cmd;  // anticpllision 
 fifodata[1] = 0x20;
 
// Clrc663.enableCRC( false );    // ��� anticollision ��������� CRC �� ����
 int rx_len = this->transceive( false, fifodata, 16 ); // �������� 16 ���, �������
 return rx_len;
}

// level 93h or 95h
// rx_buf �� ����� 8 ����
int TIso14443p3_Card::SELECT( uint8_t level, const uint8_t uid[4] ) const
{
 uint8_t fifodata[7];
 
 fifodata[0] = level;
 fifodata[1] = 0x70;    // ������� Anticollision c ���������� 70h ��� � ���� SELECT
 // ������� ����������� 4 ����, ����� �� �������� �� ������������, ��������� ������������ �� ������� �����
 COPY4( &fifodata[2], &uid[0] );

 fifodata[6] = uid[0]^uid[1]^uid[2]^uid[3];  //bcc ����
 
 int rx_len = this->transceive( true, fifodata, 7*8 ); // �������� 7���� , ������� select ������ ���� � ����������� ������
 return rx_len;
}

/*
 ���������� �����, ����������� � ���� ���� �� UID ��������� � ��� ��� ������� � card
 */
bool TIso14443p3_Card::Activate(  ) const
{
 this->reset( );   // ����� ����� �� �������

 if( this->doREQA( ) )
   {
    int rx_len;
    uint8_t rx_buf[8];
  
    if( this->length == 7 )
      {
       COPY4( &rx_buf[1], &this->uid[0] );    
       rx_buf[0] = 0x88;
      }
    else
      {
       COPY4( &rx_buf[0], &this->uid[0] );    
      }   
   
    rx_len = this->SELECT( ISO14443_ANTICOLLISION_1_COMMAND, rx_buf );
    if( rx_len == 3 )
      {
       if( this->length == 7 )
         {
          COPY4( &rx_buf[0], &this->uid[3] );    
          rx_len = this->SELECT( ISO14443_ANTICOLLISION_2_COMMAND, rx_buf );
          if( rx_len == 3 )
            {
             return true;
            } 
         } 
      }
   }   
 return false;  
}

// ������ REQA ��� ����� ���������� 4 ��� 7 ���� ������� ��������� UID � ����� ������, � ����� ������ � ��������� ACTIVE
// � ������ ������ ��������� ��������� card_info, �������� UID, ����� UID, ATQ, SAK
// \return  4 ��� 7 ���� ������� ��������� UID � ����� ������, � ����� ������ � ��������� ACTIVE
// -1 ���� ���������� �������� ��������
//         0  ���� ����� ��� ��� �������� ��������
int TIso14443p3_Card::Anticollision( )
{
 // Send ReqA command  
 int8_t ret = 0;
 //card->length = 0;   // ������� ������������
 //err = Clrc663.readRegister( HW_RC663_REG_ERROR );
 int rx_len;
 //int rx_len = ISO14443p3a_REQA( card );
 //if( rx_len )  
   { // �� REQA ��� �����
    int a_res = this->anticollisionStep( ISO14443_ANTICOLLISION_1_COMMAND );
      {       
       //uint8_t err_reg = Maf->readError( );
       //if( (err_reg & 0x7F) == 0 ) // ����� ����, �� ���� ��������
       if( a_res >=0 ) // ����� ����, �� ���� ��������
         {
          // � ����� �������� 4 ����� uid, �� ���� ������ ���� 0x88, �� ��� 7 �������� ����� 
          if( this->answer[0] != 0x88 )  // CT Cascade level not detect
            {
             // ������� ����������� 4 ����, ����� �� �������� �� ������������, ��������� ������������ �� ������� �����
             COPY4( &this->uid[0], &this->answer[0] );
             // ������ ������ SELECT ������� ������ � ������ ��� ����������� ������� UID
             rx_len = this->SELECT( ISO14443_ANTICOLLISION_1_COMMAND, this->answer );
             if( rx_len == 3 )
               {// ������ ������� ������
                this->sak = this->answer[0]; 
                this->uid[4] = 0;
                this->uid[5] = 0;
                this->uid[6] = 0;            
                this->length = 4;    
                ret= 4;   // ���������� ����� � 4 ������� UID
               } 
            }         
          else
            {// ���� uid �� 7 ����, �� ���� ����� 3 ����� �� ������ � ������ ������ ��� ��������� ���������� ���� uid
             // ������� ����������� 4 ����, ����� �� �������� �� ������������, ��������� ������������ �� ������� �����            
             COPY4( &this->uid[0], &this->answer[1] );          
          
             // ������ ������ �� ������ ����� ID
             rx_len = this->SELECT( ISO14443_ANTICOLLISION_1_COMMAND, this->answer );
             
             //err_reg = Maf->readError();
             //if( (err_reg & 0x7F)== 0 )
             if( rx_len > 0 ) 
               {// Anticollision ������� ������  
              //  err = Clrc663.readRegister( HW_RC663_REG_ERROR );
                int rx_len = this->anticollisionStep( ISO14443_ANTICOLLISION_2_COMMAND ); 
                if( rx_len >=0 )
                  {            
                   // ������� ����������� 4 ����, ����� �� �������� �� ������������, ��������� ������������ �� ������� �����            
                   COPY4( &this->uid[3], &this->answer[0] );
            
                   rx_len = this->SELECT( ISO14443_ANTICOLLISION_2_COMMAND, this->answer );
                   
                   if( rx_len ==3 )
                     {// ����� ������ 3 �����: SAK � ��� ����� CRC  
                      this->sak = this->answer[0]; 
                      this->length = 7;                               
                   //   err = Clrc663.readRegister( HW_RC663_REG_ERROR );
                      /// �������� ����������� ����� � 7 ������� UID
                      ret= 7;
                     } 
                  } 
               
               }
             else
               {        
                ret= -1;
               }  
            }   
         }  
       else
         {
          ret = -1;
         }  
      }   
     this->lastError = ret;     
    }   
 return ret;
}


///////////////////////////////////////////////////

/*
// level 93h or 95h
// rx_buf �� ����� 8 ����
static int ISO14443p3a_SELECT( uint8_t level, const uint8_t uid[4],  const TIso14443p3_Card *card )
{
 uint8_t fifodata[7];
 
 fifodata[0] = level;
 fifodata[1] = 0x70;    // ������� Anticollision c ���������� 70h ��� � ���� SELECT
 // ������� ����������� 4 ����, ����� �� �������� �� ������������, ��������� ������������ �� ������� �����
 COPY4( fifodata[2], uid[0] );

 fifodata[6] = uid[0]^uid[1]^uid[2]^uid[3];  //bcc
 
 int rx_len = transceive( true, fifodata, 7*8, card ); // �������� 7���� , ������� select ������ ���� � ����������� ������
 return rx_len;
}
*/
/*
//
// ���������� �����, ����������� � ���� ���� �� UID ��������� � ��� ��� ������� � card
//
bool ISO14443p3_Activate( const TIso14443p3_Card *card )
{
// ISO14443p3a_Reset();   // ����� ����� �� �������
 card->Reset( &Clrc663 );

// if( ISO14443p3a_REQA( card) )
 if( card->REQA( &Clrc663 ) )
   {
    int rx_len;
    uint8_t rx_buf[8];
  
    if( card->length == 7 )
      {
       COPY4( rx_buf[1], card->uid[0] );    
       rx_buf[0] = 0x88;
      }
    else
      {
       COPY4( rx_buf[0], card->uid[0] );    
      }   
   
    rx_len = ISO14443p3a_SELECT( ANTICOLLISION_1, rx_buf, card );
    if( rx_len == 3 )
      {
       if( card->length == 7 )
         {
          COPY4( rx_buf[0], card->uid[3] );    
          rx_len = ISO14443p3a_SELECT( ANTICOLLISION_2, rx_buf, card );
          if( rx_len == 3 )
            {
             return true;
            } 
         } 
      }
   }   
 return false;  
}
*/
/*
// ������ REQA ��� ����� ���������� 4 ��� 7 ���� ������� ��������� UID � ����� ������, � ����� ������ � ��������� ACTIVE
// � ������ ������ ��������� ��������� card_info, �������� UID, ����� UID, ATQ, SAK
// \return  4 ��� 7 ���� ������� ��������� UID � ����� ������, � ����� ������ � ��������� ACTIVE
// -1 ���� ���������� �������� ��������
//         0  ���� ����� ��� ��� �������� ��������
int ISO14443p3a_Anticollision( TIso14443p3_Card *card )
{// Send ReqA command  
 //uint8_t rx_buf[8];
 int8_t ret = 0;
 //card->length = 0;   // ������� ������������
 
 int rx_len;
 //int rx_len = ISO14443p3a_REQA( card );
 //if( rx_len )
 
 
   { // �� REQA ��� �����
    if( ISO14443p3a_AnticollisionStep( ANTICOLLISION_1, card ) )   
      {       
       uint8_t err_reg = Clrc663.readError( );
       if( (err_reg & 0x7F) == 0 ) // ����� ����, �� ���� ��������
         {
          // � ����� �������� 4 ����� uid, �� ���� ������ ���� 0x88, �� ��� 7 �������� ����� 
          if( card->answer[0] != 0x88 )  // CT Cascade level not detect
            {
             // ������� ����������� 4 ����, ����� �� �������� �� ������������, ��������� ������������ �� ������� �����
             COPY4( card->uid[0], card->answer[0] );
             // ������ ������ SELECT ������� ������ � ������ ��� ����������� ������� UID
             rx_len = ISO14443p3a_SELECT( ANTICOLLISION_1, card->answer,  card );
             if( rx_len == 3 )
               {// ������ ������� ������
                card->sak = card->answer[0]; 
                card->uid[4] = 0;
                card->uid[5] = 0;
                card->uid[6] = 0;            
                card->length = 4;    
                ret= 4;   // ���������� ����� � 4 ������� UID
               } 
            }         
          else
            {// ���� uid �� 7 ����, �� ���� ����� 3 ����� �� ������ � ������ ������ ��� ��������� ���������� ���� uid
             // ������� ����������� 4 ����, ����� �� �������� �� ������������, ��������� ������������ �� ������� �����            
             COPY4( card->uid[0], card->answer[1] );          
          
             // ������ ������ �� ������ ����� ID
             rx_len = ISO14443p3a_SELECT( ANTICOLLISION_1, card->answer, card );
             
             err_reg = Clrc663.readError();
             if( (err_reg & 0x7F)== 0 )
               {// Anticollision ������� ������  
                int rx_len = ISO14443p3a_AnticollisionStep( ANTICOLLISION_2, card ); 
                if( rx_len )
                  {            
                   // ������� ����������� 4 ����, ����� �� �������� �� ������������, ��������� ������������ �� ������� �����            
                   COPY4( card->uid[3], card->answer[0] );
            
                   rx_len = ISO14443p3a_SELECT( ANTICOLLISION_2, card->answer, card );
                   
                   if( rx_len ==3 )
                     {// ����� ������ 3 �����: SAK � ��� ����� CRC  
                      card->sak = card->answer[0]; 
                      card->length = 7;                               
                      /// �������� ����������� ����� � 7 ������� UID
                      ret= 7;
                     } 
                  } 
               
               }
             else
               {        
                ret= -1;
               }  
            }   
         }  
       else
         {
          ret = -1;
         }  
      }   
     card->lastError = ret;     
    }   
 return ret;
}
*/
/*
void
iso14443a_crc(uint8_t *pbtData, size_t szLen, uint8_t *pbtCrc)
{
  uint32_t wCrc = 0x6363;

  do {
    uint8_t  bt;
    bt = *pbtData++;
    bt = (bt ^ (uint8_t)(wCrc & 0x00FF));
    bt = (bt ^ (bt << 4));
    wCrc = (wCrc >> 8) ^ ((uint32_t) bt << 8) ^ ((uint32_t) bt << 3) ^ ((uint32_t) bt >> 4);
  } while (--szLen);

  *pbtCrc++ = (uint8_t)(wCrc & 0xFF);
  *pbtCrc = (uint8_t)((wCrc >> 8) & 0xFF);
}
*/

int TIso14443p3_Card::AuthenticateDES(  ) const
{
 uint8_t fifodata[7];
 
 fifodata[0] = 0x1A;
fifodata[1] = 0x00;
 
 int rx_len = this->transceive( true, fifodata, 2*8 ); // �������� 7���� , ������� select ������ ���� � ����������� ������
 if( rx_len )
   {
   }
 return rx_len;
}
