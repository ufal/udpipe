// This file is part of Parsito <http://github.com/ufal/parsito/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"
#include "parsito/configuration/configuration.h"
#include "parsito/tree/tree.h"

namespace ufal {
namespace udpipe {
namespace parsito {

// Abstract transition class
class transition {
 public:
  virtual ~transition() {}

  virtual bool applicable(const configuration& conf) const = 0;
  virtual int perform(configuration& conf) const = 0;
};

// Specific transition classes
class transition_left_arc : public transition {
 public:
  transition_left_arc(const string& label) : label(label), label_is_root(label == "root") {}

  virtual bool applicable(const configuration& conf) const override;
  virtual int perform(configuration& conf) const override;
 private:
  string label;
  bool label_is_root;
};

class transition_right_arc : public transition {
 public:
  transition_right_arc(const string& label) : label(label), label_is_root(label == "root") {}

  virtual bool applicable(const configuration& conf) const override;
  virtual int perform(configuration& conf) const override;
 private:
  string label;
  bool label_is_root;
};

class transition_shift : public transition {
 public:
  virtual bool applicable(const configuration& conf) const override;
  virtual int perform(configuration& conf) const override;
};

class transition_swap : public transition {
 public:
  virtual bool applicable(const configuration& conf) const override;
  virtual int perform(configuration& conf) const override;
};

class transition_left_arc_2 : public transition {
 public:
  transition_left_arc_2(const string& label) : label(label), label_is_root(label == "root") {}

  virtual bool applicable(const configuration& conf) const override;
  virtual int perform(configuration& conf) const override;
 private:
  string label;
  bool label_is_root;
};

class transition_right_arc_2 : public transition {
 public:
  transition_right_arc_2(const string& label) : label(label), label_is_root(label == "root") {}

  virtual bool applicable(const configuration& conf) const override;
  virtual int perform(configuration& conf) const override;
 private:
  string label;
  bool label_is_root;
};

} // namespace parsito
} // namespace udpipe
} // namespace ufal
