#pragma once

#include <stdint.h>
#include <stdio.h>
#include <cstdarg>
#include <iostream>
#include <sstream>
#include <vector>
#include "instruction.h"
#include "utils.h"

class PhaseDetector {
 protected:
  std::string name_ = "PhaseDetector";
  std::stringstream log_;
  bool newPhase_ = false;

 public:
  void
  init() {
    std::cout << "Initialized PhaseDetector: " << name_ << std::endl;
  }

  void
  log() {
    std::cout << name_ << " " << newPhase_ << " " << log_.str() << std::endl;
    log_.str(std::string());
  }

  virtual void
  instructionUpdate(
      const ooo_model_instr& instr, const phasesim::CPUCounters& cpu_counters) {
  }

  virtual void
  intervalUpdate(
      const ooo_model_instr& instr, const phasesim::CPUCounters& cpu_counters) {
  }
};
