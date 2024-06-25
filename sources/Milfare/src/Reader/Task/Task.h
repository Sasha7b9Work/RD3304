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

        // ��� ��� �������� ������-�����, ������� ������ ������ ������
        // old_pass - ���� ������� ������ ������� �����
        // new_pass - ���� ������ ����� ���������� �� ����������� ����� ��������� �����
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
