// 2023/09/03 15:58:08 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Reader/Card.h"
#include "Hardware/HAL/HAL.h"
#include "Reader/Reader.h"
#include "Reader/Events.h"
#include "Reader/Signals.h"
#include "Reader/Messages.h"
#include "Modules/Indicator/Indicator.h"
#include "Nodes/OSDP/OSDP.h"
#include "Settings/AccessCards.h"
#include "Modules/CLRC66303HN/CommandsCLRC663.h"
#include "Settings/Settings.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

/*
+------+-------+--------------------------+
| ATQA |  SAK  |                          |
+------+-------+--------------------------+
| 0x04 |  0x08 | MF Classic 1k UID4       |
| 0x44 |  0x00 | NTAG213 NTAG215 NTAG216  |
+------+-------+--------------------------+
*/


using namespace CLRC66303HN;


namespace TypeCard
{
    enum E
    {
        Unidentified,   // Неизвестная карта
        Unsupported,    // Карта не поддерживается
        NTAG213,
        NTAG215,
        NTAG216,
        Mifare_UID4_1K,
        Count
    };

    static E current = Unidentified;

    static bool IsNTAG();
    static bool IsMifare();
    static E Current();

    pchar CurrentName()
    {
        static const pchar names[E::Count] =
        {
            "",
            "Unsupported",
            "NTAG213",
            "NTAG215",
            "NTAG216",
            "Mifare_UID7_4K"
        };

        return names[Current()];
    }

    namespace NTAG
    {
        static int BlockCFG0();
        static int BlockCFG1();
    };
}


namespace Card
{
    static bool inserted = false;
    // Результат предыдущей аутентификации. Смысл в том, что если предыдущая авторизация прошла
    // неудачно, то нужно давать сигнал, если текущая авторизация успешна
    static bool prev_auth_bool = false;
    UID             uid;
    BitSet16        atqa;
    uint8           sak;
    static TypeAuth prev_auth;
}


void Card::InsertExtendedMode(const TypeAuth &type, bool auth_ok, bool new_auth, uint64 number)
{
    if (prev_auth != type || !inserted || new_auth)
    {
        inserted = true;

        prev_auth = type;

        char password_string[64];

        std::sprintf(password_string, "%llu", type.password);

        if (auth_ok)
        {
            Event::CardReadOK(uid, number, type.enabled ? password_string : "DISABLED");
        }
        else
        {
            Event::CardReadFAIL(uid, type.enabled ? password_string : "DISABLED");
        }

        auth_ok ? Signals::GoodUserCard(false) : Signals::BadUserCard(false);
    }
}


void Card::InsertNormalModeUser(uint64 number, bool auth_ok)
{
    if (!IsInserted() || (auth_ok && !prev_auth_bool))
    {
        if (ModeOffline::IsEnabled())
        {
            if (auth_ok)
            {
                if(AccessCards::Access(number))
                {
                    Signals::GoodUserCard(true);

                    ModeOffline::OpenTheLock();
                }
                else
                {
                    Signals::BadUserCard(true);
                }
            }
        }
        else if (ModeWG::IsNormal())
        {
            if (auth_ok)
            {
                if (ModeOffline::IsEnabled())
                {
                    if (AccessCards::Access(number))
                    {
                        ModeOffline::OpenTheLock();
                    }
                    else
                    {
                        ModeOffline::EnableAlarm();
                    }
                }
                else
                {
                    OSDP::Card::Insert(number);

                    if (ModeReader::IsWG())
                    {
                        BitSet64 bs = { number };

                        HAL_USART::WG26::Transmit(bs.bytes[2], bs.bytes[1], bs.bytes[0]);
                    }
                    else
                    {
                        Message::SendFormat("CARD USER %s*%s NUMBER %llu", uid.ToString(true).c_str(), uid.ToString(false).c_str(), number);
                    }
                }
            }

            prev_auth_bool = auth_ok;
        }
        else
        {
            uint guid = uid.Get3Bytes();

            HAL_USART::WG26::Transmit((uint8)guid, (uint8)(guid >> 8), (uint8)(guid >> 16));
        }

        inserted = true;
    }
}


void Card::InsertNormalModeMaster(bool auth_ok)
{
    if (!IsInserted())
    {
        inserted = true;

        if (auth_ok)
        {
            Message::SendFormat("CARD MASTER");
        }
    }
}


void Card::Eject()
{
    TypeCard::Reset();

    if (IsInserted())
    {
        inserted = false;

        if (ModeReader::IsExtended())
        {
//            LOG_WRITE("---Card remove---");
            Event::CardRemove();
        }
    }
}


bool Card::IsInserted()
{
    return inserted;
}


int TypeCard::NTAG::BlockCFG0()
{
    static const int numbers[TypeCard::Count] =
    {
        0,
        0,
        41,     // NTAG213
        131,    // NTAG215
        227,    // NTAG216
        0       // Mifare
    };

    return numbers[TypeCard::Current()];
}


int TypeCard::NTAG::BlockCFG1()
{
    static const int numbers[TypeCard::Count] =
    {
        0,
        0,
        42,     // NTAG213
        132,    // NTAG215
        228,    // NTAG216
        0       // Mifare
    };

    return numbers[TypeCard::Current()];
}


int TypeCard::NTAG::BlockPassword()
{
    static const int numbers[TypeCard::Count] =
    {
        0,
        0,
        43,     // NTAG213
        133,    // NTAG215
        229,    // NTAG216
        0       // Mifare
    };

    return numbers[TypeCard::Current()];
}


TypeCard::E TypeCard::Current()
{
    return current;
}


bool TypeCard::IsNTAG()
{
    return (current == NTAG213) || (current == NTAG215) || (current == NTAG216);
}


bool TypeCard::IsMifare()
{
    return (current == Mifare_UID4_1K);
}


uint ConvertorUID::Get() const
{
    uint8 uc7ByteUID[7] =
    {
        (uint8)(uid >> 48),
        (uint8)(uid >> 40),
        (uint8)(uid >> 32),
        (uint8)(uid >> 24),
        (uint8)(uid >> 16),
        (uint8)(uid >> 8),
        (uint8)uid
    };


    BitSet32 nuid;

    Convert7ByteUIDTo4ByteNUID(uc7ByteUID, &nuid.bytes[0]);

    BitSet32 result;
    result.bytes[0] = nuid.bytes[2];
    result.bytes[1] = nuid.bytes[1];
    result.bytes[2] = nuid.bytes[0];
    result.bytes[3] = nuid.bytes[3];

    return result.word;
};


void ConvertorUID::Convert7ByteUIDTo4ByteNUID(uint8 *uc7ByteUID, uint8 *uc4ByteUID) const
{
    uint16 CRCPreset = 0x6363;
    uint16 CRCCalculated = 0x0000;

    ComputeCrc(CRCPreset, uc7ByteUID, 3, CRCCalculated);

    uc4ByteUID[0] = (uint8)((CRCCalculated >> 8) & 0xFF);   //MSB
    uc4ByteUID[1] = (uint8)(CRCCalculated & 0xFF);          //LSB
    CRCPreset = CRCCalculated;

    ComputeCrc(CRCPreset, uc7ByteUID + 3, 4, CRCCalculated);

    uc4ByteUID[2] = (uint8)((CRCCalculated >> 8) & 0xFF);   //MSB
    uc4ByteUID[3] = (uint8)(CRCCalculated & 0xFF);          //LSB
    uc4ByteUID[0] = (uint8)(uc4ByteUID[0] | 0x0F);
    uc4ByteUID[0] = (uint8)(uc4ByteUID[0] & 0xEF);
}


uint16 ConvertorUID::UpdateCrc(uint8 ch, uint16 *lpwCrc) const
{
    ch = (uint8)(ch ^ (uint8)((*lpwCrc) & 0x00FF));
    ch = (uint8)(ch ^ (ch << 4));
    *lpwCrc = (uint16)((*lpwCrc >> 8) ^ ((uint16)ch << 8) ^ ((uint16)ch << 3) ^ ((uint16)ch >> 4));

    return(*lpwCrc);
}


void ConvertorUID::ComputeCrc(uint16 wCrcPreset, uint8 *Data, int Length, uint16 &usCRC) const
{
    uint8 chBlock;

    do
    {
        chBlock = *Data++;
        UpdateCrc(chBlock, &wCrcPreset);
    } while (--Length);

    usCRC = wCrcPreset;
}


bool TypeCard::Detect()
{
    if (current != Unidentified)
    {
        return true;
    }

    {
        Block4 block;

        if (Command::NTAG::ReadBlock(3, block))
        {
            if (block[0] == 0xE1 && block[1] == 0x10)
            {
                uint8 byte_type = block[2];

                if (byte_type == 0x12)
                {
                    current = TypeCard::NTAG213;

                    return true;
                }
                else if (byte_type == 0x3E)
                {
                    current = TypeCard::NTAG215;

                    return true;
                }
                else if (byte_type == 0x6D)
                {
                    current = TypeCard::NTAG216;

                    return true;
                }
            }
        }
    }

    {
        Block16 block;

        if (Command::Mifare::ReadBlock(0, block))
        {
            block = block;
        }
    }

    current = Unsupported;

    return false;
}


void TypeCard::Reset()
{
    current = Unidentified;
}


bool Card::RAW::SetPassword(uint64 password)
{
    if (TypeCard::IsNTAG())
    {
        BitSet64 bs(password);

        return Command::NTAG::Write2Blocks(TypeCard::NTAG::BlockPassword(), bs.bytes);
    }
    else if (TypeCard::IsMifare())
    {
        return false;
    }

    return false;
}


bool Card::RAW::EnableCheckPassword()
{
    bool saccess = false;

    if (TypeCard::IsNTAG())
    {
        Block4 block;

        if (Command::NTAG::ReadBlock(TypeCard::NTAG::BlockCFG0(), block))
        {
            block[3] = 0x04;             // Защищаем доступ ко всем блокам, начиная с 4-го

            if (Command::NTAG::WriteBlock(TypeCard::NTAG::BlockCFG0(), block.bytes))
            {
                if (Command::NTAG::ReadBlock(TypeCard::NTAG::BlockCFG1(), block))
                {
                    block[0] = 0x80;     // Защита от чтения и от записи

                    if (Command::NTAG::WriteBlock(TypeCard::NTAG::BlockCFG1(), block.bytes))
                    {
                        saccess = true;
                    }
                }
            }
        }
    }
    else if (TypeCard::IsMifare())
    {

    }

    return saccess;
}


bool Card::RAW::ReadDataWitouthAuth()
{
    TypeCard::Detect();

    if (TypeCard::IsNTAG())
    {
        uint buffer = 0;

        return Command::NTAG::ReadBlock(TypeCard::NTAG::BlockCFG1(), &buffer);
    }
    else if (TypeCard::IsMifare())
    {
        return false;
    }

    return false;
}


bool Card::RAW::ReadNumber(uint64 *number)
{
    if (TypeCard::IsNTAG())
    {
        BitSet64 bit_set = 0;

        bool result = Command::NTAG::Read2Blocks(4, &bit_set);

        *number = bit_set.long_word;

        return result;
    }
    else if (TypeCard::IsMifare())
    {
        return false;
    }

    return false;
}


bool Card::RAW::WriteNumber(uint64 number)
{
    if (TypeCard::IsNTAG())
    {
        BitSet64 bit_set(number);

        return Command::NTAG::Write2Blocks(4, bit_set.bytes);
    }
    else if (TypeCard::IsMifare())
    {
        return false;
    }

    return false;
}


bool Card::RAW::WriteSettings(const SettingsMaster &set)
{
    if (TypeCard::IsNTAG())
    {
        return Command::NTAG::WriteData(4, &set, set.Size());
    }
    else if (TypeCard::IsMifare())
    {
        return false;
    }

    return false;
}


bool Card::RAW::WriteDataToBlocks(int num_first_block, uint8 *data, int num_bytes)
{
    if (TypeCard::IsNTAG())
    {
        return Command::NTAG::WriteData(num_first_block, data, num_bytes);
    }
    else if (TypeCard::IsMifare())
    {
        return false;
    }

    return false;
}


bool Card::RAW::ReadDataFromblocks(int num_first_block, uint8 *data, int num_bytes)
{
    if (TypeCard::IsNTAG())
    {
        return Command::NTAG::ReadData(num_first_block, data, num_bytes);
    }
    else if (TypeCard::IsMifare())
    {
        return false;
    }

    return false;
}


bool Card::RAW::ReadBitmapCards(char bitmap_cards[500])
{
    bitmap_cards[0] = '\0';

    int number_block = SettingsMaster::FIRST_BLOCK_BITMAP_CARDS;

    for(int counter = 0; counter < SettingsMaster::NUMBER_BLOCKS_BITMAP_CARDS; counter++)
    {
        Block4 block;

        if (Command::NTAG::ReadBlock(number_block++, block))
        {
            for (uint i = 0; i < 4; i++)
            {
                char byte[32];
                std::sprintf(byte, "%02X", block[i]);
                std::strcat(bitmap_cards, byte);
            }
        }
        else
        {
            return false;
        }
    }

    return true;
}


bool Card::RAW::WriteBitmapCards(char bitmap_cards[160])
{
    bool success = true;

    uint8 buffer[4];

    pchar pointer = bitmap_cards;

    int number_block = SettingsMaster::FIRST_BLOCK_BITMAP_CARDS;    // Номер блока - от 15 до 39
    int number_byte = 0;                                            // Номер байта - от 0 до 3
    const int last_block = SettingsMaster::FIRST_BLOCK_BITMAP_CARDS + SettingsMaster::NUMBER_BLOCKS_BITMAP_CARDS - 1;

    while (*pointer)
    {
        char data[32];

        char byte1 = *pointer++;
        char byte2 = *pointer++;

        std::sprintf(data, "%c%c", byte1, byte2);

        buffer[number_byte++] = (uint8)std::strtoull(data, nullptr, 16);

        if (number_byte == 4)
        {
            if (number_block > last_block)
            {
                return success;
            }

            if (!Command::NTAG::WriteBlock(number_block++, buffer))
            {
                success = false;
            }

            number_byte = 0;
        }
    }

    if (number_byte != 0)
    {
        while (number_byte < 4)
        {
            buffer[number_byte++] = 0x00;
        }

        if (number_block > last_block)
        {
            return success;
        }

        if (!Command::NTAG::WriteBlock(number_block++, buffer))
        {
            success = false;
        }

        number_byte = 0;
    }

    std::memset(buffer, 0, 4);

    while (number_block <= last_block)
    {
        if (!Command::NTAG::WriteBlock(number_block++, buffer))
        {
            success = false;
        }
    }

    return success;
}


String<> UID::ToString(bool full) const
{
    if (!calculated)
    {
        return String<>("NULL");
    }
    else
    {
        if (Is7BytesUID())
        {
            if (full)
            {
                return String<>("%06X%08X", (uint)(uid >> 32), (uint)(uid));
            }
            else
            {
                return String<>("%06X", Get3Bytes());
            }
        }
        else
        {
            if (full)
            {
                return String<>("%08X", (uint)uid);
            }
            else
            {
                return String<>("%06X", Get3Bytes());
            }
        }
    }
}


uint UID::Get3Bytes() const
{
    if (!calculated)
    {
        return 0;
    }

    if (Is7BytesUID())
    {
        return ConvertorUID(uid).Get() & 0xFFFFFF;
    }

    return ((uint)uid) & 0xFFFFFF;
}


void UID::Clear()
{
    std::memset(bytes, 0, 10);
    uid = 0;
    calculated = false;
}

void UID::Calculate()
{
    calculated = false;

    uid = 0;

    if (bytes[0] != 0)
    {
        if (Is7BytesUID())
        {
            uid = ((uint64)bytes[8]);
            uid |= (((uint64)bytes[7]) << 8);
            uid |= (((uint64)bytes[6]) << 16);
            uid |= (((uint64)bytes[5]) << 24);
            uid |= (((uint64)bytes[3]) << 32);
            uid |= (((uint64)bytes[2]) << 40);
            uid |= (((uint64)bytes[1]) << 48);
        }
        else
        {
            uid = ((uint64)bytes[3]);
            uid |= (((uint64)bytes[2]) << 8);
            uid |= (((uint64)bytes[1]) << 16);
            uid |= (((uint64)bytes[0]) << 24);
        }

        calculated = true;
    }
}
