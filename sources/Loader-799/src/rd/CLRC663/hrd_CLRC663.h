#ifndef HRD_CLRC663_H
#define HRD_CLRC663_H

#include <stdint.h>

#include "../hrd_Globals.h"

#include "../scmRTOS/Common/scmRTOS.h"

#include "../Drv/Spi/SpiAbstractBus.h"
#include "hrd_CLRC663_Registers.h"

#include "../CardAnalogInterface.h"
//#include "hrd_CLRC663_SPI.h"

extern "C" void PIOINT2_IRQHandler(void );
#if 0
//! Абстрактный класс для интерфейса работы с картами стандарта ISO14443
class TCardAnalofInterface
{
 public:

   virtual uint8_t readError() const = 0;
   /*!
    * Осуществить передачу в карту и принять ответ
    * \param crc_enable если true, то рассчитываеть и добавлять конткольную сумму в посылку, а в ответе проверять контрольную сумму
    * \param tx данные для передачи
    * \param tx_bit_num количество передаваемых данных в БИТАХ (не в байтах)
    * \param rx_buf буфер приемника
    * \param rx_buf_size размер буфера приемника в байтах
    * \return количество принятых байт
    */
   virtual int transceive( bool crc_enable, const uint8_t *tx, int tx_bit_num, uint8_t *rx_buf, int rx_buf_size ) = 0;
   
   /*!
    * Передать с контрольной суммой
    */
 //  virtual int transmit( const uint8_t *tx, int tx_byte_num )=0;
   virtual void fieldOff() =0;
   virtual void fieldOn()=0;  
   
   /*!
    * Если микросхема сама реализует аутентификацию по Crypto 1, то переопределить эту функцию в наследнике
    * \param cmd команда по ключу A или B, 0x60 или 0x61 соответнственно 
    * \param uid 4 байта UID
    * \param bl_addr номер(адрес) блока данных карты
    * \param key 48 битный ключ для аутентификации
    * \return 1 если аутентификация прошла успешно
    */
   virtual int cryptoAuthentification( uint8_t cmd, const uint8_t uid[4], uint8_t bl_addr, const uint8_t key[6] )
    {
     return 0;
    }
};

#endif

/*!
 Абстрактный класс для работы с CLRC663
*/
class TClrc663: public TCardAnalofInterface
{
 /*!< Обработчтик прерывания */
 friend void PIOINT2_IRQHandler(void ); // обработчтик прерывания
 
 public:
   void init();
   virtual uint8_t readError() const;
   virtual int transceive( bool crc_enable, const uint8_t *tx, int tx_bit_num, uint8_t *rx_buf, int rx_buf_size );  
   
   /*! 
    * Выключение поля
    */
   virtual void fieldOff();
   virtual void fieldOn();     
      
   int cryptoAuthentificationA( const uint8_t uid[4], uint8_t bl_addr, const uint8_t key[6] ) ;
   int cryptoAuthentificationB( const uint8_t uid[4], uint8_t bl_addr, const uint8_t key[6] ) ;
   
 protected:
   virtual bool writeBytes( uint8_t addr, uint8_t size, const uint8_t *tx ) = 0;
   virtual int readFifo( uint8_t addr, uint8_t size, uint8_t *rx ) = 0;
   virtual int readByte( uint8_t addr, uint8_t *rx ) const =0;
  
   /*!
    * Очистка FIFO
    */
   void flushFIFO();
   void enableCRC( bool enable );
   
   void resetCrypto();
   
   OS::TEventFlag CLRCEventFlag;
   
   uint8_t getFIFOLength()
     {
      return readRegister( HW_RC663_REG_FIFOLENGTH );
     }
     
   void enableRxIrq()
     {
      writeRegister( HW_RC663_REG_IRQ0EN, HW_RC663_BIT_IRQINV 
                            //| HW_RC663_BIT_IDLEIRQ | HW_RC663_BIT_TXIRQ
                            | HW_RC663_BIT_RXIRQ ); 
      writeRegister( HW_RC663_REG_IRQ1EN, HW_RC663_BIT_IRQPUSHPULL | HW_RC663_BIT_GLOBALIRQ );   //IRQEn1
     }  
   virtual int cryptoAuthentification( uint8_t cmd, const uint8_t uid[4], uint8_t bl_addr, const uint8_t key[6] );  
 public:    
   uint8_t readRegister( uint8_t reg_addr ) const
     {
      uint8_t buf;
      //Clrc663SPI.read( reg_addr, &buf );
      readByte( reg_addr, &buf );
      return buf;
     } 
     
   void writeRegister( uint8_t reg_addr, uint8_t val );
  // void reset() ; 
  
  void clearIRQ();
  
  void writeFIFO( const uint8_t *tx, int bytenum );
  int  readFIFO( uint8_t *rx, int bytenum );  
  
  bool waitInterrupt( int timeout, uint8_t int_mask0, uint8_t int_mask1 );
  void idleCommand();
  
     
  int CPLD_command();
};

////////////////////////////////////////
/*!
 * Класс для работы с CLRC663 через SPI
 */
class TClrc663Spi:public TClrc663
{
 public:
    TClrc663Spi( TSpiAbstractBus *spi )
      {
       this->spi = spi ;
      }
   void init();

 protected:   
   virtual bool writeBytes( uint8_t addr, uint8_t size, const uint8_t *tx );
   virtual int  readFifo( uint8_t addr, uint8_t size, uint8_t *rx );
   virtual int  readByte( uint8_t addr, uint8_t *rx )const;   
  // int read( uint8_t addr, uint8_t *rx );
  
   void setCS( bool s ) const;
    
   TSpiAbstractBus *spi;
};

extern TClrc663Spi Clrc663;


#endif
