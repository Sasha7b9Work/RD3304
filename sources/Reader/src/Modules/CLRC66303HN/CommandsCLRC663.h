// 2022/7/6 10:32:31 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Reader/Card.h"


namespace CLRC66303HN
{
    static const int SIZE_BLOCK = 4;

    namespace Command
    {
        void Idle();

        void Send(uint8);

        void Send(uint8, uint8);

        void Send(uint8, uint8, uint8, uint8, uint8);

        void Send(uint8, uint8, uint8, uint8, uint8, uint8, uint8);

        void Send(uint8 *buffer, int size);

        // Посылают команду антиколлизии CL1 и ожидает ответ. Возвращает true в случае получения ответа
        bool AnticollisionCL(int cl, UID *uid);

        bool SelectCL(int cl, UID *uid);

        bool PasswordAuth(uint64);

        namespace NTAG
        {
            bool ReadBlock(int num_block, uint8 buffer[4]);

            bool ReadBlock(int num_block, uint *word); //-V1071

            // size должно быть кратно 4
            bool ReadData(int num_block, void *data, int size);

            // num_block = [0x02...0x2c]
            bool WriteBlock(int num_block, const uint8 buffer[4]);

            bool Write2Blocks(int num_block, uint8 buffer[8]);

            // size должно бтыь кратно 4
            bool WriteData(int num_block, const void *data, int size);
        }

        namespace Milfare
        {
            bool ReadBlock(int num_block, uint8 buffer[4]);

            bool ReadBlock(int num_block, uint *word); //-V1071

            // size должно быть кратно 4
            bool ReadData(int num_block, void *data, int size);

            // num_block = [0x02...0x2c]
            bool WriteBlock(int num_block, const uint8 buffer[4]);

            bool Write2Blocks(int num_block, uint8 buffer[8]);

            // size должно бтыь кратно 4
            bool WriteData(int num_block, const void *data, int size);
        }
    }
}
