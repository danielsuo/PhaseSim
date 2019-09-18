#pragma once

#include "PhaseDetector.h"

class BranchCounterPhaseDetector : public PhaseDetector {
  uint64_t prev_branch_count_ = 0;
  uint64_t threshold_ = 0;

 public:
  BranchCounterPhaseDetector(uint64_t threshold = 0) : threshold_(threshold) {
    name_ = "BranchCounterPhaseDetector";
  }

  void
  update(
      const ooo_model_instr& instr,
      const phasesim::CPUCounters& cpu_counters) override {

    uint64_t delta = (prev_branch_count_ > cpu_counters.num_branches)
        ? (prev_branch_count_ - cpu_counters.num_branches)
        : (cpu_counters.num_branches - prev_branch_count_);

    log_ << delta;
  }
};
