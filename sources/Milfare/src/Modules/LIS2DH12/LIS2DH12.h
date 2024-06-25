// 2022/6/24 15:52:45 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


// Структура содержит нативные данные, считанные из акселерометра
union StructDataRaw
{
    StructDataRaw(int16 val = 0) : raw(val) { }

    struct
    {
        uint8 lo;
        uint8 hi;
    };

    int16 raw;

    float ToTemperatrue()
    {
        return 25.0f + (float)raw / 256.0f;
    }

    float ToAccelearation()
    {
        return (float)raw / 16.0f / 1000.0f;
    }
};



namespace LIS2DH12
{
    void Init();

    void Update();

    bool IsExist();

    bool IsAlarmed();

    StructDataRaw GetRawTemperature();
}
