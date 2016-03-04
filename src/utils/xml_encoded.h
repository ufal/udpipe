// This file is part of UFAL C++ Utils <http://github.com/ufal/cpp_utils/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"

namespace ufal {
namespace udpipe {
namespace utils {

//
// Declarations
//

// Print xml content while encoding <>& and optionally " using XML entities.
class xml_encoded {
 public:
  xml_encoded(string_piece str, bool encode_quot = false) : str(str), encode_quot(encode_quot) {}

  friend ostream& operator<<(ostream& os, xml_encoded data);
 private:
  string_piece str;
  bool encode_quot;
};

inline ostream& operator<<(ostream& os, xml_encoded data);

//
// Definitions
//

ostream& operator<<(ostream& os, xml_encoded data) {
  string_piece& str = data.str;
  const char* to_print = str.str;

  while (str.len) {
    while (str.len && *str.str != '<' && *str.str != '>' && *str.str != '&' && (!data.encode_quot || *str.str != '"'))
      str.str++, str.len--;

    if (str.len) {
      if (to_print < str.str) os.write(to_print, str.str - to_print);
      os << (*str.str == '<' ? "&lt;" : *str.str == '>' ? "&gt;" : *str.str == '&' ? "&amp;" : "&quot;");
      str.str++, str.len--;
      to_print = str.str;
    }
  }

  if (to_print < str.str) os.write(to_print, str.str - to_print);

  return os;
}

} // namespace utils
} // namespace udpipe
} // namespace ufal
