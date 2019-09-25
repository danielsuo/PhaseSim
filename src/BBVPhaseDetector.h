#pragma once

// Instruction working set phase detector
// Compute normalized working set distance and identify new phase if distance is
// sufficiently large
// Distance = (Union - Intersection) / Union

#include <vector>
#include "PhaseDetector.h"

class BBVPhaseDetector : public PhaseDetector {
  std::vector<uint64_t> lib1_;
  std::vector<uint64_t> lib2_;
  bool lib1_is_prev_ = false;
  uint64_t bbv_dim_ = 256;

 public:
  BBVPhaseDetector(const YAML::Node& config) {
    name_ = "BBVPhaseDetector";
    init(config);

    lib1_.resize(bbv_dim_, 0);
    lib2_.resize(bbv_dim_, 0);
  }

  void
  instructionUpdate(
      const ooo_model_instr& instr,
      const phasesim::CPUCounters& curr_counters,
      const phasesim::CPUCounters& prev_counters) override {
    if (instr.is_branch) {
      auto idx = instr.ip % bbv_dim_;
      if (lib1_is_prev_) {
        lib2_[idx]++;
      } else {
        lib1_[idx]++;
      }
    }
  }

  void
  intervalUpdate(
      const ooo_model_instr& instr,
      const phasesim::CPUCounters& curr_counters,
      const phasesim::CPUCounters& prev_counters) override {
    uint64_t sum = 0;
    for (uint32_t i = 0; i < bbv_dim_; i++) {
      delta_ +=
          (lib1_[i] > lib2_[i]) ? lib1_[i] - lib2_[i] : lib2_[i] - lib1_[i];
    }

    if (lib1_is_prev_) {
      lib1_.resize(bbv_dim_, 0);
    } else {
      lib2_.resize(bbv_dim_, 0);
    }

    lib1_is_prev_ = !lib1_is_prev_;
  }
};
