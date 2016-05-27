#!/bin/sh

set -e

[ -n "$1" ] || { echo Usage: $0 version >&2; exit 1; }

dir=Ufal-UDPipe
rm -rf $dir
mkdir -p $dir

# Local files
cp -a Build.PL Changes ../../LICENSE README ../../bindings/perl/examples t $dir
mkdir $dir/t/data
cp ../test_data/test.model $dir/t/data

# UDPipe sources
make -C ../../src_lib_only udpipe.cpp
mkdir $dir/udpipe
cp ../../src_lib_only/udpipe.[ch]* $dir/udpipe

# SWIG files
make -C ../../bindings/perl udpipe_perl.cpp
cp ../../bindings/perl/udpipe_perl.cpp $dir/udpipe
mkdir -p $dir/lib/Ufal
cp ../../bindings/perl/Ufal/UDPipe.pm UDPipe.xs $dir/lib/Ufal

# Fill in version
perl -e '%p=(); while(<>) {
  print "=head1 VERSION\n\n'"$1"'\n\n" if /^=head1 DESCRIPTION/;
  print;
  print "our \$VERSION = '"'$1'"';\n" if /^package ([^;]*);$/ and not $p{$1}++;
} ' -i $dir/lib/Ufal/UDPipe.pm

# POD file
./UDPipe.pod.sh >$dir/lib/Ufal/UDPipe.pod

# Generate manifest
(cd Ufal-UDPipe && perl Build.PL && perl Build manifest && perl Build dist && mv Ufal-UDPipe-*$1.tar.gz .. && perl Build distclean && rm -f MANIFEST.SKIP.bak)
