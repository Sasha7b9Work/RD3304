// 2022/7/6 9:44:43 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/CLRC66303HN/RegistersCLRC663.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Modules/CLRC66303HN/CLRC66303_def.h"
#include "system.h"
#include <cstring>


namespace CLRC66303HN
{
    FIFO fifo;

    IRQ0 irq0;
    IRQ1 irq1;

    void Register::Write()
    {
        uint8 buffer[2] = { (uint8)(address << 1), (uint8)data };

        HAL_SPI::WriteBuffer(DirectionSPI::Reader, buffer, 2);
    }


    void Register::Write(uint8 _data)
    {
        data = _data;

        Write();
    }


    uint8 Register::Read()
    {
        uint8 out[2] = { (uint8)((address << 1) | 1), 0 };
        uint8 in[2];

        HAL_SPI::WriteRead(DirectionSPI::Reader, out, in, 2);

        data = in[1];

        return (uint8)data;
    }


    void Register::Read(const uint8 _out[2], uint8 _in[2])
    {
        uint8 out[3] = { (uint8)((address << 1) | 1), _out[0], _out[1] };
        uint8 in[3];

        HAL_SPI::WriteRead(DirectionSPI::Reader, out, in, 3);

        _in[0] = in[1];
        _in[1] = in[2];
    }


    void FIFO::Init()
    {
        Register(MFRC630_REG_FIFOCONTROL).Write(0xB0);
    }


    void FIFO::Flush()
    {
        Register(MFRC630_REG_FIFOCONTROL).Write(0xB0);
    }


    int FIFO::Length()
    {
        return Register(MFRC630_REG_FIFOLENGTH).Read();
    }


    int FIFO::Read(uint8 *buffer, int length)
    {
        int counter = 0;

        if (length > 512)
        {
            return -1;
        }

        for (int i = 0; i < length; i++)
        {
            *buffer++ = Register(MFRC630_REG_FIFODATA).Read();
            counter++;
        }

        return counter;
    }


    void FIFO::Clear()
    {
        Register(MFRC630_REG_FIFOCONTROL).Write(0xB0);
    }


    void FIFO::Push(uint8 data)
    {
        Register(MFRC630_REG_FIFODATA).Write(data);
    }


    uint8 FIFO::Pop()
    {
        return Register(MFRC630_REG_FIFODATA).Read();
    }


    void IRQ0::Clear()
    {
        Register(MFRC630_REG_IRQ0).Write(0x7F);      // Clears all bits in IRQ0
    }


    void IRQ1::Clear()
    {
        Register(MFRC630_REG_IRQ1).Write(0x7F);
    }


    uint8 IRQ0::GetValue()
    {
        return Register(MFRC630_REG_IRQ0).Read();
    }


    uint8 IRQ1::GetValue()
    {
        return Register(MFRC630_REG_IRQ1).Read();
    }
}
