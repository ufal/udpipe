#!/bin/sh

awk '
  /exact-match/ { lemma_em=substr($0, match($0, /Lemmas:[0-9.]*/)); sub(/ .*$/, "", lemma_em); sub(/Lemmas:/, "LemmasEM:", lemma_em) }
  /lemmaid-match/ { sub(/ lemmaid-match/, ""); sub(/UAS:/, lemma_em " UAS:"); print }
' "$@"
