#include <clipp.h>

#include <yaml-cpp/yaml.h>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "PhaseManager.h"
#include "instruction.h"
#include "utils.h"

using namespace std;
using namespace clipp;

void
print_counters(map<string, uint64_t>& counters) {
  for (auto kv : counters) {
    cout << kv.first << ": " << kv.second << endl;
  }
  cout << endl;
}

int
main(int argc, char* argv[]) {
  string trace_path;
  string conf_path = "../conf/test.yml";
  uint64_t interval_length = 100000000;
  uint64_t heartbeat_period = 1000000;

  map<string, uint64_t> counters;

  auto cli =
      (required("-t", "--trace-path").doc("Trace path") &
           value("trace", trace_path),
       option("-c", "--conf-path").doc("Configuration path") &
           value("conf", conf_path),
       option("-l", "--interval-length").doc("Interval length") &
           value("length", interval_length),
       option("-l", "--heartbeat-period").doc("Heartbeat period") &
           value("period", heartbeat_period));

  if (!parse(argc, argv, cli)) {
    cout << make_man_page(cli, argv[0]);
    exit(1);
  }

  vector<PhaseDetector*> detectors;

  YAML::Node config = YAML::LoadFile(conf_path);

  PhaseManager manager(interval_length, config);

  cout << "Reading from " << trace_path << endl;

  stringstream ss;
  ss << "xz -dc " << trace_path;

  FILE* trace_file = popen(ss.str().c_str(), "r");

  input_instr instr;
  phasesim::CPUCounters curr_counters;
  phasesim::CPUCounters prev_counters;
  uint64_t interval_start = 0;
  while (fread(&instr, sizeof(instr), 1, trace_file)) {
    if (counters["instr::total"] % heartbeat_period == 0) {
      cout << endl
           << "Processed " << counters["instr::total"] << " instructions."
           << endl;
    }

    curr_counters.instructions = counters["instr::total"];
    curr_counters.cycles = curr_counters.instructions;
    if (instr.branch_info & BRANCH::branch) {
      curr_counters.branches++;
    }

    bool isNewInterval =
        manager.updatePhaseDetectors(instr, curr_counters, prev_counters);

    if (isNewInterval) {
      prev_counters = curr_counters;
      curr_counters.reset();
    }

    interval_start = counters["instr::total"];

    counters["instr::total"]++;
  }

  print_counters(counters);

  return 0;
}
