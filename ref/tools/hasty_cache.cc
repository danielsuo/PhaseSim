#include <clipp.h>

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

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

}
