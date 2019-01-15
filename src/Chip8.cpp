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
    switch (op.code & 0xf000)
    {
        case 0x0000:
            switch (op.code)
            {
                case 0x00e0:
                    std::fill_n(pixels, 256, 0);
                    drawFlag = true;
                    break;

                case 0x00ee: pc = (u16*) &memory[*(--sp)]; break;

                default:
                    printf_s("Unknow opcode [0000]: %x\n", op.code);
                    pc--;
                    break;
            }
            break;

        case 0x1000: pc = (u16*) (&memory[op.nnn]); break;

        case 0x2000:
            *sp++ = (u8*) pc - memory;
            pc = (u16*) &memory[op.nnn];
            break;

        case 0x3000:
            if (registers[op.x] == op.nn) pc++;
            break;

        case 0x4000:
            if (registers[op.x] != op.nn) pc++;
            break;

        case 0x5000:
            if (op.n != 0)
            {
                printf_s("Unknown opcode [5000]: %x", op.code);
                pc--;
                break;
            }
            if (registers[op.x] == registers[op.y]) pc++;
            break;

        case 0x6000: registers[op.x] = op.nn; break;

        case 0x7000: registers[op.x] += op.nn; break;

        case 0x8000:
            switch (op.code & 0x000f)
            {
                case 0x0000: registers[op.x] = registers[op.y]; break;

                case 0x0001: registers[op.x] |= registers[op.y]; break;

                case 0x0002: registers[op.x] &= registers[op.y]; break;

                case 0x0003: registers[op.x] ^= registers[op.y]; break;

                case 0x0004:
                    registers[0xf] = 0;
                    if (registers[op.y] > (0xff - registers[op.x]))
                        registers[0xf] = 1;
                    registers[op.x] += registers[op.y];
                    break;

                case 0x0005:
                    registers[0xf] = 0;
                    if (registers[op.x] > registers[op.y]) registers[0xf] = 1;
                    registers[op.x] -= registers[op.y];
                    break;

                case 0x0006:
                    registers[0xf] = registers[op.x] & 0b1;
                    registers[op.x] >>= 1;
                    break;

                case 0x0007:
                    registers[0xf] = 0;
                    if (registers[op.y] > registers[op.x]) registers[0xf] = 1;
                    registers[op.x] = registers[op.y] - registers[op.x];
                    break;

                case 0x000e:
                    registers[0xf] = registers[op.x] >> 7;
                    registers[op.x] <<= 1;
                    break;

                default:
                    printf_s("Unknown opcode [8000]: %x\n", op.code);
                    pc--;
                    break;
            }
            break;

        case 0x9000:
            if (op.n != 0)
            {
                printf_s("Unknown opcode [9000]: %x\n", op.code);
                pc--;
                break;
            }
            if (registers[op.x] != registers[op.y]) pc++;
            break;

        case 0xa000: savedAddress = &memory[op.nnn]; break;

        case 0xb000: pc = (u16*) &memory[op.nnn + registers[0x0]]; break;

        case 0xc000: registers[op.x] = (rand() % 255) & op.nn; break;

        case 0xd000:
            registers[0xf] = 0;
            drawFlag = true;

            for (int y = registers[op.y], i = 0; i < op.n; y++, i++)
            {
                auto sprite = savedAddress[i];
                auto x = registers[op.x];
                auto xByte = x / 8;
                auto shiftAmount = x % 8;
                auto yoff = y;
                if (y >= 32) yoff = y - 32;
                auto pixLoc = xByte + (yoff * 8);

                if ((pixels[pixLoc] & (sprite >> shiftAmount)) != 0)
                    registers[0xf] = 1;
                pixels[pixLoc] ^= sprite >> shiftAmount;

                if (xByte % 8 == 7) pixLoc = (yoff * 8) - 1;
                pixLoc++;
                shiftAmount = 8 - shiftAmount;
                if ((pixels[pixLoc] & (sprite << shiftAmount)) != 0)
                    registers[0xf] = 1;
                pixels[pixLoc] ^= sprite << shiftAmount;
            }
            break;

        case 0xe000:
            switch (op.code & 0x00ff)
            {
                case 0x009e:
                    if (keys[registers[op.x]]) pc++;
                    break;
                case 0x00a1:
                    if (!keys[registers[op.x]]) pc++;
                    break;

                default:
                    printf_s("Unknown opcode [e000]: %x\n", op.code);
                    pc--;
                    break;
            }
            break;

        case 0xf000:
            switch (op.code & 0x00ff)
            {
                case 0x0007: registers[op.x] = delayTimer; break;

                case 0x000a:
                {
                    bool canResume = false;
                    for (auto i = 0; i < 0x10; i++)
                    {
                        if (keys[i] != 0)
                        {
                            registers[op.x] = i;
                            canResume = true;
                        }
                    }
                    if (!canResume) pc--;
                }
                break;

                case 0x0015: delayTimer = registers[op.x]; break;

                case 0x0018: soundTimer = registers[op.x]; break;

                case 0x001e: savedAddress += registers[op.x]; break;

                case 0x0029: savedAddress = font[registers[op.x]]; break;

                case 0x0033:
                    savedAddress[0] = registers[op.x] / 100;
                    savedAddress[1] = (registers[op.x] / 10) % 10;
                    savedAddress[2] = registers[op.x] % 10;
                    break;

                case 0x0055:
                    std::copy_n(registers, op.x + 1, savedAddress);
                    break;

                case 0x0065:
                    std::copy_n(savedAddress, op.x + 1, registers);
                    break;

                default:
                    printf_s("Unknow opcode [f000]: %x\n", op.code);
                    pc--;
                    break;
            }
            break;

        default:
            printf_s("Unknown opcode: %x\n", op.code);
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
    code = (*opByte << 8) | *(opByte + 1);
    x = (code & 0x0f00) >> 8;
    y = (code & 0x00f0) >> 4;
    n = code & 0x000f;
    nn = code & 0x00ff;
    nnn = code & 0x0fff;
}
