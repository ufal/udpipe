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
git -C ../src/morphodita clone --depth=1 --branch=master https://github.com/ufal/morphodita
cp -a ../src/morphodita/morphodita/src/{morpho,tagger,tagset_converter,tokenizer,version,Makefile.include} ../src/morphodita/
cp -a ../src/morphodita/morphodita/{AUTHORS,CHANGES,LICENSE,README} ../src/morphodita/
rm -rf ../src/morphodita/morphodita/
sed '
  s/^namespace morphodita {/namespace udpipe {\n&/
  s/^} \/\/ namespace morphodita/&\n} \/\/ namespace udpipe/
  ' -i ../src/morphodita/*/*
perl -ple '
  BEGIN { use File::Basename }

  /^#include "([^"]*)"$/ and !-f dirname($ARGV)."/$1" and !-f "../src/$1" and $_ = "#include \"morphodita/$1\"";
  ' -i ../src/morphodita/*/*

# Disable all logging except for accuracy reporting during tagger training.
sed 's#^.*\bcerr\b.*$#//&#' -i ../src/morphodita/*/*
sed 's#^//\(.*\bcerr\b\(.*[Ii]teration\|.*accuracy\| *<< *endl\).*\)$#\1#' -i ../src/morphodita/tagger/perceptron_tagger_trainer.h
