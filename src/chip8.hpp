#ifndef __CHIPPY_H_
#define __CHIPPY_H_

#include <array>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <random>

const uint KBYTE = 1024;
const uint START_ADDRESS = 0x200;
const uint FONTSET_SIZE = 80;
const uint FONTSET_START_ADDRESS = 0x50;

#define WEIDTH_PIXEL_SIZE 32
#define HEIGHT_PIXEL_SIZE 64

class Chip8 {
public:
  // 16 general purpose 8-bit registers, named V0 to VF (F is hex digit) The VF
  // register is also used as a flag by some instructions
  std::array<uint8_t, 16> registers;

  /*
      4kb memory in chip8
      from 0x000 to 0xFFF (4096 bytes)
      - from 0x00 to 0x200 reserved for interpreter (fontset and other data
     storred here (FONTSET_START_ADDRESS = 0x50))
      - 0x200 starts the most programs, so we will load the ROM from 0x200
      - from 0x600 to 0xFFF reserved for user program and work RAM
  */
  std::array<uint8_t, 4 * KBYTE> memory;

  /*
     index register (used to store memory addresses for some instructions)
     the only 12 lower bits are used, so it can only have a value from 0x000
     to 0xFFF (12 bits)
  */
  uint16_t index{};

  /*
    program counter (stores the currently executing address in memory)
    and cause memory address is 12 bit we only use 12 bits of this reg
   */
  uint16_t pc{};

  // stack is 16 16-bit values which are used to store return addresses when
  // subroutines called
  std::array<uint16_t, 16> stack;

  /*

   this register always points to the top of the stack so if we want add
   something to stack after adding we increment this and if pop we decrement
   this register after that

  */
  uint8_t sp{};

  // delay and sound timer which are decremented at a rate of 60Hz until they
  // reach 0
  uint8_t dt{};
  uint8_t st{};

  // the chip8's keypad waas 16 keys from 0 to 9 and a to f(hx degits) we will
  // store the state of each key in this array (1 for pressed and 0 for not
  // pressed)
  std::array<uint8_t, 16> keypad;

  /*
    the first chip8 display was 64 in 32 bit monochrome display (1 for pixel on
    and 0 for pixel off) we will store the state of each pixel in this array
    (again 1 for on and 0 for off like monochrom displays)
  */
  std::array<uint32_t, HEIGHT_PIXEL_SIZE * WEIDTH_PIXEL_SIZE> display;
  // uint8_t display[HEIGHT_PIXEL_SIZE][WEIDTH_PIXEL_SIZE];

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

  /*
     chip 8 has 36 instcutions and Schip8(super chip8 has 10 more instructions
     total 46) each instruction is 2 bytes long and stored in memory as
     big-endian
  */
  // void OP_0nnn();   this instruction ignored by most modern interpreters and emulators
  void OP_00E0();
  void OP_00EE();
  void OP_1nnn();
  void OP_2nnn();
  void OP_3xkk();
  void OP_4xkk();
  void OP_5xy0();
  void OP_6xkk();
  void OP_7xkk();
  void OP_8xy0();
  void OP_8xy1();
  void OP_8xy2();
  void OP_8xy3();
  void OP_8xy4();
  void OP_8xy5();
  void OP_8xy6();
  void OP_8xy7();
  void OP_8xyE();
  void OP_9xy0();
  void OP_Annn();
  void OP_Bnnn();
  void OP_Cxkk();
  void OP_Dxyn();
  void OP_Ex9E();
  void OP_ExA1();
  void OP_Fx07();
  void OP_Fx0A();
  void OP_Fx15();
  void OP_Fx18();
  void OP_Fx1E();
  void OP_Fx29();
  void OP_Fx33();
  void OP_Fx55();
  void OP_Fx65();
  // // exits chip8 with value of N
  // void OP_001N();
  // // scroll display N lines down
  // void OP_00CN();

  std::default_random_engine randGen;
  std::uniform_int_distribution<uint8_t> randByte;
};

#endif // __CHIPPY_H_
