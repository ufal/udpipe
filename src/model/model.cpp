// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <fstream>

#include "model.h"
#include "model_morphodita_parsito.h"

namespace ufal {
namespace udpipe {

const string model::DEFAULT;

const string model::TOKENIZER_PRESEGMENTED = "presegmented";

model* model::load(const char* fname) {
  ifstream in(fname, ifstream::in | ifstream::binary);
  if (!in.is_open()) return nullptr;
  return load(in);
}

model* model::load(istream& is) {
  char len;
  if (!is.get(len)) return nullptr;
  string name(len, ' ');
  if (!is.read(&name[0], len)) return nullptr;

  if (name == "morphodita_parsito") return model_morphodita_parsito::load(is);

  return nullptr;
}

} // namespace udpipe
} // namespace ufal
