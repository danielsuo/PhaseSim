#include "utils.h"

namespace phasesim {
uint64_t Options::warmup_instructions = 10000000;
uint64_t Options::simulation_instructions = 10000000;
uint64_t Options::heartbeat_period = 1000000;
bool Options::hide_heartbeat = false;
bool Options::knob_cloudsuite = false;
bool Options::knob_low_bandwidth = false;
} // namespace phasesim