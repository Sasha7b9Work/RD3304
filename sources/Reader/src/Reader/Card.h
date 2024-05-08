// 2023/09/03 15:57:58 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Reader/Reader.h"
#include "Settings/Settings.h"
#include "Utils/String.h"


// Преобразует 7-байтный UID в 4-байтный NUID
struct ConvertorUID
{
    ConvertorUID(uint64 _uid) : uid(_uid) { }

    uint Get() const;

private:

    uint64 uid;

    void Convert7ByteUIDTo4ByteNUID(uint8 *uc7ByteUID, uint8 *uc4ByteUID) const;

    uint16 UpdateCrc(uint8 ch, uint16 *lpwCrc) const;

    void ComputeCrc(uint16 wCrcPreset, uint8 *Data, int Length, uint16 &usCRC) const;
};


struct UID
{
    UID() { Clear(); }

    void Clear();

    // Расчитать UID исходя из считнных с карты данных byte[0...9]
    void Calculate();

    // Возвращает true, если UID рассчитан
    bool Calcualted() const { return calculated; }

    // Первые 5 байт - 1 каскад, вторые 5 байт - второй каскад
    uint8 bytes[10];

    // Возвращает 3 байта для передачи по WG
    // Если короткий 4-байтный UID - то 3 из них. Если 7-байтный UID - то 3 байта NUID
    uint Get3Bytes() const;

    // Если full == true - полная форма (4 или 7 байт), если false - сокращённая - 3 байта
    String<> ToString(bool full) const;

private:

    bool calculated;

    // В самом мледшем байте хранится последний байт UID, таким образом спецификация %llu даёт правильный вывод
    uint64 uid;

    bool Is7BytesUID() const { return bytes[0] == 0x88; }
};


namespace TypeCard
{
    // Определить тип карты
    bool Detect();

    // Сброс типа карты при изьятии её из картоприёмника
    void Reset();

    namespace NTAG
    {
        int BlockPassword();
    }
}


namespace Card
{
    static const int SIZE_MEMORY = 4 * 45;

    extern UID uid;

    // Вставить в рабочем режиме
    void InsertNormalModeUser(uint number, bool auth_ok);
    void InsertNormalModeMaster(bool auth_ok);

    // Вставить в расширенном режиме
    void InsertExtendedMode(const TypeAuth &, bool auth_ok, bool new_auth, uint number);

    void Eject();

    bool IsInserted();

    // Далее функции, зависящие от типа карты - NTAG213, NTAG215, NTAG216, Milfare
    namespace RAW
    {
        // Записать пароль в предназначенные сектора
        bool SetPassword(uint64 password);

        // Включить проверку пароля
        bool EnableCheckPassword();

        // Читать данные из какого-либо блока на карте без защиты
        bool ReadDataWitouthAuth();

        // Читает номер пользовательсой карты
        bool ReadNumber(uint *);

        // Записывает номер пользовательской карты
        bool WriteNumber(uint);

        bool WriteSettings(const SettingsReader &);

        bool WriteDataToBlocks(int num_first_block, uint8 *data, int num_bytes);

        bool ReadDataFromblocks(int num_first_block, uint8 *data, int num_bytes);

        // Читать карту разрешённых карт из мастер-карты
        bool ReadBitmapCards(char bitmap_cards[500]);

        // Записать карту разрешённых карт в мастер-карту
        bool WriteBitmapCards(char bitmap_cards[160]);
    }
}
