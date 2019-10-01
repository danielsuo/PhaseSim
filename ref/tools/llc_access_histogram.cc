#include <clipp.h>
#include <stdint.h>
#include <fstream>
#include <iostream>
#include <map>
#include "utils.h"

using namespace std;
using namespace clipp;

#define LOG2_BLOCK_SIZE 6

int
main(int argc, char* argv[]) {
  string inpath;

  uint64_t interval_length = 10000000;

  auto cli =
      (value("access file", inpath),
       option("-l", "--interval-length") &
           value("interval_length", interval_length).doc("Interval length"));

  if (!parse(argc, argv, cli)) {
    cout << make_man_page(cli, argv[0]);
  }

  cout << "Opening file " << inpath << endl;

  ifstream file;
  file.open(inpath, ios::binary);

  map<uint64_t, uint64_t> access_histogram;
  map<uint64_t, uint64_t> count_histogram;

  phasesim::REPLACEMENT_STATS stats;
  uint64_t num_instructions = 0;

  cout << "Beginning analysis over interval length " << interval_length << endl;

  while (file.read((char*)&stats, sizeof(phasesim::REPLACEMENT_STATS))) {
    if (num_instructions % 1000000 == 0) {
      cout << "Read " << num_instructions << " instructions" << endl;
    }

    if (num_instructions % interval_length == 0) {
      cout << "Tabulating results and clearing access histogram "
           << num_instructions << endl;

      for (auto el : access_histogram) {
        count_histogram[el.second]++;
      }

      access_histogram.clear();
    }

    access_histogram[stats.full_addr >> LOG2_BLOCK_SIZE]++;

    num_instructions++;
  }

  for (auto el : count_histogram) {
    cout << el.first << ": " << el.second << endl;
  }

  file.close();

  return 0;
}
