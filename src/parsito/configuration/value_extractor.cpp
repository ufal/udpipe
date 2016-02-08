// This file is part of Parsito <http://github.com/ufal/parsito/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "value_extractor.h"
#include "utils/split.h"

namespace ufal {
namespace udpipe {
namespace parsito {

void value_extractor::extract(const node& n, string& value) const {
  switch (selector) {
    case FORM:
      value.assign(n.form);
      break;
    case LEMMA:
      value.assign(n.lemma);
      break;
    case LEMMA_ID:
      if (!n.misc.empty()) {
        // Try finding LId= in misc column
        auto lid = n.misc.find("LId=");
        if (lid != string::npos) {
          lid += 4;

          // Find optional | ending the lemma_id
          auto lid_end = n.misc.find('|', lid);
          if (lid_end == string::npos) lid_end = n.misc.size();

          // Store the lemma_id
          value.assign(n.misc, lid, lid_end - lid);
          break;
        }
      }
      value.assign(n.lemma);
      break;
    case TAG:
      value.assign(n.xpostag);
      break;
    case UNIVERSAL_TAG:
      value.assign(n.upostag);
      break;
    case FEATS:
      value.assign(n.feats);
      break;
    case UNIVERSAL_TAG_FEATS:
      value.assign(n.upostag).append(n.feats);
      break;
    case DEPREL:
      value.assign(n.deprel);
      break;
  }
}

bool value_extractor::create(string_piece description, string& error) {
  error.clear();

  if (description == "form")
    selector = FORM;
  else if (description == "lemma")
    selector = LEMMA;
  else if (description == "lemma_id")
    selector = LEMMA_ID;
  else if (description == "tag")
    selector = TAG;
  else if (description == "universal_tag")
    selector = UNIVERSAL_TAG;
  else if (description == "feats")
    selector = FEATS;
  else if (description == "universal_tag_feats")
    selector = UNIVERSAL_TAG_FEATS;
  else if (description == "deprel")
    selector = DEPREL;
  else
    return error.assign("Cannot parse value selector '").append(description.str, description.len).append("'!"), false;

  return true;
}

} // namespace parsito
} // namespace udpipe
} // namespace ufal
