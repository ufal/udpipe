// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "identity_tagset_converter.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

void identity_tagset_converter::convert(tagged_lemma& /*tagged_lemma*/) const {}

void identity_tagset_converter::convert_analyzed(vector<tagged_lemma>& /*tagged_lemmas*/) const {}

void identity_tagset_converter::convert_generated(vector<tagged_lemma_forms>& /*forms*/) const {}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
