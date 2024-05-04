#ifndef HRD_SPI_H
#define HRD_SPI_H

#include "stdint.h"
#include "LPC13xx.h"
#include "../Drv/Spi/SpiLpcBus.h"
#include "../scmRTOS/Common/scmRTOS.h"
#include "hrd_Globals.h"

//#include "../Drv/Spi/SpiOsBus.h"

//#define SPI_FREQ 1800000

#if 0
///////////////////////////////////////////////
class SpiLpcBus : public TSpiOsBus
{
 public:
   static void hrdinit( LPC_SSP_TypeDef *spi_s );
   
    /*!
    * Обработка прервания, сделан публичным для вызова из обработчика аппаратного прерывания
    */
   void SSP_IRQHandler( LPC_SSP_TypeDef* s);
   
 protected:     

   void clearRxFIFO( LPC_SSP_TypeDef* s);
   
   void popFromRx( LPC_SSP_TypeDef* s );
     
     // заталкиваем байты в Tx FIFO
  void pushToTx(LPC_SSP_TypeDef* s);
   
  int swapBytes( LPC_SSP_TypeDef* s, uint16_t size, const uint8_t *tx, uint8_t *rx );
   
 private:
  const uint8_t *tx_pointer;       //!< указатель по буферу передатчика
  uint8_t       *rx_pointer;       //!< указатель по буферу приемника
  uint16_t      rx_counter;        //!< счетчик принятых бат
  uint16_t      tx_counter;        //!< счетчик переданных байт  
};
#endif

template< uint32_t spi_struct >
class SPI_Bus: public TSpiLpcBus
//template< uint32_t spi_struct > class SPI_Bus: public Abstract_SPI_Bus
{
 public:
    
   /*! Инициализация    
    */ 
  // static void hrdinit( LPC_SSP_TypeDef *spi_s );
   SPI_Bus()
     {
      //hrdSpi = (LPC_SSP_TypeDef*)spi_struct;      
      this->hrdinit( (LPC_SSP_TypeDef*)spi_struct );
  
     }  
   
   /*!
    * Обмен через шину SPI
    *
    * Шина уже должна быть захвачена, и нужный CS активизирован
    * \param size количество байт для обмена
    * \param tx указатель на массив с передаваемыми байтами, если 0, то будут переданы 0xFF
    * \param rx указатель на буфер под принимаемые данные, если 0, то принимаемые данные не сохраняются
    *
    * \return 1 в случае успеха, иначе 0
    */
   virtual int swap( uint16_t size, const uint8_t *tx, uint8_t *rx );           
   
   /*!
    * Обработка прервания, сделан публичным для вызова из обработчика аппаратного прерывания
    */
   //void SSP_IRQHandler( void );
   
 protected: 
//   LPC_SSP_TypeDef const *spi_p = spi_struct;
 
   //LPC_SSP_TypeDef *hrdSpi;
  
   //OS::TMutex     mutex;        //!< мутекс для организации доступа к шине
   //OS::TEventFlag eventFlag;
/*   
   const uint8_t *tx_pointer;       //!< указатель по буферу передатчика
   uint8_t       *rx_pointer;       //!< указатель по буферу приемника
   uint16_t      rx_counter;        //!< счетчик принятых бат
   uint16_t      tx_counter;        //!< счетчик переданных байт
*/   
  // void clearRxFIFO();
  // void popFromRx();
  // void pushToTx();
   
};

//static const uint32_t adr = LPC_SSP0_BASE;

typedef SPI_Bus<LPC_SSP0_BASE>spi_type;
extern spi_type spiBus;

#endif
