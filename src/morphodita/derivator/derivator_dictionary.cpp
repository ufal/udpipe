// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "derivator_dictionary.h"
#include "utils/binary_decoder.h"
#include "utils/compressor.h"
#include "utils/unaligned_access.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

bool derivator_dictionary::parent(string_piece lemma, derivated_lemma& parent) const {
  if (dictionary) lemma.len = dictionary->lemma_id_len(lemma);

  auto lemma_data = derinet.at(lemma.str, lemma.len, [](pointer_decoder& data) {
    data.next<char>(data.next_1B());
    data.next_4B();
    data.next<uint32_t>(data.next_2B());
  });
  if (lemma_data) {
    auto parent_encoded = *(uint32_t*)(lemma_data + 1 + *lemma_data);
    if (parent_encoded) {
      unsigned parent_len = parent_encoded & 0xFF;
      auto parent_data = derinet.data_start(parent_len) + (parent_encoded >> 8);
      parent.lemma.assign((const char*) parent_data, parent_len);
      if (parent_data[parent_len])
        parent.lemma.append((const char*) parent_data + parent_len + 1, parent_data[parent_len]);
      return true;
    }
  }
  parent.lemma.clear();
  return false;
}

bool derivator_dictionary::children(string_piece lemma, vector<derivated_lemma>& children) const {
  if (dictionary) lemma.len = dictionary->lemma_id_len(lemma);

  auto lemma_data = derinet.at(lemma.str, lemma.len, [](pointer_decoder& data) {
    data.next<char>(data.next_1B());
    data.next_4B();
    data.next<uint32_t>(data.next_2B());
  });
  if (lemma_data) {
    auto children_len = *(uint16_t*)(lemma_data + 1 + *lemma_data + 4);
    auto children_encoded = (uint32_t*)(lemma_data + 1 + *lemma_data + 4 + 2);
    if (children_len) {
      children.resize(children_len);
      for (unsigned i = 0; i < children_len; i++) {
        unsigned child_len = children_encoded[i] & 0xFF;
        auto child_data = derinet.data_start(child_len) + (children_encoded[i] >> 8);
        children[i].lemma.assign((const char*) child_data, child_len);
        if (child_data[child_len])
          children[i].lemma.append((const char*) child_data + child_len + 1, child_data[child_len]);
      }
      return true;
    }
  }
  children.clear();
  return false;
}

bool derivator_dictionary::load(istream& is) {
  binary_decoder data;
  if (!compressor::load(is, data)) return false;

  try {
    for (int i = data.next_1B(); i > 0; i--)
      derinet.resize(data.next_4B());

    unsigned data_position = data.tell();
    vector<char> lemma, parent;
    for (int pass = 1; pass <= 3; pass++) {
      if (pass > 1) data.seek(data_position);

      lemma.clear();
      for (int i = data.next_4B(); i > 0; i--) {
        lemma.resize(lemma.size() - data.next_1B());
        for (int i = data.next_1B(); i > 0; i--)
          lemma.push_back(data.next_1B());

        unsigned char lemma_comment_len = data.next_1B();
        const char* lemma_comment = lemma_comment_len ? data.next<char>(lemma_comment_len) : nullptr;

        unsigned children = data.next_2B();

        if (pass == 3) parent.clear();
        enum { REMOVE_START = 1, REMOVE_END = 2, ADD_START = 4, ADD_END = 8 };
        int operations = data.next_1B();
        if (operations) {
          int remove_start = operations & REMOVE_START ? data.next_1B() : 0;
          int remove_end = operations & REMOVE_END ? data.next_1B() : 0;
          if (operations & ADD_START) {
            int add_start = data.next_1B();
            const char* str = data.next<char>(add_start);
            if (pass == 3) parent.assign(str, str + add_start);
          }
          if (pass == 3) parent.insert(parent.end(), lemma.begin() + remove_start, lemma.end() - remove_end);
          if (operations & ADD_END) {
            int add_end = data.next_1B();
            const char* str = data.next<char>(add_end);
            if (pass == 3) parent.insert(parent.end(), str, str + add_end);
          }
        }

        if (pass == 1) {
          derinet.add(lemma.data(), lemma.size(), 1 + lemma_comment_len + 4 + 2 + 4 * children);
        } else if (pass == 2) {
          unsigned char* lemma_data = derinet.fill(lemma.data(), lemma.size(), 1 + lemma_comment_len + 4 + 2 + 4 * children);
          *lemma_data++ = lemma_comment_len;
          while (lemma_comment_len--) *lemma_data++ = *lemma_comment++;
          unaligned_store_inc<uint32_t>(lemma_data, 0);
          unaligned_store_inc<uint16_t>(lemma_data, children);
          if (children) unaligned_store<uint32_t>(((uint32_t*)lemma_data) + children - 1, 0);
        } else if (pass == 3 && !parent.empty()) {
          auto lemma_data = derinet.at(lemma.data(), lemma.size(), [](pointer_decoder& data) {
            data.next<char>(data.next_1B());
            data.next_4B();
            data.next<uint32_t>(data.next_2B());
          });
          auto parent_data = derinet.at(parent.data(), parent.size(), [](pointer_decoder& data) {
            data.next<char>(data.next_1B());
            data.next_4B();
            data.next<uint32_t>(data.next_2B());
          });
          assert(lemma_data && parent_data);

          unsigned parent_offset = parent_data - parent.size() - derinet.data_start(parent.size());
          assert(parent.size() < (1<<8) && parent_offset < (1<<24));
          unaligned_store<uint32_t>((void *)(lemma_data + 1 + *lemma_data), (parent_offset << 8) | parent.size());

          unsigned lemma_offset = lemma_data - lemma.size() - derinet.data_start(lemma.size());
          assert(lemma.size() < (1<<8) && lemma_offset < (1<<24));
          auto children_len = unaligned_load<uint16_t>(parent_data + 1 + *parent_data + 4);
          auto children = (uint32_t*)(parent_data + 1 + *parent_data + 4 + 2);
          auto child_index = unaligned_load<uint32_t>(children + children_len - 1);
          unaligned_store<uint32_t>(children + child_index, (lemma_offset << 8) | lemma.size());
          if (child_index+1 < children_len)
            unaligned_store<uint32_t>(children + children_len - 1, unaligned_load<uint32_t>(children + children_len - 1) + 1);
        }
      }

      if (pass == 1)
        derinet.done_adding();
      if (pass == 2)
        derinet.done_filling();
    }
  } catch (binary_decoder_error&) {
    return false;
  }
  return true;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
