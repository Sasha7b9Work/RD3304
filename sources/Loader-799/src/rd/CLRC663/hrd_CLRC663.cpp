#include "hrd_CLRC663.h"
#include "../hrd_Globals.h"
#include "hrd_CLRC663_Registers.h"
#include "../hrd_SPI.h"
#include "LPC13xx.h"

#include "../scmRTOS/Common/scmRTOS.h"
//using namespace OS;

#define VECT_Timer0IRQ  0   //the timer register T0 CounterVal underflows
#define VECT_Timer1IRQ  1   //the timer register T1 CounterVal underflows
#define VECT_Timer2IRQ  2   //the timer register T2 CounterVal underflows
#define VECT_Timer3IRQ  3   //the timer register T3 CounterVal underflows
#define VECT_TxIRQ      4   //a transmitted data stream ends
#define VECT_RxIRQ      5   //a received data stream ends
#define VECT_IdleIRQ    6   //a command execution finishes
#define VECT_HiAlertIRQ 7   // FIFO-buffer pointer the FIFO data number has reached the top level as configured by the register WaterLevel
#define VECT_LoAlertIRQ 8   // FIFO-buffer pointer the FIFO data number has reached the bottom level as configured by the register WaterLevel
#define VECT_ErrIRQ     9   // contactless UART a communication error had been detected
#define VECT_LPCDIRQ   10   //LPCD a card was detected when in low-power card detection mode
#define VECT_RxSOFIRQ  11   // Receiver detection of a SOF or a subcarrier
#define VECT_GlobalIRQ 12   //all interrupt sources will be set if another interrupt request source is set

//typedef void (*InterruptVect)(void);

extern "C"
void PIOINT2_IRQHandler(void )
{
 OS::scmRTOS_ISRW_TYPE ISR; //для переключения задачи после обработки прерывания

 uint32_t mask = LPC_GPIO2->MIS;
 if( mask &(1<<10 ) )
   {
    LPC_GPIO2->IC = (1<<10); // очистка флага прерывания  
    Clrc663.CLRCEventFlag.signal();
//    Clrc663.interruptHandler();
   } 
}

void TClrc663::clearIRQ()
{
 writeRegister( HW_RC663_REG_IRQ0 ,0x7F );
 writeRegister( HW_RC663_REG_IRQ1 ,0x7F );
}

////////////////////////////

/*
uint8_t hrd_CLRC663_ReadVersion()
{
 uint8_t version;
 version = hrd_CLRC663_ReadRegister( 0x7f );
 return version;
}*/

/*
void hrd_CLRC663_ReadEEPROM( uint16_t addr, uint8_t *rx, int bytenum )
{
 uint8_t buf[4];
  
 Clrc663.flushFIFO();
 
 buf[0] = addr>>8;
 buf[1] = addr;
 buf[2] = bytenum;
 // заносим в FIFO параметры, адрес и количество байт для чтения
 Clrc663.writeFIFO( buf, 3 );
// irq0 = hrd_CLRC663_ReadRegister( 0x06 );
 hrd_CLRC663_PrepareCommandInterrupt();
 
 Clrc663.writeRegister( HW_RC663_REG_COMMAND ,HW_RC663_CMD_READE2 );    // команда чтения
 // \todo ожидать завершения команды

 if( hrd_CLRC663_WaitInterrupt( HW_RC663_BIT_IDLEIRQ, 0, TICKS_PER_SEC/20 ) ) 
   {
    Clrc663.readFIFO( rx, bytenum ); // чтение результата из FIFO
   } 
}*/

/*
int TClrc663::CPLD_command()
{
// hrd_CLRC663_Reset();
 idleCommand();
 
 fieldOff();
 
 volatile static uint8_t version;
 version =readRegister( 0x7f );
 
 volatile static uint8_t t_control = readRegister(HW_RC663_REG_TCONTROL);
// Cunfigure
 writeRegister( HW_RC663_REG_LPCD_QMIN, 0xC0); // bRegister = 0x96
 writeRegister( HW_RC663_REG_LPCD_QMAX, 0xFF);
 writeRegister( HW_RC663_REG_LPCD_IMIN, 0xC0);

 writeRegister( HW_RC663_REG_LPCD_OPTIONS ,0x00);

 writeRegister( HW_RC663_REG_T3RELOADHI ,0x02);
 writeRegister( HW_RC663_REG_T3RELOADLO ,0xF7);

 writeRegister( HW_RC663_REG_T4RELOADHI ,0x06); 
 writeRegister( HW_RC663_REG_T4RELOADLO ,0x40);

 writeRegister( HW_RC663_REG_T4CONTROL, 0x34); //bRegister =0xdc
 
 writeRegister( HW_RC663_REG_LPCD_RESULT_Q, 0);
 writeRegister( HW_RC663_REG_RCV, 0x52); 
 
 volatile static uint8_t rcv =readRegister( HW_RC663_REG_RCV ); 
 
 writeRegister( HW_RC663_REG_RXANA, 0x03 );
 
 clearIRQ();
 CLRCEventFlag.clear();
 
 //writeRegister( HW_RC663_REG_TCONTROL, 0x88 );
 t_control = readRegister(HW_RC663_REG_TCONTROL);
 
 writeRegister(HW_RC663_REG_IRQ0EN, HW_RC663_BIT_IRQINV |HW_RC663_BIT_IDLEIRQ); //bRegister = 0x90 
 writeRegister(HW_RC663_REG_IRQ1EN,HW_RC663_BIT_IRQPUSHPULL | HW_RC663_BIT_GLOBALIRQ | HW_RC663_BIT_LPCDIRQ | HW_RC663_BIT_TIMER3IRQ );

 writeRegister( HW_RC663_REG_COMMAND ,HW_RC663_CMD_LPCD | HW_RC663_BIT_STANDBY); 
 
 bool cret = CLRCEventFlag.wait(TICKS_PER_SEC*5);
 
 volatile static uint8_t s = readRegister(HW_RC663_REG_COMMAND);
 t_control = readRegister(HW_RC663_REG_TCONTROL);
 volatile static uint8_t r_i = readRegister( HW_RC663_REG_LPCD_RESULT_I ) ;
 volatile static uint8_t r_q = readRegister( HW_RC663_REG_LPCD_RESULT_Q ) ;
 
 
//////////////////////// 
 //T3ReloadHi = 2
 writeRegister( HW_RC663_REG_T3RELOADHI ,0x02);
 //T3ReloadLo = F7
 writeRegister( HW_RC663_REG_T3RELOADLO ,0xF7);

 //T4ReloadHi = 6
 writeRegister( HW_RC663_REG_T4RELOADHI ,0x06);
 //T4ReloadLo = 40
 writeRegister( HW_RC663_REG_T4RELOADLO ,0x40);

 writeRegister( HW_RC663_REG_T4CONTROL, HW_RC663_BIT_T4AUTOLPCD | HW_RC663_BIT_T4AUTOWAKEUP 
                                      | HW_RC663_VALUE_TCLK_LFO_16_KHZ | HW_RC663_BIT_T4AUTORESTARTED); //0x28
                                      
 //  Set LPCD_IRQ_Clr to zero to get correct LPCD measurements period and to avoid not detecting card in first LPCD cycle when Filter is ON. This is also a workaround for HW Artifacts : artf207360, artf207359 and artf207361 of Rc66303 Si. 
 writeRegister( HW_RC663_REG_LPCD_RESULT_Q, 0);

 // Set Qmin register 
     //bRegister = bQMin | (uint8_t)((bIMax & 0x30U) << 2U);
 writeRegister( HW_RC663_REG_LPCD_QMIN, 0x96); // bRegister = 0x96
 
 // Set Qmax register
 //        bRegister = bQMax | (uint8_t)((bIMax & 0x0CU) << 4U);
 writeRegister(HW_RC663_REG_LPCD_QMAX, 0x98 ); //bRegister = 0x98

 // Set Imin register 
        //bRegister = bIMin | (uint8_t)((bIMax & 0x03U) << 6U);
 writeRegister( HW_RC663_REG_LPCD_IMIN, 0xE9 ); //bRegister=0xE9

 uint8_t bRegister;
// Set Mix2Adc bit 
 bRegister=readRegister( HW_RC663_REG_RCV );
 uint8_t rcv_bak = bRegister;
 bRegister |= HW_RC663_BIT_RX_SHORT_MIX2ADC;
 writeRegister( HW_RC663_REG_RCV, HW_RC663_BIT_RX_SHORT_MIX2ADC); // bRegister=0x40

 // Backup current RxAna setting 
 uint8_t bRxAnaBackup = readRegister( HW_RC663_REG_RXANA );
 // Raise receiver gain to maximum 
 writeRegister( HW_RC663_REG_RXANA, HW_RC663_LPCD_RECEIVER_GAIN);
                                      
 
 writeRegister(HW_RC663_REG_IRQ0EN, HW_RC663_BIT_IRQINV|HW_RC663_BIT_IDLEIRQ); //bRegister = 0x90 
 writeRegister( HW_RC663_REG_IRQ1EN ,HW_RC663_BIT_IRQPUSHPULL | HW_RC663_BIT_GLOBALIRQ| HW_RC663_BIT_LPCDIRQ); //bRegister = 0xE0
 
 bRegister = readRegister( HW_RC663_REG_T4CONTROL);
 // Start T4.
 bRegister |= HW_RC663_BIT_T4STARTSTOPNOW | HW_RC663_BIT_T4RUNNING;
 writeRegister( HW_RC663_REG_T4CONTROL, bRegister); //bRegister =0xdc

 // Wait until T4 is started
 do
   {
    bRegister=readRegister(HW_RC663_REG_T4CONTROL );
   }
 while(0U == (bRegister & HW_RC663_BIT_T4RUNNING));
 
 clearIRQ();
 CLRCEventFlag.clear();
 
 static uint8_t irq0 = readRegister( HW_RC663_REG_IRQ0 );
 static uint8_t irq1 = readRegister( HW_RC663_REG_IRQ1 );
 
 writeRegister( HW_RC663_REG_COMMAND ,HW_RC663_CMD_LPCD | HW_RC663_BIT_STANDBY);
 
 bool ret = CLRCEventFlag.wait(TICKS_PER_SEC*15);
 
 irq0 = readRegister( HW_RC663_REG_IRQ0 );
 irq1 = readRegister( HW_RC663_REG_IRQ1 );
 
 // Restore RxAna register 
///> writeRegister( HW_RC663_REG_RXANA, bRxAnaBackup);
 
 // Clear Mix2Adc bit 
 bRegister = readRegister( HW_RC663_REG_RCV );
 bRegister &= (uint8_t)~(uint8_t)HW_RC663_BIT_RX_SHORT_MIX2ADC;
///> writeRegister( HW_RC663_REG_RCV, bRegister);
 
 // Stop Timer4 if started 
 bRegister = readRegister( HW_RC663_REG_T4CONTROL );
 bRegister |= HW_RC663_BIT_T4STARTSTOPNOW;
 bRegister &= (uint8_t)~(uint8_t)HW_RC663_BIT_T4RUNNING;
 ///> writeRegister( HW_RC663_REG_T4CONTROL, bRegister );

 //<idleCommand();
 // Clear LPCD interrupt source to avoid any spurious LPCD_IRQ to be triggered. 
 //<writeRegister( HW_RC663_REG_LPCD_RESULT_Q, HW_RC663_BIT_LPCDIRQ_CLR);

// return ret?1:0; 
 //volatile uint8_t version;
 //version =readRegister( 0x7f );
 
 //LPCD_QMin =C0
 //writeRegister( HW_RC663_REG_LPCD_QMIN ,0xC0);
 //LPCD_QMax = FF
 //writeRegister( HW_RC663_REG_LPCD_QMAX ,0xFF);
 //LPCD_IMin = C0
 //writeRegister( HW_RC663_REG_LPCD_IMIN ,0xC0);

 //LPCD_Options = 0
 //writeRegister( HW_RC663_REG_LPCD_OPTIONS ,0x00);

 //T3ReloadHi = 2
 //writeRegister( HW_RC663_REG_T3RELOADHI ,0x02);
 //T3ReloadLo = F7
 //writeRegister( HW_RC663_REG_T3RELOADLO ,0xF7);
 
 //T4ReloadHi = 6
 //writeRegister( HW_RC663_REG_T4RELOADHI ,0x06);
 //T4ReloadLo = 40
 //writeRegister( HW_RC663_REG_T4RELOADLO ,0x40);
 
 //T4Control = 34       // T4AutoTrimm T4AutoLPCDT 4AutoWakeUp 
 // Configure T4 for AutoLPCD and Autowakeup and to use 16KHz LFO.
 //writeRegister( HW_RC663_REG_T4CONTROL,
   //               HW_RC663_BIT_T4AUTOLPCD|HW_RC663_BIT_T4AUTOWAKEUP
     //            |HW_RC663_VALUE_TCLK_LFO_16_KHZ |HW_RC663_BIT_T4AUTORESTARTED);  // T4AutoWakeUp 
 
 //LPCD_Q_Result = 0
 //writeRegister( HW_RC663_REG_LPCD_RESULT_Q ,0x0);
 
 //Rcv = 52  LPCD mode for ADC, internal analog block (RX), Collision has at least 1/2 of signal strength
 //writeRegister( HW_RC663_REG_RCV ,0x52);
 
 //RxAna = 03   rcv_gain =3
 //writeRegister( HW_RC663_REG_RXANA ,0x03);
 
 //clearIRQ();
 //writeRegister( HW_RC663_REG_IRQ0EN ,HW_RC663_BIT_IRQINV|HW_RC663_BIT_IDLEIRQ);    // IdleIRQEn);
 //writeRegister( HW_RC663_REG_IRQ1EN ,HW_RC663_BIT_IRQPUSHPULL | HW_RC663_BIT_GLOBALIRQ| HW_RC663_BIT_LPCDIRQ);
 //CLRCEventFlag.clear();
 
 static uint8_t status = readRegister( HW_RC663_REG_TCONTROL ) ;
 
 status = readRegister( HW_RC663_REG_STATUS ) ;
 // CPLD command 
// writeRegister( HW_RC663_REG_COMMAND ,HW_RC663_CMD_LPCD | HW_RC663_BIT_STANDBY);
  
 //status = readRegister( HW_RC663_REG_COMMAND  ) ;
 
 //ret = CLRCEventFlag.wait(TICKS_PER_SEC*5);
 
 status = readRegister( HW_RC663_REG_STATUS ) ;
 static uint8_t result_i = readRegister( HW_RC663_REG_LPCD_RESULT_I ) ;
 static uint8_t result_q = readRegister( HW_RC663_REG_LPCD_RESULT_Q ) ;
 
// static uint8_t timer3h = readRegister(HW_RC663_REG_T3COUNTERVALHI );
 //static uint8_t timer3l = readRegister(HW_RC663_REG_T3COUNTERVALLO );
 
 //tatic uint8_t timer4h = readRegister(HW_RC663_REG_T4COUNTERVALHI );
 //static uint8_t timer4l = readRegister(HW_RC663_REG_T4COUNTERVALLO );
 
 if( ret )
   {
    uint8_t irq1 = readRegister( HW_RC663_REG_IRQ1 );
    if( irq1 & (1<<5 ) )// CPLD
      {
       //sleep(10);
       return 1; 
      } 
   }
 return 0;  
}

*/

/*
void hrd_CLRC663_CPLD_Poll()
{
}
*/

/*
 * \param tx указатель на передаваемые данные
 * \param tx_bit_num количество передаваемых БИТ!!! 
 * \return количество принятых байт
*/
/*
int hrd_CLRC663_Transceive( const uint8_t *tx, int tx_bit_num, uint8_t *rx_buf, int rx_buf_size )
{
 int result_rx_len = 0; 
 int last_bits = tx_bit_num % 8;
 int txbyte = tx_bit_num/8 + (last_bits?1:0);
 if( last_bits )
   {
    Clrc663.writeRegister( HW_RC663_REG_TXDATANUM, HW_RC663_BIT_DATAEN | last_bits );
   } 
 else  
   {
    Clrc663.writeRegister( HW_RC663_REG_TXDATANUM, HW_RC663_BIT_DATAEN );
   } 
 
 Clrc663.writeFIFO( tx, txbyte );
 
 Clrc663.clearIRQ();
 
 Clrc663.enableRxIrq();  
 
 Clrc663.writeRegister( HW_RC663_REG_COMMAND, HW_RC663_CMD_TRANSCEIVE ); 
 
 if( Clrc663.waitInterrupt( HW_RC663_BIT_RXIRQ, 0 , TICKS_PER_SEC/4) )
   {
    int actual_rx_len = Clrc663.get_FIFO_Length(); // количество байт лежащих в FIFO
    if( actual_rx_len )
      {
       int rx_len; // сколтко байт будет перенесено в в буфер
       if( actual_rx_len <= rx_buf_size )
         {
          rx_len = actual_rx_len;
         }
       else
         {
          rx_len = rx_buf_size;
         }               
       Clrc663.readFIFO( rx_buf, rx_len);
       if( rx_len < actual_rx_len )
         {// если в буфер влезает не все, то чистим остатки в FIFO, байты теряются
          Clrc663.flushFIFO();
         }
       result_rx_len = rx_len;
      } 
   }
 return result_rx_len;   
}*/

//////////////////////////////


#define CLRC663_CS_PORT_NUM LPC_GPIO2       //! номер порта GPIO
#define CLRC663_CS_PIN_NUM  2       //! номер пина в порте GPIO

void TClrc663::init()
{ 
 // P2.9 = PDOWN
 // P2.1 = HI  включение 5 вольт
 LPC_IOCON->PIO3_1  = 0x00;
 LPC_GPIO2->DIR |= (1<<9)|(1<<1); // на выход
 GPIO_SET_VAL( LPC_GPIO2, 9, 0);
 
 GPIO_SET_VAL( LPC_GPIO2, 1, 1);  // переключатель питания
 
 //P0.1 IRQIN
// LPC_IOCON->PIO0_1 = 0x00; 
 //GPIO_SET_VAL( 0, 1, 1);
 //LPC_GPIO0->DIR |= (0x01 << 1);  
  
 // P2.10 вход ORQ
 LPC_GPIO2->IC = (1UL<<10 );    // очистка флага прерывания
 LPC_GPIO2->IS &= ~(1UL<<10);   // по изменению
 LPC_GPIO2->IBE  &= ~(1UL<<10); //
 LPC_GPIO2->IEV  &= ~(1UL<<10); // по спаду
 LPC_GPIO2->IE |= (1UL<<10);    // разрешение прерывания с ноги 
 
 NVIC_EnableIRQ( EINT2_IRQn );    // разрешение прерываний с порта
}

void TClrc663::resetCrypto(  )
{
/* GPIO_SET_VAL( LPC_GPIO2, 9, 0);
 sleep(10);
 GPIO_SET_VAL( LPC_GPIO2, 9, 1);
 sleep(10);
 GPIO_SET_VAL( LPC_GPIO2, 9, 0);
 sleep(20);*/
 writeRegister( HW_RC663_REG_STATUS,0 );    // отключаем Crypto 
}

void TClrc663::writeRegister( uint8_t reg_addr, uint8_t val )
{
 this->writeBytes( reg_addr, 1, &val );
}

uint8_t TClrc663::readError() const
{
 return readRegister( HW_RC663_REG_ERROR );
}

void TClrc663::flushFIFO()
{
 uint8_t fiforeg = readRegister( HW_RC663_REG_FIFOCONTROL );
 fiforeg |= (1<<4);
 
 writeRegister( HW_RC663_REG_FIFOCONTROL, fiforeg );
}

void TClrc663::writeFIFO( const uint8_t *tx, int bytenum )
{
 this->writeBytes( HW_RC663_REG_FIFODATA, bytenum, tx );
}

int TClrc663::readFIFO( uint8_t *rx, int bytenum )
{
 return readFifo( HW_RC663_REG_FIFODATA, bytenum, rx );
}

void TClrc663::fieldOff()
{
 writeRegister( HW_RC663_REG_DRVMODE ,0x86 );
 resetCrypto(  );
}

void TClrc663::fieldOn()
{
 writeRegister( HW_RC663_REG_DRVMODE ,0x8E );
}

bool TClrc663::waitInterrupt( int timeout, uint8_t int_mask0, uint8_t int_mask1 )
{
 bool ret = CLRCEventFlag.wait( timeout );
 if( ret )
   {
    writeRegister( HW_RC663_REG_IRQ0, (~int_mask0) & 0x7F );
    writeRegister( HW_RC663_REG_IRQ1, (~int_mask1) & 0x7F );
   } 
 return ret;
}

void TClrc663::idleCommand() 
{
 writeRegister( HW_RC663_REG_COMMAND ,HW_RC663_CMD_IDLE ); 
}

void TClrc663::enableCRC( bool enable )
{
 uint8_t rx  =0;
 uint8_t tx  =0;

 if( enable )
   {
    tx =(1<<4)|(2<<2)|(1<<0) ;      // crc16 стартовое 0x6363
    rx =(1<<7)|(1<<4)|(2<<2)|(1<<0);// crc16 стартовое 0x6363
   }
 writeRegister( HW_RC663_REG_TXCRCCON, tx );
 writeRegister( HW_RC663_REG_RXCRCCON, rx );
}

// \return 1 в случае успеха, иначе 0 
/*int TClrc663::transmit( const uint8_t *tx, int tx_byte_num )
{
 flushFIFO();
 
 enableCRC( true );
 writeRegister( HW_RC663_REG_TXDATANUM, HW_RC663_BIT_DATAEN );

 writeFIFO( tx, tx_byte_num );

 clearIRQ(); 
 
 writeRegister( HW_RC663_REG_COMMAND, HW_RC663_CMD_TRANSMIT );
 writeRegister( HW_RC663_REG_IRQ0EN, HW_RC663_BIT_IRQINV | HW_RC663_BIT_TXIRQ ); 
 //writeRegister( HW_RC663_REG_IRQ1EN, HW_RC663_BIT_IRQPUSHPULL | HW_RC663_BIT_GLOBALIRQ );   //IRQEn1
 
 if( waitInterrupt( HW_RC663_BIT_TXIRQ, 0 , TICKS_PER_SEC/10) ) // ожидаем прерывания по завершению передачи
   {
    return 1;
   } 
 return 0;  
}
*/

/*
 * \param tx указатель на передаваемые данные
 * \param tx_bit_num количество передаваемых БИТ!!! 
 * \return количество принятых байт
*/
int TClrc663::transceive( bool crc_enable, const uint8_t *tx, int tx_bit_num, uint8_t *rx_buf, int rx_buf_size )
{
 int result_rx_len = 0; 
 int last_bits = tx_bit_num % 8;
 int txbyte = tx_bit_num/8 + (last_bits?1:0);
 
 
// result_rx_len = readRegister( HW_RC663_REG_TXAMP );
 writeRegister( HW_RC663_REG_TXAMP, /*(3<<6)|*/0x15);
 
 idleCommand();
 /* Flush FiFo */
 flushFIFO();
 enableCRC( crc_enable );
 
 if( last_bits )
   {
    writeRegister( HW_RC663_REG_TXDATANUM, HW_RC663_BIT_DATAEN | last_bits );
   } 
 else  
   {
    writeRegister( HW_RC663_REG_TXDATANUM, HW_RC663_BIT_DATAEN );
   } 
 
 writeFIFO( tx, txbyte );
 
 clearIRQ();
 
 enableRxIrq();  
 
 writeRegister( HW_RC663_REG_COMMAND, HW_RC663_CMD_TRANSCEIVE ); 
 
 if( waitInterrupt( HW_RC663_BIT_RXIRQ, 0 , TICKS_PER_SEC/30) )
   {
    int actual_rx_len = getFIFOLength(); // количество байт лежащих в FIFO
    if( actual_rx_len )
      {
       int rx_len; // сколько байт будет перенесено в в буфер
       if( actual_rx_len <= rx_buf_size )
         {
          rx_len = actual_rx_len;
         }
       else
         {
          rx_len = rx_buf_size;
         }               
       readFIFO( rx_buf, rx_len);
       if( rx_len < actual_rx_len )
         {// если в буфер влезает не все, то чистим остатки в FIFO, байты теряются
          flushFIFO();
         }
       result_rx_len = rx_len;
      } 
   }
 return result_rx_len;   
}

int TClrc663::cryptoAuthentificationA( const uint8_t uid[7], uint8_t bl_addr, const uint8_t key[6] ) 
{
 return cryptoAuthentification( 0x60, uid, bl_addr, key );
}

int TClrc663::cryptoAuthentificationB( const uint8_t uid[7], uint8_t bl_addr, const uint8_t key[6] ) 
{
 return cryptoAuthentification( 0x61, uid, bl_addr, key );
}

int TClrc663::cryptoAuthentification( uint8_t cmd, const uint8_t uid[7], uint8_t bl_addr, const uint8_t key[6] ) 
{
 int res = 0;
 static uint8_t status;
 static uint8_t err;
 static uint8_t len;
 
 static uint8_t buffer[16];
 
 idleCommand();
 
 status = readRegister( HW_RC663_REG_STATUS );
 err = readError();
 // загружаем ключ
  /* Flush FiFo */
 flushFIFO();
 writeFIFO( key, 6 );
 //clearIRQ();
 writeRegister( HW_RC663_REG_IRQ0EN, HW_RC663_BIT_IRQINV | HW_RC663_BIT_IDLEIRQ ); 
 writeRegister( HW_RC663_REG_IRQ1EN, HW_RC663_BIT_IRQPUSHPULL | HW_RC663_BIT_GLOBALIRQ );   //IRQEn1
 
 writeRegister( HW_RC663_REG_COMMAND, HW_RC663_CMD_LOADKEY ); 
 
 //OS::sleep(50);
 
 if( waitInterrupt( HW_RC663_BIT_IDLEIRQ, 0 , TICKS_PER_SEC/4) )
   {// запуск аутентификации
    uint8_t tx_buf[7];
    tx_buf[0] = cmd;
    tx_buf[1] = bl_addr;
    tx_buf[2] = uid[0];
    tx_buf[3] = uid[1];
    tx_buf[4] = uid[2];
    tx_buf[5] = uid[3];
    
    flushFIFO();
    writeFIFO( tx_buf, 6 );
    clearIRQ();
    writeRegister( HW_RC663_REG_IRQ0EN, HW_RC663_BIT_IRQINV | HW_RC663_BIT_IDLEIRQ ); 
    writeRegister( HW_RC663_REG_IRQ1EN, HW_RC663_BIT_IRQPUSHPULL | HW_RC663_BIT_GLOBALIRQ );   //IRQEn1
    
    writeRegister( HW_RC663_REG_COMMAND, HW_RC663_CMD_MFAUTHENT ); 
    
    status = readRegister( HW_RC663_REG_STATUS );
    err = readRegister( HW_RC663_REG_ERROR );
    
    if( waitInterrupt( HW_RC663_BIT_IDLEIRQ, 0 , TICKS_PER_SEC/4) )
      {       
      
    //OS::sleep(50);    
    status = readRegister( HW_RC663_REG_STATUS );
    err = readRegister( HW_RC663_REG_ERROR );
    
    len = readRegister(HW_RC663_REG_FIFOLENGTH );
    if(len)
      {
       readFIFO( buffer, len );
      }
    
    if( status & (1<<5 ) )
      {
       res = 1;
      } 
      }
   }
 else
   {  
    //writeRegister( HW_RC663_REG_IRQ0EN, HW_RC663_BIT_IRQINV ); 
    //writeRegister( HW_RC663_REG_IRQ1EN, HW_RC663_BIT_IRQPUSHPULL );   //IRQEn1
    idleCommand();  
    //writeRegister( HW_RC663_REG_STATUS,0 );
   } 
  
 return res;  
}

////////////////////////////////////////////////////////////////////////////////
/*TClrc663Spi::void init( TSpiAbstractBus *spi )
{
 this->spi = spi ;
}*/
void TClrc663Spi::init()
{
 TClrc663::init();
 
 // P2.2 = CS
 LPC_IOCON->PIO2_2  = 0x00;     //SSEL как GPIO
 CLRC663_CS_PORT_NUM->DIR |= (1<<2);
 GPIO_SET_VAL( CLRC663_CS_PORT_NUM, CLRC663_CS_PIN_NUM, 1);    // начальное значение 1
}


bool TClrc663Spi::writeBytes( uint8_t addr, uint8_t size, const uint8_t *tx )
{
 uint8_t buf[1];
 if( spi->lock() )
   {
    setCS( true );
 
    buf[0] = (addr<<1); // адрес в старших 7 битах, а младший бит должен быть 0 - что означает запись
    if( spi->swap( 1, buf, 0 ) ) // сначала передаем стартовый адрес, а затем все осталльные байты
      {
       if( spi->swap( size, tx, 0 ) )
         {
         }
      }      
    setCS( false );

    spi->unlock();
    return true;
   } 
 return false;  
}

// \return 1 в случае успеха, иначе 0 
int TClrc663Spi::readFifo( uint8_t addr, uint8_t size, uint8_t *rx )
{
 int res = 0;
 if( spi->lock() )       // захватываем шину
   {
    setCS( true );
 
    addr = (addr<<1)|1;
 
   // uint8_t *b = rx;
 
    int i; 
    for( i=0; i < size; i++ )
      {
     //  if( i < BUF_SIZE-1 )
       rx[ i ] = addr;
      }
    //b[ i ] =0;  // последний адрес фиктивный
    if( spi->swap( 1, &rx[0], 0 ) ) // первый байт отдельно, т.к. адрес байта, который будет передан следующим
      {
       if( spi->swap( size, &rx[0], rx ) )
         {
          res = 1;
         } 
      }
    setCS( false );
    spi->unlock();     //освобождаем шину
   }
 return res;   
}


void TClrc663Spi::setCS( bool s ) const
{
 if( s )
   GPIO_SET_VAL( CLRC663_CS_PORT_NUM, CLRC663_CS_PIN_NUM, 0);
 else  
   GPIO_SET_VAL( CLRC663_CS_PORT_NUM, CLRC663_CS_PIN_NUM, 1);
}   


// чтение одного байта
// \return 1 в случае успеха, иначе 0 
int TClrc663Spi::readByte( uint8_t addr, uint8_t *rx ) const
{
 int res = 0;
 if( spi->lock() )       // захватываем шину
   {
    setCS( true );
 
    uint8_t buffer[2];
    addr = (addr<<1)|1;
 
    buffer[0] = addr;
    buffer[1] = 0;
 
    if( spi->swap( 2, buffer, buffer ) ) // первый байт отдельно, т.к. адрес байта, который будет передан следующим
      {
       *rx = buffer[1];
       res =1;
      }
 
    setCS( false );  
    spi->unlock();     //освобождаем шину
   } 
 return res;
}
