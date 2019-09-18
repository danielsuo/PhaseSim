#include "PhaseManager.h"
#include "BranchCounterPhaseDetector.h"
#include "CPIPhaseDetector.h"

PhaseManager::PhaseManager(uint64_t intervalLength)
    : intervalLength_(intervalLength) {
  detectors_.push_back(new BranchCounterPhaseDetector());
  detectors_.push_back(new CPIPhaseDetector());
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
  if (!isNewInterval(instr.instr_id)) {
    return;
  }

  for (uint32_t i = 0; i < detectors_.size(); i++) {
    detectors_[i]->update(instr, cpu_counters);
    detectors_[i]->log();
  }
}
