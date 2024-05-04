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
    * ��������� ���������, ������ ��������� ��� ������ �� ����������� ����������� ����������
    */
   void SSP_IRQHandler( LPC_SSP_TypeDef* s);
   
 protected:     

   void clearRxFIFO( LPC_SSP_TypeDef* s);
   
   void popFromRx( LPC_SSP_TypeDef* s );
     
     // ����������� ����� � Tx FIFO
  void pushToTx(LPC_SSP_TypeDef* s);
   
  int swapBytes( LPC_SSP_TypeDef* s, uint16_t size, const uint8_t *tx, uint8_t *rx );
   
 private:
  const uint8_t *tx_pointer;       //!< ��������� �� ������ �����������
  uint8_t       *rx_pointer;       //!< ��������� �� ������ ���������
  uint16_t      rx_counter;        //!< ������� �������� ���
  uint16_t      tx_counter;        //!< ������� ���������� ����  
};
#endif

template< uint32_t spi_struct >
class SPI_Bus: public TSpiLpcBus
//template< uint32_t spi_struct > class SPI_Bus: public Abstract_SPI_Bus
{
 public:
    
   /*! �������������    
    */ 
  // static void hrdinit( LPC_SSP_TypeDef *spi_s );
   SPI_Bus()
     {
      //hrdSpi = (LPC_SSP_TypeDef*)spi_struct;      
      this->hrdinit( (LPC_SSP_TypeDef*)spi_struct );
  
     }  
   
   /*!
    * ����� ����� ���� SPI
    *
    * ���� ��� ������ ���� ���������, � ������ CS �������������
    * \param size ���������� ���� ��� ������
    * \param tx ��������� �� ������ � ������������� �������, ���� 0, �� ����� �������� 0xFF
    * \param rx ��������� �� ����� ��� ����������� ������, ���� 0, �� ����������� ������ �� �����������
    *
    * \return 1 � ������ ������, ����� 0
    */
   virtual int swap( uint16_t size, const uint8_t *tx, uint8_t *rx );           
   
   /*!
    * ��������� ���������, ������ ��������� ��� ������ �� ����������� ����������� ����������
    */
   //void SSP_IRQHandler( void );
   
 protected: 
//   LPC_SSP_TypeDef const *spi_p = spi_struct;
 
   //LPC_SSP_TypeDef *hrdSpi;
  
   //OS::TMutex     mutex;        //!< ������ ��� ����������� ������� � ����
   //OS::TEventFlag eventFlag;
/*   
   const uint8_t *tx_pointer;       //!< ��������� �� ������ �����������
   uint8_t       *rx_pointer;       //!< ��������� �� ������ ���������
   uint16_t      rx_counter;        //!< ������� �������� ���
   uint16_t      tx_counter;        //!< ������� ���������� ����
*/   
  // void clearRxFIFO();
  // void popFromRx();
  // void pushToTx();
   
};

//static const uint32_t adr = LPC_SSP0_BASE;

typedef SPI_Bus<LPC_SSP0_BASE>spi_type;
extern spi_type spiBus;

#endif
