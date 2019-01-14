#include "Chip8.hpp"

Chip8::Chip8()
    : memory {},
      pc {(u16*) &memory[0x200]},
      savedAddress {nullptr},
      registers {},
      stack {},
      sp {stack},
      delayTimer {0},
      soundTimer {0},
      pixels {},
      drawFlag {true}
{
}

Chip8::~Chip8() {}

void Chip8::load(u8* program, u16 size)
{
    auto* memoryProgram = memory + 0x200;
    for (auto i = 0; i < size; i++) { memoryProgram[i] = program[i]; }
}

void Chip8::cycle()
{
    // the fun bit.
}

void Chip8::reset(bool keepProgram)
{
    pc = (u16*) &memory[0x200];
    savedAddress = nullptr;

    for (auto& r : registers) r = 0;

    for (auto& s : stack) s = 0;
    sp = stack;

    delayTimer = 0;
    soundTimer = 0;

    for (auto& pixel : pixels) { pixel = 0; }

    drawFlag = true;

    if (keepProgram) return;

    for (auto* i = memory + 0x200; i < memory + 0x1000; i++) *i = 0;
}

DecodedOpcode::DecodedOpcode(u16* pc)
{
    auto* opByte = (u8*) pc;
    opcode = (*opByte << 8) | *(opByte + 1);
    x = (opcode & 0x0f00) >> 8;
    y = (opcode & 0x00f0) >> 4;
    n = opcode & 0x000f;
    nn = opcode & 0x00ff;
    nnn = opcode & 0x0fff;
}
