// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"
#include "morpho.h"
#include "persistent_unordered_map.h"
#include "small_stringops.h"
#include "tag_filter.h"
#include "utils/binary_decoder.h"
#include "utils/unaligned_access.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

// Declarations
template <class LemmaAddinfo>
class morpho_dictionary {
 public:
  void load(binary_decoder& data);
  void analyze(string_piece form, vector<tagged_lemma>& lemmas) const;
  bool generate(string_piece lemma, const tag_filter& filter, vector<tagged_lemma_forms>& lemmas_forms) const;
 private:
  persistent_unordered_map lemmas, roots, suffixes;

  vector<string> tags;
  vector<vector<pair<string, vector<uint16_t>>>> classes;
};


// Definitions
template <class LemmaAddinfo>
void morpho_dictionary<LemmaAddinfo>::load(binary_decoder& data) {
  // Prepare lemmas and roots hashes
  for (int i = data.next_1B(); i > 0; i--)
    lemmas.resize(data.next_4B());
  for (int i = data.next_1B(); i > 0; i--)
    roots.resize(data.next_4B());

  // Perform two pass over the lemmas and roots data, filling the hashes.

  vector<char> lemma(max(lemmas.max_length(), roots.max_length()));
  vector<char> root(max(lemmas.max_length(), roots.max_length()));
  unsigned data_position = data.tell();
  for (int pass = 1; pass <= 2; pass++) {
    if (pass > 1) data.seek(data_position);

    int lemma_len = 0;
    int root_len = 0;

    for (int i = data.next_4B(); i > 0; i--) {
      lemma_len -= data.next_1B();
      for (int i = data.next_1B(); i > 0; i--)
        lemma[lemma_len++] = data.next_1B();
      unsigned char lemma_info_len = data.next_1B();
      const char* lemma_info = lemma_info_len ? data.next<char>(lemma_info_len) : nullptr;
      unsigned lemma_roots = data.next_1B();

      unsigned char* lemma_data /* to keep compiler happy */ = nullptr;
      unsigned lemma_offset /* to keep compiler happy */ = 0;

      if (pass == 1) {
        lemmas.add(lemma.data(), lemma_len, 1 + lemma_info_len + 1 + lemma_roots * (sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint16_t)));
      } else /*if (pass == 2)*/ {
        lemma_data = lemmas.fill(lemma.data(), lemma_len, 1 + lemma_info_len + 1 + lemma_roots * (sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint16_t)));
        lemma_offset = lemma_data - lemma_len - lemmas.data_start(lemma_len);

        *lemma_data++ = lemma_info_len;
        if (lemma_info_len) small_memcpy(lemma_data, lemma_info, lemma_info_len), lemma_data += lemma_info_len;
        *lemma_data++ = lemma_roots;
      }

      small_memcpy(root.data(), lemma.data(), lemma_len); root_len = lemma_len;
      for (unsigned i = 0; i < lemma_roots; i++) {
        enum { REMOVE_START = 1, REMOVE_END = 2, ADD_START = 4, ADD_END = 8 };
        int operations = data.next_1B();
        if (operations & REMOVE_START) { int from = data.next_1B(), to = 0; while (from < root_len) root[to++] = root[from++]; root_len = to; }
        if (operations & REMOVE_END) root_len -= data.next_1B();
        if (operations & ADD_START) {
          int from = root_len, to = from + data.next_1B(); while (from > 0) root[--to] = root[--from]; root_len += to;
          for (int i = 0; i < to; i++) root[i] = data.next_1B();
        }
        if (operations & ADD_END)
          for (int len = data.next_1B(); len > 0; len--)
            root[root_len++] = data.next_1B();
        uint16_t clas = data.next_2B();

        if (pass == 1) { // for each root
          roots.add(root.data(), root_len, sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint8_t));
        } else /*if (pass == 2)*/ {
          unsigned char* root_data = roots.fill(root.data(), root_len, sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint8_t));
          unsigned root_offset = root_data - root_len - roots.data_start(root_len);

          unaligned_store_inc<uint16_t>(root_data, clas);
          unaligned_store_inc<uint32_t>(root_data, lemma_offset);
          unaligned_store_inc<uint8_t>(root_data, lemma_len);
          assert(uint8_t(lemma_len) == lemma_len);

          unaligned_store_inc<uint32_t>(lemma_data, root_offset);
          unaligned_store_inc<uint8_t>(lemma_data, root_len);
          unaligned_store_inc<uint16_t>(lemma_data, clas);
          assert(uint8_t(root_len) == root_len);
        }
      }
    }

    if (pass == 1) { // after the whole pass
      lemmas.done_adding();
      roots.done_adding();
    } else /*if (pass == 2)*/ {
      lemmas.done_filling();
      roots.done_filling();
    }
  }

  // Load tags
  tags.resize(data.next_2B());
  for (auto&& tag : tags) {
    tag.resize(data.next_1B());
    for (unsigned i = 0; i < tag.size(); i++)
      tag[i] = data.next_1B();
  }

  // Load suffixes
  suffixes.load(data);

  // Fill classes from suffixes
  suffixes.iter_all([this](const char* suffix, int len, pointer_decoder& data) mutable {
    unsigned classes_len = data.next_2B();
    const uint16_t* classes_ptr = data.next<uint16_t>(classes_len);
    const uint16_t* indices_ptr = data.next<uint16_t>(classes_len + 1);
    uint32_t tags_len = unaligned_load<uint16_t>(indices_ptr);
    for (unsigned i = 0; i < classes_len; i++)
      tags_len += uint16_t(unaligned_load<uint16_t>(indices_ptr + i + 1) - unaligned_load<uint16_t>(indices_ptr + i));
    const uint16_t* tags_ptr = data.next<uint16_t>(tags_len);

    string suffix_str(suffix, len);
    uint32_t index = unaligned_load<uint16_t>(indices_ptr), prev_index = 0;
    for (unsigned i = 0; i < classes_len; i++) {
      auto classes_ptr_i = unaligned_load<uint16_t>(classes_ptr + i);
      if (classes_ptr_i >= classes.size()) classes.resize(classes_ptr_i + 1);
      prev_index = index;
      index += uint16_t(unaligned_load<uint16_t>(indices_ptr + i + 1) - unaligned_load<uint16_t>(indices_ptr + i));
      classes[classes_ptr_i].emplace_back(suffix_str, vector<uint16_t>());
      for (const uint16_t* ptr = tags_ptr + prev_index; ptr < tags_ptr + index; ptr++)
        classes[classes_ptr_i].back().second.emplace_back(unaligned_load<uint16_t>(ptr));
    }
  });
}

template <class LemmaAddinfo>
void morpho_dictionary<LemmaAddinfo>::analyze(string_piece form, vector<tagged_lemma>& lemmas) const {
  int max_suffix_len = suffixes.max_length();

  uint16_t* suff_stack[16]; vector<uint16_t*> suff_heap;
  uint16_t** suff = max_suffix_len <= 16 ? suff_stack : (suff_heap.resize(max_suffix_len), suff_heap.data());
  int suff_len = 0;
  for (int i = form.len; i >= 0 && suff_len < max_suffix_len; i--, suff_len++) {
    suff[suff_len] = (uint16_t*) suffixes.at(form.str + i, suff_len, [](pointer_decoder& data) {
      data.next<uint16_t>(2 * data.next_2B());
      data.next<uint16_t>(data.next_2B());
    });
    if (!suff[suff_len]) break;
  }

  for (int root_len = int(form.len) - --suff_len; suff_len >= 0 && root_len < int(roots.max_length()); suff_len--, root_len++)
    if (unaligned_load<uint16_t>(suff[suff_len])) {
      unsigned suff_classes = unaligned_load<uint16_t>(suff[suff_len]);
      uint16_t* suff_data = suff[suff_len] + 1;

      roots.iter(form.str, root_len, [&](const char* root, pointer_decoder& root_data) {
        uint16_t root_class = root_data.next_2B();
        unsigned lemma_offset = root_data.next_4B();
        unsigned lemma_len = root_data.next_1B();

        if (small_memeq(form.str, root, root_len)) {
          uint16_t* suffix_class_ptr = unaligned_lower_bound(suff_data, suff_classes, root_class);
          if (suffix_class_ptr < suff_data + suff_classes && unaligned_load<uint16_t>(suffix_class_ptr) == root_class) {
            const unsigned char* lemma_data = this->lemmas.data_start(lemma_len) + lemma_offset;
            string lemma((const char*)lemma_data, lemma_len);
            if (lemma_data[lemma_len]) lemma += LemmaAddinfo::format(lemma_data + lemma_len + 1, lemma_data[lemma_len]);

            uint16_t* suff_tag_indices = suff_data + suff_classes;
            uint16_t* suff_tags = suff_tag_indices + suff_classes + 1;
            for (unsigned i = unaligned_load<uint16_t>(suff_tag_indices + (suffix_class_ptr - suff_data));
                 i < unaligned_load<uint16_t>(suff_tag_indices + (suffix_class_ptr - suff_data) + 1); i++)
              lemmas.emplace_back(lemma, tags[unaligned_load<uint16_t>(suff_tags + i)]);
          }
        }
      });
    }
}

template <class LemmaAddinfo>
bool morpho_dictionary<LemmaAddinfo>::generate(string_piece lemma, const tag_filter& filter, vector<tagged_lemma_forms>& lemmas_forms) const {
  LemmaAddinfo addinfo;
  int raw_lemma_len = addinfo.parse(lemma);
  bool matched_lemma = false;

  lemmas.iter(lemma.str, raw_lemma_len, [&](const char* lemma_str, pointer_decoder& data) {
    unsigned lemma_info_len = data.next_1B();
    const auto* lemma_info = data.next<unsigned char>(lemma_info_len);
    unsigned lemma_roots_len = data.next_1B();
    auto* lemma_roots_ptr = data.next<unsigned char>(lemma_roots_len * (sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint16_t)));

    if (small_memeq(lemma.str, lemma_str, raw_lemma_len) && addinfo.match_lemma_id(lemma_info, lemma_info_len) && LemmaAddinfo::generatable(lemma_info, lemma_info_len)) {
      matched_lemma = true;

      vector<tagged_form>* forms = nullptr;
      pointer_decoder lemma_roots(lemma_roots_ptr);
      for (unsigned i = 0; i < lemma_roots_len; i++) {
        unsigned root_offset = lemma_roots.next_4B();
        unsigned root_len = lemma_roots.next_1B();
        unsigned clas = lemma_roots.next_2B();

        const unsigned char* root_data = roots.data_start(root_len) + root_offset;
        for (auto&& suffix : classes[clas]) {
          string root_with_suffix;
          for (auto&& tag : suffix.second)
            if (filter.matches(tags[tag].c_str())) {
              if (!forms) {
                lemmas_forms.emplace_back(string(lemma.str, raw_lemma_len) + LemmaAddinfo::format(lemma_info, lemma_info_len));
                forms = &lemmas_forms.back().forms;
              }

              if (root_with_suffix.empty() && root_len + suffix.first.size()) {
                root_with_suffix.reserve(root_len + suffix.first.size());
                root_with_suffix.assign((const char*)root_data, root_len);
                root_with_suffix.append(suffix.first);
              }

              forms->emplace_back(root_with_suffix, tags[tag]);
            }
        }
      }
    }
  });

  return matched_lemma;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
