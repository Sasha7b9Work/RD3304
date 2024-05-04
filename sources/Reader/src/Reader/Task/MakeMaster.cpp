// 2023/09/06 20:24:43 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Reader/Task/Task.h"
#include "Modules/CLRC66303HN/CommandsCLRC663.h"
#include "Settings/Settings.h"
#include "Hardware/HAL/HAL.h"
#include "Reader/Messages.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>


using namespace CLRC66303HN;


namespace Task
{
    namespace MakeMaster
    {
        static bool exist = false;
        static bool make_full = false;      // Если true - делать мастер-карту с полной конфигурацией

        static uint64 old_password = 0;     // Нужно только в режиме "только пароль"

        static SettingsReader set;
        static const int SIZE_ACCESS_CARDS = 160;
        static char bitmap_cards[SIZE_ACCESS_CARDS];

        void Create(const SettingsReader &_set, pchar _bitmap_cards)
        {
            if (std::strlen(_bitmap_cards) < SIZE_ACCESS_CARDS)
            {
                std::strcpy(bitmap_cards, _bitmap_cards);
            }
            else
            {
                std::memcpy(bitmap_cards, _bitmap_cards, SIZE_ACCESS_CARDS);
            }

            bitmap_cards[SIZE_ACCESS_CARDS - 1] = '\0';

            set = _set;
            set.s04.word = (uint)-1;

            make_full = true;
            exist = true;
        }

        void Create(uint64 _old_pass, uint64 _new_pass)
        {
            old_password = _old_pass;

            set.SetPassword(_new_pass);

            make_full = false;
            exist = true;
        }

        // Полная конфигурация
        static bool Make()
        {
            if (make_full)
            {
                set.Hash() = set.CalculateHash();
                if (!Card::RAW::WriteBitmapCards(bitmap_cards))                             // Записываем битовую карту разрешённых карт
                {
                    return false;
                }
            }
            else
            {
                set.PrepareMasterOnlyPassword(set.Password());
            }

            if (Card::RAW::WriteSettings(set) &&                                            // Пишем все данные в карту за раз
                Card::RAW::SetPassword(make_full ? set.OldPassword() : old_password ) &&    // Установить пароль на карту
                Card::RAW::EnableCheckPassword())                                           // Закрываем сектора от чтения/записи
            {
                return true;
            }

            return false;
        }

        void Run()
        {
            if (exist)
            {
                exist = false;

                HAL_USART::UART::TransmitF("MAKE MASTER OLD_PASS=%llu NEW_PASS=%llu %s",
                    make_full ? set.OldPassword() : old_password,
                    set.Password(),
                    Make() ? "OK" : "FAIL");
            }
        }
    }
}
