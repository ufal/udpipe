// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <fstream>

#include "conllu_elementary_features.h"
#include "czech_elementary_features.h"
#include "generic_elementary_features.h"
#include "feature_sequences.h"
#include "perceptron_tagger.h"
#include "tagger.h"
#include "tagger_ids.h"
#include "utils/new_unique_ptr.h"
#include "utils/path_from_utf8.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

tagger* tagger::load(istream& is) {
  tagger_id id = tagger_id(is.get());
  switch (id) {
    case tagger_ids::CZECH2:
    case tagger_ids::CZECH2_3:
    case tagger_ids::CZECH3:
      {
        auto res = new_unique_ptr<perceptron_tagger<persistent_feature_sequences<persistent_czech_elementary_features>>>(tagger_ids::decoding_order(id), tagger_ids::window_size(id));
        if (res->load(is)) return res.release();
        break;
      }
    case tagger_ids::GENERIC2:
    case tagger_ids::GENERIC2_3:
    case tagger_ids::GENERIC3:
    case tagger_ids::GENERIC4:
      {
        auto res = new_unique_ptr<perceptron_tagger<persistent_feature_sequences<persistent_generic_elementary_features>>>(tagger_ids::decoding_order(id), tagger_ids::window_size(id));
        if (res->load(is)) return res.release();
        break;
      }
    case tagger_ids::CONLLU2:
    case tagger_ids::CONLLU2_3:
    case tagger_ids::CONLLU3:
      {
        auto res = new_unique_ptr<perceptron_tagger<persistent_feature_sequences<persistent_conllu_elementary_features>>>(tagger_ids::decoding_order(id), tagger_ids::window_size(id));
        if (res->load(is)) return res.release();
        break;
      }
  }

  return nullptr;
}

tagger* tagger::load(const char* fname) {
  ifstream f(path_from_utf8(fname).c_str(), ifstream::binary);
  if (!f) return nullptr;

  return load(f);
}

tokenizer* tagger::new_tokenizer() const {
  auto morpho = get_morpho();
  return morpho ? morpho->new_tokenizer() : nullptr;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
