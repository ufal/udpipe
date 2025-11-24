// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>
#include <set>
#include <map>
#include <unordered_map>

#include "derivator_dictionary.h"
#include "derivator_dictionary_encoder.h"
#include "morphodita/morpho/morpho.h"
#include "morphodita/morpho/morpho_ids.h"
#include "utils/binary_encoder.h"
#include "utils/compressor.h"
#include "utils/new_unique_ptr.h"
#include "utils/split.h"
#include "trainer/training_failure.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

void derivator_dictionary_encoder::encode(istream& is, istream& dictionary, bool verbose, ostream& os) {
  // Load the morphology
//  cerr << "Loading morphology: ";
  auto dictionary_start = dictionary.tellg();

  if (dictionary.peek() == morpho_ids::DERIVATOR_DICTIONARY) {
//    cerr << "The given model already has a derivator, dropping it." << endl;
    (void)dictionary.get();
    auto derinet = new_unique_ptr<derivator_dictionary>();
    if (!derinet->load(dictionary)) training_failure("Could not load the derivator of the given morpho model!");
  }

  unique_ptr<morpho> morpho(morpho::load(dictionary));
  if (!morpho) training_failure("Cannot load morpho model from given file!");
  if (morpho->get_derivator()) training_failure("The given morpho model already has a derivator!");
  auto dictionary_end = dictionary.tellg();
//  cerr << "done" << endl;

  // Load the derivator
//  cerr << "Loading derivator data: ";

  struct lemma_info {
    string sense;
    string comment;
    string parent;
    set<string> parents;
    unsigned children;
    unsigned mark;

    lemma_info(const string& sense = string(), const string& comment = string())
        : sense(sense), comment(comment), children(0), mark(0) {}
  };
  map<string, lemma_info> derinet;

  string line;
  string part_lid, lemma_lid, lemma_comment;
  vector<string> tokens;
  vector<string> parts;
  unordered_map<string, lemma_info> matched[2];
  vector<tagged_lemma_forms> matched_lemmas_forms;
  while (getline(is, line)) {
    split(line, '\t', tokens);
    if (tokens.size() != 2) training_failure("Expected two tab separated columns on derivator line '" << line << "'!");

    // Generate all possible lemmas and parents
    for (int i = 0; i < 2; i++) {
      split(tokens[i], ' ', parts);
      if (parts.size() > 2) training_failure("The derivator lemma desctiption '" << tokens[i] << "' contains two or more spaces!");
      bool is_lemma_id = parts.size() == 1;

      part_lid.assign(parts[0], 0, morpho->lemma_id_len(parts[0]));
      morpho->generate(parts[0], is_lemma_id ? nullptr : parts[1].c_str(), morpho::NO_GUESSER, matched_lemmas_forms);

      matched[i].clear();
      for (auto&& lemma_forms : matched_lemmas_forms) {
        lemma_lid.assign(lemma_forms.lemma, 0, morpho->lemma_id_len(lemma_forms.lemma));

        if (!is_lemma_id || part_lid == lemma_lid) {
          // Choose only the shortest lemma comment for the lemma id of lemma_form.lemma
          lemma_comment.assign(lemma_forms.lemma, lemma_lid.size(), string::npos);
          auto it = matched[i].emplace(lemma_lid, lemma_info(lemma_lid.substr(morpho->raw_lemma_len(lemma_lid)), lemma_comment));
          if (!it.second &&
              (lemma_comment.size() < it.first->second.comment.size() ||
               (lemma_comment.size() == it.first->second.comment.size() && lemma_comment < it.first->second.comment)))
            it.first->second.comment.assign(lemma_comment);
        }
      }
    }
    if (matched[0].empty() || matched[1].empty()) {
//      if (verbose)
//        cerr << "Could not match a lemma from line '" << line << "', skipping." << endl;
      continue;
    }

    // Ignore self-loops, i.e., when any parent is equal to any child.
    bool self_loop = false;
    for (auto&& lemma : matched[0])
      if (matched[1].count(lemma.first)) {
        self_loop = true;
        break;
      }
    if (self_loop) {
//      if (verbose)
//        cerr << "Ignoring self-loop from line '" << line << "', skipping." << endl;
      continue;
    }

    // Store the possible parents
    derinet.insert(matched[0].begin(), matched[0].end());
    derinet.insert(matched[1].begin(), matched[1].end());
    for (auto&& lemma : matched[0])
      for (auto&& parent : matched[1])
        derinet[lemma.first].parents.insert(parent.first);
  }
//  cerr << "done" << endl;

  // Choose unique parent for every lemma
  for (auto&& lemma : derinet)
    if (!lemma.second.parents.empty()) {
      // Try locating lexicographically smallest parent with the same sense
      for (auto&& parent : lemma.second.parents)
        if (derinet[parent].sense == lemma.second.sense) {
          lemma.second.parent.assign(parent);
          break;
        }

      // Otherwise, choose the lexicographically smallest parent
      if (lemma.second.parent.empty())
          lemma.second.parent.assign(*lemma.second.parents.begin());

      // Add this edge also to the parent
      derinet[lemma.second.parent].children++;

//      if (verbose)
//        cerr << lemma.first << lemma.second.comment << " -> " << lemma.second.parent << derinet[lemma.second.parent].comment << endl;
    }

  // Make sure the derinet contains no cycles
  unsigned mark = 0;
  bool contains_cycle = false;
  for (auto&& lemma : derinet) {
    lemma.second.mark = ++mark;
    for (auto node = derinet.find(lemma.first); !node->second.parent.empty(); ) {
      node = derinet.find(node->second.parent);
      if (node->second.mark) {
        if (node->second.mark == mark) {
//          cerr << "The derivator data contains a cycle with lemma '" << node->first << "' starting from '" << lemma.first << "'!" << endl;
          contains_cycle = true;
        }
        break;
      }
      node->second.mark = mark;
    }
  }
  if (contains_cycle)
    training_failure("The derivator data contains cycles, aborting!");

  // Encode the derivator
//  cerr << "Encoding derivator: ";
  os.put(morpho_ids::DERIVATOR_DICTIONARY);

  binary_encoder enc;

  vector<int> lengths;
  for (auto&& lemma : derinet) {
    if (lemma.first.size() >= lengths.size())
      lengths.resize(lemma.first.size() + 1);
    lengths[lemma.first.size()]++;
  }
  enc.add_1B(lengths.size());
  for (auto&& length : lengths)
    enc.add_4B(length);

  enc.add_4B(derinet.size());
  string prev = "";
  for (auto&& lemma : derinet) {
    int cpl = 0;
    while (prev[cpl] && prev[cpl] == lemma.first[cpl]) cpl++;
    enc.add_1B(prev.size() - cpl);
    enc.add_1B(lemma.first.size() - cpl);
    enc.add_data(lemma.first.c_str() + cpl);

    enc.add_1B(lemma.second.comment.size());
    enc.add_data(lemma.second.comment);

    enc.add_2B(lemma.second.children);

    if (lemma.second.parent.empty()) {
      enc.add_1B(0);
    } else {
      unsigned best_lemma_from = 0, best_parent_from = 0, best_len = 0;
      for (unsigned lemma_from = 0; lemma_from < lemma.first.size(); lemma_from++)
        for (unsigned parent_from = 0; parent_from < lemma.second.parent.size(); parent_from++) {
          unsigned len = 0;
          while (lemma_from + len < lemma.first.size() &&
                 parent_from + len < lemma.second.parent.size() &&
                 lemma.first[lemma_from+len] == lemma.second.parent[parent_from+len])
            len++;
          if (len > best_len) best_lemma_from = lemma_from, best_parent_from = parent_from, best_len = len;
        }

      enum { REMOVE_START = 1, REMOVE_END = 2, ADD_START = 4, ADD_END = 8 };
      enc.add_1B(REMOVE_START * (best_lemma_from>0) + REMOVE_END * (best_lemma_from+best_len<lemma.first.size()) +
             ADD_START * (best_parent_from>0) + ADD_END * (best_parent_from+best_len<lemma.second.parent.size()));
      if (best_lemma_from > 0) enc.add_1B(best_lemma_from);
      if (best_lemma_from + best_len < lemma.first.size()) enc.add_1B(lemma.first.size() - best_lemma_from - best_len);
      if (best_parent_from > 0) {
        enc.add_1B(best_parent_from);
        enc.add_data(string_piece(lemma.second.parent.c_str(), best_parent_from));
      }
      if (best_parent_from + best_len < lemma.second.parent.size()) {
        enc.add_1B(lemma.second.parent.size() - best_parent_from - best_len);
        enc.add_data(lemma.second.parent.c_str() + best_parent_from + best_len);
      }
    }

    prev.assign(lemma.first);
  }
  compressor::save(os, enc);

  // Append the morphology after the derivator dictionary model
  if (!dictionary.seekg(dictionary_start, dictionary.beg)) training_failure("Cannot seek in the morpho model!");
  for (auto length = dictionary_end - dictionary_start; length; length--)
    os.put(dictionary.get());

//  cerr << "done" << endl;
}

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
