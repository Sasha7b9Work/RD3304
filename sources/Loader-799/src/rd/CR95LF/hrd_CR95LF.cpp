#include "hrd_CR95LF.h"


#include "LPC13xx.h"

extern "C"
void PIOINT0_IRQHandler(void )
{
 OS::scmRTOS_ISRW_TYPE ISR; //для переключения задачи после обработки прерывания

 uint32_t mask = LPC_GPIO0->MIS;
 if( mask &(1<<1 ) )
   {
    LPC_GPIO0->IC = (1<<1); // очистка флага прерывания  
//    Cr95lf.CLRCEventFlag.signal();

   } 
}

void TCr95lf::init()
{
 state = 0;
//P0.1 IRQIN
 //LPC_IOCON->PIO0_1 = 0x00; 
// GPIO_SET_VAL( LPC_GPIO0, 1, 1);
 //LPC_GPIO0->DIR |= (0x01 << 1);  
 
/* 
 LPC_GPIO0->IC = (1UL<<1 );    // очистка флага прерывания
 LPC_GPIO0->IS &= ~(1UL<<1);   // по изменению
 LPC_GPIO0->IBE  &= ~(1UL<<1); //
 LPC_GPIO0->IEV  &= ~(1UL<<1); // по спаду
 LPC_GPIO0->IE |= (1UL<<0);    // разрешение прерывания с ноги 
 
 NVIC_EnableIRQ( EINT0_IRQn );    // разрешение прерываний с порта
*/
}

/*
uint8_t TCr95lf::readError() const 
{
//! \todo
 return 0;
}
*/

int TCr95lf::poll( uint8_t mask )
{
 int i = 128; // количество попыток для поллинга
 int retvalue = -1; // возвращаемое значение
 uint8_t buf[2];
 uint8_t ans;
 
 if( this->lock() ) // захват шины
   {
    buf[0] = CR95HF_POLL; // control byte, 

    if( this->swap( 1, buf, buf ) )
      {
       while( i-- )
         {
          buf[0] = 0x03;       
          if( this->swap( 1, buf, &ans ) )
            {
             if( (ans & mask )  && (ans != 0xFF))
               {
                // есть ответ
                retvalue = 0;
                break;
               } 
            } 
         }    
      }
    //setCS( false ); 
    //pause();    // гарантированная пауза, чтобы микросхема успела отработать
    this->unlock(); 
   }
 return retvalue;  
}


void TCr95lf::sendCmdNoPoll( uint8_t cmd, uint8_t len, const uint8_t *data, uint8_t suffix )
{
 uint8_t buf[3];

 if( this->lock() )  
   {
    //setCS( true );
    buf[0] = CR95HF_SEND_CMD;
    buf[1] = cmd;    
    buf[2] = len;
    if( suffix )
      buf[2]++;
    this->swap( 3, buf, buf );
    if( len )
      {
       this->swap( len, data, 0 ); 
      }
    if( suffix != 0  )
      {
       this->swap( 1, &suffix, 0 ); 
      }  
    //setCS( false );
    this->unlock();     //освобождаем шину
   } 
 //pause();
}

void TCr95lf::sendCmd( uint8_t cmd, uint8_t len, const uint8_t *data, uint8_t suffix )
{
 if( poll(TX_READY) == 0)
   {
    sendCmdNoPoll( cmd, len, data, suffix );
   }
}

int TCr95lf::readData( uint8_t* resp, uint8_t rxbuf_len, uint8_t *rx_buf )
{
 int data_len = 0;
 if( this->lock() )
   { 
    //setCS( true );
    uint8_t buf[3];
    buf[0] = CR95HF_READ;
    if( this->swap( 3, buf, buf ) )
      {
       *resp = buf[1];  // код ответа
       data_len = buf[2]; // количество присланных данных       
       if( data_len > rxbuf_len )
         data_len = rxbuf_len;
       if( data_len )
         this->swap( data_len, 0, rx_buf );    // считываем данные
      }   
    //setCS( false );
   //pause(); // пауза после команды
    this->unlock();
    return data_len;
   }
 return data_len;  
 
}

void TCr95lf::fieldOff()
{
 uint8_t buf[2];
 buf[0] = CR95HF_PROTO_OFF;
 buf[1] = 0;
// buf[2] = 1;
 //buf[3] = 0;//16;  

 sendCmd( CR95HF_CMD_PROTOCOL_SELECT, 2, buf );
  if( poll(RX_READY) == 0)
   {
    uint8_t resp;
    uint8_t data[2];
    readData( &resp, 2, data );
   // return 0;
   }
 //return -1;  

}

void TCr95lf::fieldOn()
{
 if( state == 0 )
   {
    this->startup();
    state =1;
   }

 uint8_t buf[8];
 buf[0] = CR95HF_PROTO_14443A;
 buf[1] = 0;
 buf[2] = 4;
 buf[3] = 0;//16;  

 sendCmd( CR95HF_CMD_PROTOCOL_SELECT, 4, buf );
 if( poll(RX_READY) == 0)
   {
    uint8_t resp;
    uint8_t data[2];
    readData( &resp, 2, data );
   // return 0;
   }
   
 /*  
 buf[0] = 0x68;
 buf[1] = 0x00;
 buf[2] = 0x00;//16;    
 sendCmd( CR95HF_CMD_WR_REG, 3, buf );  
 if( poll(RX_READY) == 0)
   {
    uint8_t resp;
    uint8_t data[2];
    readData( &resp, 2, data );  
   }
 
 buf[0] = 0x69;
 buf[1] = 0x01;
 buf[2] = 0x00; 
 sendCmd( CR95HF_CMD_RD_REG, 3, buf );  
 if( poll(RX_READY) == 0)
   {
    uint8_t resp;
    uint8_t data[2];
    readData( &resp, 2, data );  
   }
 */
 
/*
 buf[0] = 0x68;
 buf[1] = 0x01;
 buf[2] = 0x01;//16;   
 buf[3] = 0xD7;//0xD3;
 sendCmd( CR95HF_CMD_WR_REG, 4, buf );  
  if( poll(RX_READY) == 0)
   {
    uint8_t resp;
    uint8_t data[2];
    readData( &resp, 2, data );  
   }
*/
}

int TCr95lf::getIDN( uint8_t *data, int max_len )
{
 sendCmd( CR95HF_CMD_IDN, 0,0 );
 if( poll(RX_READY) == 0)
   {
    uint8_t resp;
    readData( &resp, max_len, data );
    return 0;
   }
 return -1;  
}

int TCr95lf::transceive( bool crc_enable, const uint8_t *tx, int tx_bit_num, uint8_t *rx_buf, int rx_buf_size )
{//! \todo

 int result_rx_len = 0; 
 int last_bits = tx_bit_num % 8;
 int txbyte_num = tx_bit_num/8 + (last_bits?1:0);
 if( last_bits == 0 )
   last_bits = 8;
 uint8_t suffix = last_bits | (crc_enable?(1<<5):0);
 
 sendCmd( CR95HF_CMD_SEND_RECV, txbyte_num, tx, suffix );
   
 if( poll( RX_READY ) == 0)
   {
    uint8_t resp;
    result_rx_len = readData( &resp, rx_buf_size, rx_buf );          
    if( resp == 0x80 )
      {
       result_rx_len -=3; // не учитываем в ответе респонс байт, байт длины и суффикс
       uint8_t r_suffix1 = rx_buf[ result_rx_len ];
       if( r_suffix1 & 0x80 ) // была коллизия
         {
          int col_bit = rx_buf[ result_rx_len+1 ]*8 +(rx_buf[ result_rx_len+2 ]&0x0f); // определем бит с коллизией
          result_rx_len |= 0x80000000 | 0x01000000 |(col_bit<<8);
         }       
       if(r_suffix1 & ((1<<4)) )  // были ошибки в parity
         {
          result_rx_len |= 0x80000000 | 0x02000000; // признак ошибки data integrity
         }
      }
   }     
 
 return result_rx_len;
}

void TCr95lf::startup()
{// стартовая последовательность на IRQ_IN
 LPC_IOCON->PIO0_1 = 0x00;
 LPC_GPIO0->DATA |= (0x01 << 1);  
 LPC_GPIO0->DIR |= (0x01 << 1);  
 OS::sleep(TICKS_PER_SEC/50);

 LPC_GPIO0->DATA &= ~(0x01 << 1); 
 //systick_delay(1);
 OS::sleep(TICKS_PER_SEC/50);
 LPC_GPIO0->DATA |= (0x01 << 1); 
 //systick_delay(2);  
 OS::sleep(TICKS_PER_SEC/50);

}

//////////////////////////////////////////////////////////
void TCr95lfSpi::init()
{
 TCr95lf::init();

 // P0.2 = CS
 LPC_IOCON->PIO0_2  = 0x00;//SSEL как GPIO
 LPC_GPIO0->DIR |= (2<<1);
 GPIO_SET_VAL( LPC_GPIO0, 2, 1); 

}

void TCr95lfSpi::setCS( bool s ) const
{
 if( s )
   GPIO_SET_VAL( LPC_GPIO0, 2, 0);
 else  
   GPIO_SET_VAL( LPC_GPIO0, 2, 1);
}

int TCr95lfSpi::swap( uint16_t size, const uint8_t *tx, uint8_t *rx )
{
 return spi->swap( size, tx, rx );
}

bool TCr95lfSpi::lock()
{
 if( spi->lock() )
   {
    setCS( true );
    return true;
   }
 return false;   
}

void TCr95lfSpi::unlock()
{
 setCS( false );
 spi->unlock();
}

uint8_t TCr95lf::calibrateStep(  uint8_t dh )
{
 uint8_t res =0;
 uint8_t tx[16];
 tx[0] = 0x03;
 
 tx[1] = 0xA1;
 tx[2] = 0x00;
 
 tx[3] = 0xF8;
 tx[4] = 0x01;
 
 tx[5] = 0x18;
 tx[6] = 0x00;
 
 tx[7] = 0x20;
 tx[8] = 0x60;
 tx[9] = 0x60;
 
 tx[10] =0x00;
 tx[11] =dh;
 
 tx[12] =0x3F;
 tx[13] =0x01; 
 
 sendCmd( CR95HF_CMD_IDLE, 0x0E, tx, 0 );
 OS::sleep(TICKS_PER_SEC*(1./2+1./4-1./8 -1./16+1./32) /*+1./4-1./8-1./16)*/ );
 if( poll( RX_READY ) == 0)
   {
    //if( this->lock() )
      {
       uint8_t buf[4];
       buf[0] = CR95HF_READ;
       uint8_t resp;
       if( readData( &resp, 2, buf ) )       
//       if( this->swap( 4, buf, buf ) )  
         {
          res = buf[0];
         }
     //  this->unlock();  
      } 
   }
 return res;  
}

int TCr95lf::calibrate( uint8_t *DacDataL, uint8_t *DacDataH )
{
// uint8_t res = 0;
/*
 uint8_t DacDataRef=0;
 uint8_t h=0xFC;
 uint8_t ans;
 ans = calibrateStep(0x00);
 if( ans == 0x02 )
   {// step 1
    ans = calibrateStep(0xFC);
    if( ans ==  0x01 )
      {
       h-=0x80;
       ans = calibrateStep(h);
       if( ans == 0x01 )
         h-=0x40;
       else
         h+=0x40;
         
       ans = calibrateStep(h);
       if( ans == 0x01 )
         h-=0x20;
       else
         h+=0x20;   
       
       ans = calibrateStep(h);
       if( ans == 0x01 )
         h-=0x10;
       else
         h+=0x10;   
         
       ans = calibrateStep(h);
       if( ans == 0x01 )
         h-=0x08;
       else
         h+=0x08;     
       
       ans = calibrateStep(h);
       if( ans == 0x01 )
         h-=0x04;
       else
         h+=0x04;
                
       ans = calibrateStep(h);
       if( ans == 0x01 )
         DacDataRef = h-4;
       else  
         DacDataRef = h;
         
       *DacDataL =  DacDataRef-8; 
       *DacDataH =  DacDataRef+8; 
       
       // тест с картой
        
      }
   }
*/
  {
         uint8_t tx[16];
         tx[0] =0x0B;
         
         tx[1] =0x21; 
         tx[2] =0x00; 
         
         tx[3] =0x78;  //7901?
         tx[4] =0x01; 
         
         tx[5] = 0x18;
         tx[6] = 0x00;
         
         tx[7] = 0x20;
         tx[8] = 0x60;
         tx[9] = 0x60;
         
         tx[10] = 0x2C; //*DacDataL;
         tx[11] = 0x3C;//*DacDataH;

         tx[12] =0x3F;
         tx[13] =0xF0; 
         //01070E0B 2100 7801 1800 206060 64743F0     
        
         int i;
         for( i=0;i<60; i++ )
           {
            sendCmd( CR95HF_CMD_IDLE, 0x0E, tx, 0 );
            OS::sleep(TICKS_PER_SEC);
            if( poll( RX_READY ) == 0)
              {
               uint8_t buf[4];
               buf[0] = CR95HF_READ;
               uint8_t resp;
               if( readData( &resp, 2, buf ) )       
                 {
                  volatile static uint8_t es = buf[0];
                  if( es == 2 )
                    {//detect card
                     return 1;
                    }
                  else
                    {//return -1;
                    }  
              }
          }      
         }
      }   
   return 0;
}
