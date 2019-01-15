#include "Chip8.hpp"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <ctime>
#include <random>

Chip8::Chip8()
    : pc {(u16*) &memory[0x200]},
      savedAddress {},
      registers {},
      stack {},
      sp {stack},
      soundTimer {},
      delayTimer {},
      pixels {},
      drawFlag {true},
      keys {},
      font {{0xf0, 0x90, 0x90, 0x90, 0xf0}, // 0
          {0x20, 0x60, 0x20, 0x20, 0x70},   // 1
          {0xf0, 0x10, 0xf0, 0x80, 0xf0},   // 2
          {0xf0, 0x10, 0xf0, 0x10, 0xf0},   // 3
          {0x90, 0x90, 0xf0, 0x10, 0x10},   // 4
          {0xf0, 0x80, 0xf0, 0x10, 0xf0},   // 5
          {0xf0, 0x80, 0xf0, 0x90, 0xf0},   // 6
          {0xf0, 0x10, 0x20, 0x40, 0x40},   // 7
          {0xf0, 0x90, 0xf0, 0x90, 0xf0},   // 8
          {0xf0, 0x90, 0xf0, 0x10, 0xf0},   // 9
          {0xf0, 0x90, 0xf0, 0x90, 0x90},   // a
          {0xe0, 0x90, 0xe0, 0x90, 0xe0},   // b
          {0xf0, 0x80, 0x80, 0x80, 0xf0},   // c
          {0xe0, 0x90, 0x90, 0x90, 0xe0},   // d
          {0xf0, 0x80, 0xf0, 0x80, 0xf0},   // e
          {0xf0, 0x80, 0xf0, 0x80, 0x80}}

{
    printf_s("size: %d\n", sizeof(interpreterSpace) - 0x200);
    assert(sizeof(interpreterSpace) < 0x200);
    srand(std::time(nullptr));
}

Chip8::~Chip8() {}

void Chip8::load(u8* program, u16 size)
{
    auto* memoryProgram = memory + 0x200;
    for (auto i = 0; i < size; i++) { memoryProgram[i] = program[i]; }
}

void Chip8::cycle()
{
    auto op = DecodedOpcode(pc++);
    switch (op.opcode & 0xf000)
    {
        case 0x0000:
            switch (op.opcode)
            {
                case 0x00e0:
                    for (auto& pixel : pixels) pixel = 0;
                    drawFlag = true;
                    break;

                default:
                    printf_s("Unknow opcode [0x0000]: 0x%X\n", op.opcode);
                    pc--;
                    break;
            }
            break;

        case 0x1000: pc = (u16*) &memory[op.nnn]; break;

        case 0x4000:
            if (registers[op.x] != op.nn) pc++;
            break;

        case 0x6000: registers[op.x] = op.nn; break;

        case 0x7000: registers[op.x] += op.nn; break;

        case 0x8000:
            switch (op.opcode & 0x000f)
            {
                case 0x0000: registers[op.x] = registers[op.y]; break;

                default:
                    printf_s("Unknown opcode [0x8000]: %X", op.opcode);
                    pc--;
                    break;
            }
            break;

        case 0xc000: registers[op.x] = (rand() % 0x100) & op.nn; break;

        case 0xd000:
            for (int y = registers[op.y], i = 0; i < op.n; y++, i++)
            {
                auto sprite = savedAddress[i];
                for (int x = registers[op.x], xcol = 0; xcol < 8; x++, xcol++)
                {
                    if ((sprite & (0x80 >> xcol)) != 0)
                    {
                        if (pixels[x + y * 64] == 1) registers[0xf] = 1;
                        pixels[x + y * 64] ^= 1;
                        drawFlag = true;
                    }
                }
            }
            break;

        case 0xa000: savedAddress = &memory[op.nnn]; break;

        default:
            printf_s("Unknown opcode: 0x%X\n", op.opcode);
            pc--;
            break;
    }
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
