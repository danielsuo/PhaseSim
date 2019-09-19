#pragma once

// Instruction working set phase detector
// Compute normalized working set distance and identify new phase if distance is
// sufficiently large
// Distance = (Union - Intersection) / Union

#include <algorithm>
#include <set>
#include "PhaseDetector.h"

class IWSPhaseDetector : public PhaseDetector {
  std::set<uint64_t> ips1_;
  std::set<uint64_t> ips2_;
  bool ips1_is_prev_ = false;

  float threshold_ = 0.04;

 public:
  IWSPhaseDetector(float threshold = 0.04) : threshold_(threshold) {
    name_ = "IWSPhaseDetector";
  }

  void
  instructionUpdate(
      const ooo_model_instr& instr,
      const phasesim::CPUCounters& cpu_counters) override {
    if (ips1_is_prev_) {
      ips2_.insert(instr.ip);
    } else {
      ips1_.insert(instr.ip);
    }
  }

  void
  intervalUpdate(
      const ooo_model_instr& instr,
      const phasesim::CPUCounters& cpu_counters) override {
    std::set<uint64_t> iws_intersection;
    std::set<uint64_t> iws_union;
    std::set_intersection(
        ips1_.begin(),
        ips1_.end(),
        ips2_.begin(),
        ips2_.end(),
        std::inserter(iws_intersection, iws_intersection.begin()));
    std::set_union(
        ips1_.begin(),
        ips1_.end(),
        ips2_.begin(),
        ips2_.end(),
        std::inserter(iws_union, iws_union.begin()));

    float delta = ((float)iws_union.size() - (float)iws_intersection.size()) / (float)iws_union.size();

    newPhase_ = delta > threshold_;
    log_ << delta << " " << iws_intersection.size() << " " << iws_union.size();

    if (ips1_is_prev_) {
      ips1_.clear();
    } else {
      ips2_.clear();
    }

    ips1_is_prev_ = !ips1_is_prev_;
  }
};
