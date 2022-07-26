#!/usr/bin/perl
use warnings;
use strict;
use utf8;
use open qw(:std :utf8);

use File::Copy;

sub write_sentences {
  my ($target_file, @sentences) = @_;

  open (my $f, ">", $target_file) or die "Cannot open file $target_file: $!";
  foreach my $sentence (@sentences) {
    print $f "$sentence\n";
  }
  close $f;
}


@ARGV >= 2 or die "Usage: $0 target_directory lang <train_file\n";
my $target = shift @ARGV;
my $lang = shift @ARGV;

# Load training sentences
my ($sentence, @sentences) = ("");
while (<STDIN>) {
  chomp;
  if (/^$/) {
    push @sentences, $sentence;
    $sentence = "";
  } else {
    $sentence .= $_ . "\n";
  }
}
die "Unfinished sentence" if $sentence;

# Generate everything from train_file
my $train_end = int(@sentences * 90 / 100);
die "Zero sentences for dev_file" if $train_end == int(@sentences);

write_sentences("$target/$lang-ud-train.conllu", @sentences[0..$train_end-1]);
write_sentences("$target/$lang-ud-dev.conllu", @sentences[$train_end..$#sentences]);
