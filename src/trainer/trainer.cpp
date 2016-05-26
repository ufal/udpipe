// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <sstream>

#include "trainer.h"
#include "trainer_morphodita_parsito.h"
#include "training_failure.h"

namespace ufal {
namespace udpipe {

const string trainer::DEFAULT;
const string trainer::NONE = "none";

bool trainer::train(const string& method, const vector<sentence>& training, const vector<sentence>& heldout,
                    const string& tokenizer, const string& tagger, const string& parser, ostream& os, string& error) {
  error.clear();

  stringstream os_buffer;
  os_buffer.put(method.size());
  os_buffer.write(method.c_str(), method.size());

  try {
    if (method == "morphodita_parsito") {
      if (!trainer_morphodita_parsito::train(training, heldout, tokenizer, tagger, parser, os_buffer, error))
        return false;
    } else {
      error.assign("Unknown UDPipe method '").append(method).append("'!");
      return false;
    }
  } catch (training_error& e) {
    error.assign(e.what());
    return false;
  }

  os << os_buffer.rdbuf();
  return true;
}

unsigned trainer::hyperparameter_integer(unsigned run, unsigned index, unsigned minimum, unsigned maximum) {
  return minimum + int((maximum - minimum + 1) * rnd(run, index));
}

double trainer::hyperparameter_uniform(unsigned run, unsigned index, double minimum, double maximum) {
  return minimum + (maximum - minimum) * rnd(run, index);
}

double trainer::hyperparameter_logarithmic(unsigned run, unsigned index, double minimum, double maximum) {
  return exp(log(minimum) + (log(maximum) - log(minimum)) * rnd(run, index));
}

double trainer::rnd(unsigned run, unsigned index) {
  uint32_t state = 12345U;
  for (unsigned i = 0; i < 10; i++)
    state = state * 1103515245U + run * 19999999U + index * 1000000007U + 12345U;
  return (state >> 16) / double(1<<16);
}

} // namespace udpipe
} // namespace ufal
