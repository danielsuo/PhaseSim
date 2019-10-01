#pragma once

#include <math.h>
#include <stdint.h>
#include <yaml-cpp/yaml.h>
#include <vector>
#include "PhaseDetector.h"
#include "instruction.h"
#include "utils.h"

class PhaseManager {
  std::vector<PhaseDetector*> detectors_;
  uint64_t intervalStartInstruction_ = 0;

  // Settable parameters
  uint64_t intervalLength_ = 10000000;

  // Private functions

 public:
  PhaseManager(uint64_t intervalLength, const YAML::Node& config);

  bool isNewInterval(uint64_t current_cycle);
  void updatePhaseDetectors(
      const input_instr& instr,
      const phasesim::CPUCounters& curr_counters,
      const phasesim::CPUCounters& prev_counters);
};
