#!/bin/bash

for d in ${@:-*/}; do
  l=`basename "$d"`

  for f in $l/*.dev; do
    [ -f "$f" ] || continue
    base=${f%.dev}
    [ -s "$base.model" ] || continue
    echo -n "$l "
    perl -ne '
      sub extract_from_log() {
        if ($ARGV =~ /^(.*).dev$/) {
          open (my $f, "<", "$1.log");
          my @s;
          while (<$f>) {
            /Epoch (\d+).*heldout tokens: (\S+), sentences: (\S+)/ and $s[$1] = "$2 $3 ";
            /Choosing parameters from epoch (\d+)/ and print $s[$1];
          }
          close $f;
        } else {
          print "0P/0R/0 0P/0R/0 ";
        }
      }
      #extract_from_log();

      #/No SpaceAfter/ && extract_from_log();
      #/multiwords/ && next;
      /^Tokenizer (?:words|sentences).*precision: ([0-9.]*%).*recall: ([0-9.]*%).*f1: ([0-9.]*%)/ && print "$1P/$2R/$3 ";
      #/ sentences/ && extract_from_log();
    ' $base.dev $base.test
    echo $base
#  done | sort -srV -t/ -k5
  done | perl -aF'/[\s\/]/' -ne 'push @l,$_; $l{$_}=2*$F[3]+$F[6];END{foreach (sort {$l{$b}<=>$l{$a}} @l) {print}}'
  echo
done | perl -pe 's/(\d+\.\d+)%/sprintf "%.2f", $1/ge' | column -te | less -S
