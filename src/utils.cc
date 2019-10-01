#include "utils.h"

namespace phasesim {
uint64_t Options::warmup_instructions = 10000000;
uint64_t Options::simulation_instructions = 10000000;
uint64_t Options::heartbeat_period = 1000000;
uint64_t Options::phase_interval_length = 1000000;
bool Options::save_llc_accesses = false;
bool Options::hide_heartbeat = false;
bool Options::knob_cloudsuite = false;
bool Options::knob_low_bandwidth = false;
std::string Options::output_dir = "../tmp";
std::string Options::yaml_path = "../conf/test.yml";

std::ofstream Files::llc_accesses;
} // namespace phasesim
