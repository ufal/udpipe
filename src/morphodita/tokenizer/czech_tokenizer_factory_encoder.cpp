// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2019 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "czech_tokenizer_factory_encoder.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

void czech_tokenizer_factory_encoder::encode(czech_tokenizer::tokenizer_language language, unsigned version, ostream& os) {
  os.put(language);
  os.put(version);
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
