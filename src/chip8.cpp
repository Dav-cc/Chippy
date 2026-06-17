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
  // 1000 xxxx yyyy 0110
  uint8_t X = (opcode & 0x0F00) >> 8u;
  uint8_t Y = (opcode & 0x00F0) >> 4u;
  if (registers[X] & 0x0001) {
    registers[0xF] = 1;
  } else {
    registers[0xF] = 0;
  }
  registers[X] /= 2;
}

// set Vx = Vy - Vx set Vf = not borrow
void Chip8::OP_8xy7() {
  // 1000 xxxx yyyy 0111
  uint8_t X = (opcode & 0x0F00) >> 8u;
  uint8_t Y = (opcode & 0x00F0) >> 4u;
  if (registers[Y] > registers[X]) {
    registers[0xF] = 1;
  } else {
    registers[0xF] = 0;
  }
  registers[X] = registers[Y] - registers[X];
}

// set Vx = Vx SHL 1
void Chip8::OP_8xyE() {
  // 1000 xxxx yyyy EEEE
  uint8_t X = (opcode & 0x0F00) >> 8u;
  uint8_t Y = (opcode & 0x00F0) >> 4u;

  registers[0xF] = (registers[X] & 0x80) >> 7u;
  registers[X] >>= 1; // *2
}

// skipp next instruxtion if Vx != Vy
void Chip8::OP_9xy0() {
  // 1000 xxxx yyyy EEEE
  uint8_t X = (opcode & 0x0F00) >> 8u;
  uint8_t Y = (opcode & 0x00F0) >> 4u;
  if (registers[Y] != registers[X]) {
    pc += 2;
  }
}

// set Index register to nnn
void Chip8::OP_Annn() {
  uint16_t address = (opcode & 0x0FFF);
  index = address;
}

// jump to location nnn +V0
void Chip8::OP_Bnnn() {
  uint16_t address = (opcode & 0x0FFF);
  pc = address + registers[0];
}

// load Vx  random bytes & kk
void Chip8::OP_Cxkk() {
  uint8_t X = (opcode & 0x0F00) >> 8u;
  uint8_t kk = (opcode & 0x00FF);
  registers[X] = randByte(randGen) & kk;
}

//  Display n-byte sprite starting at memory location I at (Vx, Vy)
//  set VF = collision.
void Chip8::OP_Dxyn() {
  uint8_t X = (opcode & 0x0F00) >> 8u;
  uint8_t Y = (opcode & 0x00F0) >> 4u;
  uint8_t byte = (opcode & 0x000F);

  // we wrap this positions up so if Vx was bigger than WEISTH_PIXLE_SIZE
  // or HEIGHT_PIXLE_SIZE we could able to draw sprite
  uint8_t xs = registers[X] % WEIDTH_PIXEL_SIZE;
  uint8_t ys = registers[Y] % HEIGHT_PIXEL_SIZE;

  // if collision accurd this flage sets to 1
  registers[0xF] = 0;

  // TODO :IMPLEMENT THIS FLAGE LATER
}

// skipped next intruction if key with value of Vx is pressed
void Chip8::OP_Ex9E() {
  uint8_t X = (opcode & 0x0F00) >> 8u;
  uint8_t pressedkey = registers[X];

  if (keypad[pressedkey]) {
    pc += 2;
  }
}

// skipped next intruction if key with value of Vx is not pressed
void Chip8::OP_ExA1() {
  uint8_t X = (opcode & 0x0F00) >> 8u;
  uint8_t pressedkey = registers[X];

  if (!keypad[pressedkey]) {
    pc += 2;
  }
}

// Set Vx = delay timer value.
void Chip8::OP_Fx07() {
  uint8_t X = (opcode & 0x0F00) >> 8u;
  registers[X] = dt;
}

// which key is pressed set it to X register
void Chip8::OP_Fx0A() {
  uint8_t X = (opcode & 0x0F00) >> 8u;
  if (keypad[0]) {
    registers[X] = 0;
  } else if (keypad[1]) {
    registers[X] = 1;
  } else if (keypad[2]) {
    registers[X] = 2;
  } else if (keypad[3]) {
    registers[X] = 3;
  } else if (keypad[4]) {
    registers[X] = 4;
  } else if (keypad[5]) {
    registers[X] = 5;
  } else if (keypad[6]) {
    registers[X] = 6;
  } else if (keypad[7]) {
    registers[X] = 7;
  } else if (keypad[8]) {
    registers[X] = 8;
  } else if (keypad[9]) {
    registers[X] = 9;
  } else if (keypad[10]) {
    registers[X] = 10;
  } else if (keypad[11]) {
    registers[X] = 11;
  } else if (keypad[12]) {
    registers[X] = 12;
  } else if (keypad[13]) {
    registers[X] = 13;
  } else if (keypad[14]) {
    registers[X] = 14;
  } else if (keypad[15]) {
    registers[X] = 15;
  }
}

// set dellay timer to X
void Chip8::OP_Fx15(){
    uint8_t X = (opcode & 0x0F00) >> 8u;
    dt = registers[X];
}

// set dellay timer to X
void Chip8::OP_Fx18(){
    uint8_t X = (opcode & 0x0F00) >> 8u;
    st = registers[X];
}

// add VX to I
void Chip8::OP_Fx1E(){
    uint8_t X = (opcode & 0x0F00) >> 8u;
    index += registers[X];
}

