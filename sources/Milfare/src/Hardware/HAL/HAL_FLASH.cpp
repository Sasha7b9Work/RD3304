// 2022/7/15 9:04:36 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Modules/CLRC66303HN/RegistersCLRC663.h"
#ifdef MCU_GD
#else
    #include <stm32f1xx_hal.h>
#endif


namespace HAL_FLASH
{
    // Сохранить конфиг в память
    static void LoadConfigAntenna();
    static void LoadConfigProtocol();
}


void HAL_FLASH::LoadAntennaConfiguration106()
{
    LoadConfigAntenna();
}


void HAL_FLASH::LoadProtocol()
{
    LoadConfigProtocol();
}


void HAL_FLASH::LoadConfigAntenna()
{
    static const uint SIZE = 18;
    static const uint8 data[SIZE] =
    {
        0x86,    // DrvMode
        0x1F,    // TxAmp
        0x39,    // DrvCon
        0x0A,    // Txl
        0x18,    // TXCrcPreset
        0x18,    // RxCrcCon
        0x0F,    // TxDataNum
        0x21,    // TxModWidth
        0x00,    // TxSym10BurstLen
        0xC0,    // TxWaitCtrl
        0x12,    // TxWaitLo
        0xCF,    // TxFrameCon
        0x00,    // RsSofD
        0x04,    // RxCtrl
        0x90,    // RxWait
        0x5C,    // RxThreshold
        0x12,    // Rcv
        0x0A     // RxAna
    };

    uint8 address = 0x28;

    for(uint i = 0; i < SIZE; i++)
    {
        CLRC66303HN::Register(address++).Write(data[i]);
    }

//    CLRC66303HN::Register(0x28).Write(0x86);    // DrvMode
//    CLRC66303HN::Register(0x29).Write(0x1F);    // TxAmp
//    CLRC66303HN::Register(0x2A).Write(0x39);    // DrvCon
//    CLRC66303HN::Register(0x2B).Write(0x0A);    // Txl
//    CLRC66303HN::Register(0x2C).Write(0x18);    // TXCrcPreset
//    CLRC66303HN::Register(0x2D).Write(0x18);    // RxCrcCon
//    CLRC66303HN::Register(0x2E).Write(0x0F);    // TxDataNum
//    CLRC66303HN::Register(0x2F).Write(0x21);    // TxModWidth
//    CLRC66303HN::Register(0x30).Write(0x00);    // TxSym10BurstLen
//    CLRC66303HN::Register(0x31).Write(0xC0);    // TxWaitCtrl
//    CLRC66303HN::Register(0x32).Write(0x12);    // TxWaitLo
//    CLRC66303HN::Register(0x33).Write(0xCF);    // TxFrameCon
//    CLRC66303HN::Register(0x34).Write(0x00);    // RsSofD
//    CLRC66303HN::Register(0x35).Write(0x04);    // RxCtrl
//    CLRC66303HN::Register(0x36).Write(0x90);    // RxWait
//    CLRC66303HN::Register(0x37).Write(0x5C);    // RxThreshold
//    CLRC66303HN::Register(0x38).Write(0x12);    // Rcv
//    CLRC66303HN::Register(0x39).Write(0x0A);    // RxAna
}


void HAL_FLASH::LoadConfigProtocol()
{
    static const uint SIZE = 24;
    static const uint8 data[SIZE] =
    {
        0x20,    // TxBitMod     20
        0x00,    // RFU          00
        0x04,    // TxDataCon    04
        0x50,    // TxDataMod    50
        0x40,    // TxSymFreq    40
        0x00,    // TxSym0H      00
        0x00,    // TxSym0L      00
        0x00,    // TxSym1H      00
        0x00,    // TxSym1L      00
        0x00,    // TxSym2       00
        0x00,    // TxSym3       00
        0x00,    // TxSym10Len   00
        0x00,    // TxSym32Len   00
        0x00,    // TxSym10BurstCtrl 00
        0x00,    // TxSym10Mod   00
        0x50,    // TxSym32Mod   50
        0x02,    // RxBitMod     02
        0x00,    // RxEofSym     00
        0x00,    // RxSyncValH   00
        0x01,    // RxSyncVaIL   01
        0x00,    // RxSyncMod    00
        0x08,    // RxMod        08
        0x80,    // RxCorr       80
        0xB2     // FabCal       B2
    };

    uint8 address = 0x48;

    for(uint i = 0; i < SIZE; i++)
    {
        CLRC66303HN::Register(address++).Write(data[i]);
    }


//    CLRC66303HN::Register(0x48).Write(0x20);    // TxBitMod     20
//    CLRC66303HN::Register(0x49).Write(0x00);    // RFU          00
//    CLRC66303HN::Register(0x4A).Write(0x04);    // TxDataCon    04
//    CLRC66303HN::Register(0x4B).Write(0x50);    // TxDataMod    50
//    CLRC66303HN::Register(0x4C).Write(0x40);    // TxSymFreq    40
//    CLRC66303HN::Register(0x4D).Write(0x00);    // TxSym0H      00
//    CLRC66303HN::Register(0x4E).Write(0x00);    // TxSym0L      00
//    CLRC66303HN::Register(0x4F).Write(0x00);    // TxSym1H      00
//    CLRC66303HN::Register(0x50).Write(0x00);    // TxSym1L      00
//    CLRC66303HN::Register(0x51).Write(0x00);    // TxSym2       00
//    CLRC66303HN::Register(0x52).Write(0x00);    // TxSym3       00
//    CLRC66303HN::Register(0x53).Write(0x00);    // TxSym10Len   00
//    CLRC66303HN::Register(0x54).Write(0x00);    // TxSym32Len   00
//    CLRC66303HN::Register(0x55).Write(0x00);    // TxSym10BurstCtrl 00
//    CLRC66303HN::Register(0x56).Write(0x00);    // TxSym10Mod   00
//    CLRC66303HN::Register(0x57).Write(0x50);    // TxSym32Mod   50
//    CLRC66303HN::Register(0x58).Write(0x02);    // RxBitMod     02
//    CLRC66303HN::Register(0x59).Write(0x00);    // RxEofSym     00
//    CLRC66303HN::Register(0x5A).Write(0x00);    // RxSyncValH   00
//    CLRC66303HN::Register(0x5B).Write(0x01);    // RxSyncVaIL   01
//    CLRC66303HN::Register(0x5C).Write(0x00);    // RxSyncMod    00
//    CLRC66303HN::Register(0x5D).Write(0x08);    // RxMod        08
//    CLRC66303HN::Register(0x5E).Write(0x80);    // RxCorr       80
//    CLRC66303HN::Register(0x5F).Write(0xB2);    // FabCal       B2
}
