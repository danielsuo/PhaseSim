#pragma once

#include "stdint.h"

namespace phasesim {
struct Options {
  static uint64_t warmup_instructions;
  static uint64_t simulation_instructions;
  static uint64_t heartbeat_period;
  static bool hide_heartbeat;
  static bool knob_cloudsuite;
  static bool knob_low_bandwidth;
};
} // namespace phasesim
