// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#define VERSION 528
#define NAME "CKE"


#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
    #pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
    #pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
    #pragma clang diagnostic ignored "-Wwritable-strings"
    #pragma clang diagnostic ignored "-Wcast-align"
    #pragma clang diagnostic ignored "-Wcast-qual"
    #pragma clang diagnostic ignored "-Wformat-nonliteral"
    #pragma clang diagnostic ignored "-Wold-style-cast"
    #pragma clang diagnostic ignored "-Wpadded"
    #pragma clang diagnostic ignored "-Wglobal-constructors"
#else
    #ifdef WIN32
        #define __disable_irq()
        #define __enable_irq()
        #define __DSB()
        #define __ISB()
        #define __attribute__(x)
        #define WIN32_LEAN_AND_MEAN
        #ifndef GUI
            #define asm()
        #endif
    #endif
#endif


typedef unsigned char      uint8;
typedef signed char        int8;
typedef unsigned short     uint16;
typedef signed short       int16;
typedef unsigned int       uint;
typedef const char        *pchar;
typedef unsigned char      uchar;
typedef unsigned long long uint64;


#ifdef WIN32
    #define __asm(x)
#else
//    #pragma anon_unions
#endif


union BitSet16
{
    BitSet16() : half_word(0) { }

    BitSet16(uint8 low, uint8 hi)
    {
        byte[0] = low;
        byte[1] = hi;
    }

    uint16 half_word;
    uint8  byte[2];
};


union BitSet32
{
    uint   word;
    uint16 half_word[2];
    uint8  bytes[4];

    static BitSet32 FromBytes(uint8 b0, uint8 b1, uint8 b2, uint8 b3)
    {
        BitSet32 bs = { (uint)(b0 | (b1 << 8) | (b2 << 16) | (b3 << 24)) };

        return bs;
    }
};


union BitSet64
{
    BitSet64(uint64 _value = 0) : long_word(_value) {}
    uint64 long_word;
    uint   word[2];
    uint16 half_word[4];
    uint8  bytes[8];
};


#define _GET_BIT(value, bit)   (((value) >> (bit)) & 0x01)
#define _SET_BIT(value, bit)   ((value) |= (1 << (bit)))
#define _CLEAR_BIT(value, bit) ((value) &= (uint)(~(1 << (bit))))


#define ERROR_VALUE_FLOAT   1.111e29f


#define _bitset(bits)                               \
  ((uint8)(                                         \
  (((uint8)((uint)bits / 01)        % 010) << 0) |  \
  (((uint8)((uint)bits / 010)       % 010) << 1) |  \
  (((uint8)((uint)bits / 0100)      % 010) << 2) |  \
  (((uint8)((uint)bits / 01000)     % 010) << 3) |  \
  (((uint8)((uint)bits / 010000)    % 010) << 4) |  \
  (((uint8)((uint)bits / 0100000)   % 010) << 5) |  \
  (((uint8)((uint)bits / 01000000)  % 010) << 6) |  \
  (((uint8)((uint)bits / 010000000) % 010) << 7)))

#define BINARY_U8( bits ) _bitset(0##bits)


#include "opt.h"
#include "Utils/Log.h"


#ifdef GUI
    #include "defines_GUI.h"
#endif
