// 2024/03/27 13:28:27 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/Player/Player.h"
#include "Hardware/Power.h"
#include "Settings/Settings.h"
#include "Modules/Player/TableSounds.h"
#include "Hardware/HAL/HAL.h"
#include "Modules/Memory/Memory.h"
#include "Hardware/Timer.h"
#include "Device/Device.h"
#include "Nodes/OSDP/OSDP.h"


namespace Player
{
    namespace Buffer
    {
        static const int SIZE_BUFFER_BYTES = 2048;

#define BEGIN_BUFFER_SAMPLES  (&Buffer::buffer[3])

#define SAMPLES_IN_BUFFER (Buffer::SIZE_BUFFER_BYTES / sizeof(int16)) 

        static int16 buffer[SIZE_BUFFER_BYTES / 2 + 3];    // � ������ 6 ��������� ����. ����� ���� - ��� ������ ������� Memory::ReadBufferFast(),
                                                // �� ��� ������������ ���� 6 - ��� �� ���

        static int all_samples = 0;             // ������� �������� � ������������� �������
        static int load_samples = 0;            // ������� �������� ��������� �� ������
        static uint address = 0;                // �� ����� ������ ����� ��������� ��������

        static uint8 volume = 0;
        static bool need_load_begin = false;    // ����� ��������� ������ �������� ������
        static bool need_load_end = false;      // ����� ��������� ������ �������� ������

        // ��������� �����, ������� � ������ _buffer, ���������� �� ������
        static void FillBuffer(void *_buffer, int size)
        {
            size /= 2;                                          // ��������� � ��� ���� ������ ��������, ������ ��� ��� ���� ��������� ��� ������� � ������� �������

            uint8 temp[5];
            std::memcpy(temp, ((uint8 *)_buffer) - 5, 5);
            Memory::ReadBufferFast(address, _buffer, size);
            std::memcpy(((uint8 *)_buffer) - 5, temp, 5);

            load_samples += size / (int)sizeof(int16);
            address += (uint)size;

            int16 *pointer_src = (int16 *)_buffer;              // ������ ���� ������� ��� �����������
            pointer_src += size / 2;
            pointer_src--;

            int16 *pointer_dest = (int16 *)_buffer;             // ���� �������� �������
            pointer_dest += size;
            pointer_dest--;

            for(int i = 0; i < size / 2; i++)
            {
                int16 value = (int16)(*pointer_src-- * 0.4f * volume / 3.0f);
                if(HAL::IsDebugBoard())
                {
                    value /= 16;
                }

                *pointer_dest-- = value;
                *pointer_dest-- = value;
            }
        }

        static void Update()
        {
            static bool is_updating = false;

            if (is_updating)
            {
                return;
            }

            is_updating = true;

            if (load_samples == 0)
            {
                FillBuffer(BEGIN_BUFFER_SAMPLES, SIZE_BUFFER_BYTES);
            }
            else if(need_load_begin)
            {
                need_load_begin = false;

                FillBuffer(BEGIN_BUFFER_SAMPLES, SIZE_BUFFER_BYTES / 2);
            }
            else if (need_load_end)
            {
                need_load_end = false;

                FillBuffer(BEGIN_BUFFER_SAMPLES + SAMPLES_IN_BUFFER / 2, SIZE_BUFFER_BYTES / 2);
            }

            is_updating = false;
        }

        // ������������� � ������������ �������
        static void Prepare(int num_melody, uint8 _volume)
        {
            volume = _volume;

            load_samples = 0;

            all_samples = TableSounds::CountSamples(num_melody);

            address = TableSounds::AddressSound(num_melody);

            Buffer::Update();
        }
    }

    static bool is_playing = false;
}


bool Player::IsPlaying()
{
    return is_playing;
}


void Player::Init()
{

}


void Player::Play(TypeSound::E type)
{
    if (Power::IsFailure())
    {
        return;
    }

    PlayFromMemory(gset.Melody(type), gset.Volume(type));
}


void Player::PlayFromMemory(uint8 num_sound_in_memory, uint8 volume)
{
    if (is_playing)
    {
        return;
    }

    is_playing = true;

    Buffer::Prepare(num_sound_in_memory, volume);

    HAL_I2S::Start(BEGIN_BUFFER_SAMPLES, (int)SAMPLES_IN_BUFFER);

//    HAL_TIM13::Start(1);
}


void Player::Stop()
{
//    HAL_TIM13::Stop();

    HAL_I2S::Stop();

    is_playing = false;
}


void Player::CallbackOnHalfTransmit()
{
    Buffer::need_load_begin = true;
}


void Player::CallbackOnFullTransmit()
{
    Buffer::need_load_end = true;
}


void Player::CallbackInTimer13()
{
    if (is_playing)
    {
        Buffer::Update();

        if (Buffer::load_samples >= Buffer::all_samples)
        {
            Stop();
        }
    }
}


bool Player::Update()
{
    if (!is_playing && !ModeOffline::IsEnabled() && Device::IsRunning() && !OSDP::IsEnabled() && !ModeReader::IsWrite())
    {
        if (pinSND.IsLow())
        {
            Player::Play(TypeSound::Beep);
        }
        else
        {
            static bool prev_lr = false;    // ����������
            static bool prev_lg = false;    // ���������

            bool lr = pinLR.IsLow();
            bool lg = pinLG.IsLow();

            if (lr != prev_lr || lg != prev_lg)
            {
                if (lg && (lg != prev_lg))
                {
                    Player::Play(TypeSound::Green);
                }
                else if (lr && (lr != prev_lr))
                {
                    Player::Play(TypeSound::Red);
                }

                prev_lr = lr;
                prev_lg = lg;
            }
        }
    }

    if (is_playing)
    {
        CallbackInTimer13();
    }

    return is_playing;
}
