#include "defines.h"
#include "Hardware/Timer.h"
#include <stdlib.h>


#define SW_I2C_WAIT_TIME    1 // 10us 100kHz


#define I2C_READ            0x01
#define READ_CMD            1
#define WRITE_CMD           0


#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif


#define SDA_TO_LOW()    GPIOB->BSRR = GPIO_PIN_7 << 16u
#define SDA_TO_HI()     GPIOB->BSRR = GPIO_PIN_7
#define GET_SDA()       (GPIOB->IDR & GPIO_PIN_7)
#define SCL_TO_LOW()    GPIOB->BSRR = GPIO_PIN_6 << 16u;
#define SCL_TO_HI()     GPIOB->BSRR = GPIO_PIN_6

#define SDA_SET_INPUT()     GPIOB->CRL |= 0x80000000;  GPIOB->CRL &= 0x7FFFFFFF
#define SDA_SET_OUTPUT()    GPIOB->CRL &= 0x7FFFFFFF; GPIOB->CRL |= 0x70000000

#define DELAY()     __asm("NOP"); __asm("NOP")



static void DelayUS(uint32_t delay)
{
    volatile int counter = (int)(delay);

    while (counter--) {}
}


static void sda_out(uint8_t out)
{
    if (out)
    {
        SDA_TO_HI();
    }
    else
    {
        SDA_TO_LOW();
    }
}


static void i2c_clk_data_out()
{
    SCL_TO_HI();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    SCL_TO_LOW()
}

static void i2c_port_initial()
{
    SDA_TO_HI();
    SCL_TO_HI();
}


static void i2c_start_condition()
{
    SDA_TO_HI();
    SCL_TO_HI();
    DELAY();
    DELAY();
    SDA_TO_LOW();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    SCL_TO_LOW()
    DELAY();
}


static void i2c_stop_condition()
{
    SDA_TO_LOW();
    SCL_TO_HI();
    DELAY();
    SDA_TO_HI();
    DELAY();
}


static uint8_t i2c_check_ack()
{
    uint8_t ack;
    int i;
    unsigned int temp;
    SDA_SET_INPUT();
    SCL_TO_HI();
    ack = 0;
    DELAY();
    for (i = 10; i > 0; i--)
    {
        temp = !GET_SDA();
        if (temp)
        {
            ack = 1;
            break;
        }
    }
    SCL_TO_LOW()
    SDA_SET_OUTPUT();
    DELAY();
    return ack;
}

static void i2c_check_not_ack()
{
    SDA_SET_INPUT();
    i2c_clk_data_out();
    SDA_SET_OUTPUT();
    DELAY();
}

static void i2c_slave_address(uint8_t IICID, uint8_t readwrite)
{
    int x;

    if (readwrite)
    {
        IICID |= I2C_READ;
    }
    else
    {
        IICID &= ~I2C_READ;
    }

    SCL_TO_LOW()

    for (x = 7; x >= 0; x--)
    {
        sda_out((uint8)(IICID & (1 << x)));
        DELAY();
        i2c_clk_data_out();
    }
}

static void i2c_register_address(uint8_t addr)
{
    SCL_TO_LOW()

    for (int x = 7; x >= 0; x--)
    {
        sda_out((uint8)(addr & (1 << x)));
        DELAY();
        i2c_clk_data_out();
    }
}

static void i2c_send_ack()
{
    SDA_SET_OUTPUT();
    SDA_TO_LOW();
    DELAY();
    SCL_TO_HI();
    DELAY();
    SDA_TO_LOW();
    DELAY();
    SCL_TO_LOW()
    SDA_SET_OUTPUT();
    DELAY();
}

static void SW_I2C_Write_Data(uint8_t data)
{
    SCL_TO_LOW()

    for (int x = 7; x >= 0; x--)
    {
        sda_out((uint8)(data & (1 << x)));
        DELAY();
        i2c_clk_data_out();
    }
}

static uint8_t SW_I2C_Read_Data()
{
    uint8_t readdata = 0;
    SDA_SET_INPUT();

    for (int x = 8; x--;)
    {
        SCL_TO_HI();
        readdata <<= 1;
        if (GET_SDA())
            readdata |= 0x01;
        DELAY();
        SCL_TO_LOW()
        DELAY();
    }

    SDA_SET_OUTPUT();
    return readdata;
}

static uint8_t SW_I2C_Read_8addr(uint8_t IICID, uint8_t regaddr, uint8_t *pdata, uint8_t rcnt)
{
    uint8_t returnack = TRUE;
    uint8_t index;

    if (!rcnt) return FALSE;

    i2c_port_initial();
    i2c_start_condition();
    i2c_slave_address(IICID, WRITE_CMD);
    if (!i2c_check_ack()) { returnack = FALSE; }
    DELAY();
    DELAY();
    DELAY();
    i2c_register_address(regaddr);
    if (!i2c_check_ack()) { returnack = FALSE; }
    DELAY();
    DELAY();
    i2c_start_condition();
    i2c_slave_address(IICID, READ_CMD);
    if (!i2c_check_ack()) { returnack = FALSE; }
    if (rcnt > 1)
    {
        for (index = 0; index < (rcnt - 1); index++)
        {
            DELAY();
            pdata[index] = SW_I2C_Read_Data();
            i2c_send_ack();
        }
    }
    DelayUS(SW_I2C_WAIT_TIME);
    pdata[rcnt - 1] = SW_I2C_Read_Data();
    i2c_check_not_ack();
    i2c_stop_condition();

    return returnack;
}

//static uint8_t SW_I2C_Read_16addr(uint8_t IICID, uint16_t regaddr, uint8_t *pdata, uint8_t rcnt)
//{
//    uint8_t returnack = TRUE;
//    uint8_t index;
//
//    if (!rcnt) return FALSE;
//
//    i2c_port_initial();
//    i2c_start_condition();
//    //写ID
//    i2c_slave_address(IICID, WRITE_CMD);
//    if (!i2c_check_ack()) { returnack = FALSE; }
//    DELAY();
//    //写高八位地址
//    i2c_register_address((uint8_t)(regaddr >> 8));
//    if (!i2c_check_ack()) { returnack = FALSE; }
//    DELAY();
//    //写低八位地址
//    i2c_register_address((uint8_t)regaddr);
//    if (!i2c_check_ack()) { returnack = FALSE; }
//    DELAY();
//    //重启I2C总线
//    i2c_start_condition();
//    //读ID
//    i2c_slave_address(IICID, READ_CMD);
//    if (!i2c_check_ack()) { returnack = FALSE; }
//    //循环读数据
//    if (rcnt > 1)
//    {
//        for (index = 0; index < (rcnt - 1); index++)
//        {
//            DELAY();
//            pdata[index] = SW_I2C_Read_Data();
//            i2c_send_ack();
//        }
//    }
//    DelayUS(SW_I2C_WAIT_TIME);
//    pdata[rcnt - 1] = SW_I2C_Read_Data();
//    i2c_check_not_ack();
//    i2c_stop_condition();
//
//    return returnack;
//}

static uint8_t SW_I2C_Write_8addr(uint8_t IICID, uint8_t regaddr, const uint8_t *pdata, uint8_t rcnt)
{
    uint8_t returnack = TRUE;

    if (!rcnt) return FALSE;

    i2c_port_initial();
    i2c_start_condition();
    i2c_slave_address(IICID, WRITE_CMD);
    if (!i2c_check_ack())
    {
        returnack = FALSE;
    }
    DELAY();
    i2c_register_address(regaddr);
    if (!i2c_check_ack())
    {
        returnack = FALSE;
    }
    DELAY();
    for (int index = 0; index < rcnt; index++)
    {
        SW_I2C_Write_Data(pdata[index]);
        if (!i2c_check_ack())
        {
            returnack = FALSE;
        }
        DELAY();
    }
    i2c_stop_condition();
    return returnack;
}


//static uint8_t SW_I2C_Write_16addr(uint8_t IICID, uint16_t regaddr, uint8_t *pdata, uint8_t rcnt)
//{
//    uint8_t returnack = TRUE;
//
//    if (!rcnt) return FALSE;
//
//    i2c_port_initial();
//    i2c_start_condition();
//    //写ID
//    i2c_slave_address(IICID, WRITE_CMD);
//    if (!i2c_check_ack()) { returnack = FALSE; }
//    DELAY();
//    //写高八位地址
//    i2c_register_address((uint8_t)(regaddr >> 8));
//    if (!i2c_check_ack()) { returnack = FALSE; }
//    DELAY();
//    //写低八位地址
//    i2c_register_address((uint8_t)regaddr);
//    if (!i2c_check_ack()) { returnack = FALSE; }
//    DELAY();
//    //写数据
//    for (int index = 0; index < rcnt; index++)
//    {
//        SW_I2C_Write_Data(pdata[index]);
//        if (!i2c_check_ack()) { returnack = FALSE; }
//        DELAY();
//    }
//    i2c_stop_condition();
//    return returnack;
//}


namespace HAL_I2C
{
    void Init()
    {
        //  PinAF_OD    pinSCL(GPIOB, GPIO_PIN_6);    // Output
        //  PinAF_OD    pinSDA(GPIOB, GPIO_PIN_7);    // Output, Input

        static GPIO_InitTypeDef is =
        {//    SCL          SDA
            GPIO_PIN_6 | GPIO_PIN_7,
            GPIO_MODE_OUTPUT_OD,
            GPIO_NOPULL,
            GPIO_SPEED_HIGH
        };

        HAL_GPIO_Init(GPIOB, &is);

        GPIOB->ODR |= (1 << 7);
    }

    bool Read(uint8 dev_id, uint8 reg_addr, uint8 *reg_data, uint8 len)
    {
        return SW_I2C_Read_8addr((uint8)(dev_id << 1), reg_addr, reg_data, len) != FALSE;
    }

    void Read8(uint8 dev_id, uint8 reg_addr, uint8 *data)
    {
        SW_I2C_Read_8addr((uint8)(dev_id << 1), reg_addr, data, 1);
    }

    bool Write(uint8 dev_id, uint8 reg_addr, const uint8 *reg_data, uint8 len)
    {
        return SW_I2C_Write_8addr((uint8)(dev_id << 1), reg_addr, reg_data, len) != FALSE;
    }

    void Write8(uint8 dev_id, uint8 reg_addr, uint8 data)
    {
        SW_I2C_Write_8addr((uint8)(dev_id << 1), reg_addr, &data, 1);
    }
}
