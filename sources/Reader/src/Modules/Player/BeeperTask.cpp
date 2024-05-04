// 2023/12/20 10:25:36 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/Player/Player.h"


namespace Player
{
    namespace Task
    {
        static TypeSound::E sound = TypeSound::Count;

//        static uint time_on = 0;
//        static uint time_off = 0;
//        static int count = 0;
    }
}


void Player::Task::Set(TypeSound::E _sound, uint /*_time_on*/, uint /*_time_off*/, int /*_count*/)
{
    sound = _sound;
//    time_on = _time_on;
//    time_off = _time_off;
//    count = _count;

    Player::Stop();

    Player::Play(sound);
}
