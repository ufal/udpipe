#!/usr/bin/perl
use warnings;
use strict;
use utf8;
use open qw(:std :utf8);

use File::Copy;

sub load_sentences {
  my ($input_file) = @_;

  my $res = open(my $file, "<", $input_file);
  if (not $res) {
    warn "Ignoring non-existing file $input_file\n";
    return ();
  }

  my ($sentence, @sentences) = ("");
  while (<$file>) {
    chomp;
    if (/^$/) {
      push @sentences, $sentence;
      $sentence = "";
    } else {
      $sentence .= $_ . "\n";
    }
  }
  die "Unfinished sentence" if $sentence;
  close $file;

  return @sentences;
}

sub write_sentences {
  my ($target_file, @sentences) = @_;

  open (my $f, ">", $target_file) or die "Cannot open file $target_file: $!";
  foreach my $sentence (@sentences) {
    print $f "$sentence\n";
  }
  close $f;
}


@ARGV >= 2 or die "Usage: $0 train_file dev_file\n";
my $train_file = shift @ARGV;
my $dev_file = shift @ARGV;

my @train_sentences = load_sentences($train_file);
my @dev_sentences = load_sentences($dev_file);

if (@train_sentences < 9*@dev_sentences) {
  my @sentences = (@train_sentences, @dev_sentences);
  my $train_end = @sentences - int(@sentences / (1 + 9));
  warn sprintf("Resplitting files $train_file and $dev_file from sentence ratio %f to %f\n",
    @train_sentences ? @dev_sentences / @train_sentences : "inf", (@sentences - $train_end) / $train_end);
  write_sentences($train_file, @sentences[0..$train_end-1]);
  write_sentences($dev_file, @sentences[$train_end..$#sentences]);
}
