// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <ctime>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "udpipe.h"

using namespace ufal::udpipe;
using namespace std;

int main(int argc, char* argv[]) {
  if (argc < 4) {
    cerr << "Usage: " << argv[0] << " input_format output_format model" << endl;
    return 1;
  }

  cerr << "Loading UDPipe model: " << flush;
  unique_ptr<model> model(model::load(argv[3]));
  if (!model) {
    cerr << "Cannot load UDPipe model '" << argv[1] << "'!" << endl;
    return 1;
  }
  cerr << "done." << endl;

  pipeline pipeline(model.get(), argv[1], pipeline::DEFAULT, pipeline::DEFAULT, argv[2]);

  string line, para, error;
  while (cin) {
    para.clear();
    while (getline(cin, line)) {
      para.append(line).append("\n");
      if (line.empty()) break;
    }

    if (!para.empty() && !pipeline.process(para, cout, error)) {
      cerr << "An error occured: " << error << endl;
      return 1;
    }
  }

  return 0;
}
