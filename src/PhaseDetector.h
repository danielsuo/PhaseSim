#pragma once

#include <stdint.h>
#include <stdio.h>
#include <cstdarg>
#include <iostream>
#include <sstream>
#include <vector>
#include <yaml-cpp/yaml.h>
#include "instruction.h"
#include "utils.h"

class PhaseDetector {
 protected:
  std::string name_ = "PhaseDetector";
  std::stringstream log_;
  bool newPhase_ = false;

  YAML::Node config_;

  float threshold_ = 0.0;

 public:
  void
  init(const YAML::Node& config) {
    config_ = config[name_];
    threshold_ = config_["threshold"].as<float>();
    std::cout << name_ << std::endl;
    std::cout << config_ << std::endl;
    std::cout << config_["threshold"] << std::endl;
  }

  void
  log() {
    std::cout << name_ << " " << newPhase_ << " " << log_.str() << std::endl;
    log_.str(std::string());
  }

  virtual void
  instructionUpdate(
      const ooo_model_instr& instr, const phasesim::CPUCounters& cpu_counters) {
  }

  virtual void
  intervalUpdate(
      const ooo_model_instr& instr, const phasesim::CPUCounters& cpu_counters) {
  }
};
