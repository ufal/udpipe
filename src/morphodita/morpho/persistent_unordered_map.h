// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cstring>
#include <map>
#include <unordered_map>

#include "common.h"
#include "small_stringops.h"
#include "utils/binary_decoder.h"
#include "utils/binary_encoder.h"
#include "utils/pointer_decoder.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

// Declarations
class persistent_unordered_map {
 public:
  // Accessing function
  template <class EntrySize>
  inline const unsigned char* at(const char* str, int len, EntrySize entry_size) const;

  template <class T>
  inline const T* at_typed(const char* str, int len) const;

  template <class EntryProcess>
  inline void iter(const char* str, int len, EntryProcess entry_process) const;

  template <class EntryProcess>
  inline void iter_all(EntryProcess entry_process) const;

  // Two helper functions accessing some internals
  inline int max_length() const;
  inline const unsigned char* data_start(int len) const;

  // Creation functions
  persistent_unordered_map() {}
  template <class Entry, class EntryEncode>
  persistent_unordered_map(const unordered_map<string, Entry>& map, double load_factor, EntryEncode entry_encode);
  template <class Entry, class EntryEncode>
  persistent_unordered_map(const unordered_map<string, Entry>& map, double load_factor, bool add_prefixes, bool add_suffixes, EntryEncode entry_encode);

  // Manual creation functions
  inline void resize(unsigned elems);
  inline void add(const char* str, int str_len, int data_len);
  inline void done_adding();
  inline unsigned char* fill(const char* str, int str_len, int data_len);
  inline void done_filling();

  // Serialization
  inline void load(binary_decoder& data);
  inline void save(binary_encoder& enc);

 private:
  struct fnv_hash;
  vector<fnv_hash> hashes;

  template <class Entry, class EntryEncode>
  void construct(const map<string, Entry>& map, double load_factor, EntryEncode entry_encode);
};


// Definitions
struct persistent_unordered_map::fnv_hash {
  fnv_hash(unsigned num) {
    mask = 1;
    while (mask < num)
      mask <<= 1;
    hash.resize(mask + 1);
    mask--;
  }
  fnv_hash(binary_decoder& data) {
    uint32_t size = data.next_4B();
    mask = size - 2;
    hash.resize(size);
    memcpy(hash.data(), data.next<uint32_t>(size), size * sizeof(uint32_t));

    size = data.next_4B();
    this->data.resize(size);
    memcpy(this->data.data(), data.next<char>(size), size);
  }

  inline uint32_t index(const char* data, int len) const {
    if (len <= 0) return 0;
    if (len == 1) return *(const uint8_t*)data;
    if (len == 2) return *(const uint16_t*)data;

    uint32_t hash = 2166136261U;
    while (len--)
      hash = (hash ^ unsigned(*data++)) * 16777619U;
    return hash & mask;
  }

  inline void save(binary_encoder& enc);

  unsigned mask;
  vector<uint32_t> hash;
  vector<unsigned char> data;
};

template <class EntrySize>
const unsigned char* persistent_unordered_map::at(const char* str, int len, EntrySize entry_size) const {
  if (unsigned(len) >= hashes.size()) return nullptr;

  unsigned index = hashes[len].index(str, len);
  const unsigned char* data = hashes[len].data.data() + hashes[len].hash[index];
  const unsigned char* end = hashes[len].data.data() + hashes[len].hash[index+1];

  if (len <= 2)
    return data != end ? data + len : nullptr;

  while (data < end) {
    if (small_memeq(str, data, len)) return data + len;
    data += len;
    pointer_decoder decoder(data);
    entry_size(decoder);
  }

  return nullptr;
}

template <class T>
const T* persistent_unordered_map::at_typed(const char* str, int len) const {
  if (unsigned(len) >= hashes.size()) return nullptr;

  unsigned index = hashes[len].index(str, len);
  const unsigned char* data = hashes[len].data.data() + hashes[len].hash[index];
  const unsigned char* end = hashes[len].data.data() + hashes[len].hash[index+1];

  if (len <= 2)
    return data != end ? (const T*)(data + len) : nullptr;

  while (data < end) {
    if (small_memeq(str, data, len)) return (const T*)(data + len);
    data += len + sizeof(T);
  }

  return nullptr;
}

template <class EntryProcess>
void persistent_unordered_map::iter(const char* str, int len, EntryProcess entry_process) const {
  if (unsigned(len) >= hashes.size()) return;

  unsigned index = hashes[len].index(str, len);
  const unsigned char* data = hashes[len].data.data() + hashes[len].hash[index];
  const unsigned char* end = hashes[len].data.data() + hashes[len].hash[index+1];

  while (data < end) {
    auto start = (const char*) data;
    data += len;
    pointer_decoder decoder(data);
    entry_process(start, decoder);
  }
}

template <class EntryProcess>
void persistent_unordered_map::iter_all(EntryProcess entry_process) const {
  for (unsigned len = 0; len < hashes.size(); len++) {
    const unsigned char* data = hashes[len].data.data();
    const unsigned char* end = data + hashes[len].data.size();

    while (data < end) {
      auto start = (const char*) data;
      data += len;
      pointer_decoder decoder(data);
      entry_process(start, len, decoder);
    }
  }
}

int persistent_unordered_map::max_length() const {
  return hashes.size();
}

const unsigned char* persistent_unordered_map::data_start(int len) const {
  return unsigned(len) < hashes.size() ? hashes[len].data.data() : nullptr;
}

void persistent_unordered_map::resize(unsigned elems) {
  if (hashes.size() == 0) hashes.emplace_back(1);
  else if (hashes.size() == 1) hashes.emplace_back(1<<8);
  else if (hashes.size() == 2) hashes.emplace_back(1<<16);
  else hashes.emplace_back(elems);
}

void persistent_unordered_map::add(const char* str, int str_len, int data_len) {
  if (unsigned(str_len) < hashes.size())
    hashes[str_len].hash[hashes[str_len].index(str, str_len)] += str_len + data_len;
}

void persistent_unordered_map::done_adding() {
  for (auto&& hash : hashes) {
    int total = 0;
    for (auto&& len : hash.hash) total += len, len = total - len;
    hash.data.resize(total);
  }
}

unsigned char* persistent_unordered_map::fill(const char* str, int str_len, int data_len) {
  if (unsigned(str_len) < hashes.size()) {
    unsigned index = hashes[str_len].index(str, str_len);
    unsigned offset = hashes[str_len].hash[index];
    small_memcpy(hashes[str_len].data.data() + offset, str, str_len);
    hashes[str_len].hash[index] += str_len + data_len;
    return hashes[str_len].data.data() + offset + str_len;
  }
  return nullptr;
}

void persistent_unordered_map::done_filling() {
  for (auto&& hash : hashes)
    for (int i = hash.hash.size() - 1; i >= 0; i--)
      hash.hash[i] = i > 0 ? hash.hash[i-1] : 0;
}

void persistent_unordered_map::load(binary_decoder& data) {
  unsigned sizes = data.next_1B();

  hashes.clear();
  for (unsigned i = 0; i < sizes; i++)
    hashes.emplace_back(data);
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
