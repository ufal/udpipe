#!/bin/bash

# This file is part of UDPipe <http://github.com/ufal/udpipe/>.
#
# Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
# Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

rm -rf ../src/morphodita/*
git -C ../src/morphodita clone --depth=1 --branch=stable https://github.com/ufal/morphodita
cp -a ../src/morphodita/morphodita/src/{derivator,morpho,tagger,tagset_converter,tokenizer,version,Makefile.include} ../src/morphodita/
cp -a ../src/morphodita/morphodita/{AUTHORS,CHANGES.md,LICENSE,README} ../src/morphodita/
rm -rf ../src/morphodita/morphodita/
sed '
  s/^namespace morphodita {/namespace udpipe {\n&/
  s/^} \/\/ namespace morphodita/&\n} \/\/ namespace udpipe/
  ' -i ../src/morphodita/*/*
perl -ple '
  BEGIN { use File::Basename }

  /^#include "([^"]*)"$/ and !-f dirname($ARGV)."/$1" and !-f "../src/$1" and $_ = "#include \"morphodita/$1\"";
  ' -i ../src/morphodita/*/*
sed '
  /^#include "/,/^$/{/^$/i#include "trainer/training_failure.h"
};s/runtime_failure/training_failure/
  ' -i `grep -Rl runtime_failure ../src/morphodita`

# Disable all logging except for accuracy reporting during tagger and tokenizer training.
sed 's#^.*\bcerr\b.*$#//&#' -i `ls ../src/morphodita/*/* | grep -v ../src/morphodita/tokenizer/gru_tokenizer_network_trainer.h`
sed 's#^//\(.*\bcerr\b\(.*[Ii]teration\|.*accuracy\| *<< *endl\).*\)$#\1#' -i ../src/morphodita/tagger/perceptron_tagger_trainer.h
sed 's#^//\( *\).*cerr\b.*\(load_data([^)]*)\).*$#&\n\1\2;#' -i ../src/morphodita/tagger/tagger_trainer.h
sed ':s;N;$!bs; s#\n\( *if (verbose)\n[^\n]*\bcerr\b\)#\n//\1#g' -i ../src/morphodita/derivator/derivator_dictionary_encoder.cpp

# Add patch for larger number of sum of suffix tags
patch -d ../src/morphodita/morpho <<EOF
--- morpho_dictionary_encoder.h	2017-03-01 04:26:06.528951096 +0100
+++ morpho_dictionary_encoder.h.new	2017-03-01 04:26:04.744951136 +0100
@@ -270,12 +270,13 @@
     enc.add_2B(suffix.size());
     for (auto&& clas : suffix)
       enc.add_2B(clas.first);
-    int tags = 0;
+    uint32_t tags = 0, prev_tags = 0;
     for (auto&& clas : suffix) {
-      enc.add_2B(tags);
+      enc.add_2B(tags - prev_tags < (1<<16) ? uint16_t(tags) : tags);
+      prev_tags = tags;
       tags += clas.second.size();
     }
-    enc.add_2B(tags);
+    enc.add_2B(tags - prev_tags < (1<<16) ? uint16_t(tags) : tags);
     for (auto&& clas : suffix)
       for (auto&& tag : clas.second)
         enc.add_2B(tag);
EOF
patch -d ../src/morphodita/morpho <<EOF
--- morpho_dictionary.h	2017-12-18 10:08:00.344536475 +0100
+++ morpho_dictionary.h.new	2017-12-18 10:14:35.935694091 +0100
@@ -136,17 +136,21 @@
   suffixes.iter_all([this](const char* suffix, int len, pointer_decoder& data) mutable {
     unsigned classes_len = data.next_2B();
     const uint16_t* classes_ptr = data.next<uint16_t>(classes_len);
-    const uint16_t* indices_ptr = data.next<uint16_t>(classes_len);
-    const uint16_t* tags_ptr = data.next<uint16_t>(data.next_2B());
+    const uint16_t* indices_ptr = data.next<uint16_t>(classes_len + 1);
+    uint32_t tags_len = unaligned_load<uint16_t>(indices_ptr);
+    for (unsigned i = 0; i < classes_len; i++)
+      tags_len += uint16_t(unaligned_load<uint16_t>(indices_ptr + i + 1) - unaligned_load<uint16_t>(indices_ptr + i));
+    const uint16_t* tags_ptr = data.next<uint16_t>(tags_len);
 
     string suffix_str(suffix, len);
+    uint32_t index = unaligned_load<uint16_t>(indices_ptr), prev_index = 0;
     for (unsigned i = 0; i < classes_len; i++) {
       auto classes_ptr_i = unaligned_load<uint16_t>(classes_ptr + i);
       if (classes_ptr_i >= classes.size()) classes.resize(classes_ptr_i + 1);
+      prev_index = index;
+      index += uint16_t(unaligned_load<uint16_t>(indices_ptr + i + 1) - unaligned_load<uint16_t>(indices_ptr + i));
       classes[classes_ptr_i].emplace_back(suffix_str, vector<uint16_t>());
-      for (const uint16_t* ptr = tags_ptr + unaligned_load<uint16_t>(indices_ptr + i),
-                         * end = tags_ptr + unaligned_load<uint16_t>(indices_ptr + i + 1);
-           ptr < end; ptr++)
+      for (const uint16_t* ptr = tags_ptr + prev_index; ptr < tags_ptr + index; ptr++)
         classes[classes_ptr_i].back().second.emplace_back(unaligned_load<uint16_t>(ptr));
     }
   });
EOF
