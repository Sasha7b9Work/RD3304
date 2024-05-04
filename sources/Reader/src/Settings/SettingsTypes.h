// 2024/01/26 23:45:33 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct BaudRate
{
    enum E
    {
        _9600,
        _19200,
        _38400,
        Count
    };

    explicit BaudRate(E v) : value(v) { }

    uint ToRAW() const;

    static E FromUInt(uint);

    operator E() { return value; }

private:

    E value;
};


struct Weigand
{
    enum E
    {
        _26,
        _33,
        _34,
        _37,
        _40,
        _42,
        Count
    };

    Weigand(E v) : value(v) { }

    int ToRAW() const;

    E value;
};


struct TypeSound
{
    enum E
    {
        Beep,
        Green,
        Red,
        Count
    };
};


struct Color
{
    explicit Color(uint color = 0, float brightness = 0.0f)
    {
        value = (uint)((uint)(color & 0x00FFFFFF) + (uint)((uint8)(brightness * 255.0f) << 24));
    }
    explicit Color(uint8 red, uint8 green, uint8 blue, float brightness = 0.0f)
    {
        value = (uint)((red << 16) + (green << 8) + blue + ((uint8)(brightness * 255.0f) << 24));
    }

    uint8 GetRed() const { return (uint8)(value >> 16); }
    uint8 GetGreen() const { return (uint8)(value >> 8); }
    uint8 GetBlue() const { return (uint8)(value); }

    float GetRedRelativeF() const { return (float)GetRed() / 255.0f; }
    float GetGreenRelativeF() const { return (float)GetGreen() / 255.0f; }
    float GetBlueRelativeF() const { return (float)GetBlue() / 255.0f; }

    uint8 GetBirghtness() const { return (uint8)(value >> 24); }
    float GetBrightnessRelativeF() const { return (float)GetBirghtness() / 255.0f; }

    static Color FromUint(uint);

    uint value;
};
