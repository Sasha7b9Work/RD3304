// 2023/12/17 15:49:19 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Utils/Buffer.h"


struct AnswerOSDP
{
    AnswerOSDP(uint8 control_byte) : recv_control_byte(control_byte) { }
    void Transmit();

protected:

    Buffer<uint8, 256> buffer;

    static uint8 SQN;

    uint8 GetControl();

    // Общая стартовая последовательность
    void CommonStart(uint8 address, OSDP_ANS::E);

    // Общая завершающая последовательность
    void CommonEnd();

    // Добавить в конец буфера
    void AppendByte(uint8);
    void AppendUInt16(uint16);
    void AppendThreeBytes(uint);
    void AppendUInt(uint);
    void AppendUInt64(uint64);

    // Записать по определённому адресу в буфер
    void WriteUInt16(int address, uint16);

private:

    uint8 recv_control_byte;      // Принятый контрольный бит
};


struct AnswerACK : public AnswerOSDP
{
    AnswerACK(uint8 address, uint8 cntrl_code);
};


struct NAK
{
    enum E
    {
        _NONE,
        _ERROR_CRC_,                    // Message check character(s) error (bad cks/crc/mac[4])
        ERROR_COMMAND_LENGTH,           // Command length error
        COMMAND_NOT_IMPLEMENTED,        // Unknown Command Code – Command not implemented by PD
        ERROR_HEADER,                   // Unexpected sequence number detected in the header
        NOT_SUPPORTED_SECURRITY_BLOCK,  // This PD does not support the security block that was received
        REQUIRED_ENCRYPTED,             // Encrypted communication is required to process this command
        NOT_SUPPORTED_BIO_TYPE,         // BIO_TYPE not supported
        NOT_SUPPORTED_BIO_FORMAT        // BIO_FORMAT not supported
    };
};


struct AnswerNAK : public AnswerOSDP
{
    AnswerNAK(uint8 address, uint8 cntrl_code, NAK::E);
};


struct AnswerNumberCard : public AnswerOSDP
{
    AnswerNumberCard(uint8 address, uint8 cntrl_code, uint64 number);
};


struct AnswerPDID : public AnswerOSDP
{
    AnswerPDID(uint8 address, uint8 cntrl_code, uint vendor, uint8 number, uint8 version, uint serial_number, uint firmware);
};


struct AnswerCAP : public AnswerOSDP
{
    AnswerCAP(uint8 address, uint8 cntrl_code);

private:

    void AppendCode(uint8 code, uint8 compilance = 1, uint8 number = 1);
};


struct AnswerLSTAT : public AnswerOSDP
{
    AnswerLSTAT(uint8 address, uint8 cntrl_code);
};


struct AnswerRSTAT : public AnswerOSDP
{
    AnswerRSTAT(uint8 address, uint8 cntrl_code);
};


struct AnswerCOM : public AnswerOSDP
{
    AnswerCOM(uint8 address, uint8 cntrl_code);
};
