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
