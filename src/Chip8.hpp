#pragma once

#include "Defines.hpp"

struct Chip8
{
    u8 memory[0x1000];
    u16* pc; // Program Counter
    u8* savedAddress;
    u8 registers[0x10];
    u16 stack[0x10];
    u16* sp; // stack pointer
    u8 delayTimer;
    u8 soundTimer;
    u8 pixels[64 * 32];
    bool drawFlag;

    Chip8();
    ~Chip8();

    void load(u8* program, u16 size);
    void cycle();
    void reset(bool keepProgram);

    typedef void (*operation)();
};

struct DecodedOpcode
{
    int opcode;
    int x;
    int y;
    int n;
    int nn;
    int nnn;

    DecodedOpcode(u16* pc);
};
