#!/bin/sh

set -e

[ -n "$1" ] || { echo Usage: $0 version >&2; exit 1; }

dir=ufal.udpipe
rm -rf $dir
mkdir -p $dir

# Local files
cp -a Changes ../../LICENSE MANIFEST.in ../../bindings/python/examples tests $dir
mkdir $dir/tests/data
cp ../test_data/test.model $dir/tests/data

# UDPipe sources and SWIG files
make -C ../../src_lib_only udpipe.cpp
make -C ../../bindings/python clean
make -C ../../bindings/python udpipe_python.cpp
cp -a ../../bindings/python/ufal $dir
cp ../../src_lib_only/udpipe.[ch]* $dir/ufal/udpipe
cp ../../bindings/python/udpipe_python.cpp $dir/ufal/udpipe

# Fill in version
sed "s/^\\( *version *= *'\\)[^']*'/\\1$1'/" setup.py >$dir/setup.py
sed "s/^# *__version__ *=.*$/__version__ = \"$1\"/" $dir/ufal/udpipe/__init__.py -i

# README file
./README.sh >$dir/README

# Generate sdist
(cd ufal.udpipe && python3 setup.py sdist && cd dist && tar xf ufal.udpipe-$1.tar.gz)
