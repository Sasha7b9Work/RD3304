#ifndef _ISO14443P3_H_
#define _ISO14443P3_H_

#include "CardUid.h"

//#include "../Hardware/CLRC663/hrd_CLRC663.h"
#include "../Hardware/CardAnalogInterface.h"

#define Type_NO                    0

#define Type_Mifare_Classic_Mini   0x1000
#define Type_MIFARE_Classic_1K     0x1001
#define Type_MIFARE_Classic_2K     0x1002
#define Type_MIFARE_Classic_4K     0x1004

#define Type_Mifare_Plus_S_SL0     0x2000
#define Type_Mifare_Plus_S_2K_SL1  0x2102
#define Type_Mifare_Plus_S_4K_SL1  0x2104 
#define Type_Mifare_Plus_S_SL3     0x2300

#define Type_Mifare_Plus_X_SL0     0x3000 
#define Type_Mifare_Plus_X_2K_SL1  0x3102 
#define Type_Mifare_Plus_X_4K_SL1  0x3104
#define Type_Mifare_Plus_X_2K_SL2  0x3202
#define Type_Mifare_Plus_X_4K_SL2  0x3204
#define Type_Mifare_Plus_X_SL3     0x3300
//#define Type_Mifare_Plus_X_SL0       14 

#define Type_Mifare_UL             0x4000
#define Type_Mifare_ULC            0x4100

#define Type_Mifare_DESFIRE        0x5000


#define Type_SmartMX_MIFARE_1K     0x6001
#define Type_SmartMX_MIFARE_4K     0x6004

#define Type_TagNPlay               0x7000


struct TIso14443p3_Card: public TUid
{
 friend uint8_t *getDataAddrForOperation( uint8_t param, uint8_t *param_size );
 public:         

   // команды протокола ISO14443p3
   static const uint8_t ISO14443_REQA_COMMAND             = 0x26;
   static const uint8_t ISO14443_WUPA_COMMAND             = 0x52;
   static const uint8_t ISO14443_ANTICOLLISION_1_COMMAND  = 0x93;
   static const uint8_t ISO14443_ANTICOLLISION_2_COMMAND  = 0x95;
   static const uint16_t ISO14443_HALT_COMMAND            = 0x5000;
   
   /*! Конструктор
    * \param maf указатель на интерфейс с Analog frontend
    */
   TIso14443p3_Card( TCardAnalofInterface* maf )
     {
      this->Maf = maf;
     }
   
   /*! Сброс карты питанием (выключением поля)
    *  После применения поле остается включенным
    */ 
   void reset( ) const;

   /*! Переводит карту в состояние halt
    */
   void doHalt( ) const;
   
   /*!
    * Выполянет комадну ATQA
    * \return 0 если нет ответа и 1 если ответ есть и this->atq содержит ATQ
    */
   int doREQA( ) const;
   
   /*!
    * Выполянет комадну WUPA
    * \return 0 если нет ответа и 1 если ответ есть и this->atq содержит ATQ
    */
   int doWUPA( ) const;
   
   bool Activate( ) const;
   int Anticollision( );

  int AuthenticateDES(  ) const;
 protected:
   TCardAnalofInterface* Maf;      //!< указатель на интерфейс работы с картой
 
   mutable uint8_t atq[2];
   uint8_t sak;
   uint8_t type;
   mutable int8_t  lastError;
  
   // блок данных для общения     
   mutable uint8_t lastCommand;
   mutable uint8_t answerLength;      
   mutable uint8_t answer[36];
  
   int transceive( bool crc_enable, const uint8_t *tx, int tx_bit_num ) const;
   int req( uint8_t cmd ) const;
   int anticollisionStep( uint8_t cmd );
   int SELECT( uint8_t level, const uint8_t uid[4] ) const;
};

#endif
