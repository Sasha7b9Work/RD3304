

/*
 Загрузчик должен запускаться только через аппаратныя рестарт, т.к.
 расчитан на состояние аппаратуры после рестарта.
*/
#include <gd32e23x.h>

#include <string.h>
#include <stdint.h>
#include "config.h"
#include "booter_aes.h"
#include "booter_crc.h"
#include "Hardware/hrd_Iap.h"

#include "fmw.h"

#include "Update.h"

#include "Hardware/HAL/HAL.h"


#define USER_APPLICATION_BASE_ADDRESS 0x8001000


#ifndef WIN32

static __attribute__((naked, noreturn)) void BootJumpASM(uint32_t SP, uint32_t RH)
{
    __asm("MSR MSP,r0");
    __asm("BX  r1");
} 


static __attribute__((noreturn)) void BootJump(uint32_t *Address)
{
//    if( CONTROL_nPRIV_Msk & __get_CONTROL( ) )
//    {  /* not in privileged mode */
//        EnablePrivilegedMode( ) ;
//    }

    for(int i = 0; i < 8; i++)
    {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }

    SysTick->CTRL = 0;
    SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;

//    SCB->SHCSR &= ~( SCB_SHCSR_USGFAULTENA_Msk |
//                    SCB_SHCSR_BUSFAULTENA_Msk |
//                    SCB_SHCSR_MEMFAULTENA_Msk ) ;

    if(CONTROL_SPSEL_Msk & __get_CONTROL())
    {  /* MSP is not active */
        __set_MSP(__get_PSP());
        __set_CONTROL(__get_CONTROL() & ~CONTROL_SPSEL_Msk);
    }

    SCB->VTOR = (uint32_t)Address;

//    nvic_vector_table_set(USER_APPLICATION_BASE_ADDRESS, 0);

    BootJumpASM(Address[0], Address[1]);
}

#endif


/* Main Program */
int main(void)
{
    HAL::Init();

#ifndef WIN32
    
    // \todo
//    SysTick->CTRL = (1 << 2) | (1 << 0); // Control and Status Register, cpuclk, start(0)

#endif

    uint32_t magicNum = *MAGIC_NUM_ADDR;
    // магическое число от основной программы (если она спровоцировала рестарт) 
    // *((uint32_t*)0x10000000) = 0;
    *MAGIC_NUM_ADDR = 0;

    Init_CRC32();

    volatile uint32_t sfid;
    sfid = HAL_SPI::ReadID();
    //sflash_read( UPDATE_START_ADDR, 8*4, fileHeader );
    // читаем заголовок файла
    read_header();

    //((void(*)(void))(*((unsigned long*)(MAIN_PROG_FIRST_ADDR + 4))))();
    // проверка наличия файла образа в Serial flash
   // if( *(uint32_t*)fileHeader == 0x30314643 ) // проверка сигнатуры файла


    if (check_header())
    {
        uint32_t fileProgramVersion;
        //fileProgramVersion = NTOH( &fileHeader[12] );
        fileProgramVersion = get_file_program_version();
        // сверить версию программы в файле с той что находится в ROM
        uint32_t romProgramVersion = *((uint32_t *)(MAIN_PROG_FIRST_ADDR + 516));
//        if (fileProgramVersion != romProgramVersion)
        {// версии не совпали, теперь проверяем файл образа
         //uint32_t progSig = *((uint32_t*)&fileHeader[8]);
            uint32_t progSig = get_file_program_sig();
            if (progSig == PROGRAM_SIGNATURE)
            {
                if (check_file())
                {// контрольная сумма рбраза верная, можно прошивать
                    doUpdate( /*fileHeader*/);
                }
            }

        }
    }

    if (magicNum != 0x12345678) // в ОЗУ нет признака принудительной отмены запуска основной программы
    {
        if (CheckMainProgControlSum() || magicNum == 0xFEDCBA98) //сходится контрольная сумма или в ОЗУ есть признак разрешающий запуск с неверной контрольной суммой
        {
            { // передача управления на стартовый адрес, прерывания все еще запрещены
             // стартовый адрес это второе слово от начала области основной программы

#ifndef WIN32
                BootJump((uint32_t *)USER_APPLICATION_BASE_ADDRESS);
#endif
            }
        }
    }

    while (1)                                // Loop forever 
    {// тут можно выдать сообщение в UART
    // sflash_read( UPDATE_START_ADDR, 1024+16, buffer )         ;
    }
}

