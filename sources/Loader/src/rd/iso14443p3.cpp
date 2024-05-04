#include <stdint.h>
#include "Hardware/hrd_Globals.h"
#include "iso14443p3.h"

#include "scmRTOS/Common/scmRTOS.h"

//#include "Hardware/CLRC663/hrd_CLRC663.h"
//#include "Hardware/CLRC663/hrd_CLRC663_Registers.h"
#include "Main/Utils.h"


/*! Сброс карты питанием
 */
void TIso14443p3_Card::reset( ) const
{
 Maf->fieldOff();   // выключаем поле
 
 OS::sleep(TICKS_PER_SEC/50);   // минимум 5100us 
 
 Maf->fieldOn();    //включаем поле
 
 OS::sleep(TICKS_PER_SEC/50);   // минимум 5100us
}

/*! Передача посылки в карту и прием ответа
 *
 * \param[in] crc_enable включить CRC
 * \param[in] tx указатель на массив передаваемых байт
 * \param[in] tx_bit_num количество передваемых БИТ !!!
 *  \return если не отрицательное число(бит 31 равен 0), то младший байт(биты 7..0) это количество принятых байт в ответ
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
 * Передача команды Halt
 */
void TIso14443p3_Card::doHalt(  ) const
{ 
 uint8_t fifodata[2];

 fifodata[0] = ISO14443_HALT_COMMAND >> 8;
 fifodata[1] = ISO14443_HALT_COMMAND & 0xFF;
 
 uint8_t rx_buf[2];
 
 Maf->transceive(true, fifodata, 2*8, rx_buf, 2 );
 //! \todo проверка результата
 OS::sleep(TICKS_PER_SEC/20);
}

/*!
 * Переводит карту в состояние READY, путем посылки ей команды REQA или WUPA.
 * Антенна уже должна быть включена и карта прошла Reset, иначе ответа не будет
 * \param cmd[in] код команды REQA или WUPA
 * \return 0 если нет ответа или 1 и this->atq содержит ATQ полученное от карты
 */
int TIso14443p3_Card::req( uint8_t cmd ) const
{
 uint8_t ret =0;
 uint8_t fifodata[2];
 fifodata[0] = cmd; 

 int rx_len = this->transceive( false, fifodata, 7 ); // передача 7 бит, команда WUPA или REQA длиной 7 бит
 if( rx_len == 2 )
   {
    this->atq[0]=this->answer[0];   // заноcим ответ в структуру
    this->atq[1]=this->answer[1];
    ret = 1;
   }
 this->lastError = ret;  
 return ret;  
} 

/*!
 * Переводит карту в состояние READY, путем посылки ей команды REQA.
 * Антенна уже должна быть включена и карта прошла Reset, иначе ответа не будет
 * \return 0 если нет ответа или 1 и this->atq содержит ATQ полученное от карты
 */
int TIso14443p3_Card::doREQA( ) const
{
 return this->req( ISO14443_REQA_COMMAND );
}

/*!
 * Переводит карту в состояние READY, путем посылки ей команды WUPA.
 * Антенна уже должна быть включена и карта прошла Reset, иначе ответа не будет
 * \return 0 если нет ответа или 1 и this->atq содержит ATQ полученное от карты
 */
int TIso14443p3_Card::doWUPA(  ) const
{
 return this->req( ISO14443_WUPA_COMMAND );
}

//cmd = 92h или 95h
int TIso14443p3_Card::anticollisionStep( uint8_t cmd )
{
 uint8_t fifodata[2];

 fifodata[0] = cmd;  // anticpllision 
 fifodata[1] = 0x20;
 
// Clrc663.enableCRC( false );    // для anticollision добавлять CRC не надо
 int rx_len = this->transceive( false, fifodata, 16 ); // передача 16 бит, команда
 return rx_len;
}

// level 93h or 95h
// rx_buf не менее 8 байт
int TIso14443p3_Card::SELECT( uint8_t level, const uint8_t uid[4] ) const
{
 uint8_t fifodata[7];
 
 fifodata[0] = level;
 fifodata[1] = 0x70;    // команда Anticollision c параметром 70h это и есть SELECT
 // быстрое копирование 4 байт, может не работать на архитектурах, требующих выравнивания на границу слова
 COPY4( &fifodata[2], &uid[0] );

 fifodata[6] = uid[0]^uid[1]^uid[2]^uid[3];  //bcc байт
 
 int rx_len = this->transceive( true, fifodata, 7*8 ); // передача 7байт , команда select должна быть с контрольной суммой
 return rx_len;
}

/*
 Активирует карту, находящуюся в поле если ее UID совпадает с тем что занесен в card
 */
bool TIso14443p3_Card::Activate(  ) const
{
 this->reset( );   // сброс карты по питанию

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

// Делает REQA для карты возвращает 4 или 7 если успешно определен UID с такой длиной, и карта прешла в состояние ACTIVE
// В случае успеха заполняет структуру card_info, занчение UID, длину UID, ATQ, SAK
// \return  4 или 7 если успешно определен UID с такой длиной, и карта прешла в состояние ACTIVE
// -1 если обнаружена ситуация коллизии
//         0  если карты нет или возникои проблемы
int TIso14443p3_Card::Anticollision( )
{
 // Send ReqA command  
 int8_t ret = 0;
 //card->length = 0;   // признак невалидности
 //err = Clrc663.readRegister( HW_RC663_REG_ERROR );
 int rx_len;
 //int rx_len = ISO14443p3a_REQA( card );
 //if( rx_len )  
   { // на REQA был ответ
    int a_res = this->anticollisionStep( ISO14443_ANTICOLLISION_1_COMMAND );
      {       
       //uint8_t err_reg = Maf->readError( );
       //if( (err_reg & 0x7F) == 0 ) // карта одна, не было коллизий
       if( a_res >=0 ) // карта одна, не было коллизий
         {
          // в ответ получили 4 байта uid, но если первый байт 0x88, то это 7 байтовая карта 
          if( this->answer[0] != 0x88 )  // CT Cascade level not detect
            {
             // быстрое копирование 4 байт, может не работать на архитектурах, требующих выравнивания на границу слова
             COPY4( &this->uid[0], &this->answer[0] );
             // теперь делаем SELECT первого уровня с только что полученными байтами UID
             rx_len = this->SELECT( ISO14443_ANTICOLLISION_1_COMMAND, this->answer );
             if( rx_len == 3 )
               {// селект успешно прошел
                this->sak = this->answer[0]; 
                this->uid[4] = 0;
                this->uid[5] = 0;
                this->uid[6] = 0;            
                this->length = 4;    
                ret= 4;   // обнаружена карта с 4 байтным UID
               } 
            }         
          else
            {// если uid из 7 байт, то пока берем 3 байта из ответа и делаем селект для получения оставшихся байт uid
             // быстрое копирование 4 байт, может не работать на архитектурах, требующих выравнивания на границу слова            
             COPY4( &this->uid[0], &this->answer[1] );          
          
             // делаем селект по первой части ID
             rx_len = this->SELECT( ISO14443_ANTICOLLISION_1_COMMAND, this->answer );
             
             //err_reg = Maf->readError();
             //if( (err_reg & 0x7F)== 0 )
             if( rx_len > 0 ) 
               {// Anticollision второго уровня  
              //  err = Clrc663.readRegister( HW_RC663_REG_ERROR );
                int rx_len = this->anticollisionStep( ISO14443_ANTICOLLISION_2_COMMAND ); 
                if( rx_len >=0 )
                  {            
                   // быстрое копирование 4 байт, может не работать на архитектурах, требующих выравнивания на границу слова            
                   COPY4( &this->uid[3], &this->answer[0] );
            
                   rx_len = this->SELECT( ISO14443_ANTICOLLISION_2_COMMAND, this->answer );
                   
                   if( rx_len ==3 )
                     {// длина ответа 3 байта: SAK и два байта CRC  
                      this->sak = this->answer[0]; 
                      this->length = 7;                               
                   //   err = Clrc663.readRegister( HW_RC663_REG_ERROR );
                      /// успешное обнаружение карты с 7 байтным UID
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
// rx_buf не менее 8 байт
static int ISO14443p3a_SELECT( uint8_t level, const uint8_t uid[4],  const TIso14443p3_Card *card )
{
 uint8_t fifodata[7];
 
 fifodata[0] = level;
 fifodata[1] = 0x70;    // команда Anticollision c параметром 70h это и есть SELECT
 // быстрое копирование 4 байт, может не работать на архитектурах, требующих выравнивания на границу слова
 COPY4( fifodata[2], uid[0] );

 fifodata[6] = uid[0]^uid[1]^uid[2]^uid[3];  //bcc
 
 int rx_len = transceive( true, fifodata, 7*8, card ); // передача 7байт , команда select должна быть с контрольной суммой
 return rx_len;
}
*/
/*
//
// Активирует карту, находящуюся в поле если ее UID совпадает с тем что занесен в card
//
bool ISO14443p3_Activate( const TIso14443p3_Card *card )
{
// ISO14443p3a_Reset();   // сброс карты по питанию
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
// Делает REQA для карты возвращает 4 или 7 если успешно определен UID с такой длиной, и карта прешла в состояние ACTIVE
// В случае успеха заполняет структуру card_info, занчение UID, длину UID, ATQ, SAK
// \return  4 или 7 если успешно определен UID с такой длиной, и карта прешла в состояние ACTIVE
// -1 если обнаружена ситуация коллизии
//         0  если карты нет или возникои проблемы
int ISO14443p3a_Anticollision( TIso14443p3_Card *card )
{// Send ReqA command  
 //uint8_t rx_buf[8];
 int8_t ret = 0;
 //card->length = 0;   // признак невалидности
 
 int rx_len;
 //int rx_len = ISO14443p3a_REQA( card );
 //if( rx_len )
 
 
   { // на REQA был ответ
    if( ISO14443p3a_AnticollisionStep( ANTICOLLISION_1, card ) )   
      {       
       uint8_t err_reg = Clrc663.readError( );
       if( (err_reg & 0x7F) == 0 ) // карта одна, не было коллизий
         {
          // в ответ получили 4 байта uid, но если первый байт 0x88, то это 7 байтовая карта 
          if( card->answer[0] != 0x88 )  // CT Cascade level not detect
            {
             // быстрое копирование 4 байт, может не работать на архитектурах, требующих выравнивания на границу слова
             COPY4( card->uid[0], card->answer[0] );
             // теперь делаем SELECT первого уровня с только что полученными байтами UID
             rx_len = ISO14443p3a_SELECT( ANTICOLLISION_1, card->answer,  card );
             if( rx_len == 3 )
               {// селект успешно прошел
                card->sak = card->answer[0]; 
                card->uid[4] = 0;
                card->uid[5] = 0;
                card->uid[6] = 0;            
                card->length = 4;    
                ret= 4;   // обнаружена карта с 4 байтным UID
               } 
            }         
          else
            {// если uid из 7 байт, то пока берем 3 байта из ответа и делаем селект для получения оставшихся байт uid
             // быстрое копирование 4 байт, может не работать на архитектурах, требующих выравнивания на границу слова            
             COPY4( card->uid[0], card->answer[1] );          
          
             // делаем селект по первой части ID
             rx_len = ISO14443p3a_SELECT( ANTICOLLISION_1, card->answer, card );
             
             err_reg = Clrc663.readError();
             if( (err_reg & 0x7F)== 0 )
               {// Anticollision второго уровня  
                int rx_len = ISO14443p3a_AnticollisionStep( ANTICOLLISION_2, card ); 
                if( rx_len )
                  {            
                   // быстрое копирование 4 байт, может не работать на архитектурах, требующих выравнивания на границу слова            
                   COPY4( card->uid[3], card->answer[0] );
            
                   rx_len = ISO14443p3a_SELECT( ANTICOLLISION_2, card->answer, card );
                   
                   if( rx_len ==3 )
                     {// длина ответа 3 байта: SAK и два байта CRC  
                      card->sak = card->answer[0]; 
                      card->length = 7;                               
                      /// успешное обнаружение карты с 7 байтным UID
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
 
 int rx_len = this->transceive( true, fifodata, 2*8 ); // передача 7байт , команда select должна быть с контрольной суммой
 if( rx_len )
   {
   }
 return rx_len;
}
