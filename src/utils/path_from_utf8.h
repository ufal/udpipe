// This file is part of UFAL C++ Utils <http://github.com/ufal/cpp_utils/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
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

#ifdef _WIN32
inline wstring path_from_utf8(const char* str);
inline wstring path_from_utf8(const string& str);
#else
inline string path_from_utf8(const char* str);
inline const string& path_from_utf8(const string& str);
#endif

//
// Definitions
//

#ifdef _WIN32

inline wstring path_from_utf8(const char* str) {
  // We could implement this using codecvt_utf8_utf16, but it is not available
  // in GCC 4.9, which we still use. We could also use MultiByteToWideChar,
  // but using it would require changing our build infrastructure -- hence
  // we implement the conversion manually.
  wstring wstr;
  while (*str) {
    char32_t chr;
    if (((unsigned char)*str) < 0x80) chr = (unsigned char)*str++;
    else if (((unsigned char)*str) < 0xC0) chr = '?', ++str;
    else if (((unsigned char)*str) < 0xE0) {
      chr = (((unsigned char)*str++) & 0x1F) << 6;
      if (((unsigned char)*str) < 0x80 || ((unsigned char)*str) >= 0xC0) chr = '?';
      else chr += ((unsigned char)*str++) & 0x3F;
    } else if (((unsigned char)*str) < 0xF0) {
      chr = (((unsigned char)*str++) & 0x0F) << 12;
      if (((unsigned char)*str) < 0x80 || ((unsigned char)*str) >= 0xC0) chr = '?';
      else {
        chr += (((unsigned char)*str++) & 0x3F) << 6;
        if (((unsigned char)*str) < 0x80 || ((unsigned char)*str) >= 0xC0) chr = '?';
        else chr += ((unsigned char)*str++) & 0x3F;
      }
    } else if (((unsigned char)*str) < 0xF8) {
      chr = (((unsigned char)*str++) & 0x07) << 18;
      if (((unsigned char)*str) < 0x80 || ((unsigned char)*str) >= 0xC0) chr = '?';
      else {
        chr += (((unsigned char)*str++) & 0x3F) << 12;
        if (((unsigned char)*str) < 0x80 || ((unsigned char)*str) >= 0xC0) chr = '?';
        else {
          chr += (((unsigned char)*str++) & 0x3F) << 6;
          if (((unsigned char)*str) < 0x80 || ((unsigned char)*str) >= 0xC0) chr = '?';
          else chr += ((unsigned char)*str++) & 0x3F;
        }
      }
    } else chr = '?', ++str;

    if (chr <= 0xFFFF) wstr.push_back(chr);
    else if (chr <= 0x10FFFF) {
      wstr.push_back(0xD800 + ((chr - 0x10000) >> 10));
      wstr.push_back(0xDC00 + ((chr - 0x10000) & 0x3FF));
    } else {
      wstr.push_back('?');
    }
  }
  return wstr;
}

inline wstring path_from_utf8(const string& str) {
  return path_from_utf8(str.c_str());
}

#else

inline string path_from_utf8(const char* str) {
  return str;
}

inline const string& path_from_utf8(const string& str) {
  return str;
}

#endif

} // namespace utils
} // namespace udpipe
} // namespace ufal
