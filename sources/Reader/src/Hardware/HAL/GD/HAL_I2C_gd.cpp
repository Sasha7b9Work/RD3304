// 2024/02/11 10:52:46 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"


namespace HAL_I2C
{
#define I2C_SPEED     100000
    const uint I2C_PERIPH = I2C0;
    const uint8 SLAVE_ADDRESS = (0x19 << 1);

    static const uint TIMEOUT = 2;

    static bool WaitFlagYes(i2c_flag_enum);
    static bool WaitFlagNo(i2c_flag_enum);
}


bool HAL_I2C::WaitFlagYes(i2c_flag_enum flag)
{
    TimeMeterMS meter;

//    while (i2c_flag_get(I2C_PERIPH, flag))
    while((I2C_REG_VAL(I2C_PERIPH, flag) & BIT(I2C_BIT_POS(flag))) != RESET)
    {
        if (meter.ElapsedMS() > TIMEOUT)
        {
            return false;
        }
    }

    return true;
}


bool HAL_I2C::WaitFlagNo(i2c_flag_enum flag)
{
    TimeMeterMS meter;

//    while (!i2c_flag_get(I2C_PERIPH, flag))
    while ((I2C_REG_VAL(I2C_PERIPH, flag) & BIT(I2C_BIT_POS(flag))) == RESET)
    {
        if (meter.ElapsedMS() > TIMEOUT)
        {
            return false;
        }
    }

    return true;
}


void HAL_I2C::Init()
{
    // PB6 SCL alternate I2C0
    // PB7 SDA alternate I2C0

    gpio_af_set(GPIOB, GPIO_AF_1, GPIO_PIN_6);
    gpio_af_set(GPIOB, GPIO_AF_1, GPIO_PIN_7);

    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_6);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_6);

    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_7);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_7);

//    i2c_clock_config(I2C_PERIPH, I2C_SPEED, I2C_DTCY_2);
    uint32_t pclk1, clkc, freq, risetime;
    uint32_t temp;

    pclk1 = rcu_clock_freq_get(CK_APB1);
    /* I2C peripheral clock frequency */
    freq = (uint32_t)(pclk1 / 1000000U);
    if (freq >= 0x0000007FU) {
        freq = 0x0000007FU;
    }
    temp = I2C_CTL1(I2C_PERIPH);
    temp &= ~I2C_CTL1_I2CCLK;
    temp |= freq;

    I2C_CTL1(I2C_PERIPH) = temp;

    /* the maximum SCL rise time is 1000ns in standard mode */
    risetime = (uint32_t)((pclk1 / 1000000U) + 1U);
    if (risetime >= 0x0000007FU) {
        I2C_RT(I2C_PERIPH) = 0x0000007FU;
    }
    else if (risetime <= 0x00000002U) {
        I2C_RT(I2C_PERIPH) = 0x00000002U;
    }
    else {
        I2C_RT(I2C_PERIPH) = risetime;
    }
    clkc = (uint32_t)(pclk1 / (I2C_SPEED * 2U));
    if (clkc < 0x04U) {
        /* the CLKC in standard mode minmum value is 4 */
        clkc = 0x04U;
    }

    I2C_CKCFG(I2C_PERIPH) |= (I2C_CKCFG_CLKC & clkc);

    /* configure I2C address */
//    i2c_mode_addr_config(I2C_PERIPH, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, SLAVE_ADDRESS);
    /* SMBus/I2C mode selected */
    uint32_t ctl = 0U;

    ctl = I2C_CTL0(I2C_PERIPH);
    ctl &= ~(I2C_CTL0_SMBEN);
    ctl |= I2C_I2CMODE_ENABLE;
    I2C_CTL0(I2C_PERIPH) = ctl;
    /* configure address */
    uint addr = SLAVE_ADDRESS & 0x000003FFU;
    I2C_SADDR0(I2C_PERIPH) = (I2C_ADDFORMAT_7BITS | addr);

    /* enable I2C_PERIPH */
//    i2c_enable(I2C_PERIPH);
    I2C_CTL0(I2C_PERIPH) |= I2C_CTL0_I2CEN;

//    i2c_ack_config(I2C_PERIPH, I2C_ACK_ENABLE);
    I2C_CTL0(I2C_PERIPH) = ((I2C_CTL0(I2C_PERIPH) & ~(I2C_CTL0_ACKEN)) | I2C_ACK_ENABLE);
}


bool HAL_I2C::Read(uint8, uint8 reg_addr, uint8 *buf, uint8 len)
{
    TimeMeterMS meter;

    i2c_stop_on_bus(I2C_PERIPH);

    /* wait until I2C bus is idle */
    if (!WaitFlagYes(I2C_FLAG_I2CBSY))
    {
        return false;
    }

    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C_PERIPH);

    /* wait until SBSEND bit is set */
    if (!WaitFlagNo(I2C_FLAG_SBSEND))
    {
        return false;
    }

    /* send slave address to I2C bus */
//    i2c_master_addressing(I2C_PERIPH, SLAVE_ADDRESS, I2C_TRANSMITTER);
    I2C_DATA(I2C_PERIPH) = SLAVE_ADDRESS;

    /* wait until ADDSEND bit is set */
    WaitFlagNo(I2C_FLAG_ADDSEND);

    /* clear ADDSEND bit */
    i2c_flag_clear(I2C_PERIPH, I2C_FLAG_ADDSEND);

    /* send command */
//    i2c_data_transmit(I2C_PERIPH, reg_addr);
    I2C_DATA(I2C_PERIPH) = DATA_TRANS(reg_addr);

    WaitFlagNo(I2C_FLAG_TBE);

    i2c_start_on_bus(I2C_PERIPH);

    WaitFlagNo(I2C_FLAG_SBSEND);

//    i2c_master_addressing(I2C_PERIPH, SLAVE_ADDRESS, I2C_RECEIVER);
    I2C_DATA(I2C_PERIPH) = SLAVE_ADDRESS | 1;

    WaitFlagNo(I2C_FLAG_ADDSEND);

    i2c_flag_clear(I2C_PERIPH, I2C_FLAG_ADDSEND);

    if (len == 1)
    {
//        i2c_ack_config(I2C_PERIPH, I2C_ACK_DISABLE);
        I2C_CTL0(I2C_PERIPH) = ((I2C_CTL0(I2C_PERIPH) & ~(I2C_CTL0_ACKEN)) | I2C_ACK_DISABLE);

        i2c_stop_on_bus(I2C_PERIPH);

//        while (!i2c_flag_get(I2C_PERIPH, I2C_FLAG_RBNE))
        while ((I2C_REG_VAL(I2C_PERIPH, I2C_FLAG_RBNE) & BIT(I2C_BIT_POS(I2C_FLAG_RBNE))) == RESET)
        {
            if (meter.ElapsedMS() > TIMEOUT)
            {
                break;
            }
        }

        buf[0] = (uint8_t)DATA_RECV(I2C_DATA(I2C_PERIPH));
    }
    else
    {
        for (int i = 0; i < len; i++)
        {
            if (i == len - 1)
            {
//                i2c_ack_config(I2C_PERIPH, I2C_ACK_DISABLE);
                I2C_CTL0(I2C_PERIPH) = ((I2C_CTL0(I2C_PERIPH) & ~(I2C_CTL0_ACKEN)) | I2C_ACK_DISABLE);
                i2c_stop_on_bus(I2C_PERIPH);
            }

//            while (!i2c_flag_get(I2C_PERIPH, I2C_FLAG_RBNE))
            while ((I2C_REG_VAL(I2C_PERIPH, I2C_FLAG_RBNE) & BIT(I2C_BIT_POS(I2C_FLAG_RBNE))) == RESET)
            {
                if (meter.ElapsedMS() > TIMEOUT)
                {
                    break;
                }
            }

//            buf[i] = i2c_data_receive(I2C_PERIPH);
            buf[i] = (uint8_t)DATA_RECV(I2C_DATA(I2C_PERIPH));
        }
    }

    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(I2C_PERIPH);

    /* wait until stop condition generate */
    while (I2C_CTL0(I2C_PERIPH) & 0x0200)
    {
        if (meter.ElapsedMS() > TIMEOUT)
        {
            break;
        }
    }

    /* enable acknowledge */
//    i2c_ack_config(I2C_PERIPH, I2C_ACK_ENABLE);
    I2C_CTL0(I2C_PERIPH) = ((I2C_CTL0(I2C_PERIPH) & ~(I2C_CTL0_ACKEN)) | I2C_ACK_ENABLE);

    return true;
}


void HAL_I2C::Read8(uint8 id, uint8 reg_addr, uint8 *data)
{
    Read(id, reg_addr, data, 1);
}


bool HAL_I2C::Write(uint8, uint8 reg_addr, const uint8 *reg_data, uint8 len)
{
    TimeMeterMS meter;

    /* wait until I2C bus is idle */
    if (!WaitFlagYes(I2C_FLAG_I2CBSY))
    {
        return false;
    }

    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C_PERIPH);

    /* wait until SBSEND bit is set */
    if (!WaitFlagNo(I2C_FLAG_SBSEND))
    {
        return false;
    }

    /* send slave address to I2C bus */
//    i2c_master_addressing(I2C_PERIPH, SLAVE_ADDRESS, I2C_TRANSMITTER);
    I2C_DATA(I2C_PERIPH) = SLAVE_ADDRESS;

    /* wait until ADDSEND bit is set */
    if(!WaitFlagNo(I2C_FLAG_ADDSEND))
    {
        return false;
    }

    /* N=1,reset ACKEN bit before clearing ADDRSEND bit */
//    i2c_ack_config(I2C_PERIPH, I2C_ACK_DISABLE);
    I2C_CTL0(I2C_PERIPH) = ((I2C_CTL0(I2C_PERIPH) & ~(I2C_CTL0_ACKEN)) | I2C_ACK_DISABLE);

    /* clear ADDSEND bit */
    i2c_flag_clear(I2C_PERIPH, I2C_FLAG_ADDSEND);

    /* send command */
//    i2c_data_transmit(I2C_PERIPH, reg_addr);
    I2C_DATA(I2C_PERIPH) = DATA_TRANS(reg_addr);

    /* wait until the TBE bit is set */
    WaitFlagNo(I2C_FLAG_TBE);

    /* send array of data */
    for (int i = 0; i < len; i++)
    {
//        i2c_data_transmit(I2C_PERIPH, *reg_data++);
        I2C_DATA(I2C_PERIPH) = DATA_TRANS(*reg_data++);

        /* wait until the TBE bit is set */
        if (!WaitFlagNo(I2C_FLAG_TBE))
        {
            break;
        }
    }

    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(I2C_PERIPH);

    /* wait until stop condition generate */
    while (I2C_CTL0(I2C_PERIPH) & 0x0200)
    {
        if (meter.ElapsedMS() > TIMEOUT)
        {
            break;
        }
    }

    /* Enable Acknowledge */
//    i2c_ack_config(I2C_PERIPH, I2C_ACK_ENABLE);
    I2C_CTL0(I2C_PERIPH) = ((I2C_CTL0(I2C_PERIPH) & ~(I2C_CTL0_ACKEN)) | I2C_ACK_ENABLE);

    return true;
}


void HAL_I2C::Write8(uint8 id, uint8 reg_addr, uint8 data)
{
    Write(id, reg_addr, &data, 1);
}
