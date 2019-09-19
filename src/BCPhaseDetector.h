#pragma once

// Branch counter phase detector
// Count number of dynamic branches per interval

#include "PhaseDetector.h"

class BCPhaseDetector : public PhaseDetector {
  uint64_t prev_branch_count_ = 0;

 public:
  BCPhaseDetector(const YAML::Node& config) {
    name_ = "BCPhaseDetector";
    init(config);
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

    log_ << delta << " " << prev_branch_count_ << " "
         << cpu_counters.num_branches;
    prev_branch_count_ = cpu_counters.num_branches;
  }
};
