// 2022/7/6 10:32:35 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/CLRC66303HN/CommandsCLRC663.h"
#include "Modules/CLRC66303HN/RegistersCLRC663.h"
#include "Modules/CLRC66303HN/CLRC66303HN.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Hardware/Timer.h"
#include "Utils/Math.h"
#include <cstring>
#include <cstdio>


uint8 &Block4::operator[](uint i)
{
    if (i < 4)
    {
        return bytes[i];
    }

    static uint8 null;
    return null;
}

uint8 &Block16::operator[](uint i)
{
    if (i < 16)
    {
        return bytes[i];
    }

    static uint8 null;
    return null;
}


namespace CLRC66303HN
{
    void Command::Idle()
    {
        Register(MFRC630_REG_COMMAND).Write(0x00);
    }


    void Command::Send(uint8 command)
    {
        Send(&command, 1);
    }


    void Command::Send(uint8 command, uint8 data)
    {
        uint8 buffer[2] = { command, data };

        Send(buffer, 2);
    }


    void Command::Send(uint8 command, uint8 d0, uint8 d1, uint8 d2, uint8 d3)
    {
        uint8 buffer[5] = { command, d0, d1, d2, d3 };

        Send(buffer, 5);
    }


    void Command::Send(uint8 command, uint8 data0, uint8 data1, uint8 data2, uint8 data3, uint8 data4, uint8 data5)
    {
        uint8 buffer[7] = { command, data0, data1, data2, data3, data4, data5 };

        Send(buffer, 7);
    }


    void Command::Send(uint8 *buffer, int size)
    {
        Idle();

        fifo.Flush();

        for (int i = 0; i < size; i++)
        {
            fifo.Push(*buffer++);
        }

        Register(MFRC630_REG_COMMAND).Write(MFRC630_CMD_TRANSCEIVE);
    }


    bool Command::AnticollisionCL(int cl, UID *uid)
    {
        Command::Idle();
        fifo.Clear();
        irq0.Clear();

        Register(0x2C).Write(0x18);        // Switches the CRC extention ON in tx direction //-V525
        Register(0x2D).Write(0x18);        // Switches the CRC extention OFF in rx direction

        Register(0x2E).Write(0x08);        // All bits will be sent via NFC

        irq0.Clear();

        Command::Send((cl == 1) ? 0x93U : 0x95U, 0x20U);

        TimeMeterMS meter;

        while (meter.ElapsedMS() < 10)
        {
            if (!pinIRQ_TRX.IsHi())
            {
                if (RegError().Read() & RegError::CollDet)
                {
                    return false;
                }

                if (irq0.GetValue() & IRQ0::ErrIRQ)
                {
                    return false;
                }
                else
                {
                    int i0 = (cl == 1) ? 0 : 5;

                    uid->bytes[i0 + 0] = fifo.Pop();
                    uid->bytes[i0 + 1] = fifo.Pop();
                    uid->bytes[i0 + 2] = fifo.Pop();
                    uid->bytes[i0 + 3] = fifo.Pop();
                    uid->bytes[i0 + 4] = fifo.Pop();

                    return true;
                }
            }
        }

        return false;
    }


    bool Command::SelectCL(int cl, UID *uid)
    {
        Command::Idle();
        fifo.Clear();

        Register(0x2C).Write(0x19);        // Switches the CRC extention ON in tx direction //-V525
        Register(0x2D).Write(0x19);        // Switches the CRC extention OFF in rx direction

        Register(0x2E).Write(0x08);

        irq0.Clear();

        int i0 = (cl == 1) ? 0 : 5;

        Command::Send(cl == 1 ? 0x93U : 0x95U, 0x70,  uid->bytes[i0 + 0],
                                                      uid->bytes[i0 + 1],
                                                      uid->bytes[i0 + 2],
                                                      uid->bytes[i0 + 3],
                                                      uid->bytes[i0 + 4]);

        TimeMeterMS meter;

        while (meter.ElapsedMS() < 10)
        {
            if (!pinIRQ_TRX.IsHi())
            {
                if (irq0.GetValue() & IRQ0::ErrIRQ)
                {
                    return false;
                }
                else
                {
                    uint8 sak = fifo.Pop();

                    if (_GET_BIT(sak, 2) == 1)
                    {
                        return true;
                    }

                    if (_GET_BIT(sak, 2) == 0)
                    {
                        uid->Calculate();
                        return true;
                    }
                }
            }
        }

        return false;
    }
}


bool CLRC66303HN::Command::NTAG::Write2Blocks(int num_block, uint8 buffer[8])
{
    return WriteBlock(num_block, buffer) && WriteBlock(num_block + 1, buffer + 4);
}


bool CLRC66303HN::Command::NTAG::WriteBlock(int block_address, const uint8 _source[4])
{
    uint8 source[4];

    std::memcpy(source, _source, 4);

#ifdef SAFE_PASSWORD

    if (block_address == TypeCard::NTAG::BlockPassword())
    {
        BitSet32 bs = BitSet32::FromBytes(source[0], source[1], source[2], source[3]);

        if (bs.word > 10)
        {
            std::memset(source, 0, 4);
        }
    }
    else if (block_address == TypeCard::NTAG::BlockPassword() + 1)
    {
        BitSet32 bs = BitSet32::FromBytes(source[0], source[1], source[2], source[3]);

        if (bs.word)
        {
            std::memset(source, 0, 4);
        }
    }

#endif

    Idle();

    fifo.Clear();

    // set appropriate CRC registers, only for Tx
    Register(MFRC630_REG_TXCRCPRESET).Write(MFRC630_RECOM_14443A_CRC | MFRC630_CRC_ON);
    Register(MFRC630_REG_RXCRCCON).Write(MFRC630_RECOM_14443A_CRC | MFRC630_CRC_OFF);

    // enable the global IRQ for idle, errors and timer.
    Register(MFRC630_REG_IRQ0EN).Write(MFRC630_IRQ0EN_IDLE_IRQEN | MFRC630_IRQ0EN_ERR_IRQEN);
    Register(MFRC630_REG_IRQ1EN).Write(MFRC630_IRQ1EN_TIMER0_IRQEN);

    irq0.Clear();  // clear irq0
    irq1.Clear();  // clear irq1

    // Go into send, then straight after in receive.
    Send(MFRC630_MF_CMD_WRITE, (uint8)block_address);

    TimeMeterMS meter;

    while (!(irq1.GetValue() & MFRC630_IRQ1_GLOBAL_IRQ))    // stop polling irq1 and quit the timeout loop.
    {
        if (meter.ElapsedMS() > 100)
        {
            return 0;
        }
    }

    Idle();

    if (irq0.GetValue() & MFRC630_IRQ0_ERR_IRQ)
    {
        // some error
        return false;
    }

    if (fifo.Length() != 1)
    {
        return false;
    }

    uint8 res = 0;

    fifo.Read(&res, 1);

    if (res != MFRC630_MF_ACK)
    {
        return false;
    }

    irq0.Clear();  // clear irq0
    irq1.Clear();  // clear irq1

    // go for the second stage.
    Send(source, 16);

    meter.Reset();

    // block until we are done
    while (!(irq1.GetValue() & MFRC630_IRQ1_GLOBAL_IRQ))    // stop polling irq1 and quit the timeout loop.
    {
        if (meter.ElapsedMS() > 100)
        {
            return false;
        }
    }

    Idle();

    if (irq0.GetValue() & MFRC630_IRQ0_ERR_IRQ)
    {
        // some error
        return false;
    }

    if (fifo.Length() != 1)
    {
        return false;
    }

    fifo.Read(&res, 1);

    if (res == MFRC630_MF_ACK)
    {
        return true;  // second stage was responded with ack! Write successful.
    }

    return false;
}


namespace CLRC66303HN
{
    namespace Command
    {
        namespace NTAG
        {
            static bool ReadBlockRAW(int num_block, uint8 buffer[4])
            {
                Idle();

                fifo.Clear();

                Register(MFRC630_REG_TXCRCPRESET).Write(MFRC630_RECOM_14443A_CRC | MFRC630_CRC_ON);
                Register(MFRC630_REG_RXCRCCON).Write(MFRC630_RECOM_14443A_CRC | MFRC630_CRC_ON);

                // enable the global IRQ for idle, errors and timer.
                Register(MFRC630_REG_IRQ0EN).Write(MFRC630_IRQ0EN_IDLE_IRQEN | MFRC630_IRQ0EN_ERR_IRQEN);
                Register(MFRC630_REG_IRQ1EN).Write(MFRC630_IRQ1EN_TIMER0_IRQEN);

                irq0.Clear();  // clear irq0
                irq1.Clear();  // clear irq1

                // Go into send, then straight after in receive.
                Send(MFRC630_MF_CMD_READ, (uint8)num_block);

                TimeMeterMS meter;

                while (!(irq1.GetValue() & MFRC630_IRQ1_GLOBAL_IRQ))        // stop polling irq1 and quit the timeout loop.
                {
                    if (meter.ElapsedMS() > 100)
                    {
                        return false;
                    }
                }

                Idle();

                if (irq0.GetValue() & MFRC630_IRQ0_ERR_IRQ)
                {
                    // some error
                    return false;
                }

                // all seems to be well...
                int buffer_length = fifo.Length();
                int rx_len = (buffer_length <= 4) ? buffer_length : 4;
                fifo.Read(buffer, rx_len);

                return (rx_len == 4);
            }
        }

        namespace Mifare
        {
            namespace Command
            {
                static void LoadKey(uint8 *key_nr)
                {
                    Idle();
                    fifo.Flush();
                    fifo.Write(key_nr, 6);
                    Register(MFRC630_REG_COMMAND).Write(MFRC630_CMD_LOADKEY);
                }

                static void Auth(uint8 key_type, uint8 block, const uint8 *uid)
                {
                    Idle();

                    uint8 data[6] = { key_type, block, uid[0], uid[1], uid[2], uid[3] };

                    fifo.Flush();

                    fifo.Write(data, 6);

                    Register(MFRC630_REG_COMMAND).Write(MFRC630_CMD_MFAUTHENT);
                }
            }

            //authentification requires the relevant key and the uid, afterwards data will be in exchange between tag and reader
            //for most parts, manufacturer key will be used and key A is relevant (there are two types of keys, likely one is for reading action, the other for write access [type b])
            static bool Auth(uint8 key_type, uint8 block, uint8 *uid)
            {
                Idle();
                fifo.Flush();

                // According to datashet Interrupt on idle and timer with MFAUTHENT, but lets
                // include ERROR as well.
                Register(MFRC630_REG_IRQ0EN).Write(MFRC630_IRQ0EN_IDLE_IRQEN | MFRC630_IRQ0EN_ERR_IRQEN);
                Register(MFRC630_REG_IRQ1EN).Write(MFRC630_IRQ1EN_TIMER0_IRQEN);  // only trigger on timer for irq1

                irq0.Clear();
                irq1.Clear();

                Idle();
                fifo.Flush();

                // start the authentication procedure.
                Command::Auth(key_type, block, uid);

                TimeMeterMS meter;

                while (!(irq1.GetValue() & MFRC630_IRQ1_GLOBAL_IRQ))
                {
                    if (meter.ElapsedMS() > 100)
                    {
                        break;
                    }
                }

                // status is always valid, it is set to 0 in case of authentication failure.
                bool result = (Register(MFRC630_REG_STATUS).Read() & MFRC630_STATUS_CRYPTO1_ON) != 0;

                return result;
            }

            static bool ReadBlockRAW(int num_block, uint8 buffer[16])
            {
                uint8 default_key[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
                Command::LoadKey(default_key);

//                if (!_Auth(MFRC630_MF_AUTH_KEY_A, (uint8)num_block, Card::uid.bytes))

                uint8 bytes_uid[4] = { 0x83, 0x0E, 0xEB, 0xE9 };
//                uint8 bytes_uid[4] = { 0xE9, 0xEB, 0x0E, 0x83 };

                if (!Auth(MFRC630_MF_AUTH_KEY_A, (uint8)num_block, bytes_uid))
                {
                    return false;
                }

                Idle();

                fifo.Clear();

                Register(MFRC630_REG_TXCRCPRESET).Write(MFRC630_RECOM_14443A_CRC | MFRC630_CRC_ON);
                Register(MFRC630_REG_RXCRCCON).Write(MFRC630_RECOM_14443A_CRC | MFRC630_CRC_ON);

                // enable the global IRQ for idle, errors and timer.
                Register(MFRC630_REG_IRQ0EN).Write(MFRC630_IRQ0EN_IDLE_IRQEN | MFRC630_IRQ0EN_ERR_IRQEN);
                Register(MFRC630_REG_IRQ1EN).Write(MFRC630_IRQ1EN_TIMER0_IRQEN);

                irq0.Clear();
                irq1.Clear();

                Send(MFRC630_MF_CMD_READ, (uint8)num_block);

                TimeMeterMS meter;

                while (!(irq1.GetValue() & MFRC630_IRQ1_GLOBAL_IRQ))
                {
                    if (meter.ElapsedMS() > 100)
                    {
                        return false;
                    }
                }

                Idle();

                if (irq0.GetValue() & MFRC630_IRQ0_ERR_IRQ)
                {
                    return false;
                }

                int buffer_length = fifo.Length();
                int rx_len = (buffer_length <= 16) ? buffer_length : 16;
                fifo.Read(buffer, rx_len);

                return (rx_len == 16);
            }
        }
    }
}


bool CLRC66303HN::Command::NTAG::ReadBlock(int num_block, Block4 &block)
{
    uint8 data1[4];

    if (ReadBlockRAW(num_block, data1))
    {
        uint8 data2[4];

        if (ReadBlockRAW(num_block, data2))
        {
            if (ReadBlockRAW(num_block, block.bytes))
            {
                return Math::ArraysEqueals(block.bytes, data1, data2, 4);
            }
        }
    }

    return false;
}


bool CLRC66303HN::Command::Mifare::ReadBlock(int num_block, Block16 &block)
{
    uint8 data1[16];

    if (ReadBlockRAW(num_block, data1))
    {
        uint8 data2[16];

        if (ReadBlockRAW(num_block, data2))
        {
            if (ReadBlockRAW(num_block, block.bytes))
            {
                return Math::ArraysEqueals(block.bytes, data1, data2, 16);
            }
        }
    }

    return false;
}


bool CLRC66303HN::Command::NTAG::ReadBlock(int num_block, uint *word)
{
    Block4 block;

    bool result = ReadBlock(num_block, block);

    *word = block.ToWord();

    return result;
}


bool CLRC66303HN::Command::NTAG::Read2Blocks(int num_block, BitSet64 *bit_set)
{
    return ReadBlock(num_block, &bit_set->word[0]) && ReadBlock(num_block + 1, &bit_set->word[1]);
}


bool CLRC66303HN::Command::PasswordAuth(uint64 password)
{
    Idle();

    fifo.Clear();

    Register(MFRC630_REG_TXCRCPRESET).Write(MFRC630_RECOM_14443A_CRC | MFRC630_CRC_ON);
    Register(MFRC630_REG_RXCRCCON).Write(MFRC630_RECOM_14443A_CRC | MFRC630_CRC_ON);

    // enable the global IRQ for idle, errors and timer.
    Register(MFRC630_REG_IRQ0EN).Write(MFRC630_IRQ0EN_IDLE_IRQEN | MFRC630_IRQ0EN_ERR_IRQEN);
    Register(MFRC630_REG_IRQ1EN).Write(MFRC630_IRQ1EN_TIMER0_IRQEN);

    irq0.Clear();  // clear irq0
    irq1.Clear();  // clear irq1

    BitSet64 pwd(password);

    // Go into send, then straight after in receive.
    Send(MFRC630_CMD_PWD_AUTH, pwd.bytes[0], pwd.bytes[1], pwd.bytes[2], pwd.bytes[3]);

    TimeMeterMS meter;

    while (!(irq1.GetValue() & MFRC630_IRQ1_GLOBAL_IRQ))        // stop polling irq1 and quit the timeout loop.
    {
        if (meter.ElapsedMS() > 100)
        {
            return false;
        }
    }

    Idle();

    if (irq0.GetValue() & MFRC630_IRQ0_ERR_IRQ)
    {
        // some error
        return false;
    }

    bool result = fifo.Length() >= 2;

    if (result)
    {
        result = TypeCard::Detect();
    }

    // all seems to be well...
    int buffer_length = fifo.Length();
    int rx_len = (buffer_length <= 2) ? buffer_length : 2;

    uint8 buffer[2];

    fifo.Read(buffer, rx_len);

    return result;
}


bool CLRC66303HN::Command::NTAG::WriteData(int num_block, const void *_data, int size)
{
    while (size % 4)
    {
        size++;
    }

    const uint8 *data = (const uint8 *)_data;
    const uint8 *const end = data + size;

    while (data < end)
    {
        int num_bytes = end - data;

        if (num_bytes >= 4)
        {
            if (!WriteBlock(num_block, data))
            {
                return false;
            }
        }
        else
        {
            Block4 block;

            if (ReadBlock(num_block, block))
            {
                for (uint i = 0; i < (uint)num_bytes; i++)
                {
                    block[i] = data[i];
                }

                if (!WriteBlock(num_block, block.bytes))
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }

        data += 4;

        num_block++;
    }

    return true;
}


bool CLRC66303HN::Command::NTAG::ReadData(int num_block, void *_data, int size)
{
    while (size % 4)
    {
        size++;
    }

    int written_bytes = 0;
    uint8 *data = (uint8 *)_data;
    const uint8 *const end = data + size;

    while (data < end)
    {
        if (ReadBlock(num_block, (uint *)data))
        {
            written_bytes += 4;
        }

        data += 4;

        num_block++;
    }

    return (written_bytes == size);
}
