// 2023/06/09 16:40:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Settings/Settings.h"
#include "Hardware/HAL/HAL.h"
#include "Modules/CLRC66303HN/CLRC66303HN.h"
#include "Hardware/Timer.h"
#include "Modules/Indicator/Indicator.h"
#include "Utils/StringUtils.h"
#include <cstring>
#include <cmath>

ProtectionBruteForce::E ProtectionBruteForce::current = ProtectionBruteForce::Enabled;
int ProtectionBruteForce::count_fail = 0;
uint ProtectionBruteForce::time_last_fail = 0;

    
static const SettingsReader factory_set =
{
    { 0xFFFFFFFF },       // s04
    { 0x00000000 },       // s05 Заводской
    { 0x00000000 },       // s06 пароль
    { 0x00000000 },       // s07 Новый
    { 0x00000000 },       // s08 пароль
    { 0x02103F00 },       // s09 WG VV ML MH
    { 0x00000404 },       // s10
    { 0xFFFF0000 },       // s11
    { 0xFF00FF00 },       // s12
    { 0x00011e05 },       // s13
    { 0x00000000 }        // s14
};

SettingsReader gset = factory_set;


void SettingsReader::Load()
{
    HAL_ROM::Load(gset);
}


void SettingsReader::Save()
{
    SettingsReader settings;

    HAL_ROM::Load(settings);

    if (std::memcmp(&settings, &gset, (uint)settings.Size()) != 0)
    {
        gset.Hash() = gset.CalculateHash();

        HAL_ROM::Save(gset);
    }
}


void SettingsReader::PSWD::Set(uint64 password)
{
    gset.SetPassword(password);

    Save();
}


void SettingsReader::ResetToFactory()
{
    gset = factory_set;

    Save();

    for (int i = 0; i < 3; i++)
    {
        Indicator::Blink(Color(0xFF00FF, 1.0f), Color(0, 0), 500, false);
    }
}


Color Color::FromUint(uint value)
{
    return Color(value & 0x00FFFFFF, (uint8)(value >> 24) / 255.0f);
}


uint ProtectionBruteForce::TimeWait()
{
    if (TIME_MS - time_last_fail > 60000)
    {
        Reset();
    }

    const int d = 10;

    if (count_fail < d)
    {
        return 0;
    }

    int count = count_fail - d;

    uint time = 500;

    while (count >= 0)
    {
        time *= 2;
        count -= d;
    }

    return time;
}


void ProtectionBruteForce::FailAttempt()
{
    time_last_fail = TIME_MS;
    count_fail++;
}


BaudRate SettingsReader::BaudRateOSDP() const
{
    return BaudRate((BaudRate::E)s13.bytes[3]);
}


uint BaudRate::ToRAW() const
{
    static const uint raws[Count] =
    {
        9600,
        19200,
        38400
    };

    return raws[value];
}


BaudRate::E BaudRate::FromUInt(uint raw)
{
    if (raw == 9600)
    {
        return _9600;
    }
    else if (raw == 19200)
    {
        return _19200;
    }
    else if (raw == 38400)
    {
        return _38400;
    }

    return BaudRate::_9600;
}


uint8 SettingsReader::Melody(TypeSound::E type) const
{
    int shift = (int)type * 4;

    return (uint8)((s09.half_word[1] >> shift) & 0x0F);
}


uint8 SettingsReader::Volume(TypeSound::E type) const
{
    int shift = (int)type * 2;

    return (uint8)((s09.bytes[1] >> shift) & 0x03);
}


void SettingsReader::SetMelody(TypeSound::E type, uint8 num)
{
    uint16 half_word = s09.half_word[1];

    half_word &= ~(0x0F << (type * 4));

    half_word |= (num << (type * 4));

    s09.half_word[1] = half_word;
}


void SettingsReader::SetVolume(TypeSound::E type, uint8 num)
{
    uint8 byte = s09.bytes[1];

    byte &= ~(0x03 << (type * 2));

    byte |= (num << (type * 2));

    s09.bytes[1] = byte;
}


void SettingsReader::SetPassword(uint64 password)
{
    BitSet64 bs(password);

    s07.word = bs.word[0];
    s08.word = bs.word[1];
}


uint64 SettingsReader::Password() const
{
    BitSet64 password;

    password.word[0] = s07.word;
    password.word[1] = s08.word;

    return password.long_word;
}


void SettingsReader::SetOldPassword(uint64 password)
{
    BitSet64 bs(password);

    s05.word = bs.word[0];
    s06.word = bs.word[1];
}


uint64 SettingsReader::OldPassword() const
{
    BitSet64 password;

    password.word[0] = s05.word;
    password.word[1] = s06.word;

    return password.long_word;
}


int Weigand::ToRAW() const
{
    static const int raws[Count] =
    {
        26,
        33,
        34,
        37,
        40,
        42
    };

    return raws[value];
}


void SettingsReader::PrepareMasterOnlyPassword(uint64 new_password)
{
    std::memset(this, 0xFF, (uint)Size());

    SetPassword(new_password);

    Hash() = CalculateHash();
}


uint SettingsReader::CalculateHash() const
{
    const uint8 *begin = (const uint8 *)this;
    const uint8 *end = (const uint8 *)&s14;
    int size = end - begin;

    return SU::CalculateHash(begin, size);
}


uint64 SettingsReader::PSWD::Get()
{
    return gset.Password();
}


uint64 SettingsReader::PSWD::GetFactory()
{
    return factory_set.OldPassword();
}


Weigand SettingsReader::GetWeigand() const
{
    return Weigand((Weigand::E)(s09.bytes[0] & 0x7f));
}


void SettingsReader::SetWeigand(Weigand w)
{
    bool enabled_OSDP = IsEnabledOSDP();

    s09.bytes[0] = (uint8)w.value;

    if (enabled_OSDP)
    {
        EnableOSDP();
    }
}


void SettingsReader::EnableOSDP()
{
    s09.bytes[0] |= 0x80;
}


bool SettingsReader::IsEnabledOSDP() const
{
    return (s09.bytes[0] & 0x80) != 0;
}
