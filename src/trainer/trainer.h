// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"
#include "sentence/sentence.h"

namespace ufal {
namespace udpipe {

class trainer {
 public:
  static bool train(const string& method, const vector<sentence>& train, const vector<sentence>& heldout,
                    const string& tokenizer, const string& tagger, const string& parser, ostream& os, string& error);

  static const string DEFAULT;
  static const string NONE;

 protected:
  static unsigned hyperparameter_integer(unsigned run, unsigned index, unsigned minimum, unsigned maximum);
  static double hyperparameter_uniform(unsigned run, unsigned index, double minimum, double maximum);
  static double hyperparameter_logarithmic(unsigned run, unsigned index, double minimum, double maximum);

 private:
  static double rnd(unsigned run, unsigned index);
};

} // namespace udpipe
} // namespace ufal
