#pragma once

// Instruction working set phase detector
// Compute normalized working set distance and identify new phase if distance is
// sufficiently large
// Distance = (Union - Intersection) / Union

#include <numeric>
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
      const input_instr& instr,
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
      const input_instr& instr,
      const phasesim::CPUCounters& curr_counters,
      const phasesim::CPUCounters& prev_counters) override {

    uint64_t sum1 = std::accumulate(lib1_.begin(), lib1_.end(), 0);
    uint64_t sum2 = std::accumulate(lib2_.begin(), lib2_.end(), 0);

    delta_ = 0;
    for (uint32_t i = 0; i < bbv_dim_; i++) {
      delta_ += fabs(((float)lib1_[i]) / sum1 - ((float)lib2_[i]) / sum2);
    }

    if (lib1_is_prev_) {
      lib1_.resize(bbv_dim_, 0);
    } else {
      lib2_.resize(bbv_dim_, 0);
    }

    lib1_is_prev_ = !lib1_is_prev_;
  }
};
