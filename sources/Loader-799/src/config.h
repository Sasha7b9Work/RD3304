#ifndef _CONFIG_H_
#define _CONFIG_H_


#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
    #pragma clang diagnostic ignored "-Wcast-align"
    #pragma clang diagnostic ignored "-Wunused-but-set-variable"
    #pragma clang diagnostic ignored "-Wdeclaration-after-statement"
    #pragma clang diagnostic ignored "-Wsign-conversion"
    #pragma clang diagnostic ignored "-Wunused-function"
    #pragma clang diagnostic ignored "-Wstrict-prototypes"
    #pragma clang diagnostic ignored "-Wmissing-prototypes"
    #pragma clang diagnostic ignored "-Wimplicit-int-conversion"
    #pragma clang diagnostic ignored "-Wcast-qual"
    #pragma clang diagnostic ignored "-Wmissing-variable-declarations"
    #pragma clang diagnostic ignored "-Wglobal-constructors"
    #pragma clang diagnostic ignored "-Wold-style-cast"
#endif


typedef unsigned int   uint;
typedef unsigned short uint16;
typedef unsigned char  uint8;


 // выбор платы, должна быть только одна единица
#define PCB_771 0
#define PCB_777 1

//! Частота ядра
//#define CPU_CLK  56000000

//! Тактовая частота переферии
//#define PCLK FREQ_CCLK

//! Адрес начала основной программы в ROM
#define MAIN_PROG_FIRST_ADDR (1024*4+0x8000000)

//! Размер области ROM где содержится основная программа (общий объем:64 - размер блока под загрузчик:4)
#define MAIN_PROG_AREA_SIZE (1024*(64-8))

//! Адрес по которому хранится магическое число, адрес слова в ОЗУ,
// через который передается информация от основной программы
#define MAGIC_NUM_ADDR ((uint32_t*)0x20000000)

//! Адрес в Serial Flash где хранится образ файла
#define UPDATE_START_ADDR   0x0

//! Время для начала работы с функцией загрузчика,
// если в течении этого времени не будет полученно ни одной команды от компьютера, то булет произведен рестарт
//#define WORK_TIME  (4*60* 3 ) // три минуты

#define PROGRAM_SIGNATURE 0x55443300

#endif


#ifdef WIN32
    #define __disable_irq()
    #define __enable_irq()
#endif
