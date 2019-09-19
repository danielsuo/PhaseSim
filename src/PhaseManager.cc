#include "PhaseManager.h"
#include "BCPhaseDetector.h"
#include "CPIPhaseDetector.h"
#include "IWSPhaseDetector.h"

PhaseManager::PhaseManager(uint64_t intervalLength)
    : intervalLength_(intervalLength) {
  detectors_.push_back(new BCPhaseDetector());
  detectors_.push_back(new CPIPhaseDetector());
  detectors_.push_back(new IWSPhaseDetector());
}

bool
PhaseManager::isNewInterval(uint64_t instr_id) {
  if (instr_id - intervalStartCycle_ >= intervalLength_) {
    intervalStartCycle_ = instr_id;
    return true;
  }

  return false;
}

void
PhaseManager::updatePhaseDetectors(
    const ooo_model_instr& instr, const phasesim::CPUCounters& cpu_counters) {
  
  // Run instruction updates with each new instruction
  for (uint32_t i = 0; i < detectors_.size(); i++) {
    detectors_[i]->instructionUpdate(instr, cpu_counters);
  }

  // If we aren't in a new interval, return
  if (!isNewInterval(instr.instr_id)) {
    return;
  }

  // Otherwise, run interval update and log
  for (uint32_t i = 0; i < detectors_.size(); i++) {
    detectors_[i]->intervalUpdate(instr, cpu_counters);
    detectors_[i]->log();
  }
}
