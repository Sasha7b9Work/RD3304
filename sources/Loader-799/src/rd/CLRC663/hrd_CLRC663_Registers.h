#ifndef HRD_CLRC663_REGISTERS_H
#define HRD_CLRC663_REGISTERS_H

/** \brief Command Register.
*
* Used for starting / stopping commands and for sending the IC into standby mode.
*/
#define HW_RC663_REG_COMMAND              0x00U

/** \brief Host-Control Register.
*
* Configure Host and SAM interfaces.
*/
#define HW_RC663_REG_HOSTCTRL             0x01U

/** \name Fifo Registers
*/
/*@{*/
/** \brief FIFO-Control Register.
*
* Set FIFO size and retrieve FIFO parameters.\n
* \b Note: Also contains 1 additional Water-Level bit (MSB) and 2 additional FIFO-Length bits (also MSB).
*/
#define HW_RC663_REG_FIFOCONTROL          0x02U

/** \brief WaterLevel Register.
*
* FIFO WaterLevel configuration.
*/
#define HW_RC663_REG_WATERLEVEL           0x03U

/** \brief FIFO-Length Register.
*
* Retrieve the number of bytes within the FIFO.
*/
#define HW_RC663_REG_FIFOLENGTH           0x04U

/** \brief FIFO-Data Register.
*
* Writing to this register moves a byte into the FIFO
* while incrementing the FIFO length and raising the internal WaterLevel.
*/
#define HW_RC663_REG_FIFODATA             0x05U
/*@}*/

/** \name IRQ Registers
*/
/*@{*/
/** \brief IRQ0 Register.
*
* Read or modify the first 7 IRQ bits.
*/
#define HW_RC663_REG_IRQ0                 0x06U

/** \brief IRQ1 Register.
*
* Read or modify the second 7 IRQ bits.
*/
#define HW_RC663_REG_IRQ1                 0x07U

/** \brief IRQ0EN Register.
*
* Enable or disable the first IRQ bits or invert the IRQ propagation.
*/
#define HW_RC663_REG_IRQ0EN               0x08U

/** \brief IRQ1EN Register.
*
* Enable or disable the second IRQ bits or enable/disable PushPull mode.
*/
#define HW_RC663_REG_IRQ1EN               0x09U
/*@}*/

/** \brief Error Register.
*
* Contains bits for the occured erros.
*/
#define HW_RC663_REG_ERROR                0x0AU

/** \brief Status Register.
*
* Contains the Crypto1 state and information about the ComState.
*/
#define HW_RC663_REG_STATUS               0x0BU

/** \brief Rx-Bit-Control Register.
*
* Set/Get Bit-granularity and collision information.
*/
#define HW_RC663_REG_RXBITCTRL            0x0CU

/** \brief Rx-Coll Register.
*
* Contains information about the collision position after a collision.
*/
#define HW_RC663_REG_RXCOLL               0x0DU

/** \name Timer Registers
*/
/*@{*/
/** \brief Timer Control Register.
*
* Provides timer control and status information for all timers.
*/
#define HW_RC663_REG_TCONTROL             0x0EU

/** \brief Timer0 Control Register.
*
* Configure the timer.
*/
#define HW_RC663_REG_T0CONTROL            0x0FU

/** \brief Timer0 Reload(High) Register.
*
* Set the most significant byte of the Reload-Value.
*/
#define HW_RC663_REG_T0RELOADHI           0x10U

/** \brief Timer0 Reload(Low) Register.
*
* Set the least significant byte of the Reload-Value.
*/
#define HW_RC663_REG_T0RELOADLO           0x11U

/** \brief Timer0 Counter(High) Register.
*
* Get the most significant byte of the Counter-Value.
*/
#define HW_RC663_REG_T0COUNTERVALHI       0x12U

/** \brief Timer0 Counter(Low) Register.
*
* Get the least significant byte of the Counter-Value.
*/
#define HW_RC663_REG_T0COUNTERVALLO       0x13U

/** \brief Timer1 Control Register.
*
* Configure the timer.
*/
#define HW_RC663_REG_T1CONTROL            0x14U

/** \brief Timer1 Reload(High) Register.
*
* Set the most significant byte of the Reload-Value.
*/
#define HW_RC663_REG_T1RELOADHI           0x15U

/** \brief Timer1 Reload(Low) Register.
*
* Set the least significant byte of the Reload-Value.
*/
#define HW_RC663_REG_T1RELOADLO           0x16U

/** \brief Timer1 Counter(High) Register.
*
* Get the most significant byte of the Counter-Value.
*/
#define HW_RC663_REG_T1COUNTERVALHI       0x17U

/** \brief Timer1 Counter(Low) Register.
*
* Get the least significant byte of the Counter-Value.
*/
#define HW_RC663_REG_T1COUNTERVALLO       0x18U

/** \brief Timer2 Control Register.
*
* Configure the timer.
*/
#define HW_RC663_REG_T2CONTROL            0x19U

/** \brief Timer2 Reload(High) Register.
*
* Set the most significant byte of the Reload-Value.
*/
#define HW_RC663_REG_T2RELOADHI           0x1AU

/** \brief Timer2 Reload(Low) Register.
*
* Set the least significant byte of the Reload-Value.
*/
#define HW_RC663_REG_T2RELOADLO           0x1BU

/** \brief Timer2 Counter(High) Register.
*
* Get the most significant byte of the Counter-Value.
*/
#define HW_RC663_REG_T2COUNTERVALHI       0x1CU

/** \brief Timer2 Counter(Low) Register.
*
* Get the least significant byte of the Counter-Value.
*/
#define HW_RC663_REG_T2COUNTERVALLO       0x1DU

/** \brief Timer3 Control Register.
*
* Configure the timer.
*/
#define HW_RC663_REG_T3CONTROL            0x1EU

/** \brief Timer3 Reload(High) Register.
*
* Set the most significant byte of the Reload-Value.
*/
#define HW_RC663_REG_T3RELOADHI           0x1FU

/** \brief Timer3 Reload(Low) Register.
*
* Set the least significant byte of the Reload-Value.
*/
#define HW_RC663_REG_T3RELOADLO           0x20U

/** \brief Timer3 Counter(High) Register.
*
* Get the most significant byte of the Counter-Value.
*/
#define HW_RC663_REG_T3COUNTERVALHI       0x21U

/** \brief Timer3 Counter(Low) Register.
*
* Get the least significant byte of the Counter-Value.
*/
#define HW_RC663_REG_T3COUNTERVALLO       0x22U

/** \brief Timer4 Control Register.
*
* Configure the timer.
*/
#define HW_RC663_REG_T4CONTROL            0x23U

/** \brief Timer4 Reload(High) Register.
*
* Set the most significant byte of the Reload-Value.
*/
#define HW_RC663_REG_T4RELOADHI           0x24U

/** \brief Timer4 Reload(Low) Register.
*
* Set the least significant byte of the Reload-Value.
*/
#define HW_RC663_REG_T4RELOADLO           0x25U

/** \brief Timer4 Counter(High) Register.
*
* Get the most significant byte of the Counter-Value.
*/
#define HW_RC663_REG_T4COUNTERVALHI       0x26U

/** \brief Timer4 Counter(Low) Register.
*
* Get the least significant byte of the Counter-Value.
*/
#define HW_RC663_REG_T4COUNTERVALLO       0x27U
/*@}*/

/** \brief Driver Mode Register.
*
* Enable / Invert the Tx-Driver and set the Clock Mode.
*/
#define HW_RC663_REG_DRVMODE              0x28U

/** \brief Tx Amplifier Register.
*
* Modify Amplitude and Carrier settings.
*/
#define HW_RC663_REG_TXAMP                0x29U

/** \brief Driver Control Register.
*
* Select / Invert drivers.
*/
#define HW_RC663_REG_DRVCON               0x2AU

/** \brief TxI Register.
*
* Contains Overshoot prevention and current control settings.
*/
#define HW_RC663_REG_TXI                  0x2BU

/** \brief Tx-CRC Control Register.
*
* Configure CRC parameters transmission.
*/
#define HW_RC663_REG_TXCRCCON             0x2CU

/** \brief Rx-CRC Control Register.
*
* Configure CRC parameters for reception.
*/
#define HW_RC663_REG_RXCRCCON             0x2DU

/** \brief Tx-DataNum Register.
*
* Set TxLastBits and configure KeepBitGrid functionality.
*/
#define HW_RC663_REG_TXDATANUM            0x2EU

/** \brief Tx-Modwidth Register.
*
* Set the modulation width.
*/
#define HW_RC663_REG_TXMODWIDTH           0x2FU

/** \brief Symbol 0 and 1 Register.
*
* Configure Burst-lengths of both symbols.
*/
#define HW_RC663_REG_TXSYM10BURSTLEN      0x30U

/** \brief Tx-Wait Control Register.
*
* Enable / Configure Tx Waiting-Time.
*/
#define HW_RC663_REG_TXWAITCTRL           0x31U

/** \brief TxWaitLo Register.
*
* Contains the Least-Significant-Bits for the Tx Waiting-Time.
*/
#define HW_RC663_REG_TXWAITLO             0x32U

/** \brief Frame control register.
*
* Contains active Start/Stop symbol and Parity settings.
*/
#define HW_RC663_REG_FRAMECON             0x33U

/** \brief RxSOFD Register.
*
* Contains Start-of-Frame and subcarrier detection bits.
*/
#define HW_RC663_REG_RXSOFD               0x34U

/** \brief Rx Control Register.
*
* Configure Receiver settings such as baudrate and EMD-suppression feature.
*/
#define HW_RC663_REG_RXCTRL               0x35U

/** \brief Rx-Wait Register.
*
* Configure Receiver Deaf-Time.
*/
#define HW_RC663_REG_RXWAIT               0x36U

/** \brief Rx-Threshold Register.
*
* Configure Receiver Threshold.
*/
#define HW_RC663_REG_RXTHRESHOLD          0x37U

/** \brief Receiver Register.
*
* Configure Collision-Level and other features.
*/
#define HW_RC663_REG_RCV                  0x38U

/** \brief Rx-Analog Register.
*
* Configure analog settings and parameters for Receiver circuitry.
*/
#define HW_RC663_REG_RXANA                0x39U

/** \brief Lpcd Options Register.
 *
 * Configure Lpcd Options.
 */
#define HW_RC663_REG_LPCD_OPTIONS         0x3AU

/** \brief Serial Speed Register.
*
* Configure serial baudrates.
*/
#define HW_RC663_REG_SERIALSPEED          0x3BU

/** \brief LPO_TRIMM Register.
*
* Trimm Control Input for Low Power Oscillator.
*/
#define HW_RC663_REG_LPO_TRIMM            0x3CU

/** \brief PLL Control Register.
*
* Configure PLL settings.
*/
#define HW_RC663_REG_PLL_CTRL             0x3DU

/** \brief PLL DivO Register.
*
* Contains PLL output.
*/
#define HW_RC663_REG_PLL_DIV              0x3EU

/** \brief LPCD QMin Register.
*
* Configure IMax(2) and QMin values for LPCD.
*/
#define HW_RC663_REG_LPCD_QMIN            0x3FU

/** \brief LPCD QMax Register.
*
* Configure IMax(1) and QMax values for LPCD.
*/
#define HW_RC663_REG_LPCD_QMAX            0x40U

/** \brief LPCD IMin Register.
*
* Configure IMax(0) and IMin values for LPCD.
*/
#define HW_RC663_REG_LPCD_IMIN            0x41U

/** \brief LPCD Result(I) Register.
*
* Contains I-Channel results of LPCD.
*/
#define HW_RC663_REG_LPCD_RESULT_I        0x42U

/** \brief LPCD Result(Q) Register.
*
* Contains Q-Channel results of LPCD.
*/
#define HW_RC663_REG_LPCD_RESULT_Q        0x43U

/** \brief Transmitter bit modulus Register.
*
* Define Tx data interpretation.
*/
#define HW_RC663_TXBITMOD                 0x48U

/** \brief Transmitter data configuration register.
*
* Contains Type of data encoding, subcarrier frequency of envelope and frequency of bitstream.
*/
#define HW_RC663_REG_TXDATACON            0x4AU

/** \brief Transmitter data modulation Register.
*
* Configure Frame step, miller coding, Pulse type, Inversion of envelop
* of data and Envelop type
*/
#define HW_RC663_REG_TXDATAMOD            0x4BU

/** \brief Transmitter Symbol Frequency.
*
* Configure Frequency for Transmitter symbol
*/
#define HW_RC663_REG_TXSYMFREQ            0x4CU

/** \brief Transmitter Symbol 0 High Register.
*
* Contains value for Transmitter Symbol 0.
*/
#define HW_RC663_REG_TXSYM0H              0x4DU

/** \brief Transmitter Symbol 0 Low Register.
*
* Contains value for Transmitter Symbol 0.
*/
#define HW_RC663_REG_TXSYM0L              0x4EU

/** \brief Transmitter Symbol 1 High Register.
*
* Contains value for Transmitter Symbol 1.
*/
#define HW_RC663_REG_TXSYM1H              0x4FU

/** \brief Transmitter Symbol 1 Low Register.
*
* Contains value for Transmitter Symbol 1.
*/
#define HW_RC663_REG_TXSYM1L              0x50U

/** \brief Tx Symbol 2 Register.
*
* Contains Symbol definition for Symbol 2.
*/
#define HW_RC663_REG_TXSYM2               0x51U

/** \brief Tx Symbol 3 Register.
*
* Contains Symbol definition for Symbol 3.
*/
#define HW_RC663_REG_TXSYM3               0x52U

/** \brief Transmitter Symbol 0 + Symbol 1 Length Register.
*
* Contains Length of Transmitter Symbol 0 + Symbol 1.
*/
#define HW_RC663_REG_TXSYM10LEN           0x53U

/** \brief Transmitter symbol 3 + symbol 2 length register.
*
* Contains No. of Valid bits in Tx Symbol 2 and Tx Symbol 3 registers.
*/
#define HW_RC663_REG_TXSYM32LEN           0x54U

/** \brief Symbol 0 and 1 Register.
*
* Configure Burst-control of both symbols.
*/
#define HW_RC663_REG_TXSYM10BURSTCTRL     0x55U

/** \brief Symbol 0 and 1 Register.
*
* Configure Modulation Register of both symbols.
*/
#define HW_RC663_REG_TXSYM10MOD           0x56U

/** \brief Transmitter symbol 3 + symbol 2 modulation register.
*
* Configures Miller encoding, Pulse type and Envelope type.
*/
#define HW_RC663_REG_TXSYM32MOD           0x57U

/** \brief Receiver bit modulation register.
*
* Contains Receiver data configurations.
*/
#define HW_RC663_REG_RXBITMOD             0x58U

/** \brief Receiver end of frame symbol register.
*
* This Register defines the pattern of the EOF symbol.
*/
#define HW_RC663_REG_RXEOFSYM             0x59U

/** \brief Receiver synchronisation value high register.
*
* Defines the high byte of the Start Of Frame (SOF) pattern, which must
* be in front of the receiving data.
*/
#define HW_RC663_REG_RXSYNCVAlH           0x5AU

/** \brief Receiver synchronisation value low register.
*
* Defines the low byte of the Start Of Frame (SOF) Pattern, which must
* be in front of the receiving data.
*/
#define HW_RC663_REG_RXSYNCVAlL           0x5BU

/** \brief Receiver synchronisation mode register.
*
* Contains Rx sync values.
*/
#define HW_RC663_REG_RXSYNCMOD            0x5CU

/** \brief Receiver modulation register.
*
* Contains Reciever configuration registers.
*/
#define HW_RC663_REG_RXMOD                0x5DU

/** \brief Receiver Correlation Register.
*
* Contains Correlation Frequency, Correlation Speed and Correlation Length.
*/
#define HW_RC663_REG_RXCORR               0x5EU

/** \brief Calibration register of the receiver.
*
* Contains Fabrication calibration of the receiver.
*/
#define HW_RC663_REG_RXSVETTE             0x5FU

/** \brief DAC value Register.
*
* Contains DAC Value.
*/
#define HW_RC663_REG_DACVAL               0x64U

/** \brief Test mode Register.
*
* Contains Test Level enable and Test mode configuration.
*/
#define HW_RC663_REG_TESTMOD              0x66U

/** \brief Analog Xtal register.
*
* Contains Xtal mode and Xtal bypassreg.
*/
#define HW_RC663_REG_ANAXTAL              0x71U

/** \brief EPCv2 mode Register.
*
* Enables or Disables Testmode Rx and EPCv2 mode.
*/
#define HW_RC663_REG_SIGPROTEST           0x72U

/** \brief Version Register.
*
* Contains IC Version and Subversion.
*/
#define HW_RC663_REG_VERSION              0x7FU

/** \name Command Register Contents (0x00)
*/
/*@{*/
#define HW_RC663_BIT_STANDBY              0x80U   /**< Standby bit; If set, the IC transits to standby mode. */
#define HW_RC663_CMD_IDLE                 0x00U   /**< No action; cancels current command execution. */
#define HW_RC663_CMD_LPCD                 0x01U   /**< Low Power Card Detection. */
#define HW_RC663_CMD_LOADKEY              0x02U   /**< Reads a key from the FIFO buffer and puts it into the key buffer. */
#define HW_RC663_CMD_MFAUTHENT            0x03U   /**< Performs the Mifare standard authentication (in Mifare Reader/Writer mode only). */
#define HW_RC663_CMD_ACKREQ               0x04U   /**< Performs a Query, a Ack and a Req-Rn for EPC V2. */
#define HW_RC663_CMD_RECEIVE              0x05U   /**< Activates the receiver circuitry. */
#define HW_RC663_CMD_TRANSMIT             0x06U   /**< Transmits data from the FIFO buffer to Card. */
#define HW_RC663_CMD_TRANSCEIVE           0x07U   /**< Like #HW_RC663_CMD_TRANSMIT but automatically activates the receiver after transmission is finished. */
#define HW_RC663_CMD_WRITEE2              0x08U   /**< Gets one byte from FIFO buffer and writes it to the internal EEPROM. */
#define HW_RC663_CMD_WRITEE2PAGE          0x09U   /**< Gets up to 64 Bytes from FIFO buffer and writes it to the EEPROM. */
#define HW_RC663_CMD_READE2               0x0AU   /**< Reads data from EEPROM and puts it into the FIFO buffer. */
#define HW_RC663_CMD_LOADREG              0x0CU   /**< Reads data from EEPROM and initializes the registers. */
#define HW_RC663_CMD_LOADPROTOCOL         0x0DU   /**< Reads data from EEPROM and initializes the registers needed for a protocol change. */
#define HW_RC663_CMD_LOADKEYE2            0x0EU   /**< Copies a Mifare key from the EEPROM into the key buffer. */
#define HW_RC663_CMD_STOREKEYE2           0x0FU   /**< Stores a Mifare key into the EEPROM. */
#define HW_RC663_CMD_SOFTRESET            0x1FU   /**< Resets the IC. */
#define HW_RC663_MASK_COMMAND             0x1FU   /**< Mask for Command-bits. */


/** \name IRQ0 Register(s) Contents (0x06/0x08)
*/
/*@{*/
#define HW_RC663_BIT_SET                  0x80U
#define HW_RC663_BIT_IRQINV               0x80U
#define HW_RC663_BIT_HIALERTIRQ           0x40U
#define HW_RC663_BIT_LOALERTIRQ           0x20U
#define HW_RC663_BIT_IDLEIRQ              0x10U
#define HW_RC663_BIT_TXIRQ                0x08U
#define HW_RC663_BIT_RXIRQ                0x04U
#define HW_RC663_BIT_ERRIRQ               0x02U
#define HW_RC663_BIT_EMDIRQ               0x01U
/*@}*/

/** \name IRQ1 Register(s) Contents (0x07/0x09)
*/
/*@{*/
/* #define HW_RC663_BIT_SET                  0x80U */
#define HW_RC663_BIT_IRQPUSHPULL          0x80U
#define HW_RC663_BIT_GLOBALIRQ            0x40U
#define HW_RC663_BIT_IRQPINEN             0x40U
#define HW_RC663_BIT_LPCDIRQ              0x20U
#define HW_RC663_BIT_TIMER4IRQ            0x10U
#define HW_RC663_BIT_TIMER3IRQ            0x08U
#define HW_RC663_BIT_TIMER2IRQ            0x04U
#define HW_RC663_BIT_TIMER1IRQ            0x02U
#define HW_RC663_BIT_TIMER0IRQ            0x01U
/*@}*/

/** \name Rx-Bit-Control Register Contents (0x0C)
*/
/*@{*/
#define HW_RC663_BIT_VALUESAFTERCOLL      0x80U
#define HW_RC663_BIT_NOCOLL               0x08U
#define HW_RC663_MASK_RXALIGN             0x70U
#define HW_RC663_MASK_RXLASTBITS          0x07U
/*@}*/


/** \name T[0-3]-Control Register Contents (0x0F/0x14/0x19/0x1E)
*/
/*@{*/
#define HW_RC663_BIT_TSTOP_RX             0x80U   /**< Stop timer on receive interrupt. */
#define HW_RC663_BIT_TAUTORESTARTED       0x08U   /**< Auto-restart timer after underflow. */
#define HW_RC663_BIT_TSTART_TX            0x10U   /**< Start timer on transmit interrupt. */
#define HW_RC663_BIT_TSTART_LFO           0x20U   /**< Use this timer for LFO trimming. */
#define HW_RC663_BIT_TSTART_LFO_UV        0x30U   /**< Use this timer for LFO trimming (generate UV at a trimming event). */
#define HW_RC663_MASK_TSTART              0x30U   /**< Mask for TSTART bits. */
#define HW_RC663_VALUE_TCLK_1356_MHZ      0x00U   /**< Use 13.56MHz as input clock. */
#define HW_RC663_VALUE_TCLK_212_KHZ       0x01U   /**< Use 212KHz as input clock. */
#define HW_RC663_VALUE_TCLK_T0            0x02U   /**< Use timer0 as input clock. */
#define HW_RC663_VALUE_TCLK_T1            0x03U   /**< Use timer1 as input clock. */
/*@}*/

/** \name Tx-DataNum Register Contents (0x2E)
*/
/*@{*/
#define HW_RC663_BIT_KEEPBITGRID          0x10U
#define HW_RC663_BIT_DATAEN               0x08U
#define HW_RC663_MASK_TXLASTBITS          0x07U
#define HW_RC663_MASK_SYMBOL_SEND         0x08U
/*@}*/

/** \name T4-Control Register Contents (0x23)
*/
/*@{*/
#define HW_RC663_BIT_T4RUNNING            0x80U
#define HW_RC663_BIT_T4STARTSTOPNOW       0x40U
#define HW_RC663_BIT_T4AUTOTRIMM          0x20U
#define HW_RC663_BIT_T4AUTOLPCD           0x10U
#define HW_RC663_BIT_T4AUTORESTARTED      0x08U
#define HW_RC663_BIT_T4AUTOWAKEUP         0x04U
/*#define PHHAL_HW_RC663_MASK_TSTART              0x30U*/
#define HW_RC663_VALUE_TCLK_LFO_16_KHZ    0x00U
#define HW_RC663_VALUE_TCLK_LFO_2_KHZ     0x01U
#define HW_RC663_VALUE_TCLK_LFO_1_KHZ     0x02U
#define HW_RC663_VALUE_TCLK_LFO_500_HZ    0x03U

/** \name Rx-Receiver Register Contents (0x38)
*/
/*@{*/
#define HW_RC663_BIT_RX_SINGLE            0x80U
#define HW_RC663_BIT_RX_SHORT_MIX2ADC     0x40U
#define HW_RC663_BIT_USE_SMALL_EVAL       0x04U
#define HW_RC663_MASK_RX_SIGPRO_IN_SEL    0x30U
#define HW_RC663_MASK_COLLLEVEL           0x03U
/*@}*/

/*@{*/
#define HW_RC663_BIT_LPCDIRQ_CLR          0x40U
/*@}*/


/** \name Macros used in LPCD.
*/
/*@{*/
#define HW_RC663_LPCD_RECEIVER_GAIN               0x03U    /**< Receiver Gain when LPCD CMD is executed. */
#define HW_RC663_FILTER_MASK                      0x00FFU  /**< Mask to get if Filter is Enable (#PH_ON) or disable (#PH_OFF). */
#define HW_RC663_FILTER_OPTION_MASK               0xFF00U  /**< Mask to get Filter option. */
#define HW_RC663_FILTER_BIT_OPTION1               0x10U    /**< Bit 4 of dataparams 'bLpcdOption' is used to store Filter option1. */
#define HW_RC663_FILTER_BIT_OPTION2               0x20U    /**< Bit 5 of dataparams 'bLpcdOption' is used to store Filter option2. */
/*@}*/



#endif
