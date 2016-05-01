// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <unordered_set>

#include "common.h"
#include "sentence/sentence.h"
#include "utils/named_values.h"

namespace ufal {
namespace udpipe {

class trainer_morphodita_parsito {
 public:
  static bool train(const string& data, const string& tokenizer, const string& tagger, const string& parser, ostream& os, string& error);

 private:
  static bool train_tokenizer(const vector<sentence>& data, const string& options, ostream& os, string& error);
  static bool train_tagger(const vector<sentence>& data, const string& options, ostream& os, string& error);
  static bool train_parser(const vector<sentence>& data, const string& options, ostream& os, string& error);

  static bool train_tagger_model(const vector<sentence>& data, unsigned model, unsigned models, const named_values::map& tagger, ostream& os, string& error);
  static bool can_combine_tag(const word& w, string& error);
  static const string& combine_tag(const word& w, bool xpostag, bool feats, string& combined_tag);
  static const string& combine_lemma(const word& w, bool use_lemma, const unordered_set<string>& drop_lemmas = unordered_set<string>());

  static const string& option_str(const named_values::map& options, const string& name, int model = -1);
  static bool option_int(const named_values::map& options, const string& name, int& value, string& error, int model = -1);
  static bool option_bool(const named_values::map& options, const string& name, bool& value, string& error, int model = -1);
  static bool option_double(const named_values::map& options, const string& name, double& value, string& error, int model = -1);

  static const string empty_string;
  static const string tag_separators;
  static const string tagger_features_tagger;
  static const string tagger_features_lemmatizer;
  static const string parser_nodes;
};

} // namespace udpipe
} // namespace ufal
