// 2023/12/18 16:19:58 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device/Device.h"


// Идентификатор программы.
// Заносится в исходный текст программы на этапе ее написания.
// Это число используется загрузчиком.Для СКЕ это значение равно EC010000h.
static const uint PROGRAM_SIGNATURE __attribute__((used)) __attribute__((section(".ARM.__at_0x08001200"))) = 0x55443300;

// Версия программы. 32х разрядное число. Заносится в
// исходный текст программы на этапе ее написания и сборки.
// Это число используется загрузчиком.
static const uint PROGRAM_VERSION __attribute__((used)) __attribute__((section(".ARM.__at_0x08001204"))) = VERSION;

// Длина программы в байтах. Длину определяет t4crypt исходя из характеристик hex файла и заносит по
// этому адресу перед кодированием.
static const uint PROGRAM_LENGTH __attribute__((used)) __attribute__((section(".ARM.__at_0x08001208"))) = 0xFFFFFFFF;

// Контрольная сумма программы.Рассчитывается приложением t4crypt по алгоритму CRC32 с полином
// EDB88320h и заносится по данному адресу при кодировании.Это значение будет использоваться загрузчиком
// для определения целостности основной программы перед каждым запуском.
static const uint PROGRAM_CRC __attribute__((used)) __attribute__((section(".ARM.__at_0x0800120C"))) = 0x12345678;

// Дата обновления программы. Дата и час(GMT), когда было произведено обновление программы.
// Выставляется загрузчиком на основании времени полученного от программы загрузки. 32х разрядное число.
// Последовательность байт прямая(little - endian).Старшие 8 бит это год считая от 2000. Биты с 16 по 23 месяц.
// Биты с 8 по 15 день месяца.Биты с 0 по 7 это час по гринвичу.
static const uint DATA_UPDATE __attribute__((used)) __attribute__((section(".ARM.__at_0x08001120"))) = 0xFFFFFFFF;

// Текстовое название программы.
// Это 32х битный адрес ASCIIZ строки c символьным представлением типа и версии программы.
// Получается автоматически в результате работы компоновщика(linker).
//    static const char * const PROGRAM_NAME_ASCII = "NAME""VERSION";
#define CREATE_STRING_PROGRAM(name, ver)    #name#ver
static const char *const PROGRAM_NAME __attribute__((used)) __attribute__((section(".ARM.__at_0x08001124"))) = CREATE_STRING_PROGRAM(NAME, VERSION);


uint Device::Info::Hardware::Vendor()
{
//    uint result = PROGRAM_SIGNATURE;
//
//    result = 0x4B444F53;

    return 0x4B444F53;
}


uint8 Device::Info::Hardware::ManufacturedModelNumber()
{
    return 0x41;
}


uint8 Device::Info::Hardware::ManufacturedVersion()
{
    return 0x01;
}


String<> Device::Info::Hardware::Version_ASCII_HEX()
{
    uint16 *pointer = (uint16 *)(HAL_ROM::ADDRESS_BASE + 0x206); //-V566

    uint16 version = *pointer;

    return String<>("%02X,%02X", (uint8)(version >> 8), (uint8)(version));
}


String<> Device::Info::Hardware::DateManufactured_ASCII()
{
    uint *pointer = (uint *)(HAL_ROM::ADDRESS_BASE + 0x210); //-V566

    return String<>("%08X", *pointer);
}


uint64 Device::Info::Hardware::MacUint64()
{
    struct StructMAC
    {
        uint8 b[6];
    };

    StructMAC *pointer = (StructMAC *)(HAL_ROM::ADDRESS_BASE + 0x214);

    StructMAC mac = *pointer;

    uint64 result = 0;

    int shift = 0;

    for (int i = 5; i >= 0; i--)
    {
        result |= (uint8)((int)mac.b[i] << shift);

        shift += 8;
    }

    return result;
}


String<> Device::Info::Hardware::SerialNumber_ASCII_HEX()
{
    return String<>("%08X", SerialNumber_UINT());
}


uint Device::Info::Hardware::SerialNumber_UINT()
{
    return PROGRAM_SIGNATURE;
}


String<> Device::Info::Loader::Version_ASCII_HEX()
{
    uint16 version = (uint16)PROGRAM_VERSION;

    return String<>("%02X,%02X", (uint8)(version >> 8), (uint8)version);
}


String<> Device::Info::Firmware::TypeID_ASCII()
{
    return String<>("RD330300");
}


String<> Device::Info::Firmware::Version_ASCII_HEX()
{
    uint version = Version_UINT();

    return String<>("%02X,%02X(%08X)", (uint8)(version >> 8), (uint8)version, version);
}


String<> Device::Info::Firmware::Version_ASCII_DEC()
{
    uint version = Version_UINT();

    return String<>("%u", version);
}


uint Device::Info::Firmware::Version_UINT()
{
    return PROGRAM_VERSION;
}


String<> Device::Info::Firmware::Info_ASCII()
{
    return String<>(PROGRAM_NAME);
}
