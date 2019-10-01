#pragma once

#include <map>
#include <string>
#include <fstream>
#include "stdint.h"

namespace phasesim {
struct Options {
  static uint64_t warmup_instructions;
  static uint64_t simulation_instructions;
  static uint64_t heartbeat_period;
  static uint64_t phase_interval_length;
  static bool save_llc_accesses;
  static bool hide_heartbeat;
  static bool knob_cloudsuite;
  static bool knob_low_bandwidth;
  static std::string output_dir;
  static std::string yaml_path;
};

struct Files {
  static std::ofstream llc_accesses;
};

struct GlobalCounters {};

struct CPUCounters {
  // Accumulators: Counters for which we are interested in deltas from one
  // period to the next (e.g., cycles)
  uint64_t instructions = 0;
  uint64_t cycles = 0;

  // Resetable accumulators: Like accumulators, but reset to 0 each interval
  int64_t branches = 0;

  // Levels: Counters for which we are interested in the current level (e.g., LQ
  // occupancy)
  uint64_t lq_occupancy = 0;

  CPUCounters() {}

  friend CPUCounters
  operator-(const CPUCounters& c1, const CPUCounters& c2) {
    CPUCounters counters;

    // Accumulators
    counters.instructions = c1.instructions - c2.instructions;
    counters.cycles = c1.cycles - c2.cycles;

    // Resetable accumulators
    counters.branches = c1.branches - c2.branches;

    // Levels
    counters.lq_occupancy = c1.lq_occupancy;

    return counters;
  }

  void
  reset() {
    branches = 0;
  }
};

struct REPLACEMENT_STATS {
  uint32_t cpu;
  uint32_t set;
  uint32_t way;
  uint64_t full_addr;
  uint64_t ip;
  uint64_t victim_addr;
  uint32_t type;
  uint32_t cache_type;
  uint8_t hit;
};
} // namespace phasesim
