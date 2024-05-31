// 2023/08/29 19:55:25 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Utils/Buffer.h"
#include "Utils/Mutex.h"


class BufferUSART : public Buffer512
{
public:
    BufferUSART() : Buffer512() { }

    bool FirstBytesIs(pchar) const;

    // Слово nuw_word == word. Нумерация с 1
    bool WordIs(int num_word, pchar string) const;

    int CountWords() const;

    // Возвращает int из слова num_word
    bool GetInt(int num_word, int *value) const;

    bool GetUint(int num_word, uint *value) const;

    bool GetUint64(int num_word, uint64 *value) const;

    // Берёт значение из 16-ричной строки
    bool GetUIntFromHEX(int num_word, uint *value) const;

    // Читает настройки возвращате true, если удалось
    bool ReadSettings(int num_words, SettingsReader &) const;

    // Возвращает true, если принятая crc32 соотвествует рассчитанному
    bool Crc32IsMatches(int num_words) const;
};


namespace Communicator
{
    void Update(BufferUSART &);

    void WriteConfitToUSART();
}
