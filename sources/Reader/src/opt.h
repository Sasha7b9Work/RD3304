// 2023/11/22 08:09:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#ifdef TYPE_BOARD_799
    #define MCU_GD
#endif



#define TIME_RISE_LEDS              250             // Время нарастания света от нуля до полного
#ifdef MCU_GD
    #define MEMORY_FOR_SOUNDS           (872 * 1024)    // Зарезервировано места для звуков
#else
    #define MEMORY_FOR_SOUNDS           (360 * 1024)    // Зарезервировано места для звуков
#endif
#define NUMBER_SOUDNS               10              // Количество звуков во внешней памяти
#define SIZE_FIRMWARE_STORAGE       (128 * 1024)
#define ADDRESS_SOUNDS              SIZE_FIRMWARE_STORAGE

//#define SAFE_PASSWORD                               // "Защщищённый пароль" - карта может быть запаролена паролем не более 10 (от 0 до 10)
