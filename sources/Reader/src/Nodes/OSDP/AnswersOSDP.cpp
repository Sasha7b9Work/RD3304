// 2023/12/17 15:49:07 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Nodes/OSDP/OSDP.h"
#include "Nodes/OSDP/AnswersOSDP.h"
#include "Utils/Math.h"


uint8 AnswerOSDP::SQN = 0;


void AnswerOSDP::Transmit()
{
    HAL_USART::OSDP::Transmit(buffer.Data(), buffer.Size());
}


uint8 AnswerOSDP::GetControl()
{
    uint8 control = (uint8)(recv_control_byte & 3);

    _SET_BIT(control, 2);

    return control;
}


void AnswerOSDP::AppendUInt16(uint16 data)
{
    AppendByte((uint8)data);
    AppendByte((uint8)(data >> 8));
}


void AnswerOSDP::AppendByte(uint8 data)
{
    buffer.Append(data);
}


void AnswerOSDP::AppendThreeBytes(uint data)
{
    for (int i = 0; i < 3; i++)
    {
        AppendByte((uint8)data);
        data >>= 8;
    }
}


void AnswerOSDP::AppendUInt(uint data)
{
    for (int i = 0; i < 4; i++)
    {
        AppendByte((uint8)data);
        data >>= 8;
    }
}


void AnswerOSDP::WriteUInt16(int address, uint16 data)
{
    buffer[address] = (uint8)data;
    buffer[address + 1] = (uint8)(data >> 8);
}


void AnswerOSDP::CommonStart(uint8 address, OSDP_ANS::E command)
{
    AppendByte(OSDP_SOM);                   // Начало посылки
    AppendByte((uint8)(address | 0x80));   // Куда шлём

    AppendUInt16(0);                        // Резервируем место для размера

    AppendByte(GetControl());               // Добавляем управляющий байт

    AppendByte((uint8)command);             // Номер команды
}


void AnswerOSDP::CommonEnd()
{
    AppendUInt16(0);                // Резервируем место для CRC

    WriteUInt16(2, (uint16)buffer.Size());

    WriteUInt16(buffer.Size() - 2, Math::CalculateCRC(buffer.Data(), buffer.Size() - 2));
}


AnswerNumberCard::AnswerNumberCard(uint8 address, uint8 cntrl_code, uint number) : AnswerOSDP(cntrl_code)
{
    CommonStart(address, OSDP_ANS::RAW);

    AppendByte(1);               // Reader Number
    AppendByte(0);               // Format Code
    AppendUInt16(3 * 8);            // BitCount
    AppendThreeBytes(number);

    CommonEnd();
}


AnswerACK::AnswerACK(uint8 address, uint8 cntrl_code) : AnswerOSDP(cntrl_code)
{
    CommonStart(address, OSDP_ANS::ACK);

    CommonEnd();
}


AnswerNAK::AnswerNAK(uint8 address, uint8 cntrl_code, NAK::E nak) : AnswerOSDP(cntrl_code)
{
    CommonStart(address, OSDP_ANS::NAK);

    AppendByte((uint8)nak);

    CommonEnd();
}


AnswerPDID::AnswerPDID(uint8 address, uint8 cntrl_code, uint vendor, uint8 number, uint8 version, uint serial_number, uint firmware) : AnswerOSDP(cntrl_code)
{
    CommonStart(address, OSDP_ANS::PDID);

    AppendThreeBytes(vendor);
    AppendByte(number);
    AppendByte(version);
    AppendUInt(serial_number);
    AppendThreeBytes(firmware);

    CommonEnd();
}


AnswerCOM::AnswerCOM(uint8 address, uint8 cntrl_code) : AnswerOSDP(cntrl_code)
{
    CommonStart(address, OSDP_ANS::COM);

    AppendByte(gset.AddressOSDP());
    AppendUInt(gset.BaudRateOSDP().ToRAW());  

    CommonEnd();
}


AnswerCAP::AnswerCAP(uint8 address, uint8 cntrl_code) : AnswerOSDP(cntrl_code)
{
    /*
    *  0 : 
    *  1 : 
    *  2 : 
    */

    CommonStart(address, OSDP_ANS::PDCAP);

    AppendCode(4, 1, 1);        // LED Control
                                // 1 - the PD support on/off control only
                                // 2 - the PD supports timed commands
                                // 3 - like 02, plus bi-color LEDs
                                // 4 - like 02, plus tri-color LEDs

    AppendCode(5, 1, 1);        // Audible Output
                                // 1 - the PD support on/off control only
                                // 2 - the PD supports timed commands

    CommonEnd();
}


void AnswerCAP::AppendCode(uint8 code, uint8 compilance, uint8 number)
{
    AppendByte(code);           // Function Code
    AppendByte(compilance);     // compilance
    AppendByte(number);         // Number of
}


AnswerLSTAT::AnswerLSTAT(uint8 /*address*/, uint8 cntrl_code) : AnswerOSDP(cntrl_code)
{

}


AnswerRSTAT::AnswerRSTAT(uint8 /*address*/, uint8 cntrl_code) : AnswerOSDP(cntrl_code)
{

}
