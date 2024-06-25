// 2023/11/22 08:09:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#ifdef TYPE_BOARD_799
    #define MCU_GD
#endif



#define TIME_RISE_LEDS              250             // ����� ���������� ����� �� ���� �� �������
#ifdef MCU_GD
    #define MEMORY_FOR_SOUNDS           (872 * 1024)    // ��������������� ����� ��� ������
#else
    #define MEMORY_FOR_SOUNDS           (360 * 1024)    // ��������������� ����� ��� ������
#endif
#define NUMBER_SOUDNS               10              // ���������� ������ �� ������� ������
#define SIZE_FIRMWARE_STORAGE       (128 * 1024)
#define ADDRESS_SOUNDS              SIZE_FIRMWARE_STORAGE

//#define SAFE_PASSWORD                               // "����������� ������" - ����� ����� ���� ���������� ������� �� ����� 10 (�� 0 �� 10)
