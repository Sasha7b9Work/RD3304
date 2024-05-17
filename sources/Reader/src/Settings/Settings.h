// 2023/06/09 16:40:37 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Settings/SettingsTypes.h"


struct ProtectionBruteForce
{
    enum E
    {
        Enabled,
        Disabled
    };

    static void Set(E v)
    {
        current = v;
    }

    // При неподошедшем пароле вызываем эту функцию
    static void FailAttempt();

    // Пароль подошёл. Сбрасываем счётчик
    static void Reset()
    {
        count_fail = 0;
    }

    // Время ожидания до очередной попытки аутентификации
    static uint TimeWait();

private:

    static E current;
    static int count_fail;      // Количество неуспешных попыток
    static uint time_last_fail; // Время последний неудачной попытки
};


// Настройки мастер-карты.
// Они же - настройки считывателя
struct SettingsReader
{
    BitSet32 s04;   // Для признака мастер-карты
    BitSet32 s05;   // Старый пароль. Должен совпадать с установленным сейчас
    BitSet32 s06;   // на считывателе. 0xFF, если карта меняет только пароль
    BitSet32 s07;   // Новый пароль. Будет установлен на считыватель
    BitSet32 s08;   // после отработки мастер-карты
    BitSet32 s09;   // WG[0], oсдп-режим, громкости[1], мелодии[2...3]
    BitSet32 s10;   // Байт на номер, первый блок номера
    BitSet32 s11;   // "Красный" цвет
    BitSet32 s12;   // "Зелёный" цвет
    BitSet32 s13;   // Время блокировки замка, время тревоги, адрес OSDP, baud rate OSDP
    BitSet32 s14;   // Контрольная сумма (всех предыдущих значений)

    // Подготавливает структуру для записи мастер-карты установки только пароля
    void     PrepareMasterOnlyPassword(uint64 new_password);

    // Возвращает размер в байтах
    int      Size() const { return sizeof(*this); }
    uint64   OldPassword() const;
    uint64   Password() const;
    void     SetPassword(uint64);
    void     SetOldPassword(uint64);
    Color    ColorRed() const                  { return Color::FromUint(s11.word); }
    void     SetColorRed(const Color &color)   { s11.word = color.value; }
    Color    ColorGreen() const                { return Color::FromUint(s12.word); }
    void     SetColorGreen(const Color &color) { s12.word = color.value; }
    void     EnableOSDP();
    bool     IsEnabledOSDP() const;
    Weigand  GetWeigand() const;
    void     SetWeigand(Weigand w);
    uint8    Melody(TypeSound::E) const;
    void     SetMelody(TypeSound::E, uint8);
    uint8    Volume(TypeSound::E) const;
    void     SetVolume(TypeSound::E, uint8);
    uint8    SizeNumber() const              { return s10.bytes[0]; }
    void     SetSizeNumber(uint8 s)          { s10.bytes[0] = s; }
    uint8    BeginNumber() const             { return s10.bytes[1]; }
    void     SetBeginNumber(uint8 b)         { s10.bytes[1] = b; }
    uint     TimeLock() const                { return (uint)s13.bytes[0] * 1000; }
    void     SetTimeLock(uint8 t)            { s13.bytes[0] = t; }
    uint     TimeAlarm() const               { return (uint)s13.bytes[1] * 1000; }
    void     SetTimeAlarm(uint8 t)           { s13.bytes[1] = t; }
    bool     IsSecurityModeEnabled() const   { return (s10.bytes[2] & 2) != 0; }
    void     SetSecurityModeEnabled(bool en) { en ? (s10.bytes[2] |= 2) : (s10.bytes[2] &= ~2); }
    bool     IsOfflineModeAldowed() const    { return (s10.bytes[2] & 1) != 0; }                    // Разрешено использование в автономном режиме (номера разрешённых
    void     SetOfflineModeAllowed(bool en)  { en ? (s10.bytes[2] |= 1) : (s10.bytes[2] &= ~1); }   // карт хранятся в считывателе)
    uint8    AddressOSDP() const             { return s13.bytes[2]; }
    void     SetAddressOSDP(uint8 address)   { s13.bytes[2] = address; }
    BaudRate BaudRateOSDP() const;
    void     SetBaudRateOSDP(BaudRate::E b)  { s13.bytes[3] = (uint8)b; }

    uint     CalculateHash() const;         // Рассчитывет хэш (старая методология)
    uint     CalculateCRC32() const;        // Рассчитывает CRC32 (новая методология)
    uint    &Hash() { return s14.word; }    // Ссылка на ячейку с сохранённой контрольной суммой

    static void Load();

    static void Save();

    // Сброс настроек на заводские
    static void ResetToFactory();

    struct PSWD
    {
        // Возвращает текущий пароль
        static uint64 Get();

        // Установка нового пароля с мастер-карты
        static void Set(uint64);

        static uint64 GetFactory();
    };
};


extern SettingsReader gset;
