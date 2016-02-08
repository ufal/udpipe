// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <algorithm>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include "common.h"
#include "persistent_unordered_map_encoder.h"
#include "raw_morpho_dictionary_reader.h"
#include "utils/binary_encoder.h"
#include "utils/new_unique_ptr.h"
#include "utils/parse_int.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

// Declarations
template <class LemmaAddinfo>
class morpho_dictionary_encoder {
 public:
  static void encode(istream& is, int max_suffix_len, binary_encoder& enc);
};

// Definitions
template <class LemmaAddinfo>
class dictionary {
 public:
  void load(istream& is, int max_suffix_len);
  void encode(binary_encoder& enc);

 private:
  class trie {
   public:
    trie() : depth(0) {}

    void add(const char* str) {
      if (!*str) return;

      for (auto&& child : children)
        if (child.first == *str) {
          child.second->add(str + 1);
          depth = max(depth, 1 + child.second->depth);
          return;
        }
      children.emplace_back(*str, new_unique_ptr<trie>());
      children.back().second->add(str + 1);
      depth = max(depth, 1 + children.back().second->depth);
    }

    string find_candidate_prefix(int max_suffix_len) {
      string current, best;
      int best_length = 0;
      find_candidate_prefix(max_suffix_len, current, best, best_length, 0);
      return best;
    }
    void find_candidate_prefix(int max_suffix_len, string& current, string& best, int& best_length, int length) {
      if (depth < max_suffix_len && length > best_length) {
        best = current;
        best_length = length;
      }
      for (auto&& child : children) {
        current.push_back(child.first);
        child.second->find_candidate_prefix(max_suffix_len, current, best, best_length, children.size() == 1 ? length + 1 : 1);
        current.resize(current.size() - 1);
      }
    }

    vector<pair<char, unique_ptr<trie>>> children;
    int depth;
  };

  class histogram {
   public:
    void add(const string& str) {
      if (str.size() >= lengths.size()) lengths.resize(str.size() + 1);
      lengths[str.size()].insert(str);
    }

    void encode(binary_encoder& enc) {
      enc.add_1B(lengths.size());
      for (auto&& set : lengths)
        enc.add_4B(set.size());
    }

    vector<unordered_set<string>> lengths;
  };

  struct lemma_info {
    lemma_info(string lemma) {
      this->lemma = lemma.substr(0, addinfo.parse(lemma, true));
    }

    string lemma;
    LemmaAddinfo addinfo;
    struct lemma_form_info {
      lemma_form_info(string form, int clas) : form(form), clas(clas) {}

      string form;
      int clas;

      bool operator<(const lemma_form_info& other) const { return form < other.form || (form == other.form && clas < other.clas); }
    };
    vector<lemma_form_info> forms;

    bool operator<(const lemma_info& other) const { return lemma < other.lemma || (lemma == other.lemma && addinfo.data < other.addinfo.data); }
  };

  unordered_map<string, int> classes;
  unordered_map<string, map<int, vector<int>>> suffixes;

  vector<string> tags;
  unordered_map<string, int> tags_map;

  histogram lemmas_hist, forms_hist;

  vector<lemma_info> lemmas;
};

template <class LemmaAddinfo>
void morpho_dictionary_encoder<LemmaAddinfo>::encode(istream& is, int max_suffix_len, binary_encoder& enc) {
  dictionary<LemmaAddinfo> dict;

  // Load the dictionary and create classes
  dict.load(is, max_suffix_len);

  // Encode the dictionary
  dict.encode(enc);
}

template <class LemmaAddinfo>
void dictionary<LemmaAddinfo>::load(istream& is, int max_suffix_len) {
  // Load lemmas and create classes
  raw_morpho_dictionary_reader raw(is);
  string lemma;
  vector<pair<string, string>> forms;
  while(raw.next_lemma(lemma, forms)) {
    // Make sure forms are unique
    sort(forms.begin(), forms.end());
    auto forms_end = unique(forms.begin(), forms.end());
    if (forms_end != forms.end()) {
      cerr << "Warning: repeated form-tag in lemma " << lemma << '.' << endl;
      forms.erase(forms_end, forms.end());
    }

    // Create lemma_info
    lemmas.emplace_back(lemma);
    auto& lemma_info = lemmas.back();
    lemmas_hist.add(lemma_info.lemma);

    // Create classes
    while (!forms.empty()) {
      trie t;
      for (auto&& form : forms)
        t.add(form.first.c_str());

      // Find prefix of forms in class being added.
      string prefix = t.find_candidate_prefix(max_suffix_len);

      // Find forms of the class being added.
      auto start = forms.begin();
      while (start != forms.end() && start->first.compare(0, prefix.size(), prefix) != 0) start++;
      if (start == forms.end()) runtime_failure("Internal error when generating classes, cannot find prefix '" << prefix << "'!");
      auto end = start;
      while (end != forms.end() && end->first.compare(0, prefix.size(), prefix) == 0) end++;

      // Find common prefix of class forms -- may be larger than prefix.
      int common_prefix = prefix.size();
      while (common_prefix < int(start->first.size()) && start->first[common_prefix] == (end-1)->first[common_prefix]) common_prefix++;

      string clas;
      for (auto form = start; form != end; form++) {
        if (!clas.empty()) clas.push_back('\t');
        clas.append(form->first, common_prefix, string::npos);
        clas.push_back('\t');
        clas.append(form->second);
      }

      auto class_it = classes.emplace(clas, classes.size());
      int class_id = class_it.first->second;
      if (class_it.second) {
        // New class, add it, together with its tags.
        for (auto form = start; form != end; form++) {
          int tag = tags_map.emplace(form->second, tags.size()).first->second;
          if (tag >= int(tags.size())) tags.emplace_back(form->second);
          suffixes[form->first.substr(common_prefix)][class_id].emplace_back(tag);
        }
      }

      // Move forms in the class being added to lemma and remove them from unprocessed forms.
      lemma_info.forms.emplace_back(start->first.substr(0, common_prefix), class_id);
      forms_hist.add(lemma_info.forms.back().form);
      forms.erase(start, end);
    }
    stable_sort(lemma_info.forms.begin(), lemma_info.forms.end());
  }
  stable_sort(lemmas.begin(), lemmas.end());
}

template <class LemmaAddinfo>
void dictionary<LemmaAddinfo>::encode(binary_encoder& enc) {
  // Encode lemmas and forms
  lemmas_hist.encode(enc);
  forms_hist.encode(enc);

  string prev = "";
  enc.add_4B(lemmas.size());
  for (auto&& lemma : lemmas) {
    int cpl = 0;
    while (prev[cpl] && prev[cpl] == lemma.lemma[cpl]) cpl++;

    enc.add_1B(prev.length() - cpl);
    enc.add_1B(lemma.lemma.size() - cpl);
    enc.add_data(lemma.lemma.substr(cpl));
    enc.add_1B(lemma.addinfo.data.size());
    enc.add_data(lemma.addinfo.data);
    enc.add_1B(lemma.forms.size());

    string prev_form = lemma.lemma;
    for (auto&& lemma_form : lemma.forms) {
      unsigned best_prev_from = 0, best_form_from = 0, best_len = 0;
      for (unsigned prev_from = 0; prev_from < prev_form.size(); prev_from++)
        for (unsigned form_from = 0; form_from < lemma_form.form.size(); form_from++) {
          unsigned len = 0;
          while (prev_from + len < prev_form.size() && form_from + len < lemma_form.form.size() && prev_form[prev_from+len] == lemma_form.form[form_from+len]) len++;
          if (len > best_len) best_prev_from = prev_from, best_form_from = form_from, best_len = len;
        }

      enum { REMOVE_START = 1, REMOVE_END = 2, ADD_START = 4, ADD_END = 8 };
      enc.add_1B(REMOVE_START * (best_prev_from>0) + REMOVE_END * (best_prev_from+best_len<prev_form.size()) +
             ADD_START * (best_form_from>0) + ADD_END * (best_form_from+best_len<lemma_form.form.size()));
      if (best_prev_from > 0) enc.add_1B(best_prev_from);
      if (best_prev_from + best_len < prev_form.size()) enc.add_1B(prev_form.size() - best_prev_from - best_len);
      if (best_form_from > 0) {
        enc.add_1B(best_form_from);
        enc.add_data(lemma_form.form.substr(0, best_form_from));
      }
      if (best_form_from + best_len < lemma_form.form.size()) {
        enc.add_1B(lemma_form.form.size() - best_form_from - best_len);
        enc.add_data(lemma_form.form.substr(best_form_from + best_len));
      }
      enc.add_2B(lemma_form.clas);

      prev_form = lemma_form.form;
    }

    prev = lemma.lemma;
  }

  // Encode tags
  enc.add_2B(tags.size());
  for (auto&& tag : tags) {
    enc.add_1B(tag.size());
    enc.add_data(tag);
  }

  // Encode classes
  persistent_unordered_map(suffixes, 5, false, true, [](binary_encoder& enc, const map<int, vector<int>>& suffix) {
    enc.add_2B(suffix.size());
    for (auto&& clas : suffix)
      enc.add_2B(clas.first);
    int tags = 0;
    for (auto&& clas : suffix) {
      enc.add_2B(tags);
      tags += clas.second.size();
    }
    enc.add_2B(tags);
    for (auto&& clas : suffix)
      for (auto&& tag : clas.second)
        enc.add_2B(tag);
  }).save(enc);
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
