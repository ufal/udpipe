// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <fstream>

#include "czech_tokenizer_factory.h"
#include "generic_tokenizer_factory.h"
#include "gru_tokenizer_factory.h"
#include "tokenizer_ids.h"
#include "utils/new_unique_ptr.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

tokenizer_factory* tokenizer_factory::load(istream& is) {
  tokenizer_id id = tokenizer_id(is.get());
  switch (id) {
    case tokenizer_ids::GENERIC:
      {
        auto res = new_unique_ptr<generic_tokenizer_factory>();
        if (res->load(is)) return res.release();
        break;
      }
    case tokenizer_ids::GRU:
      {
        auto res = new_unique_ptr<gru_tokenizer_factory>();
        if (res->load(is)) return res.release();
        break;
      }
    case tokenizer_ids::CZECH:
      {
        auto res = new_unique_ptr<czech_tokenizer_factory>();
        if (res->load(is)) return res.release();
        break;
      }
    case tokenizer_ids::ENGLISH:
      break;
  }

  return nullptr;
}

tokenizer_factory* tokenizer_factory::load(const char* fname) {
  ifstream f(fname, ifstream::binary);
  if (!f) return nullptr;

  return load(f);
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
