#include "PhaseManager.h"
#include "BBVPhaseDetector.h"
#include "BCPhaseDetector.h"
#include "CPIPhaseDetector.h"
#include "IWSPhaseDetector.h"

PhaseManager::PhaseManager(uint64_t intervalLength, const YAML::Node& config)
    : intervalLength_(intervalLength) {
  detectors_.push_back(new BBVPhaseDetector(config));
  detectors_.push_back(new BCPhaseDetector(config));
  detectors_.push_back(new CPIPhaseDetector(config));
  detectors_.push_back(new IWSPhaseDetector(config));
}

bool
PhaseManager::isNewInterval(uint64_t instr_id) {
  return instr_id - intervalStartInstruction_ >= intervalLength_;
}

void
PhaseManager::updatePhaseDetectors(
    const input_instr& instr,
    const phasesim::CPUCounters& curr_counters,
    const phasesim::CPUCounters& prev_counters) {
  // Run instruction updates with each new instruction
  for (uint32_t i = 0; i < detectors_.size(); i++) {
    detectors_[i]->instructionUpdate(instr, curr_counters, prev_counters);
  }

  // If we aren't in a new interval, return
  if (!isNewInterval(curr_counters.instructions)) {
    return;
  }

  // Reset start cycle
  intervalStartInstruction_ = curr_counters.instructions;

  // Otherwise, run interval update and log
  for (uint32_t i = 0; i < detectors_.size(); i++) {
    detectors_[i]->intervalUpdate(instr, curr_counters, prev_counters);
    detectors_[i]->log();
  }
}
