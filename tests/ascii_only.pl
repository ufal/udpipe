#!/usr/bin/perl
use warnings;
use strict;
use File::Find;

my $nonascii = 0;
sub process {
  /\.(c|cpp|h|i)$|^Makefile$/ or return;

  open(my $f, "<", $_) or die "Cannot open file $_: $!";
  while (<$f>) {
    if (/([^[:ascii:]])/) {
      $nonascii = 1;
      print "$File::Find::name: $_";
      last;
    }
  }
  close($f);
}
find(\&process, @ARGV ? $ARGV[0] : '.');
exit($nonascii);
