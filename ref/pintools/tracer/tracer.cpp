
/*! @file
 *  This is an example of the PIN tool that demonstrates some basic PIN APIs
 *  and could serve as the starting point for developing your first PIN tool
 */

#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <string>
#include "pin.H"
#include "xed/xed-category-enum.h"

#define NUM_INSTR_DESTINATIONS 2
#define NUM_INSTR_SOURCES 4

// RESOURCES
// https://software.intel.com/sites/landingpage/pintool/docs/81205/Pin/html/group__INS__BASIC__API__GEN__IA32.html
// https://software.intel.com/sites/landingpage/pintool/docs/81205/Pin/html/group__RTN__BASIC__API.html
// https://software.intel.com/sites/landingpage/pintool/docs/97503/Pin/html/group__INST__ARGS.html

// To collect
// - category
// - opcode
// - branching
//   - is branch
//   - is call
//   - taken / not taken
//   - direct / indirect
//   - conditional / unconditional
//   - forward / backwards
// - routines

struct BRANCH {
  static const uint8_t branch = 0b00000001;
  static const uint8_t taken = 0b00000010;
  static const uint8_t call = 0b00000100;
  static const uint8_t direct = 0b00001000;
  static const uint8_t cond = 0b00010000;
  static const uint8_t fwd = 0b00100000;
  static const uint8_t ret = 0b01000000;

  static void
  print(uint8_t info) {
    cout << "Branch: ";
    if (info & branch) {
      cout << "true" << endl;
    } else {
      cout << "false" << endl;
    }

    cout << "Taken: ";
    if (info & taken) {
      cout << "true" << endl;
    } else {
      cout << "false" << endl;
    }

    cout << "Call: ";
    if (info & call) {
      cout << "true" << endl;
    } else {
      cout << "false" << endl;
    }

    cout << "Direct: ";
    if (info & direct) {
      cout << "true" << endl;
    } else {
      cout << "false" << endl;
    }

    cout << "Conditional: ";
    if (info & cond) {
      cout << "true" << endl;
    } else {
      cout << "false" << endl;
    }

    cout << "Forward: ";
    if (info & fwd) {
      cout << "true" << endl;
    } else {
      cout << "false" << endl;
    }

    cout << "Return: ";
    if (info & ret) {
      cout << "true" << endl;
    } else {
      cout << "false" << endl;
    }
  }
};

class input_instr {
 public:
  // instruction pointer or PC (Program Counter)
  uint64_t ip;

  // globally-unique routine id
  uint64_t routine_id;

  // instruction category
  // See https://intelxed.github.io/ref-manual/xed-category-enum_8h.html
  uint8_t category;

  // branch info
  uint8_t branch_info;

  uint8_t destination_registers[NUM_INSTR_DESTINATIONS]; // output registers
  uint8_t source_registers[NUM_INSTR_SOURCES]; // input registers

  uint64_t destination_memory[NUM_INSTR_DESTINATIONS]; // output memory
  uint64_t source_memory[NUM_INSTR_SOURCES]; // input memory

  input_instr() {
    ip = 0;
    routine_id = 0;
    category = 0;
    branch_info = 0;

    for (uint32_t i = 0; i < NUM_INSTR_SOURCES; i++) {
      source_registers[i] = 0;
      source_memory[i] = 0;
    }

    for (uint32_t i = 0; i < NUM_INSTR_DESTINATIONS; i++) {
      destination_registers[i] = 0;
      destination_memory[i] = 0;
    }
  };
};

/* ================================================================== */
// Global variables
/* ================================================================== */

UINT64 instrCount = 0;

FILE* out;

bool output_file_closed = false;
bool tracing_on = false;

input_instr curr_instr;

/* ===================================================================== */
// Command line switches
/* ===================================================================== */
KNOB<string> KnobOutputFile(
    KNOB_MODE_WRITEONCE,
    "pintool",
    "o",
    "champsim.trace",
    "specify file name for Champsim tracer output");

KNOB<UINT64> KnobSkipInstructions(
    KNOB_MODE_WRITEONCE,
    "pintool",
    "s",
    "0",
    "How many instructions to skip before tracing begins");

KNOB<UINT64> KnobTraceInstructions(
    KNOB_MODE_WRITEONCE,
    "pintool",
    "t",
    "1000000",
    "How many instructions to trace");

/* ===================================================================== */
// Utilities
/* ===================================================================== */

/*!
 *  Print out help message.
 */
INT32
Usage() {
  cerr << "This tool creates a register and memory access trace" << endl
       << "Specify the output trace file with -o" << endl
       << "Specify the number of instructions to skip before tracing with -s"
       << endl
       << "Specify the number of instructions to trace with -t" << endl
       << endl;

  cerr << KNOB_BASE::StringKnobSummary() << endl;

  return -1;
}

/* ===================================================================== */
// Analysis routines
/* ===================================================================== */

void
BeginInstruction(VOID* ip, UINT32 op_code, UINT32 category) {
  instrCount++;
  if (instrCount > KnobSkipInstructions.Value()) {
    tracing_on = true;

    if (instrCount >
        (KnobTraceInstructions.Value() + KnobSkipInstructions.Value()))
      tracing_on = false;
  }

  if (!tracing_on)
    return;

  // reset the current instruction
  curr_instr.ip = (unsigned long long int)ip;
  curr_instr.routine_id = 0;

  curr_instr.branch_info = 0;
  curr_instr.category = category;

  for (int i = 0; i < NUM_INSTR_DESTINATIONS; i++) {
    curr_instr.destination_registers[i] = 0;
    curr_instr.destination_memory[i] = 0;
  }

  for (int i = 0; i < NUM_INSTR_SOURCES; i++) {
    curr_instr.source_registers[i] = 0;
    curr_instr.source_memory[i] = 0;
  }
}

void
EndInstruction() {
  if (instrCount > KnobSkipInstructions.Value()) {
    tracing_on = true;

    if (instrCount <=
        (KnobTraceInstructions.Value() + KnobSkipInstructions.Value())) {
      // keep tracing
      fwrite(&curr_instr, sizeof(input_instr), 1, out);
    } else {
      tracing_on = false;
      // close down the file, we're done tracing
      if (!output_file_closed) {
        fclose(out);
        output_file_closed = true;
      }

      exit(0);
    }
  }
}

void
BranchHandler(UINT32 is_taken) {
  if (is_taken != 0) {
    curr_instr.branch_info |= BRANCH::taken;
  }
}

void
RegRead(UINT32 i, UINT32 index) {
  if (!tracing_on)
    return;

  REG r = (REG)i;

  // check to see if this register is already in the list
  int already_found = 0;
  for (int i = 0; i < NUM_INSTR_SOURCES; i++) {
    if (curr_instr.source_registers[i] == ((unsigned char)r)) {
      already_found = 1;
      break;
    }
  }
  if (already_found == 0) {
    for (int i = 0; i < NUM_INSTR_SOURCES; i++) {
      if (curr_instr.source_registers[i] == 0) {
        curr_instr.source_registers[i] = (unsigned char)r;
        break;
      }
    }
  }
}

void
RegWrite(REG i, UINT32 index) {
  if (!tracing_on)
    return;

  REG r = (REG)i;

  int already_found = 0;
  for (int i = 0; i < NUM_INSTR_DESTINATIONS; i++) {
    if (curr_instr.destination_registers[i] == ((unsigned char)r)) {
      already_found = 1;
      break;
    }
  }
  if (already_found == 0) {
    for (int i = 0; i < NUM_INSTR_DESTINATIONS; i++) {
      if (curr_instr.destination_registers[i] == 0) {
        curr_instr.destination_registers[i] = (unsigned char)r;
        break;
      }
    }
  }
  /*
     if(index==0)
     {
     curr_instr.destination_register = (unsigned long long int)r;
     }
     */
}

void
MemoryRead(VOID* addr, UINT32 index, UINT32 read_size) {
  if (!tracing_on)
    return;

  // check to see if this memory read location is already in the list
  int already_found = 0;
  for (int i = 0; i < NUM_INSTR_SOURCES; i++) {
    if (curr_instr.source_memory[i] == ((unsigned long long int)addr)) {
      already_found = 1;
      break;
    }
  }
  if (already_found == 0) {
    for (int i = 0; i < NUM_INSTR_SOURCES; i++) {
      if (curr_instr.source_memory[i] == 0) {
        curr_instr.source_memory[i] = (unsigned long long int)addr;
        break;
      }
    }
  }
}

void
MemoryWrite(VOID* addr, UINT32 index) {
  if (!tracing_on)
    return;

  // check to see if this memory write location is already in the list
  int already_found = 0;
  for (int i = 0; i < NUM_INSTR_DESTINATIONS; i++) {
    if (curr_instr.destination_memory[i] == ((unsigned long long int)addr)) {
      already_found = 1;
      break;
    }
  }
  if (already_found == 0) {
    for (int i = 0; i < NUM_INSTR_DESTINATIONS; i++) {
      if (curr_instr.destination_memory[i] == 0) {
        curr_instr.destination_memory[i] = (unsigned long long int)addr;
        break;
      }
    }
  }
}

/* ===================================================================== */
// Instrumentation callbacks
/* ===================================================================== */

// Is called for every instruction and instruments reads and writes
VOID
Instruction(INS ins, VOID* v) {
  // begin each instruction with this function
  UINT32 opcode = INS_Opcode(ins);
  std::string opcode_string = OPCODE_StringShort(opcode);
  UINT32 category = INS_Category(ins);
  std::string category_string = CATEGORY_StringShort(category);

  // Get globally-unique routine ID
  curr_instr.routine_id = RTN_Id(INS_Rtn(ins));

  INS_InsertCall(
      ins,
      IPOINT_BEFORE,
      (AFUNPTR)BeginInstruction,
      IARG_INST_PTR,
      IARG_UINT32,
      opcode,
      IARG_UINT32,
      category,
      IARG_END);

  // instrument branch instructions
  if (INS_IsBranch(ins)) {
    curr_instr.branch_info |= BRANCH::branch;
    INS_InsertCall(
        ins,
        IPOINT_BEFORE,
        (AFUNPTR)BranchHandler,
        IARG_BRANCH_TAKEN,
        IARG_END);
  }

  if (INS_IsCall(ins)) {
    curr_instr.branch_info |= BRANCH::call;
    // printf("Call: %d\n", curr_instr.branch_info);
  }

  if (INS_IsDirectBranchOrCall(ins)) {
    curr_instr.branch_info |= BRANCH::direct;
    // printf("Direct: %d\n", curr_instr.branch_info);
  }

  if (category == XED_CATEGORY_COND_BR) {
    curr_instr.branch_info |= BRANCH::cond;
    // printf("Conditional: %d\n", curr_instr.branch_info);
  }

  if (INS_IsBranchOrCall(ins)) {
    INS next_ins = INS_Next(ins);
    if (INS_Address(next_ins) > INS_Address(ins)) {
      curr_instr.branch_info |= BRANCH::fwd;
    }
  }

  if (INS_IsRet(ins)) {
    curr_instr.branch_info |= BRANCH::ret;
  }

  // instrument register reads
  UINT32 readRegCount = INS_MaxNumRRegs(ins);
  for (UINT32 i = 0; i < readRegCount; i++) {
    UINT32 regNum = INS_RegR(ins, i);

    INS_InsertCall(
        ins,
        IPOINT_BEFORE,
        (AFUNPTR)RegRead,
        IARG_UINT32,
        regNum,
        IARG_UINT32,
        i,
        IARG_END);
  }

  // instrument register writes
  UINT32 writeRegCount = INS_MaxNumWRegs(ins);
  for (UINT32 i = 0; i < writeRegCount; i++) {
    UINT32 regNum = INS_RegW(ins, i);

    INS_InsertCall(
        ins,
        IPOINT_BEFORE,
        (AFUNPTR)RegWrite,
        IARG_UINT32,
        regNum,
        IARG_UINT32,
        i,
        IARG_END);
  }

  // instrument memory reads and writes
  UINT32 memOperands = INS_MemoryOperandCount(ins);

  // Iterate over each memory operand of the instruction.
  for (UINT32 memOp = 0; memOp < memOperands; memOp++) {
    if (INS_MemoryOperandIsRead(ins, memOp)) {
      UINT32 read_size = INS_MemoryReadSize(ins);

      INS_InsertCall(
          ins,
          IPOINT_BEFORE,
          (AFUNPTR)MemoryRead,
          IARG_MEMORYOP_EA,
          memOp,
          IARG_UINT32,
          memOp,
          IARG_UINT32,
          read_size,
          IARG_END);
    }
    if (INS_MemoryOperandIsWritten(ins, memOp)) {
      INS_InsertCall(
          ins,
          IPOINT_BEFORE,
          (AFUNPTR)MemoryWrite,
          IARG_MEMORYOP_EA,
          memOp,
          IARG_UINT32,
          memOp,
          IARG_END);
    }
  }

  // finalize each instruction with this function
  INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)EndInstruction, IARG_END);
}

// Is called for every routine and instruments reads and writes
VOID
Routine(RTN rtn, VOID* v) {}

/*!
 * Print out analysis results.
 * This function is called when the application exits.
 * @param[in]   code            exit code of the application
 * @param[in]   v               value specified by the tool in the
 *                              PIN_AddFiniFunction function call
 */
VOID
Fini(INT32 code, VOID* v) {
  // close the file if it hasn't already been closed
  if (!output_file_closed) {
    fclose(out);
    output_file_closed = true;
  }
}

/*!
 * The main procedure of the tool.
 * This function is called when the application image is loaded but not yet
 * started.
 * @param[in]   argc            total number of elements in the argv array
 * @param[in]   argv            array of command line arguments,
 *                              including pin -t <toolname> -- ...
 */
int
main(int argc, char* argv[]) {
  // Initialize PIN library. Print help message if -h(elp) is specified
  // in the command line or the command line is invalid
  if (PIN_Init(argc, argv))
    return Usage();

  const char* fileName = KnobOutputFile.Value().c_str();

  out = fopen(fileName, "ab");
  if (!out) {
    cout << "Couldn't open output trace file. Exiting." << endl;
    exit(1);
  }

  PIN_InitSymbols();

  // Register function to be called to instrument instructions
  INS_AddInstrumentFunction(Instruction, 0);

  // Register function to be called to instrument routines
  RTN_AddInstrumentFunction(Routine, 0);

  // Register function to be called when the application exits
  PIN_AddFiniFunction(Fini, 0);

  cerr << "===============================================" << endl;
  cerr << "This application is instrumented by the PhaseSim Trace Generator"
       << endl;
  cerr << "Trace saved in " << KnobOutputFile.Value() << endl;
  cerr << "===============================================" << endl;

  // Start the program, never returns
  PIN_StartProgram();

  return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
