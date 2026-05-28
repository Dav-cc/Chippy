#ifndef __CHIPPY_H_
#define __CHIPPY_H_

#include <array>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <random>

const uint START_ADDRESS = 0x200;
const uint FONTSET_SIZE = 80;
const uint FONTSET_START_ADDRESS = 0x50;

#define WEIDTH_PIXEL_SIZE 32
#define HEIGHT_PIXEL_SIZE 64

class Chip8 {
public:
  // 16 8-bit register
  std::array<uint8_t, 16> registers;

  // all memory we have (4KB)
  std::array<uint8_t, 4096> memory;

  // index register
  uint16_t index{};

  // program counter register
  uint16_t pc{};

  // 16 level stack
  std::array<uint16_t, 16> stack;

  // stack pointer
  uint8_t sp{};

  // delay and sound timer
  uint8_t delayTimer{};
  uint8_t soundTimer{};

  // keypad
  std::array<uint8_t, 16> keypad;

  // video (2D array of pixels which can be 1(on) or 0(off))
  std::array<uint32_t, HEIGHT_PIXEL_SIZE * WEIDTH_PIXEL_SIZE> video;

  // opration code
  uint16_t opcode;

  Chip8()
      : randGen(std::chrono::system_clock::now().time_since_epoch().count()) {
    // set the pc register to lowest memory address for startup
    pc = START_ADDRESS;

    uint8_t fontset[FONTSET_SIZE] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    for (unsigned int i = 0; i < FONTSET_SIZE; ++i) {
      memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }
    randByte = std::uniform_int_distribution<uint8_t>(0, 255u);
  }
  void LoadRom(char const *filename);
  void OP_00E0();
  void OP_00EE();

  std::default_random_engine randGen;
  std::uniform_int_distribution<uint8_t> randByte;
};

#endif // __CHIPPY_H_
