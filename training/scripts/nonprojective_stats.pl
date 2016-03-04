#!/usr/bin/perl
use warnings;
use strict;
use utf8;
use open qw(:std :utf8);

my ($sentences, $words) = (0, 0);
my ($nonproj_sentences, $nonproj_edges) = (0, 0);
my @edge_degree_sentences;
my @gap_degree_sentences;

my @pred = (-1);
my @desc = ([]);

sub nonprojective_stats {
  my ($node) = @_;

  my ($edges, $edge_degree, $gap_degree) = (0, 0, 0);
  if ($node && @{$desc[$node]} > 0) {
    my @v = (0) x @pred;
    sub visit {
      my ($node, $v_ref) = @_;
      $v_ref->[$node] = 1;
      foreach my $d (@{$desc[$node]}) {
        visit($d, $v_ref) if not $v_ref->[$d];
      }
    }
    visit($node, \@v);

    foreach my $d (@{$desc[$node]}) {
      my $child_edge_degree = 0;

      my $l = $d < $node ? $d : $node;
      my $r = $d > $node ? $d : $node;
      for (my $i = $l + 1; $i < $r; $i++) {
        $child_edge_degree++ if !$v[$i] && ($pred[$i] < $l || $pred[$i] > $r);
      }
      $edges++ if $child_edge_degree;
      $edge_degree = $child_edge_degree if $child_edge_degree > $edge_degree;
    }

    $gap_degree = -1;
    for (my $i = 1; $i < @v; $i++) {
      $gap_degree++ if $v[$i] && !$v[$i-1];
    }
  }

  foreach my $d (@{$desc[$node]}) {
    my ($child_edges, $child_edge_degree, $child_gap_degree) = nonprojective_stats($d);
    $edges += $child_edges;
    $edge_degree = $child_edge_degree if $child_edge_degree > $edge_degree;
    $gap_degree = $child_gap_degree if $child_gap_degree > $gap_degree;
  }

  return ($edges, $edge_degree, $gap_degree);
}

while(<>) {
  chomp;

  # Ignore comments
  next if /^#/;

  if (/^$/) {
    # Process tree
    for (my $i = 1; $i < @pred; $i++) { push @desc, []; }
    for (my $i = 1; $i < @pred; $i++) { push @{$desc[$pred[$i]]}, $i; }

    my ($nonproj, $edge_degree, $gap_degree) = nonprojective_stats(0);

    $sentences++;
    $words += @pred - 1;
    $nonproj_sentences += $nonproj ? 1 : 0;
    $nonproj_edges += $nonproj;
    $edge_degree_sentences[$edge_degree]++;
    $gap_degree_sentences[$gap_degree]++;

    @pred = (-1);
    @desc = ([]);
  } else {
    # Append to tree
    next if /^\d+-\d+/;
    my @tokens = split/\t/;
    die "Line '$_' does not contain 10 columns." if @tokens != 10;
    die "Cannot append line '$_' to a tree with " . scalar(@pred) . " nodes" if $tokens[0] ne @pred;
    push @pred, $tokens[6];
  }
}

printf "Data size: %d sentences with %d words.\n", $sentences, $words;
printf "Nonprojective: %d (%.2f%%) sentences, %d (%.2f%%) edges.\n", $nonproj_sentences, 100*$nonproj_sentences/$sentences, $nonproj_edges, 100*$nonproj_edges/$words;
print "Edge degrees: " . join(", ", map {sprintf "%.2f%%", 100*($_ // 0)/$sentences} @edge_degree_sentences) . "\n";
print "Gap degrees: " . join(", ", map {sprintf "%.2f%%", 100*($_ // 0)/$sentences} @gap_degree_sentences) . "\n";
