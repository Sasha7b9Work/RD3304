#ifndef HRD_CR95LF_H
#define HRD_CR95LF_H

#include <stdint.h>

#include "../hrd_Globals.h"

#include "../scmRTOS/Common/scmRTOS.h"

#include "../Drv/Spi/SpiAbstractBus.h"

#include "../CardAnalogInterface.h"


//����� ������
#define CR95HF_POLL_RX    0x08
#define CR95HF_POLL_TX    0x04

//������
#define CR95HF_ERR_POLL_TOUT          (-100)

#define RX_READY (1<<3)
#define TX_READY (1<<2)


extern "C" void PIOINT0_IRQHandler(void );

/*!
 ����������� ����� ��� ������ � CLRC663
*/
class TCr95lf: public TCardAnalofInterface
{
 /*!< ����������� ���������� */
 friend void PIOINT0_IRQHandler(void ); // ���������� ����������
 
 public:
   void init();
  // virtual uint8_t readError() const;            
   
   /*! 
    * ���������� ����
    */
   virtual void fieldOff();
   virtual void fieldOn();     
     
   virtual int transceive( bool crc_enable, const uint8_t *tx, int tx_bit_num, uint8_t *rx_buf, int rx_buf_size );      
      
   //int cryptoAuthentificationA( const uint8_t uid[4], uint8_t bl_addr, const uint8_t key[6] ) ;
   //int cryptoAuthentificationB( const uint8_t uid[4], uint8_t bl_addr, const uint8_t key[6] ) ;
  
   int getIDN( uint8_t *data, int max_len ); 
    
   void startup();
    
   int calibrate( uint8_t *DacDataL, uint8_t *DacDataH );
    
   //����������� ����
   static const uint8_t CR95HF_SEND_CMD = 0x00;
   static const uint8_t CR95HF_RESET    = 0x01;
   static const uint8_t CR95HF_READ     = 0x02;
   static const uint8_t CR95HF_POLL     = 0x03;
        
    //�������
    static const uint8_t  CR95HF_CMD_IDN            = 0x01;
    static const uint8_t  CR95HF_CMD_PROTOCOL_SELECT= 0x02;
    static const uint8_t  CR95HF_CMD_SEND_RECV      = 0x04;
    static const uint8_t  CR95HF_CMD_IDLE           = 0x07;
    static const uint8_t  CR95HF_CMD_RD_REG         = 0x08;
    static const uint8_t  CR95HF_CMD_WR_REG         = 0x09;
    static const uint8_t  CR95HF_CMD_BAUD_RATE      = 0x0A;
    static const uint8_t  CR95HF_CMD_ECHO           = 0x55;

    //���� �������
    static const uint8_t   CR95HF_RESP_OK             = 0x00;
    static const uint8_t   CR95HF_RESP_ECHO           = 0x55;
    static const uint8_t   CR95HF_RESP_TAG_DECODED    = 0x80;
    static const uint8_t   CR95HF_RESP_INVALID_LEN    = 0x82;
    static const uint8_t   CR95HF_RESP_INVALID_PROTO  = 0x83;
    static const uint8_t   CR95HF_RESP_COMM_ERR       = 0x86;
    static const uint8_t   CR95HF_RESP_FRAME_TOUT     = 0x87;
    static const uint8_t   CR95HF_RESP_INVALID_SOF    = 0x88;
    static const uint8_t   CR95HF_RESP_RECV_BUF_OVF   = 0x89;
    static const uint8_t   CR95HF_RESP_FRAMING_ERR    = 0x8A;
    static const uint8_t   CR95HF_RESP_EGT_TOUT       = 0x8B;
    static const uint8_t   CR95HF_RESP_NFC_INVALID_LEN= 0x8C;
    static const uint8_t   CR95HF_RESP_NFC_CRC_ERR    = 0x8D;
    static const uint8_t   CR95HF_RESP_RECV_LOST      = 0x8E;
    static const uint8_t   CR95HF_RESP_VALID_BITS     = 0x90;

    //���������
    static const uint8_t    CR95HF_PROTO_OFF     = 0x00;
    static const uint8_t    CR95HF_PROTO_15693   = 0x01;
    static const uint8_t    CR95HF_PROTO_14443A  = 0x02;
    static const uint8_t    CR95HF_PROTO_14443B  = 0x03;
    static const uint8_t    CR95HF_PROTO_18092   = 0x04;
    
 protected:
     uint8_t state;
     virtual int swap( uint16_t size, const uint8_t *tx, uint8_t *rx ) = 0;
     virtual bool lock() = 0;
     virtual void unlock() = 0;
     
     void sendCmdNoPoll( uint8_t cmd, uint8_t len, const uint8_t *data, uint8_t suffix );
     int  poll( uint8_t mask ); 
     void sendCmd( uint8_t cmd, uint8_t len, const uint8_t *data, uint8_t suffix =0  );
     int  readData( uint8_t* resp, uint8_t data_len, uint8_t *data );

     uint8_t calibrateStep( uint8_t dh );
     
//   virtual bool writeBytes( uint8_t addr, uint8_t size, const uint8_t *tx ) = 0;
//   virtual int readFifo( uint8_t addr, uint8_t size, uint8_t *rx ) = 0;
//   virtual int readByte( uint8_t addr, uint8_t *rx ) const =0;
  
   /*!
    * ������� FIFO
    */
//   void flushFIFO();
//   void enableCRC( bool enable );
   
//   void resetCrypto();
   
 //  OS::TEventFlag CLRCEventFlag;
   
//   uint8_t getFIFOLength()
//     {
//      return readRegister( HW_RC663_REG_FIFOLENGTH );
 //    }
     
//   void enableRxIrq()
  //   {
//      writeRegister( HW_RC663_REG_IRQ0EN, HW_RC663_BIT_IRQINV     
  //                          | HW_RC663_BIT_RXIRQ ); 
    //  writeRegister( HW_RC663_REG_IRQ1EN, HW_RC663_BIT_IRQPUSHPULL | HW_RC663_BIT_GLOBALIRQ );   //IRQEn1
   //  }  
   //virtual int cryptoAuthentification( uint8_t cmd, const uint8_t uid[4], uint8_t bl_addr, const uint8_t key[6] );  
 public:    
  /* uint8_t readRegister( uint8_t reg_addr ) const
     {
      uint8_t buf;
      //Clrc663SPI.read( reg_addr, &buf );
      readByte( reg_addr, &buf );
      return buf;
     } */
     
//   void writeRegister( uint8_t reg_addr, uint8_t val );
  // void reset() ; 
  
  //void clearIRQ();
  
  //void writeFIFO( const uint8_t *tx, int bytenum );
  //int  readFIFO( uint8_t *rx, int bytenum );  
  
  //bool waitInterrupt( int timeout, uint8_t int_mask0, uint8_t int_mask1 );
  //void idleCommand();
  
     
  //int CPLD_command();
};

////////////////////////////////////////
/*!
 * ����� ��� ������ � Cr95lf ����� SPI
 */
class TCr95lfSpi:public TCr95lf
{
 public:
    TCr95lfSpi( TSpiAbstractBus *spi )
      {
       this->spi = spi ;
      }
   void init();

   
 protected:   
   virtual int swap( uint16_t size, const uint8_t *tx, uint8_t *rx );
   virtual bool lock() ;
   virtual void unlock();

  
   void setCS( bool s ) const;
  
   //void hrd_CR95HF_SendCmdNoPoll( uint8_t cmd, uint8_t len, const uint8_t *data, uint8_t suffix );
   //void hrd_CR95HF_SendCmd( uint8_t cmd, uint8_t len, const uint8_t *data, uint8_t suffix =0  );
   //int hrd_CR95HF_Poll( uint8_t mask ); 
   //int hrd_CR95HF_Read( uint8_t* resp, uint8_t data_len, uint8_t *data );
    
   TSpiAbstractBus *spi;
};

//extern TCr95lfSpi Cr95lf;


#endif
