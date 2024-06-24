// 2022/04/27 11:48:09 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Utils/String.h"


namespace Device
{
    void Init();

    void Update();

    bool UpdateTasks();

    // �������� �������������. ��������
    bool IsRunning();

    namespace StartState
    {
        // ����� ������ ����� �� �������� ���������
        // ���������� ���������� TX � SND
        bool NeedReset();

        // ����� ����� ����� ������������ WG - ����� �� WG ��������� ���� GUID �����
        // ���������� ���������� RX � SND
        bool NeedMinimalWG();

        // ����� ����������� �����������
        bool NeedOffline();
    }

    namespace Info
    {
        namespace Hardware
        {
            String<> Version_ASCII_HEX();           // ���������� ������
            String<> DateManufactured_ASCII();      // ���� ������������
            uint64 MacUint64();
            String<> SerialNumber_ASCII_HEX();      // �������� ����� �������
            uint   SerialNumber_UINT();             // �������� ����� �������

            uint   Vendor();
            uint8  ManufacturedModelNumber();
            uint8  ManufacturedVersion();
        }

        namespace Loader
        {
            String<> Version_ASCII_HEX();           // ������ ����������
        }

        namespace Firmware
        {
            String<> TypeID_ASCII();                // ������������� ���� �������
            String<> Version_ASCII_HEX();           // ������ ���������
            String<> Version_ASCII_DEC();           // ������ ���������
            uint     Version_UINT();                // ������ ���������
            String<> Info_ASCII();
        }
    }
}
