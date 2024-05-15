// 2022/6/14 22:08:35 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Modules/LIS2DH12/LIS2DH12.h"
#include "Modules/CLRC66303HN/CLRC66303HN.h"
#include "Hardware/Timer.h"
#include "Hardware/Power.h"
#include "Utils/Buffer.h"
#include "Utils/Mutex.h"
#include "Utils/StringUtils.h"
#include "Nodes/Communicator.h"
#include "Reader/Reader.h"
#include "Utils/RingBuffer.h"
#include "Utils/Math.h"
#include "Nodes/OSDP/OSDP.h"
#include <system.h>
#include <cstring>
#include <cctype>
#include <cstdio>
#include <cstdarg>
#include <cstdlib>


using namespace std;
using namespace SU;


// PA2     ------> USART2_TX / Сюда подаётся последовательность в режиме WG26
// PA3     ------> USART2_RX


namespace HAL_USART
{
    namespace Data
    {
        static bool filtering = true;

        static RingBufferU8 recv_buffer;

        void Append(uint8 byte)
        {
            if (filtering)
            {
                if (byte < 32 || byte > 127)
                {
                    if (byte != '\r')
                    {
                        return;
                    }
                }

                byte = (uint8)toupper((char)byte);
            }

            recv_buffer.Append(byte);
        }

        static bool Get(BufferUSART &buffer)
        {
            return recv_buffer.GetData(buffer);
        }
    }


    void EnableFiltering(bool enable)
    {
        Data::filtering = enable;
    }

    namespace Mode
    {
        // Включить режим передачи
        void Transmit();

        // Включить режим приёма
        void Receive();

        // Здесь в режиме WG мы вибираем режим бита либо режим между битами
        namespace WG
        {
            // Уровень, соотвествующий передаче бита
            void LevelBit();

            // Уровень, соотвествующий промежутку между битами
            void LevelInterval();
        }
    }

    namespace UART
    {
#ifdef MCU_GD
#else
        static uint8 buffer = 0;                    // Буфер для передачи данных через UART
#endif

        void Init();
    }

    namespace WG26
    {
        /*
        *  Старшие байты первые.
        *  Старшие биты первые.
        *  Длительность бита 100 мкс.
        *  Расстояние между битами 1 мс.
        *  1-й контрольный бит:
        *       если сложение значений первых 12 бит является нечетным числом, контрольному биту присваивается значение 1, чтобы результат сложения 13 бит был четным.
        *  2-й контрольный бит:
        *       последние 13 бит всегда дают в сумме нечетное число.
        */

        void Init();

        void Transmit(const UID &);

        void Transmit(uint8 *buffer, int size);

        void Transmit(uint8 buffer[3]);

        // Передать 26 бит, начиная со старшего
        void Transmit26bit(uint);

        // Передать один бит. По meter отмеряют время до старта передачи (1мс)
        void TransmitBit(bool, TimeMeterMS &meter);

        // Возвращает количество единиц в value от bit_start до bit_end
        int NumOnes(uint8 value, int bit_start, int bit_end);
    }

#ifdef MCU_GD
#else
    static UART_HandleTypeDef handleUART;

    void *handle = (void *)&handleUART;
#endif
}


void HAL_USART::Init()
{
    UART::Init();
}


void HAL_USART::UART::Transmit(const void *data, int size)
{
    if (!ModeReader::IsWG() && !::OSDP::IsEnabled() && !ModeOffline::IsEnabled())
    {
        Mode::Transmit();

#ifdef MCU_GD
        const uint8 *message = (const uint8 *)data;
        for (int i = 0; i < size; i++)
        {
            while ((USART_REG_VAL(USART1, USART_FLAG_TBE) & BIT(USART_BIT_POS(USART_FLAG_TBE))) == 0) {}

            USART_TDATA(USART1) = (USART_TDATA_TDATA & ((uint)*message++));
        }

        while ((USART_REG_VAL(USART1, USART_FLAG_TC) & BIT(USART_BIT_POS(USART_FLAG_TC))) == 0) {}

#else
        HAL_UART_Transmit(&handleUART, (uint8 *)data, (uint16)size, 1000);
#endif

        Mode::Receive();
    }
}


void HAL_USART::OSDP::Transmit(const void *data, int size)
{
    if (!ModeReader::IsWG() && ::OSDP::IsEnabled())
    {
        Mode::Transmit();

#ifdef MCU_GD
        const uint8 *message = (const uint8 *)data;
        for (int i = 0; i < size; i++)
        {
            while ((USART_REG_VAL(USART1, USART_FLAG_TBE) & BIT(USART_BIT_POS(USART_FLAG_TBE))) == 0) {}

            USART_TDATA(USART1) = (USART_TDATA_TDATA & ((uint)*message++));
        }

        while ((USART_REG_VAL(USART1, USART_FLAG_TC) & BIT(USART_BIT_POS(USART_FLAG_TC))) == 0) {}

#else
        HAL_UART_Transmit(&handleUART, (uint8 *)data, (uint16)size, 1000);
#endif

        Mode::Receive();
    }
}


void HAL_USART::OSDP::TransmitByte(uint8 byte)
{
    Transmit(&byte, 1);
}


void HAL_USART::UART::TransmitF(pchar format, ...)
{
    char message[256];
    va_list args;
    va_start(args, format);
    vsprintf(message, format, args);
    va_end(args);

    char data[300];

    std::sprintf(data, "%s\r\n", message);

    Transmit(data, (int)std::strlen(data));
}


void HAL_USART::WG26::Transmit(uint8 b0, uint8 b1, uint8 b2)
{
    uint8 buffer[3] = { b0, b1, b2 };

    WG26::Init();
    Transmit(buffer);
    UART::Init();
}


void HAL_USART::WG26::Transmit(uint8 *buffer, int size)
{
    uint8 bytes[3];

    while (size > 0)
    {
        bytes[0] = buffer[0];
        bytes[1] = size > 1 ? buffer[1] : 0U;
        bytes[2] = size > 2 ? buffer[2] : 0U;

        Transmit(bytes);

        size -= 3;
        buffer += 3;
    }
}


void HAL_USART::WG26::Transmit(uint8 bytes[3])
{
    // Вычисляем бит чётности. Он должен быть таким, чтобы количество единиц в первых 13 битах было чётным

    int num_ones = NumOnes(bytes[0], 0, 7) + NumOnes(bytes[1], 4, 7);            // Количество единиц

    int bit_parity_start = (num_ones % 2) ? 1 : 0;

    // Вычисляем бит нечётности. Он должен быть таким, чтобы количество единиц в последних 13 битах было нечётным

    num_ones = NumOnes(bytes[1], 0, 3) + NumOnes(bytes[2], 0, 7);

    int bit_parity_end = (num_ones % 2) ? 0 : 1;

    uint value = (uint)(bit_parity_start << 31);        // Бит чётности первых 13 передаваемых бит
    value |= (uint)(bytes[0] << 23);                          // /
    value |= (uint)(bytes[1] << 15);                          // | Три байта от старшего к младшему
    value |= (uint)(bytes[2] << 7);                           // /
    value |= (uint)(bit_parity_end << 6);                     // Бит нечётности последних 13 передаваемых бит 

    Transmit26bit(value);
}


void HAL_USART::WG26::Transmit(const UID &uid)
{
    uint8 bytes[3] = { uid.bytes[2], uid.bytes[1], uid.bytes[0] };

    Transmit(bytes);
}


void HAL_USART::WG26::Transmit26bit(uint value)
{
    Mode::WG::LevelInterval();

    TimeMeterMS meter;

    for (int i = 31; i >= 6; i--)
    {
        TransmitBit((value & (1 << i)) == 0, meter);
    }

    Mode::WG::LevelInterval();

    meter.WaitFor(1);
}


void HAL_USART::WG26::TransmitBit(bool bit, TimeMeterMS &meter)
{
    meter.WaitFor(1);

    meter.Reset();

    TimeMeterUS meterDuration;              // Для отмерения длительности импульса

#ifdef MCU_GD

//    bit ? gpio_bit_set(GPIOA, GPIO_PIN_2) : gpio_bit_reset(GPIOA, GPIO_PIN_2);
    bit ? (GPIO_BOP(GPIOA) = GPIO_PIN_2) : (GPIO_BC(GPIOA) = GPIO_PIN_2);

#else

    pinTXD2.Set(bit);

#endif

    Mode::WG::LevelBit();

    meterDuration.WaitFor(200);

    Mode::WG::LevelInterval();
}


int HAL_USART::WG26::NumOnes(uint8 value, int bit_start, int bit_end)
{
    int result = 0;

    for (int i = bit_start; i <= bit_end; i++)
    {
        if (value & (1 << i))
        {
            result++;
        }
    }

    return result;
}


void HAL_USART::WG26::Init()
{
#ifdef MCU_GD

    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

#else
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2);

    uint ctrl = GPIOA->CRL;

    _CLEAR_BIT(ctrl, 10);   // \ CNF2 = 00  Push-Pull
    _CLEAR_BIT(ctrl, 11);   // /

    _SET_BIT(ctrl, 8);      // \ MODE2 = 11 max speed 50 MHz
    _SET_BIT(ctrl, 9);      // /

    GPIOA->CRL = ctrl;

    uint odr = GPIOA->ODR;
    _SET_BIT(odr, 2);
    GPIOA->ODR = odr;
#endif
}


void HAL_USART::UART::Init()
{
    pinTXD1.Init();

#ifdef MCU_GD

    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_2);      // TX
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_3);      // RX

    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_2);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_3);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_3);

//    usart_deinit(USART1);
    rcu_periph_reset_enable(RCU_USART1RST);
    rcu_periph_reset_disable(RCU_USART1RST);

//    usart_baudrate_set(USART1, ::OSDP::IsEnabled() ? gset.BaudRateOSDP().ToRAW() : 115200U);
    uint baudval = ::OSDP::IsEnabled() ? gset.BaudRateOSDP().ToRAW() : 115200U;
    uint uclk = rcu_clock_freq_get(CK_APB1);
    /* oversampling by 16, configure the value of USART_BAUD */
    uint udiv = (uclk + baudval / 2U) / baudval;
    uint intdiv = udiv & 0x0000fff0U;
    uint fradiv = udiv & 0x0000000fU;
    USART_BAUD(USART1) = ((USART_BAUD_FRADIV | USART_BAUD_INTDIV) & (intdiv | fradiv));

//    usart_parity_config(USART1, USART_PM_NONE);
    /* disable USART */
    USART_CTL0(USART1) &= ~(USART_CTL0_UEN);
    /* clear USART_CTL0 PM,PCEN bits */
    USART_CTL0(USART1) &= ~(USART_CTL0_PM | USART_CTL0_PCEN);
    /* configure USART parity mode */
    USART_CTL0(USART1) |= USART_PM_NONE;

//    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
    USART_CTL0(USART1) &= ~USART_CTL0_TEN;
    /* configure transfer mode */
    USART_CTL0(USART1) |= USART_TRANSMIT_ENABLE;

//    usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    USART_CTL0(USART1) &= ~USART_CTL0_REN;
    /* configure receiver mode */
    USART_CTL0(USART1) |= USART_RECEIVE_ENABLE;

//    usart_word_length_set(USART1, USART_WL_8BIT);
    /* disable USART */
    USART_CTL0(USART1) &= ~(USART_CTL0_UEN);
    /* clear USART_CTL0 WL bit */
    USART_CTL0(USART1) &= ~USART_CTL0_WL;
    /* configure USART word length */
    USART_CTL0(USART1) |= USART_WL_8BIT;

//    usart_stop_bit_set(USART1, USART_STB_1BIT);
    /* disable USART */
    USART_CTL0(USART1) &= ~(USART_CTL0_UEN);
    /* clear USART_CTL1 STB bits */
    USART_CTL1(USART1) &= ~USART_CTL1_STB;
    USART_CTL1(USART1) |= USART_STB_1BIT;

//    usart_interrupt_enable(USART1, USART_INT_RBNE);
    USART_REG_VAL(USART1, USART_INT_RBNE) |= BIT(USART_BIT_POS(USART_INT_RBNE));

//    usart_enable(USART1);
    USART_CTL0(USART1) |= USART_CTL0_UEN;

#else

    pinTXD2.Init();
    pinRXD2.Init();

    __HAL_RCC_USART2_CLK_ENABLE();

    handleUART.Instance = USART2;
    handleUART.Init.BaudRate = ::OSDP::IsEnabled() ? gset.BaudRateOSDP().ToRAW() : 115200U;
    handleUART.Init.WordLength = UART_WORDLENGTH_8B;
    handleUART.Init.StopBits = UART_STOPBITS_1;
    handleUART.Init.Parity = UART_PARITY_NONE;
    handleUART.Init.Mode = UART_MODE_TX_RX;
    handleUART.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    handleUART.Init.OverSampling = UART_OVERSAMPLING_16;

    HAL_UART_Init(&handleUART);

    HAL_NVIC_SetPriority(USART2_IRQn, PRIORITY_USART);
    HAL_NVIC_EnableIRQ(USART2_IRQn);

    HAL_UART_Receive_IT(&handleUART, &buffer, 1);

#endif

    Mode::Receive();
}


void HAL_USART::Mode::Transmit()
{
    pinTXD1.ToLow();
}


void HAL_USART::Mode::Receive()
{
    pinTXD1.ToHi();
}


void HAL_USART::Mode::WG::LevelBit()
{
    pinTXD1.ToLow();
}


void HAL_USART::Mode::WG::LevelInterval()
{
    pinTXD1.ToHi();
}


bool HAL_USART::Update()
{
    static BufferUSART buffer;

    bool result = Data::Get(buffer);

    Communicator::Update(buffer);

    return result;
}


#ifdef MCU_GD

// RS485
void USART1_IRQHandler(void)
{
    if (RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_RBNE))
    {
        uint8 data = (uint8)usart_data_receive(USART1);

        HAL_USART::Data::Append(data);
    }
}

#else

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *)
{
    HAL_USART::Data::Append(HAL_USART::UART::buffer);

    HAL_UART_Receive_IT(&HAL_USART::handleUART, &HAL_USART::UART::buffer, 1);
}

#endif
