// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "MFRC630/mfrc630.h"
#include "Hardware/HAL/HAL.h"


int main(void)
{
    HAL::Init();

    mfrc630_MF_example_dump();
}
