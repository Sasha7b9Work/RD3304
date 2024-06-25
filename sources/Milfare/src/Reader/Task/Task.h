// 2023/09/03 23:51:25 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Task
{
    void Run();

    namespace Read
    {
        void Create(int num_block, int num_bytes);
    }

    namespace Write
    {
        void Create(int num_block, uint8 *data, int num_bytes);
    }

    namespace MakeMaster
    {
        void Create(const SettingsMaster &, pchar bitmap_cards);

        // Это для создания мастер-карты, которая только меняет пароль
        // old_pass - этим паролем только паролим карту
        // new_pass - этот пароль будет установлен на считыватель после отработки карты
        void Create(uint64 old_pass, uint64 new_pass);

        void Run();
    }

    namespace MakeUser
    {
        void Create(uint64 password);

        void Create(uint64 password, uint64 number);

        void Run();
    }
}
