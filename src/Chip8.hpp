#pragma once

#include "Defines.hpp"

struct Chip8
{
    union
    {
        u8 memory[0x1000];
        struct
        {
            u16* pc;
            u8* savedAddress;
            u8 registers[0x10];
            u16 stack[0x10];
            u16* sp;
            u8 soundTimer;
            u8 delayTimer;
            u8 pixels[256];
            bool drawFlag;
            bool keys[0x10];
            u8 font[0x10][5];
        } interpreterSpace;

        struct
        {
            u16* pc;
            u8* savedAddress;
            u8 registers[0x10];
            u16 stack[0x10];
            u16* sp;
            u8 soundTimer;
            u8 delayTimer;
            u8 pixels[256];
            bool drawFlag;
            bool keys[0x10];
            u8 font[0x10][5];
        };
    };

    Chip8();
    ~Chip8();

    void load(u8* program, u16 size);
    void cycle();
    void reset(bool keepProgram);

    typedef void (*operation)();
};

struct DecodedOpcode
{
    int code;
    int x;
    int y;
    int n;
    int nn;
    int nnn;

    DecodedOpcode(u16* pc);
};
