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
#include "morphodita/derivator/derivator.h"
#include "morphodita/tokenizer/tokenizer.h"

namespace ufal {
namespace udpipe {
namespace morphodita {

struct tagged_form {
  string form;
  string tag;

  tagged_form() {}
  tagged_form(const string& form, const string& tag) : form(form), tag(tag) {}
};

struct tagged_lemma {
  string lemma;
  string tag;

  tagged_lemma() {}
  tagged_lemma(const string& lemma, const string& tag) : lemma(lemma), tag(tag) {}
};

struct tagged_lemma_forms {
  string lemma;
  vector<tagged_form> forms;

  tagged_lemma_forms() {}
  tagged_lemma_forms(const string& lemma) : lemma(lemma) {}
};

class morpho {
 public:
  virtual ~morpho() {}

  static morpho* load(istream& is);
  static morpho* load(const char* fname);

  enum guesser_mode { NO_GUESSER = 0, GUESSER = 1, GUESSER_UNSPECIFIED = -1 };

  // Perform morphologic analysis of a form. The form is given by a pointer and
  // length and therefore does not need to be '\0' terminated.  The guesser
  // parameter specifies whether a guesser can be used if the form is not found
  // in the dictionary. Output is assigned to the lemmas vector.
  //
  // If the form is found in the dictionary, analyses are assigned to lemmas
  // and NO_GUESSER returned. If guesser == GUESSER and the form analyses are
  // found using a guesser, they are assigned to lemmas and GUESSER is
  // returned.  Otherwise <0 is returned and lemmas are filled with one
  // analysis containing given form as lemma and a tag for unknown word.
  virtual int analyze(string_piece form, guesser_mode guesser, vector<tagged_lemma>& lemmas) const = 0;

  // Perform morphologic generation of a lemma. The lemma is given by a pointer
  // and length and therefore does not need to be '\0' terminated. Optionally
  // a tag_wildcard can be specified (or be NULL) and if so, results are
  // filtered using this wildcard. The guesser parameter speficies whether
  // a guesser can be used if the lemma is not found in the dictionary. Output
  // is assigned to the forms vector.
  //
  // Tag_wildcard can be either NULL or a wildcard applied to the results.
  // A ? in the wildcard matches any character, [bytes] matches any of the
  // bytes and [^bytes] matches any byte different from the specified ones.
  // A - has no special meaning inside the bytes and if ] is first in bytes, it
  // does not end the bytes group.
  //
  // If the given lemma is only a raw lemma, all lemma ids with this raw lemma
  // are returned. Otherwise only matching lemma ids are returned, ignoring any
  // lemma comments. For every found lemma, matching forms are filtered using
  // the tag_wildcard. If at least one lemma is found in the dictionary,
  // NO_GUESSER is returned. If guesser == GUESSER and the lemma is found by
  // the guesser, GUESSER is returned. Otherwise, forms are cleared and <0 is
  // returned.
  virtual int generate(string_piece lemma, const char* tag_wildcard, guesser_mode guesser, vector<tagged_lemma_forms>& forms) const = 0;

  // Rawlemma and lemma id identification
  virtual int raw_lemma_len(string_piece lemma) const = 0;
  virtual int lemma_id_len(string_piece lemma) const = 0;

  // Rawform identification
  virtual int raw_form_len(string_piece form) const = 0;

  // Construct a new tokenizer instance appropriate for this morphology.
  // Can return NULL if no such tokenizer exists.
  virtual tokenizer* new_tokenizer() const = 0;

  // Return a derivator for this morphology, or NULL if it does not exist.
  // The returned instance is owned by the morphology and should not be deleted.
  virtual const derivator* get_derivator() const;

 protected:
  unique_ptr<derivator> derinet;
};

} // namespace morphodita
} // namespace udpipe
} // namespace ufal
