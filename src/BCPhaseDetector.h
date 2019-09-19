#pragma once

// Branch counter phase detector
// Count number of dynamic branches per interval

#include "PhaseDetector.h"

class BCPhaseDetector : public PhaseDetector {
  uint64_t prev_branch_count_ = 0;
  float threshold_ = 0.04;

 public:
  BCPhaseDetector(float threshold = 0.04) : threshold_(threshold) {
    name_ = "BCPhaseDetector";
  }

  void
  intervalUpdate(
      const ooo_model_instr& instr,
      const phasesim::CPUCounters& cpu_counters) override {

    uint64_t diff = (prev_branch_count_ > cpu_counters.num_branches)
        ? (prev_branch_count_ - cpu_counters.num_branches)
        : (cpu_counters.num_branches - prev_branch_count_);

    float delta = (float)diff / (float)prev_branch_count_;

    newPhase_ = delta > threshold_;

    log_ << delta << " " << prev_branch_count_ << " " << cpu_counters.num_branches;
    prev_branch_count_ = cpu_counters.num_branches;
  }
};
