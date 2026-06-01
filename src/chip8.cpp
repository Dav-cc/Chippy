#include "chip8.hpp"

void Chip8::LoadRom(char const *filename) {
  // Open the file as a stream of binary and move the file pointer to the end
  std::ifstream file(filename, std::ios::binary | std::ios::ate);

  if (file.is_open()) {
    // Get size of file and allocate a buffer to hold the contents
    std::streampos size = file.tellg();
    char *buffer = new char[size];

    // Go back to the beginning of the file and fill the buffer
    file.seekg(0, std::ios::beg);
    file.read(buffer, size);
    file.close();

    // Load the ROM contents into the Chip8's memory, starting at 0x200
    for (long i = 0; i < size; ++i) {
      memory[START_ADDRESS + i] = buffer[i];
    }

    // Free the buffer
    delete[] buffer;
  }
}

// clear screen
// caus 0 in display pixles is off so we set all pixles to 0 to clear the screen
void Chip8::OP_00E0() { display.fill(0); }

// return from subroutine
void Chip8::OP_00EE() {
  --sp;
  pc = stack[sp];
}

/*
    jump to address nnn
    jump dont remmember it's origin so we dont change this in stack
*/
void Chip8::OP_1nnn() {
  // we only need last 12 bit of address
  uint16_t address = (opcode & 0x0FFF);
  pc = address;
}

// call subroutin at nnn
void Chip8::OP_2nnn() {
  // again only 12 LSB of add
  uint16_t address = (opcode & 0x0FFF);
  // we incremented pc in cycle
  stack[sp] = pc;
  sp++;
  pc = address;
}

// skipp nexi op if Vx(register[x]) = kk
void Chip8::OP_3xkk() {
  // 0011 xxxx kkkkkkkk
  uint8_t reg = (opcode & 0x0F00) >> 8u;
  // b b b b b b b b b b b b x x x x
  uint8_t val = (opcode & 0x00FF);
  if (registers[reg] == val) {
    pc += 2;
  }
}

// skipp nexi op if Vx(register[x]) != kk
void Chip8::OP_4xkk() {
  // 0100 xxxx kkkkkkkk
  uint8_t reg = (opcode & 0x0F00) >> 8u;
  // b b b b b b b b b b b b x x x x
  uint8_t val = (opcode & 0x00FF);
  if (registers[reg] != val) {
    pc += 2;
  }
}

// skip next instruction if Vx == Vy
void Chip8::OP_5xy0() {
  // 0 1 0 1 x x x x y y y y 0 0 0 0
  uint8_t X = (opcode & 0x0F00) >> 8u;
  uint8_t Y = (opcode & 0x00F0) >> 8u;
  if (registers[X] == registers[Y]) {
    pc += 2;
  }
}

// set Vx= kk
void Chip8::OP_6xkk() {
  // 0110 xxx kkkk kkkk
  uint8_t X = (opcode & 0x0F00) >> 8u;
  uint8_t value = (opcode & 0x00FF);
  registers[X] = value;
}

// set Vx = Vx + kk
void Chip8::OP_7xkk() {
  // 0111 xxx kkkk kkkk
  uint8_t X = (opcode & 0x0F00) >> 8u;
  uint8_t value = (opcode & 0x00FF);
  registers[X] += value;
}

// set Vx = Vy
void Chip8::OP_8xy0() {
  // 1000 xxxx yyyy 0000
  uint8_t X = (opcode & 0x0F00) >> 8u;
  uint8_t Y = (opcode & 0x00F0) >> 4u;
  registers[X] = registers[Y];
}

// set Vx = Vx | Vy
void Chip8::OP_8xy1() {
  // 1000 xxxx yyyy 0001
  uint8_t X = (opcode & 0x0F00) >> 8u;
  uint8_t Y = (opcode & 0x00F0) >> 4u;
  registers[X] |= registers[Y];
}

// set Vx = Vx & Vy
void Chip8::OP_8xy2() {
  // 1000 xxxx yyyy 0010
  uint8_t X = (opcode & 0x0F00) >> 8u;
  uint8_t Y = (opcode & 0x00F0) >> 4u;
  registers[X] &= registers[Y];
}

// set Vx = Vx XOR Vy
void Chip8::OP_8xy3() {
  // 1000 xxxx yyyy 0011
  uint8_t X = (opcode & 0x0F00) >> 8u;
  uint8_t Y = (opcode & 0x00F0) >> 4u;
  registers[X] ^= registers[Y];
}

// set Vx = Vx + Vy then set VF = carry
void Chip8::OP_8xy4() {
  // 1000 xxxx yyyy 0100
  uint8_t X = (opcode & 0x0F00) >> 8u;
  uint8_t Y = (opcode & 0x00F0) >> 4u;
  uint16_t sum = registers[X] + registers[Y];
  if (sum > 255u) {
    registers[0xF] = 1;
  } else {
    registers[0xF] = 0;
  }
  registers[X] = sum & 0x00FFu;
}

// set Vx = Vx - Vy set VF= NOT BORROW, if Vx > Vy then VF set to 1 otherwise 0
void Chip8::OP_8xy5() {
  // 1000 xxxx yyyy 0101
  uint8_t X = (opcode & 0x0F00) >> 8u;
  uint8_t Y = (opcode & 0x00F0) >> 4u;
  if (registers[X] > registers[Y]) {
    registers[0xF] = 1;
  } else {

    registers[0xF] = 0;
  }
  registers[X] -= registers[Y];
}

// set Vx = Vx SHR 1
void Chip8::OP_8xy6() {
  // 1000 xxxx yyyy 0111
  uint8_t X = (opcode & 0x0F00) >> 8u;
  uint8_t Y = (opcode & 0x00F0) >> 4u;
  if (registers[X] & 0x0001) {
    registers[0xF] = 1;
  } else {
    registers[0xF] = 0;
  }
    registers[X]/=2;
}
