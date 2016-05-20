// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <fstream>

#include "czech_morpho.h"
#include "morphodita/derivator/derivator_dictionary.h"
#include "english_morpho.h"
#include "external_morpho.h"
#include "generic_morpho.h"
#include "morpho.h"
#include "morpho_ids.h"
#include "utils/new_unique_ptr.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

morpho* morpho::load(istream& is) {
  morpho_id id = morpho_id(is.get());
  switch (id) {
    case morpho_ids::CZECH:
      {
        auto res = new_unique_ptr<czech_morpho>(czech_morpho::morpho_language::CZECH, 1);
        if (res->load(is)) return res.release();
        break;
      }
    case morpho_ids::ENGLISH_V1:
    case morpho_ids::ENGLISH_V2:
    case morpho_ids::ENGLISH_V3:
      {
        auto res = new_unique_ptr<english_morpho>(id == morpho_ids::ENGLISH_V1 ? 1 :
                                                  id == morpho_ids::ENGLISH_V2 ? 2 :
                                                  3);
        if (res->load(is)) return res.release();
        break;
      }
    case morpho_ids::EXTERNAL:
      {
        auto res = new_unique_ptr<external_morpho>(1);
        if (res->load(is)) return res.release();
        break;
      }
    case morpho_ids::GENERIC:
      {
        auto res = new_unique_ptr<generic_morpho>(1);
        if (res->load(is)) return res.release();
        break;
      }
    case morpho_ids::SLOVAK_PDT:
      {
        auto res = new_unique_ptr<czech_morpho>(czech_morpho::morpho_language::SLOVAK, 3);
        if (res->load(is)) return res.release();
        break;
      }
    case morpho_ids::DERIVATOR_DICTIONARY:
      {
        auto derinet = new_unique_ptr<derivator_dictionary>();
        if (!derinet->load(is)) return nullptr;

        unique_ptr<morpho> dictionary(load(is));
        if (!dictionary) return nullptr;
        derinet->dictionary = dictionary.get();
        dictionary->derinet.reset(derinet.release());
        return dictionary.release();
      }
  }

  return nullptr;
}

morpho* morpho::load(const char* fname) {
  ifstream f(fname, ifstream::binary);
  if (!f) return nullptr;

  return load(f);
}

const derivator* morpho::get_derivator() const {
  return derinet.get();
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
