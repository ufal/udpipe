#!/bin/sh

for d in *; do
  [ -d "$d" ] || continue

  echo -n "$d"
  ../scripts/nonprojective_stats.pl $d/*.conllu | sed 's/^/\t/' | tr -d "\n"
  echo
done
