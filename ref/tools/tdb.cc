#include <unistd.h>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <readline/history.h>
#include <readline/readline.h>

#include "instruction.h"

uint64_t instruction_index = 0;
std::map<uint64_t, std::vector<std::string>> dump;

void
printi(const input_instr& instruction) {
  std::cout << NUM_INSTR_SOURCES << std::endl;
  std::cout << std::endl;
  std::cout << "index: " << std::dec << instruction_index << std::endl;
  std::cout << "counter: " << std::dec << instruction_index + 1 << std::endl;
  std::cout << "ip: " << std::hex << instruction.ip % (1 << 24) << std::endl;
  std::cout << "branch_info: " << (uint32_t)instruction.branch_info << std::endl;

  std::cout << "dst_reg: ";
  for (int i = 0; i < NUM_INSTR_DESTINATIONS; i++) {
    std::cout << (uint32_t)instruction.destination_registers[i] << " ";
  }
  std::cout << std::endl;

  std::cout << "src_reg: ";
  for (int i = 0; i < NUM_INSTR_SOURCES; i++) {
    std::cout << (uint32_t)instruction.source_registers[i] << " ";
  }
  std::cout << std::endl;

  std::cout << "dst_mem: ";
  for (int i = 0; i < NUM_INSTR_DESTINATIONS; i++) {
    std::cout << instruction.destination_memory[i] << " ";
  }
  std::cout << std::endl;

  std::cout << "src_mem: ";
  for (int i = 0; i < NUM_INSTR_SOURCES; i++) {
    std::cout << instruction.source_memory[i] << " ";
  }
  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << "SOURCE" << std::endl;

  std::vector<std::string> source = dump[instruction.ip % (1 << 24)];

  for (const auto& line : source) {
    std::cout << line;
  }

  std::cout << std::endl;
}

inline bool
readi(input_instr& instruction, FILE* trace, bool print) {
  if (!fread(&instruction, sizeof(input_instr), 1, trace)) {
    std::cout << "ERROR: no more instructions" << std::endl;
    return false;
  }

  if (print) {
    printi(instruction);
  }

  instruction_index++;

  return true;
}

FILE*
openi(std::string file) {
  std::cout << "Reading trace file " << file << std::endl;
  std::stringstream ss;
  ss << "gunzip -c " << file;
  return popen(ss.str().c_str(), "r");
}

int
main(int argc, char* argv[]) {
  std::set<uint64_t> bp_ip;
  std::set<uint64_t> bp_idx;

  read_history(".tdb_history");

  FILE* trace = nullptr;
  FILE* objdump = nullptr;

  std::regex objex("\\s+([0-9a-fA-F]+):");

  input_instr instruction;

  std::string last_line = "n";
  while (true) {
    std::string line(readline("> "));
    if (line == "") {
      line = last_line;
    }

    if (line != last_line) {
      add_history(line.c_str());
      write_history(".tdb_history");
    }

    if (line == "h" or line == "help") {
      std::cout << "Commands" << std::endl;
      std::cout << " f: load trace file" << std::endl;
      std::cout << " o: load objdump file" << std::endl;
      std::cout << " n: next instruction" << std::endl;
      std::cout << " b: see breakpoints" << std::endl;
      std::cout << " b [HEX]: set IP breakpoint" << std::endl;
      std::cout << " b [INT]: set IP index breakpoint" << std::endl;
      std::cout << " d [HEX]: delete IP breakpoint" << std::endl;
      std::cout << " d [INT]: delete IP index breakpoint" << std::endl;
      std::cout << " r: run until next breakpoint" << std::endl;
      std::cout << " q: quit" << std::endl;
    }

    else if (line == "q" or line == "Q") {
      std::cout << "Goodbye!" << std::endl;
      exit(0);
    }

    else if (line.substr(0, 1) == "f") {
      if (trace != nullptr) {
        pclose(trace);
      }
      trace = openi(line.substr(2));
    }

    else if (line.substr(0, 1) == "o") {
      std::stringstream ss;
      ss << "cat " << line.substr(2);
      objdump = popen(ss.str().c_str(), "r");

      // Hold onto disassembly if we don't see next chunk of source
      bool prev_ip = false;

      ssize_t read;
      size_t len = 0;
      char* cline = NULL;
      std::vector<std::string> objlines;
      while ((read = getline(&cline, &len, objdump)) != -1) {
        std::string objline(cline);
        objlines.push_back(objline);

        std::smatch match;
        if (std::regex_search(objline, match, objex)) {
          uint64_t ip = std::stoi(match.str(1), nullptr, 16);
          dump[ip] = objlines;
          prev_ip = true;
        } else {
          if (prev_ip) {
            objlines.resize(0);
          }
          prev_ip = false;
        }
      }

      if (cline) {
        free(cline);
      }
    }

    else if (trace == nullptr) {
      std::cout << "ERROR: no trace file loaded" << std::endl;
    }

    else if (line == "n") {
      readi(instruction, trace, true);
    }

    else if (line == "b") {
      if (bp_ip.size() > 0) {
        std::cout << "Instruction pointer breakpoints" << std::endl;
        for (const auto ip : bp_ip) {
          std::cout << std::hex << " 0x" << ip << std::endl;
        }
      }
      if (bp_idx.size() > 0) {
        std::cout << "Instruction idxber breakpoints" << std::endl;
        for (const auto idx : bp_idx) {
          std::cout << std::dec << " " << idx << std::endl;
        }
      }

      if (bp_ip.size() == 0 and bp_idx.size() == 0) {
        std::cout << "WARNING: No breakpoints" << std::endl;
      }
    }

    else if (line == "d") {
      continue;
    }

    else if (line.substr(0, 2) == "b ") {
      uint64_t bp = 0;
      try {
        if (line.substr(2, 2) == "0x") {
          bp = std::stoi(line.substr(2), nullptr, 16);
          bp = bp % (1 << 24);
          bp_ip.insert(bp);
        } else {
          bp = std::stoi(line.substr(2));
          bp_idx.insert(bp);
        }
      } catch (int e) {
        std::cout << "ERROR: Unable to parse breakpoint" << std::endl;
      }
    }

    else if (line.substr(0, 2) == "d ") {
      uint64_t bp = 0;
      try {
        if (line.substr(2, 2) == "0x") {
          bp = std::stoi(line.substr(2), nullptr, 16);
          bp = bp % (1 << 24);
          bp_ip.erase(bp);
        } else if (line.substr(2, 1) == "a") {
          bp_ip.clear();
          bp_idx.clear();
        } else {
          bp = std::stoi(line.substr(2));
          bp_idx.erase(bp);
        }
      } catch (int e) {
        std::cout << "ERROR: Unable to parse breakpoint" << std::endl;
      }
    }

    else if (line == "r") {
      if (bp_ip.size() == 0) {
        if (bp_idx.size() == 0) {
          continue;
        }

        if (*bp_idx.rbegin() < instruction_index) {
          continue;
        }

        for (const auto idx : bp_idx) {
          if (idx > instruction_index) {
            if (fseek(trace, idx * sizeof(input_instr), SEEK_SET) != 0) {
              std::cout << "ERROR: beyond end of trace" << std::endl;
              break;
            }
            instruction_index = idx - 1;
            readi(instruction, trace, true);
            break;
          }
        }
        continue;
      }
      while (true) {
        if (bp_idx.find(instruction_index) != bp_idx.end() or
            bp_ip.find(instruction.ip % (1 << 24)) != bp_ip.end()) {
          printi(instruction);
          break;
        }

        if (!readi(instruction, trace, false)) {
          break;
        }

        if (instruction_index % 10000000 == 0) {
          std::cout << std::dec << "Read " << instruction_index
                    << " instructions" << std::endl;
        }
      }
    }

    else {
      std::cout << "ERROR: Unknown command '" << line << "'" << std::endl;
    }

    last_line = line;
  }

  pclose(trace);
  pclose(objdump);

  return 0;
}
