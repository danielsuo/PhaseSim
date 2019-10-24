
/*! @file
 *  This is an example of the PIN tool that demonstrates some basic PIN APIs
 *  and could serve as the starting point for developing your first PIN tool
 */

#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <string>
#include "instruction.h"
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

/* ================================================================== */
// Global variables
/* ================================================================== */

UINT64 instrCount = 0;

std::ofstream trace_file;
std::ofstream meta_file;

bool tracing_on = false;

input_instr curr_instr;

/* ===================================================================== */
// Command line switches
/* ===================================================================== */
KNOB<string> KnobTracePath(
    KNOB_MODE_WRITEONCE,
    "pintool",
    "o",
    "phasesim.trace",
    "specify file name for PhaseSim tracer output");

KNOB<string> KnobMetaPath(
    KNOB_MODE_WRITEONCE,
    "pintool",
    "m",
    "phasesim.meta",
    "specify file name for PhaseSim tracer output");

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
  std::cerr
      << "This tool creates a register and memory access trace" << std::endl
      << "Specify the output trace file with -o" << std::endl
      << "Specify the number of instructions to skip before tracing with -s"
      << std::endl
      << "Specify the number of instructions to trace with -t" << std::endl
      << std::endl;

  std::cerr << KNOB_BASE::StringKnobSummary() << std::endl;

  return -1;
}

void
finalize() {
  std::cout << "Traced " << instrCount << " instructions" << std::endl;
  trace_file.close();
  meta_file.close();
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
        (KnobTraceInstructions.Value() + KnobSkipInstructions.Value())) {
      tracing_on = false;
    }
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
      trace_file.write((char*)&curr_instr, sizeof(input_instr));
    } else {
      finalize();
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
Routine(RTN rtn, VOID* v) {
  std::cout << "Function: " << RTN_Name(rtn) << std::endl;
}

// Is called for every image and instruments reads and writes
VOID
Image(IMG img, VOID* v) {
  if (IMG_IsMainExecutable(img)) {
    meta_file << "Main: " << IMG_Name(img) << std::endl;
  } else {
    meta_file << "Image: " << IMG_Name(img) << std::endl;
  }
}

/*!
 * Print out analysis results.
 * This function is called when the application exits.
 * @param[in]   code            exit code of the application
 * @param[in]   v               value specified by the tool in the
 *                              PIN_AddFiniFunction function call
 */
VOID
Fini(INT32 code, VOID* v) {
  finalize();
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
  if (PIN_Init(argc, argv)) {
    return Usage();
  }

  trace_file.open(KnobTracePath.Value().c_str(), std::ios::binary);
  meta_file.open(KnobMetaPath.Value().c_str());

  if (!trace_file.is_open()) {
    std::cout << "Couldn't open output trace file. Exiting." << std::endl;
    exit(1);
  }

  PIN_InitSymbols();

  // Register function to be called whenever there is a new image
  IMG_AddInstrumentFunction(Image, 0);

  // Register function to be called to instrument instructions
  INS_AddInstrumentFunction(Instruction, 0);

  // Register function to be called to instrument routines
  RTN_AddInstrumentFunction(Routine, 0);

  // Register function to be called when the application exits
  PIN_AddFiniFunction(Fini, 0);

  std::cerr << "===============================================" << std::endl;
  std::cerr
      << "This application is instrumented by the PhaseSim Trace Generator"
      << std::endl;
  std::cerr << "Trace saved in " << KnobTracePath.Value() << std::endl;
  std::cerr << "===============================================" << std::endl;

  // Start the program, never returns
  PIN_StartProgram();

  return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
