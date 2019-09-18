#pragma once

#include <math.h>
#include <stdint.h>
#include <vector>
#include "PhaseDetector.h"
#include "instruction.h"
#include "utils.h"

class PhaseManager {
  std::vector<PhaseDetector*> detectors_;
  uint64_t intervalStartCycle_ = 0;

  // Settable parameters
  uint64_t intervalLength_ = 10000000;

  // Private functions
  bool isNewInterval(uint64_t current_cycle);

 public:
  PhaseManager(uint64_t intervalLength);

  void updatePhaseDetectors(
      const ooo_model_instr& instr, const phasesim::CPUCounters& cpu_counters);
};
