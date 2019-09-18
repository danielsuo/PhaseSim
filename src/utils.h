#pragma once

#include "stdint.h"

namespace phasesim {
struct Options {
  static uint64_t warmup_instructions;
  static uint64_t simulation_instructions;
  static uint64_t heartbeat_period;
  static uint64_t phase_interval_length;
  static bool hide_heartbeat;
  static bool knob_cloudsuite;
  static bool knob_low_bandwidth;
};

struct GlobalCounters {
};

struct CPUCounters {
  uint64_t num_cycles = 0;
  uint64_t num_instructions = 0;
  uint64_t num_branches = 0;
};
} // namespace phasesim
