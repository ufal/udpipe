// This file is part of MicroRestD <http://github.com/ufal/microrestd/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cstring>
#include <ostream>
#include <string>

namespace ufal {
namespace microrestd {

struct string_piece {
  const char* str;
  size_t len;

  string_piece() : str(nullptr), len(0) {}
  string_piece(const char* str) : str(str), len(str ? strlen(str) : 0) {}
  string_piece(const char* str, size_t len) : str(str), len(len) {}
  string_piece(const std::string& str) : str(str.c_str()), len(str.size()) {}
};

inline std::ostream& operator<<(std::ostream& os, const string_piece& str) {
  os.write(str.str, str.len);
  return os;
}

} // namespace microrestd
} // namespace ufal
