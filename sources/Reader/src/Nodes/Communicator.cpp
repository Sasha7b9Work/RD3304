// 2023/08/29 19:55:32 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Nodes/Communicator.h"
#include "Hardware/HAL/HAL.h"
#include "Modules/LIS2DH12/LIS2DH12.h"
#include "Utils/StringUtils.h"
#include "Modules/CLRC66303HN/CLRC66303HN.h"
#include "Reader/Task/Task.h"
#include "Reader/Card.h"
#include "Nodes/Upgrader.h"
#include "Reader/Reader.h"
#include "Hardware/Timer.h"
#include "Reader/Messages.h"
#include "Modules/Memory/Memory.h"
#include "Modules/Indicator/Indicator.h"
#include "Reader/Events.h"
#include "Modules/Player/Player.h"
#include "Nodes/OSDP/OSDP.h"
#include "Device/Device.h"
#include "Settings/AccessCards.h"
#include "Modules/Player/TableSounds.h"
#include "Utils/Math.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>


using namespace std;
using namespace SU;


namespace Communicator
{
    // Нормальная работа
    static void UpdateNormal(BufferUSART &);

    // Закачка прошивки
    static void FuncUpdateFirmware(BufferUSART &);

    typedef void (*func)(BufferUSART &);

    static func funcUpdate = UpdateNormal;

    namespace FuncUpdate
    {
        static void Set(func _func)
        {
            funcUpdate = _func;
        }

        static void Restore()
        {
            funcUpdate = UpdateNormal;
        }
    }

    static bool Com_AUTH_REQ(BufferUSART &);
    static bool Com_AUTH(BufferUSART &);          // Команда действует в режимах Read и Write
    static bool Com_CONFIG_REQ(BufferUSART &);
    static bool Com_ERASE(BufferUSART &);
    static bool Com_ID(BufferUSART &);
    static bool Com_INFO_REQ(BufferUSART &);
    static bool Com_LED(BufferUSART &);
    static bool Com_MAKE(BufferUSART &);
    static bool Com_MODE_REQ(BufferUSART &);
    static bool Com_MODE(BufferUSART &);
    static bool Com_MSR(BufferUSART &);
    static bool Com_OSDP(BufferUSART &);
    static bool Com_PASSWORD(BufferUSART &);     // Команда делает то же, что и мастер-карта
    static bool Com_READER(BufferUSART &);
    static bool Com_READ(BufferUSART &);
    static bool Com_RESET(BufferUSART &);
    static bool Com_SOUND(BufferUSART &);
    static bool Com_UPDATE(BufferUSART &);
    static bool Com_VER(BufferUSART &);
    static bool Com_WRITE(BufferUSART &);

    typedef bool (*funcProcess)(BufferUSART &);

    struct Elem
    {
        Elem(pchar _key, funcProcess _func, uint8 vWG, uint8 vUART, uint8 vRead, uint8 vWrite) :
            key(_key), func(_func)
        {
            is_valid[ModeReader::WG] = vWG;
            is_valid[ModeReader::UART] = vUART;
            is_valid[ModeReader::Read] = vRead;
            is_valid[ModeReader::Write] = vWrite;
        }
        pchar       key;
        funcProcess func;
        uint8       is_valid[ModeReader::Count];  // 0 означает, что в данном режиме команда не обрабатывается

        bool NeedExecute() const
        {
            return  (is_valid[ModeReader::Current()] != 0);
        }
    };

    static const Elem handlers[] =
    {  //                                  WG UART Rd Wr
        Elem("#AUTH?",     Com_AUTH_REQ,   0, 0,   1, 1),
        Elem("#AUTH",      Com_AUTH,       0, 0,   1, 1),
        Elem("#CONFIG?",   Com_CONFIG_REQ, 1, 1,   1, 1),
        Elem("#ERASE",     Com_ERASE,      0, 1,   0, 1),
        Elem("#ID",        Com_ID,         1, 1,   1, 1),
        Elem("#INFO?",     Com_INFO_REQ,   1, 1,   1, 1),
        Elem("#LED",       Com_LED,        1, 1,   1, 1),
        Elem("#MAKE",      Com_MAKE,       0, 0,   0, 1),
        Elem("#MODE?",     Com_MODE_REQ,   1, 1,   1, 1),
        Elem("#MODE",      Com_MODE,       1, 1,   1, 1),
        Elem("#MSR",       Com_MSR,        1, 1,   1, 1),
        Elem("#OSDP",      Com_OSDP,       1, 1,   1, 1),
        Elem("#PASSWORD",  Com_PASSWORD,   1, 1,   1, 1),
        Elem("#READER",    Com_READER,     1, 1,   1, 1),
        Elem("#READ",      Com_READ,       0, 0,   1, 1),
        Elem("#RESET",     Com_RESET,      1, 1,   1, 1),
        Elem("#SOUND",     Com_SOUND,      1, 1,   1, 1),
        Elem("#UPDATE",    Com_UPDATE,     1, 1,   1, 1),
        Elem("#VER",       Com_VER,        1, 1,   1, 1),
        Elem("#WRITE",     Com_WRITE,      0, 0,   0, 1),
        Elem(nullptr,      nullptr,        1, 0,   0, 0)
    };
}


void Communicator::Update(BufferUSART &buffer)
{
    funcUpdate(buffer);
}


void Communicator::UpdateNormal(BufferUSART &buffer)
{
    if (buffer.Size())
    {
        if (OSDP::IsEnabled())
        {
            OSDP::Update(buffer);
        }
        else
        {
            while (true)
            {
                int pos_end = buffer.Position("\r", 1);

                if (pos_end >= 0)
                {
                    buffer[pos_end] = '\0';

                    const Elem *elem = &handlers[0];

                    bool processed = false;

                    do
                    {
                        if (buffer.FirstBytesIs(elem->key))
                        {
                            processed = elem->NeedExecute() ? elem->func(buffer) : true;

                            break;
                        }
                        elem++;

                    } while (elem->key);

                    if (!processed)
                    {
                        Message::SendFormat("#ERROR COMMAND \"%s\" %d bytes", (const char *)buffer.Data(), buffer.Size());
                    }

                    buffer.RemoveFirst(pos_end + 1);
                }
                else
                {
                    break;
                }
            }
        }
    }
}


bool Communicator::Com_AUTH(BufferUSART &buffer)
{
    bool result = true;

    if (buffer.WordIs(2, "DISABLE"))
    {
        Reader::SetAuth(TypeAuth(false, 0));
    }
    else
    {
        uint64 pass = 0;

        if (buffer.GetUint64(2, &pass))
        {
            Reader::SetAuth(TypeAuth(true, pass));
        }
        else
        {
            result = false;
        }
    }

    Com_AUTH_REQ(buffer);

    return result;
}


bool Communicator::Com_AUTH_REQ(BufferUSART &)
{
    TypeAuth type = Reader::GetAuth();

    if (type.enabled)
    {
        Message::SendFormat("AUTH IS %llu", type.password);
    }
    else
    {
        Message::Send("AUTH IS DISABLED");
    }

    return true;
}


bool Communicator::Com_MAKE(BufferUSART &buffer)
{
    bool result = false;
    uint64 new_pass = 0;

    int num_words = buffer.CountWords();

    if (buffer.WordIs(2, "MASTER"))
    {
        if (num_words == 5)
        {
            uint64 old_pass = 0;

            if (buffer.GetUint64(3, &old_pass) && buffer.GetUint64(4, &new_pass) && buffer.Crc32IsMatches(num_words))
            {
                Task::MakeMaster::Create(old_pass, new_pass);

                result = true;
            }
        }
        else if (num_words == 26)
        {
            SettingsMaster set;

            if (buffer.ReadSettings(num_words, set))
            {
                char word[512];         // \todo Если битовая карта займёт более 511 байт, то не будет работать

                Task::MakeMaster::Create(set, SU::GetString((pchar)buffer.Data(), 25, word));

                result = true;
            }
        }
    }
    else if (buffer.WordIs(2, "USER"))
    {
        if (num_words == 5)
        {
            uint number = 0;

            if (buffer.GetUint64(3, &new_pass) && buffer.GetUint(4, &number) && buffer.Crc32IsMatches(num_words))
            {
                Task::MakeUser::Create(new_pass, number);

                result = true;
            }
        }
        else if (num_words == 4)
        {
            if (buffer.GetUint64(3, &new_pass) && buffer.Crc32IsMatches(num_words))
            {
                Task::MakeUser::Create(new_pass);

                result = true;
            }
        }
    }

    return result;
}


bool BufferUSART::ReadSettings(int num_words, SettingsMaster &set) const
{
    uint64 old_pass = 0;
    uint64 new_pass = 0;
    int weigand = -1;
    uint uint_red = 0;
    uint uint_green = 0;
    int melody0 = 0;
    int melody1 = 0;
    int melody2 = 0;
    int volume0 = 0;
    int volume1 = 0;
    int volume2 = 0;
    int size_number = 0;
    int begin_number = 0;
    int time_lock = 0;
    int time_alarm = 0;
    int security_mode_enabled = 0;
    int offline_mode_allowed = 0;
    int osdp_address = 0;
    int osdp_enabled = 0;
    uint osdp_bautdrate = 0;
    int antibreak_sens = 0;
    uint antibreak_number = 0;

    if (GetUint64(3, &old_pass) &&
        GetUint64(4, &new_pass) &&
        GetInt(5, (int *)&weigand) &&
        GetUIntFromHEX(6, &uint_red) &&
        GetUIntFromHEX(7, &uint_green) &&
        GetInt(8, &melody0) &&
        GetInt(9, &melody1) &&
        GetInt(10, &melody2) &&
        GetInt(11, &volume0) &&
        GetInt(12, &volume1) &&
        GetInt(13, &volume2) &&
        GetInt(14, &size_number) &&
        GetInt(15, &begin_number) &&
        GetInt(16, &time_lock) &&
        GetInt(17, &time_alarm) &&
        GetInt(18, &security_mode_enabled) &&
        GetInt(19, &offline_mode_allowed) &&
        GetInt(20, &osdp_address) &&
        GetUint(21, &osdp_bautdrate) &&
        GetInt(22, &osdp_enabled) &&
        GetInt(23, &antibreak_sens) &&
        GetUint(24, &antibreak_number) &&
        Crc32IsMatches(num_words))
    {
        set.SetOldPassword(old_pass);
        set.SetPassword(new_pass);
        set.SetWeigand(Weigand((Weigand::E)weigand));
        set.SetColorRed(Color::FromUint(uint_red));
        set.SetColorGreen(Color::FromUint(uint_green));
        set.SetMelody(TypeSound::Beep, (uint8)melody0);
        set.SetMelody(TypeSound::Green, (uint8)melody1);
        set.SetMelody(TypeSound::Red, (uint8)melody2);
        set.SetVolume(TypeSound::Beep, (uint8)volume0);
        set.SetVolume(TypeSound::Green, (uint8)volume1);
        set.SetVolume(TypeSound::Red, (uint8)volume2);
        set.SetSizeNumber((uint8)size_number);
        set.SetBeginNumber((uint8)begin_number);
        set.SetTimeLock((uint8)time_lock);
        set.SetTimeAlarm((uint8)time_alarm);
        set.SetSecurityModeEnabled(security_mode_enabled != 0);
        set.SetOfflineModeAllowed(offline_mode_allowed != 0);
        set.SetAddressOSDP((uint8)osdp_address);
        set.SetBaudRateOSDP(BaudRate::FromUInt(osdp_bautdrate));
        set.SetAntibreakSens((uint8)antibreak_sens);
        set.SetAntibreakNumber(antibreak_number);
        if (osdp_enabled != 0)
        {
            set.EnableOSDP();
        }

        return true;
    }

    return false;
}


bool BufferUSART::Crc32IsMatches(int num_words) const
{
    uint crc = 0;

    if (GetUIntFromHEX(num_words, &crc))
    {
        pchar end = SU::PointerWord((pchar)Data(), num_words);

        return Math::CalculateCRC32(Data(), end - (pchar)Data() - 1) == crc;
    }

    return false;
}


bool Communicator::Com_MODE(BufferUSART &buffer)
{
    bool result = true;

    if (buffer.WordIs(2, "WEIGAND"))
    {
        ModeReader::Set(ModeReader::WG);
    }
    else if (buffer.WordIs(2, "UART"))
    {
        ModeReader::Set(ModeReader::UART);
    }
    else if (buffer.WordIs(2, "READ"))
    {
        ModeReader::Set(ModeReader::Read);
    }
    else if (buffer.WordIs(2, "WRITE"))
    {
        ModeReader::Set(ModeReader::Write);
    }
    else if (buffer.WordIs(2, "OSDP"))
    {
        OSDP::Init();
    }
    else
    {
        result = false;
    }

    if (result)
    {
        Message::OK();
    }

    return result;
}


bool Communicator::Com_ERASE(BufferUSART &buffer)
{
    bool result = true;

    if (buffer.WordIs(2, "FULL"))
    {
        Memory::Erase::Full();
    }
    else
    {
        int num_block = 0;

        if (buffer.GetInt(2, &num_block) && num_block >= 0 && num_block < (int)(Memory::Capasity() / Memory::SIZE_BLOCK))
        {
            Memory::Erase::Block(num_block);

            Message::OK();
        }
        else
        {
            result = false;
        }
    }

    return result;
}


bool Communicator::Com_RESET(BufferUSART &)
{
    SettingsMaster::ResetToFactory();

    Message::OK();

    return true;
}


bool Communicator::Com_MSR(BufferUSART &)
{
    double x = (double)LIS2DH12::GetAccelerationX();
    double y = (double)LIS2DH12::GetAccelerationY();
    double z = (double)LIS2DH12::GetAccelerationZ();

    double full = std::sqrt(x * x + y * y + z * z);

    Message::SendFormat("OK;MEMORY:%s;%3.1fV;%3.2fg;%3.2fg;%3.2fg;%3.1fC %f",
        Memory::Test::Run() ? "OK" : "FAIL",
        (double)HAL_ADC::GetVoltage(),
        x,
        y,
        z,
        (double)LIS2DH12::GetRawTemperature().ToTemperatrue(),
        full
    );

    return true;
}


bool Communicator::Com_UPDATE(BufferUSART &buffer)
{
    bool result = true;

    int size = 0;

    if (buffer.GetInt(2, &size))
    {
        Memory::Erase::Firmware();

        Upgrader::SetParameters(0, size);

        Indicator::TurnOn(Color(0x008000, 1.0f), 0, true);

        FuncUpdate::Set(FuncUpdateFirmware);
    }
    else
    {
        result = false;
    }

    return result;
}


bool Communicator::Com_SOUND(BufferUSART &buffer)
{
    if (buffer.WordIs(2, "LOAD"))
    {
        int number = 0;
        int size = 0;

        if (buffer.GetInt(3, &number) && buffer.GetInt(4, &size))
        {
            if (number == 0)                                                        // При получении первого звука стираем всю память
            {                                                                       // Звуки всегда идут по порядку
                Memory::Erase::Sounds();
            }

            Memory::WriteInt((uint)(TableSounds::Begin() + number), size);

            Upgrader::SetParameters(TableSounds::AddressSound(number), size);

            FuncUpdate::Set(FuncUpdateFirmware);
        }
        else
        {
            return false;
        }
    }
    else if (buffer.WordIs(2, "PLAY"))
    {
        int number = 0;
        int volume = 0;

        if (buffer.GetInt(3, &number) && buffer.GetInt(4, &volume) && number < NUMBER_SOUDNS && volume < 4)
        {
            Player::PlayFromMemory((uint8)number, (uint8)volume);
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}


bool Communicator::Com_INFO_REQ(BufferUSART &)
{
    Message::SendFormat(
        "INFO HW=%s SN=%s VH=%s VB=%s VS=%s DT=%S INFO=%s OSDP_ADDR=%d OSDP_BAUD=%d",
        Device::Info::Firmware::TypeID_ASCII().c_str(),
        Device::Info::Hardware::SerialNumber_ASCII_HEX().c_str(),
        Device::Info::Hardware::Version_ASCII_HEX().c_str(),
        Device::Info::Loader::Version_ASCII_HEX().c_str(),
        Device::Info::Firmware::Version_ASCII_DEC().c_str(),
        Device::Info::Hardware::DateManufactured_ASCII().c_str(),
        Device::Info::Firmware::Info_ASCII().c_str(),
        gset.AddressOSDP(),
        gset.BaudRateOSDP().ToRAW()
    );

    return true;
}


bool Communicator::Com_WRITE(BufferUSART &buffer)
{
    bool result = true;

    uint8 data[180];

    int num_block = 0;

    if (buffer.GetInt(2, &num_block))
    {
        int num_word = 3;

        ParseBuffer word;

        GetWord(buffer.DataChar(), num_word++, &word);

        int num_bytes = 0;

        while (word.data[0])
        {
            data[num_bytes++] = (uint8)strtoull(word.data, nullptr, 16);

            GetWord(buffer.DataChar(), num_word++, &word);
        }

        Task::Write::Create(num_block, data, num_bytes);
    }
    else
    {
        result = false;
    }

    return result;
}


bool Communicator::Com_READ(BufferUSART &buffer)
{
    bool result = true;

    int num_block = 0;
    int num_bytes = 0;

    if (buffer.GetInt(2, &num_block) && buffer.GetInt(3, &num_bytes))
    {
        Task::Read::Create(num_block, num_bytes);
    }
    else
    {
        result = false;
    }

    return result;
}


bool Communicator::Com_CONFIG_REQ(BufferUSART &)
{
    WriteConfitToUSART();

    return true;
}


void Communicator::WriteConfitToUSART()
{
    Message::SendFormat(
        "CONFIG"
        " VER=%d"
        " PASSWORD=%llu"
        " RED=%08X"
        " GREEN=%08X"
        " WEIGAND=%d"
        " MELODY_BEEP=%d/%d"
        " MELODY_GREEN=%d/%d"
        " MELODY_RED=%d/%d"
        " NUMBER_SIZE=%d"
        " NUMBER_BEGIN=%d"
        " TIME_LOCK=%d"
        " TIME_ALARM=%d"
        " OFFLINE_MODE=%d"
        " SECURITY_MODE=%d"
        " OSDP_ADDRESS=%d"
        " OSDP_BAUDRATE=%d"
        " OSDP_ENABLED=%d"
        " ANTIBREAK_SENS=%d"
        " ANTIBREAK_NUMBER=%d",
        VERSION,
        SettingsMaster::PSWD::Get(),
        gset.ColorRed().value,
        gset.ColorGreen().value,
        gset.GetWeigand().ToRAW(),
        gset.Melody(TypeSound::Beep) + 1, gset.Volume(TypeSound::Beep),
        gset.Melody(TypeSound::Green) + 1, gset.Volume(TypeSound::Green),
        gset.Melody(TypeSound::Red) + 1, gset.Volume(TypeSound::Red),
        gset.SizeNumber(),
        gset.BeginNumber(),
        gset.TimeLock() / 1000,
        gset.TimeAlarm() / 1000,
        gset.IsOfflineModeAldowed() ? 1 : 0,
        gset.IsSecurityModeEnabled() ? 1 : 0,
        gset.AddressOSDP(),
        gset.BaudRateOSDP().ToRAW(),
        gset.IsEnabledOSDP() ? 1 : 0,
        gset.GetAntibreakSensRAW(),
        gset.GetAntibreakNumber()
    );
}


void Communicator::FuncUpdateFirmware(BufferUSART &buffer)
{
    Upgrader::Update(buffer);

    if (Upgrader::IsCompleted())
    {
        FuncUpdate::Restore();
    }
}


bool Communicator::Com_VER(BufferUSART &)
{
    Message::NotImplemented();

    return true;
}


bool Communicator::Com_ID(BufferUSART &)
{
    Message::NotImplemented();

    return true;
}


bool Communicator::Com_READER(BufferUSART &)
{
    Message::NotImplemented();

    return true;
}


bool Communicator::Com_OSDP(BufferUSART &)
{
    Message::NotImplemented();

    return true;
}


bool Communicator::Com_PASSWORD(BufferUSART &buffer)
{
    bool result = true;

    if (buffer.CountWords() == 2)
    {
        uint64 pass = 0;

        result = buffer.GetUint64(2, &pass);

        if (result)
        {
            SettingsMaster::PSWD::Set(pass);

            Message::SendFormat("PASSWORD IS %llu", SettingsMaster::PSWD::Get());
        }
    }
    else
    {
        result = false;
    }

    return result;
}


bool Communicator::Com_LED(BufferUSART &buffer)
{
    struct TypeColor
    {
        enum E
        {
            _None,
            Red,
            Green
        };
    };

    static TypeColor::E type = TypeColor::_None;

    if (buffer.WordIs(2, "RED"))
    {
        ModeIndicator::Set(ModeIndicator::Internal);

        type = TypeColor::Red;

        Indicator::TurnOn(gset.ColorRed(), 0, true);

        Message::OK();
    }
    else if (buffer.WordIs(2, "GREEN"))
    {
        ModeIndicator::Set(ModeIndicator::Internal);

        type = TypeColor::Green;

        Indicator::TurnOn(gset.ColorGreen(), 0, true);

        Message::OK();
    }
    else if (buffer.WordIs(2, "OFF"))
    {
        ModeIndicator::Set(ModeIndicator::External);

        type = TypeColor::_None;

        Indicator::TurnOff(0, true);

        Message::OK();
    }
    else if (buffer.CountWords() == 3)
    {
        ModeIndicator::Set(ModeIndicator::Internal);

        uint color_red = 0;
        uint color_green = 0;

        buffer.GetUIntFromHEX(2, &color_red);
        buffer.GetUIntFromHEX(3, &color_green);

        gset.SetColorRed(Color::FromUint(color_red));

        gset.SetColorGreen(Color::FromUint(color_green));

        SettingsMaster::Save();

        if (Indicator::IsFired())
        {
            if (type == TypeColor::Red)
            {
                Indicator::TurnOn(gset.ColorRed(), 0, true);
            }
            else if (type == TypeColor::Green)
            {
                Indicator::TurnOn(gset.ColorGreen(), 0, true);
            }
        }
        else
        {
            Indicator::TurnOff(0, true);
            Indicator::Blink(gset.ColorGreen(), gset.ColorRed(), 500, false);
        }

        HAL_USART::UART::TransmitF("RED=%08X GREEN=%08X", gset.ColorRed().value, gset.ColorGreen().value);
    }
    else
    {
        return false;
    }

    return true;
}


bool Communicator::Com_MODE_REQ(BufferUSART &)
{
    Message::Send(ModeReader::Name(ModeReader::Current()));

    return true;
}


bool BufferUSART::FirstBytesIs(pchar symbols) const
{
    return std::memcmp(Data(), symbols, std::strlen(symbols)) == 0;
}


bool BufferUSART::WordIs(int num_word, pchar string) const
{
    ParseBuffer word;

    GetWord(DataChar(), num_word, &word);

    return strcmp(word.data, string) == 0;
}


int BufferUSART::CountWords() const
{
    if (size == 0 || buffer[0] == '\0')
    {
        return 0;
    }

    int num_words = 0;

    const uint8 *pointer = buffer;

    while (true)
    {
        if (*pointer == '\0')
        {
            num_words++;
            break;
        }
        else if (*pointer == ' ')
        {
            num_words++;

            while (*pointer == ' ')
            {
                pointer++;

                if (*pointer == '\0')
                {
                    break;
                }
            }
        }

        pointer++;
    }

    return num_words;
}


bool BufferUSART::GetInt(int num_word, int *value) const
{
    ParseBuffer word;

    GetWord(DataChar(), num_word, &word);

    return SU::AtoInt(word.data, value);
}


bool BufferUSART::GetUint(int num_word, uint *value) const
{
    ParseBuffer word;

    GetWord(DataChar(), num_word, &word);

    return SU::AtoUInt(word.data, value);
}


bool BufferUSART::GetUint64(int num_word, uint64 *value) const
{
    ParseBuffer word;

    GetWord(DataChar(), num_word, &word);

    return SU::AtoUInt64(word.data, value);
}


bool BufferUSART::GetUIntFromHEX(int num_word, uint *value) const
{
    ParseBuffer word;

    GetWord(DataChar(), num_word, &word);

    *value = (uint)strtoull(word.data, nullptr, 16);

    return true;
}
