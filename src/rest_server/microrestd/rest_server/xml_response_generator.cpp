// This file is part of MicroRestD <http://github.com/ufal/microrestd/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "xml_response_generator.h"

namespace ufal {
namespace microrestd {

const char* xml_response_generator::mime = "application/xml";

string_piece xml_response_generator::current() const {
  return xml.current();
}

void xml_response_generator::consume(size_t length) {
  xml.discard_current_prefix(length);
}

} // namespace microrestd
} // namespace ufal
