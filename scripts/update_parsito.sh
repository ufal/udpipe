#!/bin/bash

# This file is part of UDPipe <http://github.com/ufal/udpipe/>.
#
# Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
# Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

rm -rf ../src/parsito/*
git -C ../src/parsito clone --depth=1 --branch=master https://github.com/ufal/parsito
cp -a ../src/parsito/parsito/src/{configuration,embedding,network,parser,transition,tree,version,Makefile.include} ../src/parsito/
cp -a ../src/parsito/parsito/{AUTHORS,CHANGES,LICENSE,README} ../src/parsito/
rm -rf ../src/parsito/parsito/
sed '
  s/^namespace parsito {/namespace udpipe {\n&/
  s/^} \/\/ namespace parsito/&\n} \/\/ namespace udpipe/
  ' -i ../src/parsito/*/*
perl -ple '
  BEGIN { use File::Basename }

  /^#include "([^"]*)"$/ and !-f dirname($ARGV)."/$1" and !-f "../src/$1" and $_ = "#include \"parsito/$1\"";
  ' -i ../src/parsito/*/*
patch -d ../src/parsito/parser parser_nn_trainer.cpp <<EOF
--- parser_nn_trainer.cpp.ori	2016-05-19 10:38:10.000000000 +0200
+++ parser_nn_trainer.cpp	2016-05-19 10:38:39.235534154 +0200
@@ -12,7 +12,6 @@
 #include <fstream>
 #include <limits>
 #include <random>
-#include <thread>
 #include <unordered_set>
 
 #include "parsito/network/neural_network_trainer.h"
@@ -28,7 +27,7 @@
 
 void parser_nn_trainer::train(const string& transition_system_name, const string& transition_oracle_name,
                               const string& embeddings_description, const string& nodes_description, const network_parameters& parameters,
-                              unsigned number_of_threads, const vector<tree>& train, const vector<tree>& heldout, binary_encoder& enc) {
+                              unsigned /*number_of_threads*/, const vector<tree>& train, const vector<tree>& heldout, binary_encoder& enc) {
   if (train.empty()) runtime_failure("No training data was given!");
 
   // Random generator with fixed seed for reproducibility
@@ -432,13 +431,7 @@
     };
 
     cerr << "Iteration " << iteration << ": ";
-    if (number_of_threads > 1) {
-      vector<thread> threads;
-      for (unsigned i = 0; i < number_of_threads; i++) threads.emplace_back(training);
-      for (; !threads.empty(); threads.pop_back()) threads.back().join();
-    } else {
-      training();
-    }
+    training();
     cerr << "training logprob " << scientific << setprecision(4) << atomic_logprob;
 
     // Evaluate heldout data if present
EOF
sed '
  /^#include "/,/^$/{/^$/i#include "trainer/training_failure.h"
};s/runtime_failure/training_failure/
  ' -i `grep -Rl runtime_failure ../src/parsito`
