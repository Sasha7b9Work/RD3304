// 2024/04/16 08:46:59 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/PlayerSoft/PlayerSoft.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include <cmath>


namespace PlayerSoft
{
    static bool is_running = false;
    static uint start_time = 0;         // Время начала звучания
    static uint time_need = 0;          // Столько будем звучать

    namespace Buffer
    {
        static uint num_sample = 0;
        static bool fill_begin = true;
        static bool fill_end = true;
        static float frequency = 0.0f;

        static const int SIZE_BUFFER = 1024;

        static int16 data[SIZE_BUFFER];

        static void Prepare(float freq)
        {
            frequency = freq;

            num_sample = 0;
            fill_begin = true;
            fill_end = true;

            Update();
        }

        static void Fill(int start, int end)
        {
            /*
            *   Синусоида
            * 
            *   Y = 13383 * sin(2 * pi * f * N / 32e3)
            */

            for (int i = start; i < end; i++)
            {
                data[i] = (int16)(16838 * 0.05f * std::sinf(2 * 3.1415296f * frequency * (float)(num_sample++) / 32e3f));
            }
        }

        static void Update()
        {
            if (fill_begin)
            {
                fill_begin = false;

                Fill(0, SIZE_BUFFER / 2);
            }

            if (fill_end)
            {
                fill_end = false;

                Fill(SIZE_BUFFER / 2, SIZE_BUFFER);
            }
        }
    }
}


void PlayerSoft::Play(float frequency, uint time)
{
    is_running = true;

    start_time = TIME_MS;

    time_need = time;

    Buffer::Prepare(frequency);

    HAL_I2S::Start(Buffer::data, Buffer::SIZE_BUFFER);
}


void PlayerSoft::CallbackOnFullTransmit()
{
    Buffer::fill_end = true;
}


void PlayerSoft::CallbackOnHalfTransmit()
{
    Buffer::fill_begin = true;
}


void PlayerSoft::Update()
{
    Buffer::Update();

    if (TIME_MS - start_time >= time_need)
    {
        is_running = false;

        HAL_I2S::Stop();
    }
}


bool PlayerSoft::IsRunning()
{
    return is_running;
}
