#include <iostream>
#include <stdint.h>
#include "instruction.h"

struct dummy {
  uint64_t ip;
  uint64_t id;
  uint32_t op;
  uint16_t category;
  uint16_t branch_info;
  uint8_t reg[NUM_INSTR_DESTINATIONS];
  uint8_t mem[NUM_INSTR_DESTINATIONS];
  uint64_t reg1[4];
  uint64_t reg2[4];
};

using namespace std;
int main(uint64_t argc, char* argv[]) {
  input_instr instr;
  cout << sizeof(dummy) << endl;
  cout << sizeof(argc) << endl;
  cout << sizeof(instr) << endl;
  cout << sizeof(input_instr) << endl;

  return 0;
}
