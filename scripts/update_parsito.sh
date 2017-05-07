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

# Remove threading
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
 
 void parser_nn_trainer::train(const string& transition_system_name, const string& transition_oracle_name, bool single_root,
                               const string& embeddings_description, const string& nodes_description, const network_parameters& parameters,
-                              unsigned number_of_threads, const vector<tree>& train, const vector<tree>& heldout, binary_encoder& enc) {
+                              unsigned /*number_of_threads*/, const vector<tree>& train, const vector<tree>& heldout, binary_encoder& enc) {
   if (train.empty()) runtime_failure("No training data was given!");
 
   // Random generator with fixed seed for reproducibility
@@ -459,13 +458,7 @@
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

# Add support for returning logprob
patch -d ../src/parsito/parser parser.h <<EOF
--- a/src/parsito/parser/parser.h
+++ b/src/parsito/parser/parser.h
@@ -23,7 +23,7 @@ class parser {
  public:
   virtual ~parser() {};
 
-  virtual void parse(tree& t, unsigned beam_size = 0) const = 0;
+  virtual void parse(tree& t, unsigned beam_size = 0, double* cost = nullptr) const = 0;
 
   enum { NO_CACHE = 0, FULL_CACHE = 2147483647};
   static parser* load(const char* file, unsigned cache = 1000);
EOF
patch -d ../src/parsito/parser parser_nn.cpp <<EOF
--- a/src/parsito/parser/parser_nn.cpp
+++ b/src/parsito/parser/parser_nn.cpp
@@ -22,15 +22,16 @@ namespace parsito {
 
 parser_nn::parser_nn(bool versioned) : versioned(versioned) {}
 
-void parser_nn::parse(tree& t, unsigned beam_size) const {
+void parser_nn::parse(tree& t, unsigned beam_size, double* cost) const {
   if (beam_size > 1)
-    parse_beam_search(t, beam_size);
+    parse_beam_search(t, beam_size, cost);
   else
-    parse_greedy(t);
+    parse_greedy(t, cost);
 }
 
-void parser_nn::parse_greedy(tree& t) const {
+void parser_nn::parse_greedy(tree& t, double* cost) const {
   assert(system);
+  if (cost) *cost = 0.;
 
   // Retrieve or create workspace
   workspace* w = workspaces.pop();
@@ -50,7 +51,8 @@ void parser_nn::parse_greedy(tree& t) const {
   }
 
   // Compute which transitions to perform and perform them
-  while (!w->conf.final()) {
+  int transitions = 0;
+  for (; !w->conf.final(); transitions++) {
     // Extract nodes from the configuration
     nodes.extract(w->conf, w->extracted_nodes);
     w->extracted_embeddings.resize(w->extracted_nodes.size());
@@ -58,7 +60,7 @@ void parser_nn::parse_greedy(tree& t) const {
       w->extracted_embeddings[i] = w->extracted_nodes[i] >= 0 ? &w->embeddings[w->extracted_nodes[i]] : nullptr;
 
     // Classify using neural network
-    network.propagate(embeddings, w->extracted_embeddings, w->network_buffer, w->outcomes, &embeddings_cache, false);
+    network.propagate(embeddings, w->extracted_embeddings, w->network_buffer, w->outcomes, &embeddings_cache, cost ? true : false);
 
     // Find most probable applicable transition
     int best = -1;
@@ -68,6 +70,7 @@ void parser_nn::parse_greedy(tree& t) const {
 
     // Perform the best transition
     int child = system->perform(w->conf, best);
+    if (cost) *cost += log(w->outcomes[best]);
 
     // If a node was linked, recompute its embeddings as deprel has changed
     if (child >= 0)
@@ -77,11 +80,14 @@ void parser_nn::parse_greedy(tree& t) const {
       }
   }
 
+  if (cost && transitions)
+    *cost = *cost / transitions * (t.nodes.size() - 1);
+
   // Store workspace
   workspaces.push(w);
 }
 
-void parser_nn::parse_beam_search(tree& t, unsigned beam_size) const {
+void parser_nn::parse_beam_search(tree& t, unsigned beam_size, double* cost) const {
   assert(system);
 
   // Retrieve or create workspace
@@ -186,6 +192,8 @@ void parser_nn::parse_beam_search(tree& t, unsigned beam_size) const {
       best = i;
   w->bs_confs[iteration & 1][best].refresh_tree();
 
+  if (cost) *cost = w->bs_confs[iteration & 1][best].cost * (t.nodes.size() - 1);
+
   // Store workspace
   workspaces.push(w);
 }
EOF
patch -d ../src/parsito/parser parser_nn.h <<EOF
--- a/src/parsito/parser/parser_nn.h
+++ b/src/parsito/parser/parser_nn.h
@@ -28,15 +28,15 @@ class parser_nn : public parser {
  public:
   parser_nn(bool versioned);
 
-  virtual void parse(tree& t, unsigned beam_size = 0) const override;
+  virtual void parse(tree& t, unsigned beam_size = 0, double* cost = nullptr) const override;
 
  protected:
   virtual void load(binary_decoder& data, unsigned cache) override;
 
  private:
   friend class parser_nn_trainer;
-  void parse_greedy(tree& t) const;
-  void parse_beam_search(tree& t, unsigned beam_size) const;
+  void parse_greedy(tree& t, double* cost) const;
+  void parse_beam_search(tree& t, unsigned beam_size, double* cost) const;
 
   bool versioned;
   unsigned version;
EOF

sed '
  /^#include "/,/^$/{/^$/i#include "trainer/training_failure.h"
};s/runtime_failure/training_failure/
  ' -i `grep -Rl runtime_failure ../src/parsito`
