#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include "instruction.h"

int
main(int argc, char* argv[]) {
  std::ifstream in(argv[1], std::ifstream::ate | std::ifstream::binary);
  std::cout.imbue(std::locale(""));
  std::cout << "File size: " << std::fixed << in.tellg() << " bytes"
            << std::endl;
  std::cout << "Trace size: " << std::fixed << in.tellg() / sizeof(input_instr)
            << " instructions" << std::endl;
  return 0;
}
