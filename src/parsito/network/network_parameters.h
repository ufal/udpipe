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
#include "activation_function.h"

namespace ufal {
namespace udpipe {
namespace parsito {

struct network_trainer {
  enum network_trainer_algorithm {
    SGD,
    SGD_MOMENTUM,
    ADAGRAD,
    ADADELTA,
    ADAM,
  };

  network_trainer_algorithm algorithm;
  float learning_rate, learning_rate_final;
  float momentum, momentum2;
  float epsilon;
};

struct network_parameters {
  unsigned iterations;
  int structured_interval;
  unsigned hidden_layer;
  activation_function::type hidden_layer_type;
  network_trainer trainer;
  unsigned batch_size;
  float initialization_range;
  float l1_regularization;
  float l2_regularization;
  float maxnorm_regularization;
  float dropout_hidden, dropout_input;
  bool early_stopping;
};

} // namespace parsito
} // namespace udpipe
} // namespace ufal
