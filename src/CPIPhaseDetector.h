#pragma once

#include "PhaseDetector.h"

class CPIPhaseDetector : public PhaseDetector {
  float prev_cpi_ = 0.0;
  uint64_t prev_num_cycles_ = 0;
  uint64_t prev_num_instructions_ = 0;

  float threshold_ = 0.02;

 public:
  CPIPhaseDetector(float threshold = 0.02) : threshold_(threshold) {
    name_ = "CPIPhaseDetector";
  }
  void
  update(
      const ooo_model_instr& instr,
      const phasesim::CPUCounters& cpu_counters) override {

    uint64_t delta_num_cycles = cpu_counters.num_cycles - prev_num_cycles_;
    uint64_t delta_num_instructions =
        cpu_counters.num_instructions - prev_num_instructions_;

    float curr_cpi = ((float)delta_num_cycles / (float)delta_num_instructions);

    float delta = fabs(curr_cpi - prev_cpi_) / prev_cpi_;

    newPhase_ = delta > threshold_;
    log_ << delta << " " << curr_cpi << " " << prev_cpi_;

    prev_cpi_ = curr_cpi;
    prev_num_cycles_ = cpu_counters.num_cycles;
    prev_num_instructions_ = cpu_counters.num_instructions;
  }
};
