#!/bin/sh

set -e

[ -n "$1" ] || { echo Usage: $0 version >&2; exit 1; }

dir=ufal.udpipe
rm -rf $dir
mkdir -p $dir

# Local files
cp -a Changes ../../LICENSE MANIFEST.in ../../bindings/python/examples test $dir
mkdir $dir/test/data
cp ../test_data/test.model $dir/test/data

# UDPipe sources
make -C ../../src_lib_only udpipe.cpp
mkdir $dir/udpipe
cp ../../src_lib_only/udpipe.[ch]* $dir/udpipe

# SWIG files
make -C ../../bindings/python udpipe_python.cpp
cp ../../bindings/python/udpipe_python.cpp $dir/udpipe
cp -a ../../bindings/python/ufal $dir

# Fill in version
sed "s/^\\( *version *= *'\\)[^']*'/\\1$1'/" setup.py >$dir/setup.py

# README file
./README.sh >$dir/README

# Generate sdist
(cd ufal.udpipe && python setup.py sdist && cd dist && tar xf ufal.udpipe-$1.tar.gz)
