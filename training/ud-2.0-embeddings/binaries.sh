#!/bin/sh

make -C ../../src -j4 udpipe && cp ../../src/udpipe .
make -C ../scripts -j4 normalize_form && cp ../scripts/normalize_form .
