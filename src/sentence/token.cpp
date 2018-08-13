// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2017 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <limits>

#include "token.h"

namespace ufal {
namespace udpipe {

token::token(string_piece form, string_piece misc) {
  if (form.len) this->form.assign(form.str, form.len);
  if (misc.len) this->misc.assign(misc.str, misc.len);
}

// CoNLL-U defined SpaceAfter=No feature
bool token::get_space_after() const {
  string_piece value;

  return !(get_misc_field("SpaceAfter", value) && value.len == 2 && memcmp(value.str, "No", 2) == 0);
}

void token::set_space_after(bool space_after) {
  if (space_after)
    remove_misc_field("SpaceAfter");
  else
    start_misc_field("SpaceAfter").append("No");
}

// UDPipe-specific all-spaces-preserving SpacesBefore and SpacesAfter features
void token::get_spaces_before(string& spaces_before) const {
  string_piece value;

  if (get_misc_field("SpacesBefore", value))
    unescape_spaces(value, spaces_before);
  else
    spaces_before.clear();
}

void token::set_spaces_before(string_piece spaces_before) {
  if (spaces_before.len == 0)
    remove_misc_field("SpacesBefore");
  else
    append_escaped_spaces(spaces_before, start_misc_field("SpacesBefore"));
}

void token::get_spaces_after(string& spaces_after) const {
  string_piece value;

  if (get_misc_field("SpacesAfter", value))
    unescape_spaces(value, spaces_after);
  else
    spaces_after.assign(get_space_after() ? " " : "");
}

void token::set_spaces_after(string_piece spaces_after) {
  if (spaces_after.len == 0) {
    set_space_after(false);
    remove_misc_field("SpacesAfter");
  } else if (spaces_after.len == 1 && spaces_after.str[0] == ' ') {
    set_space_after(true);
    remove_misc_field("SpacesAfter");
  } else {
    set_space_after(true);
    append_escaped_spaces(spaces_after, start_misc_field("SpacesAfter"));
  }
}

void token::get_spaces_in_token(string& spaces_in_token) const {
  string_piece value;

  if (get_misc_field("SpacesInToken", value))
    unescape_spaces(value, spaces_in_token);
  else
    spaces_in_token.clear();
}

void token::set_spaces_in_token(string_piece spaces_in_token) {
  if (spaces_in_token.len == 0)
    remove_misc_field("SpacesInToken");
  else
    append_escaped_spaces(spaces_in_token, start_misc_field("SpacesInToken"));
}

// UDPipe-specific TokenRange feature
bool token::get_token_range(size_t& start, size_t& end) const {
  string_piece value;

  if (!get_misc_field("TokenRange", value)) return false;

  start = 0;
  while (value.len && value.str[0] >= '0' && value.str[0] <= '9') {
    if (start > (numeric_limits<size_t>::max() - (value.str[0] - '0')) / 10)
      return false;
    start = 10 * start + (value.str[0] - '0');
    value.str++, value.len--;
  }

  if (value.len == 0 || value.str[0] != ':') return false;
  value.str++, value.len--;

  end = 0;
  while (value.len && value.str[0] >= '0' && value.str[0] <= '9') {
    if (end > (numeric_limits<size_t>::max() - (value.str[0] - '0')) / 10)
      return false;
    end = 10 * end + (value.str[0] - '0');
    value.str++, value.len--;
  }

  return true;
}

void token::set_token_range(size_t start, size_t end) {
  if (start == size_t(string::npos))
    remove_misc_field("TokenRange");
  else
    start_misc_field("TokenRange").append(to_string(start)).append(1, ':').append(to_string(end));
}

// Private MISC field helpers
        // Name1=Value1|Name2=Value2|Name3=Value3
bool token::get_misc_field(string_piece name, string_piece& value) const {
  for (size_t index = 0; index < misc.size(); ) {
    if (misc.compare(index, name.len, name.str, name.len) == 0 && misc[index + name.len] == '=') {
      index += name.len + 1;
      value.str = misc.c_str() + index;
      value.len = misc.find('|', index);
      value.len = (value.len == size_t(string::npos) ? misc.size() : value.len) - index;
      return true;
    }
    index = misc.find('|', index);
    if (index != size_t(string::npos)) index++;
  }
  return false;
}

void token::remove_misc_field(string_piece name) {
  for (size_t index = 0; index < misc.size(); )
    if (misc.compare(index, name.len, name.str, name.len) == 0 && misc[index + name.len] == '=') {
      size_t end_index = misc.find('|', index + name.len + 1);
      if (end_index == size_t(string::npos)) end_index = misc.size();

      // Be careful to delete at most one neighboring '|'
      if (index)
        misc.erase(index - 1, end_index - (index - 1));
      else
        misc.erase(index, end_index + (end_index < misc.size() ? 1 : 0) - index);
    } else {
      index = misc.find('|', index);
      if (index != size_t(string::npos)) index++;
    }
}

string& token::start_misc_field(string_piece name) {
  remove_misc_field(name);
  if (!misc.empty()) misc.push_back('|');
  misc.append(name.str, name.len).push_back('=');
  return misc;
}

void token::append_escaped_spaces(string_piece spaces, string& escaped_spaces) const {
  for (unsigned i = 0; i < spaces.len; i++)
    switch (spaces.str[i]) {
      case ' ':
        escaped_spaces.push_back('\\'); escaped_spaces.push_back('s'); break;
      case '|':
        escaped_spaces.push_back('\\'); escaped_spaces.push_back('p'); break;
      case '\t':
        escaped_spaces.push_back('\\'); escaped_spaces.push_back('t'); break;
      case '\r':
        escaped_spaces.push_back('\\'); escaped_spaces.push_back('r'); break;
      case '\n':
        escaped_spaces.push_back('\\'); escaped_spaces.push_back('n'); break;
      case '\\':
        escaped_spaces.push_back('\\'); escaped_spaces.push_back('\\'); break;
      default:
        escaped_spaces.push_back(spaces.str[i]);
    }
}

void token::unescape_spaces(string_piece escaped_spaces, string& spaces) const {
  spaces.clear();

  for (unsigned i = 0; i < escaped_spaces.len; i++)
    if (escaped_spaces.str[i] != '\\' || i+1 >= escaped_spaces.len)
      spaces.push_back(escaped_spaces.str[i]);
    else switch (escaped_spaces.str[++i]) {
      case 's':
        spaces.push_back(' '); break;
      case 'p':
        spaces.push_back('|'); break;
      case 't':
        spaces.push_back('\t'); break;
      case 'r':
        spaces.push_back('\r'); break;
      case 'n':
        spaces.push_back('\n'); break;
      case '\\':
        spaces.push_back('\\'); break;
      default:
        spaces.push_back(escaped_spaces.str[i - 1]);
        spaces.push_back(escaped_spaces.str[i]);
    }
}

    string token::create_all_analyzes_misc_field(string &encoded_value) {

      string _misc = start_misc_field("ALL_ANALYZES");
      return _misc + encoded_value;

    }

    void token::remove_all_analyzes_field() {

      remove_misc_field("ALL_ANALYZES");

    }


} // namespace udpipe
} // namespace ufal
