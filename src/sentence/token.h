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

  token(string_piece form = string_piece(), string_piece misc = string_piece());

  // CoNLL-U defined SpaceAfter=No feature
  bool get_space_after() const;
  void set_space_after(bool space_after);

  // UDPipe-specific all-spaces-preserving SpacesBefore and SpacesAfter features
  void get_spaces_before(string& spaces_before) const;
  void set_spaces_before(string_piece spaces_before);
  void get_spaces_after(string& spaces_after) const;
  void set_spaces_after(string_piece spaces_after);
  void get_spaces_in_token(string& spaces_in_token) const;
  void set_spaces_in_token(string_piece spaces_in_token);

  // UDPipe-specific TokenRange feature
  bool get_token_range(size_t& start, size_t& end) const;
  void set_token_range(size_t start, size_t end);

 private:
  bool get_misc_field(string_piece name, string_piece& value) const;
  void remove_misc_field(string_piece name);
  string& start_misc_field(string_piece name);

  void append_escaped_spaces(string_piece spaces, string& escaped_spaces) const;
  void unescape_spaces(string_piece escaped_spaces, string& spaces) const;
};

} // namespace udpipe
} // namespace ufal
