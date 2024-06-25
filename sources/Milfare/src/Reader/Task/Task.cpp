// 2023/09/03 23:51:12 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Reader/Task/Task.h"
#include "Modules/CLRC66303HN/CommandsCLRC663.h"
#include "Hardware/HAL/HAL.h"
#include "Utils/StringUtils.h"
#include "Reader/Card.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>


using namespace std;
using namespace SU;
using namespace CLRC66303HN;


namespace Task
{
    namespace Read
    {
        static int num_block = 0;
        static int num_bytes = 0;
        static bool exist = false;

        void Create(int _num_block, int _num_bytes)
        {
            num_block = _num_block;
            num_bytes = _num_bytes;
            exist = true;
        }

        static void Run()
        {
            if (!exist)
            {
                return;
            }

            exist = false;

            uint8 data[Card::SIZE_MEMORY];

            if (Card::RAW::ReadDataFromblocks(num_block, data, num_bytes))
            {
//                LOG_WRITE("Reading %d bytes :", num_bytes);

                for (int i = 0; i < num_bytes; i++)
                {
                    char message[32];

                    if ((i % 4) == 0)
                    {
                        std::sprintf(message, "%02d :", num_block + i / 4);

                        HAL_USART::UART::Transmit(message, (int)std::strlen(message));
                    }

                    std::sprintf(message, " %02X", data[i]);

                    HAL_USART::UART::Transmit(message, (int)std::strlen(message));

                    if (((i + 1) % 4) == 0)
                    {
//                        LOG_WRITE("");
                    }
                }
            }
        }
    }

    namespace Write
    {
        static int num_block = 0;
        static int num_bytes = 0;
        static uint8 data[Card::SIZE_MEMORY];
        static bool exist = false;

        void Create(int _num_block, uint8 *_data, int _num_bytes)
        {
            exist = true;
            num_block = _num_block;
            num_bytes = _num_bytes;
            memcpy(data, _data, (uint)_num_bytes);
        }

        static void Run()
        {
            if (!exist)
            {
                return;
            }

            exist = false;

            if (Card::RAW::WriteDataToBlocks(num_block, data, num_bytes))
            {
//                LOG_WRITE("#WRITE %d bytes", num_bytes);
            }
            else
            {
                HAL_USART::UART::TransmitF("#ERROR WRITE");
            }
        }
    }
}


void Task::Run()
{
    Read::Run();

    Write::Run();

    MakeMaster::Run();

    MakeUser::Run();
}
