// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2017 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"
#include "utils/string_piece.h"

namespace ufal {
namespace udpipe {

class token {
 public:
  string form;
  string misc;

  token(string_piece form = string_piece(), string_piece misc = string_piece()) {
    if (form.len) this->form.assign(form.str, form.len);
    if (misc.len) this->misc.assign(misc.str, misc.len);
  }

  // CoNLL-U defined SpaceAfter=No feature
  bool get_space_after();
  void set_space_after(bool space_after);

  // UDPipe-specific all-spaces-preserving SpacesBefore and SpacesAfter features
  void get_spaces_before(string& spaces_before);
  void set_spaces_before(string_piece spaces_before);
  void get_spaces_after(string& spaces_after);
  void set_spaces_after(string_piece spaces_after);

  // UDPipe-specific TokenRange feature
  bool get_token_range(size_t& start, size_t& end);
  void set_token_range(size_t start, size_t end);

 private:
  bool get_misc_field(const char* name, string_piece& value);
  void remove_misc_field(const char* name);
  string& start_misc_field(const char* name);

  void append_escaped_spaces(string_piece spaces, string& escaped_spaces);
  void unescape_spaces(string_piece escaped_spaces, string& spaces);
};

} // namespace udpipe
} // namespace ufal
