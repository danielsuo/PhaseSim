#pragma once

// Cycles per instruction phase detector
// Record CPI for each interval and determine new phase based on % variation
// from previous interval

#include "PhaseDetector.h"

class CPIPhaseDetector : public PhaseDetector {
  float prev_cpi_ = 0.0;
  uint64_t prev_num_cycles_ = 0;
  uint64_t prev_num_instructions_ = 0;

 public:
  CPIPhaseDetector(const YAML::Node& config) {
    name_ = "CPIPhaseDetector";
    init(config);
  }

  void
  intervalUpdate(
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
