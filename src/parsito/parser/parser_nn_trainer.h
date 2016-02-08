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
#include "parsito/network/network_parameters.h"
#include "utils/binary_encoder.h"
#include "parsito/tree/tree.h"

namespace ufal {
namespace udpipe {
namespace parsito {

class parser_nn_trainer {
 public:
  static void train(const string& transition_system_name, const string& transition_oracle_name,
                    const string& embeddings_description, const string& nodes_description, const network_parameters& parameters,
                    unsigned number_of_threads, const vector<tree>& train, const vector<tree>& heldout, binary_encoder& enc);
};

} // namespace parsito
} // namespace udpipe
} // namespace ufal
