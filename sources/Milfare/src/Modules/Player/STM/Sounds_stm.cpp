// 2023/10/05 21:44:29 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/Player/STM/Sounds_stm.h"
#include "Modules/Memory/Memory.h"
#include "Modules/Player/Player.h"
#include "Utils/RingBuffer.h"
#include "Hardware/Timer.h"
#include "Utils/Math.h"
#include "Modules/Player/TableSounds.h"
#include <cmath>


namespace Sound
{
    static void CallbackOnComplete();

    struct Stream
    {
    private:

        void Clear()
        {
            index_fill_u8 = 0;
            index_read_u8 = 0;
        }

        uint8 *Protect()
        {
            return buffer;
        }

        uint8 *Destination()
        {
            return buffer + 5;
        }

        uint8 *DestinationEnd()
        {
            return Destination() + SIZE_DATA;
        }

        uint8 *Temp()
        {
            return buffer + 5 + SIZE_DATA;
        }

        void Store5Bytes(uint8 *_address_chunk)
        {
            address_chunk_temp = _address_chunk;
            std::memcpy(Temp(), address_chunk_temp - 5, 5);
        }
 
        // Возвращает адрес в буфере для чтения из внешней памяти с адреса address_exe
        uint8 *DestinationForAddress(uint address_ext)
        {
            return Destination() + ((address_ext - address_start_ext) % SIZE_DATA);
        }

        uint8 *DestinationForIndex(int index_u8)
        {
            return Destination() + (index_u8 % SIZE_DATA);
        }

        static const int SIZE_DATA = 2 * 512;  // Столько места зарезервировано для подгрузки данных

        int    all_bytes;                   // Столько байт должно быть считано
        uint   address_start_ext;           // С этого адреса начинаются данные во внешней памяти
        uint   address_next_ext;            // Адрес начала следующих считываемых данных из внешней памяти
        int    samples_left;                // Осталось проиграть сэмплов
        uint8  buffer[5 + SIZE_DATA + 5];   // Первые 5 байт зарезервированы для управлющих байт записи в самое начало записи
                                            // (Чтобы считать что-то из микросхемы памяти, в неё нужно записать 5 управляющих байт)
        uint8 *address_chunk_temp;          // Здесь сохраняем адрес данных, которые нужно сохранить, а потом восстановить
        int    index_fill_u8;               // Столько байт загружено в буфер
        int    index_read_u8;               // Столько байт считано из буфера

    public:

        volatile bool dma_complete;

        // Начать востпроизведение с адреса address, всего проиграть num_samples отсчётов
        void Start(uint _address, int _num_samples)
        {
            dma_complete = true;
            address_start_ext = _address;
            address_next_ext = _address;
            samples_left = _num_samples;
            all_bytes = _num_samples * 2;
            Clear();

            uint8 *destination = DestinationForAddress(address_start_ext);

            uint source = address_start_ext;

            int size = (SIZE_DATA >= all_bytes) ? all_bytes : SIZE_DATA;

            Write(destination, source, size);

            while (!dma_complete)
            {
            }
        }

        bool Update()
        {
            if (!dma_complete || (index_fill_u8 >= all_bytes))
            {
                return samples_left != 0;
            }

            uint8 *dest_fill = DestinationForIndex(index_fill_u8);

            uint8 *dest_read = DestinationForIndex(index_read_u8);

            if (dest_read > dest_fill)                            // Указатель чтения находится после указателя записи, укзатель записи должен догнать указатель чтения
            {
                int size = dest_read - dest_fill;

                if (size < 32)
                {
                    return samples_left != 0;
                }

                Write(dest_fill, address_next_ext, size - 16);
            }
            else if (dest_read < dest_fill)
            {
                int size = DestinationEnd() - dest_fill;

                Write(dest_fill, address_next_ext, size);

                size = DestinationForIndex(index_read_u8) - Destination();

                if (size > 0)
                {
                    Write(Destination(), address_next_ext, size);
                }
            }

            return (samples_left != 0);
        }

        void Write(uint8 *dest, uint src, int size)
        {
            if (size <= 0)
            {
                dma_complete = true;

                return;
            }

            Store5Bytes(dest);

            dma_complete = false;

            Memory::ReadBufferFastDMA(src, dest, size, CallbackOnComplete);

            while (!dma_complete)
            {
            }

            address_next_ext += (uint)size;

            index_fill_u8 += size;
        }

        void Restore5Bytes()
        {
            std::memcpy(address_chunk_temp - 5, Temp(), 5);
        }

        int16 GetSmaple(int num_sample)
        {
            samples_left--;

            int index = ((int)num_sample * 2) % SIZE_DATA;

            index_read_u8 += 2;

            int16 *dest = (int16 *)(Destination() + index);

            return *dest;
        }
    };

    static int num_sample = 0;              // Текущий проигрываемый отсчёт
    static uint8 num_sound = (uint8)-1;     // Текущий проигрываемый звук
    static int all_samples = 0;             // Число отсчётов в проигрываемой мелодии

    static Stream stream;

    static void CallbackOnComplete()
    {
        stream.Restore5Bytes();

        stream.dma_complete = true;
    }
}


void Sound::Start(uint8 _num_sound)
{
    num_sound = _num_sound;
    num_sample = 0;
    all_samples = TableSounds::CountSamples(num_sound);

    if (all_samples > 0 && all_samples < (MEMORY_FOR_SOUNDS / 2))
    {
        stream.Start(TableSounds::AddressSound(num_sound), all_samples);
    }
}


void Sound::Stop()
{
    num_sound = (uint8)-1;
}


bool Sound::IsPlaying()
{
    return (num_sound != (uint8)-1) && (num_sample < all_samples);
}


int16 Sound::NextSample()
{
    return stream.GetSmaple(num_sample++);
}


bool Sound::Update()
{
    return stream.Update();
}
