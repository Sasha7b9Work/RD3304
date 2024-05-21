// 2023/09/06 19:57:51 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Reader/Task/Task.h"
#include "Modules/CLRC66303HN/CommandsCLRC663.h"
#include "Hardware/HAL/HAL.h"
#include <cstdio>
#include <cstring>


using namespace CLRC66303HN;


namespace Task
{
    namespace MakeUser
    {
        static uint64 password = 0;
        static bool exist_password = false;

        static uint64 number = 0;
        static bool exist_number = false;

        void Create(uint64 _password)
        {
            password = _password;
            exist_password = true;
            exist_number = false;
        }

        void Create(uint64 _password, uint64 _number)
        {
            password = _password;
            number = _number;

            exist_number = true;
            exist_password = true;
        }

        void Run()
        {
            if (exist_password || exist_number)
            {
                bool result = true;

                if (exist_number)
                {
                    if (!Card::RAW::WriteNumber(number))
                    {
                        result = false;
                    }
                }

                if (exist_password)
                {
                    if (!Card::RAW::SetPassword(password))
                    {
                        result = false;
                    }
                }

                if (!Card::RAW::EnableCheckPassword())
                {
                    result = false;
                }

                if (exist_number)
                {
                    HAL_USART::UART::TransmitF("MAKE USER PASSWORD %llu NUMBER %u %s ", password, number, result ? "OK" : "FAIL");
                }
                else
                {
                    HAL_USART::UART::TransmitF("MAKE USER PASSWORD %llu %s", password, result ? "OK" : "FAIL");
                }

                exist_password = false;
                exist_number = false;
            }
        }
    }
}
