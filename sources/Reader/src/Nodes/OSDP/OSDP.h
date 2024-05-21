// 2023/12/15 14:44:07 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Nodes/Communicator.h"


/*
*   Примеры сообщений

*   osdp_POLL
*       0xFF
*       0x53    SOM
*       0x01    ADDRESS
*       0x08    LEN_LSB     LEN == 8
*       0x00    LEN_MSB
*       0x04    CTRL        CRC16
*       0x60    osdp_POLL
*       0xBA    CRC_LSB
*       0x00    CRC_MSB
*/



#define OSDP_SOM        0x53

#define OSDP_REQ_POLL   0x60
#define OSDP_REQ_ID     0x61
#define OSDP_REQ_CAP    0x62
#define OSDP_REQ_LSTAT  0x64
#define OSDP_REQ_RSTAT  0x67
#define OSDP_REQ_LED    0x69
#define OSDP_REQ_BUZ    0x6A
#define OSDP_REQ_COMSET 0x6E

struct OSDP_ANS
{
    enum E
    {
        ACK    = 0x40,
        NAK    = 0x41,
        PDID   = 0x45,
        PDCAP  = 0x46,
        LSTATR = 0x48,
        RSTATR = 0x4B,
        RAW    = 0x50,
        COM    = 0x54,
        BUSY   = 0x79
    };
};


namespace OSDP
{
    void Init();

    bool IsEnabled();

    void Update(BufferUSART &);

    namespace Card
    {
        void Insert(uint64 number);
    }
}
