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
#include "sentence/word.h"

namespace ufal {
namespace udpipe {

class trainer_morphodita_parsito {
 public:
  static bool train(const string& data, const string& tokenizer, const string& tagger, const string& parser, ostream& os, string& error);

 private:
  static const string& combine_tags(const word& w, string& combined_tag);
};

} // namespace udpipe
} // namespace ufal
