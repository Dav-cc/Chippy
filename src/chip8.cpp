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


// skipp nexi op if Vx(register[x]) = kk
void Chip8::OP_4xkk() {
  // 0100 xxxx kkkkkkkk
  uint8_t reg = (opcode & 0x0F00) >> 8u;
  // b b b b b b b b b b b b x x x x
  uint8_t val = (opcode & 0x00FF);
  if (registers[reg] != val) {
    pc += 2;
  }
}
