#!/bin/bash

[ $# -ge 2 ] || { echo "Usage: $0 target_dir layer files" >&2; exit 1; }

target="$1"; shift
layer="$1"; shift

resources="$(dirname $0)/resources"

for file in "$@"; do
  treex -Lcs Read::PDT schema_dir=$resources from=$file.$layer top_layer=$layer Write::CoNLLU to=$target/$file.conllu use_tree_id_as_sent_id=1 upos=is_parenthesis_root xpos=tag feats=is_member deprel=afun
  gawk -i inplace '
    BEGIN {s = 1; p = 1; last_pid = -1}
    /^# sent_id = / {
      split($4, parts, /-/); p_s = parts[length(parts)]; if (match(p_s, /^p[0-9]+s[0-9]+$/) > 0) {
        pid = substr(p_s, 0, index(p_s, "s")); if (pid != last_pid) { print "# newpar id = '"${file//\//-}"'-p" p; p += 1; last_pid = pid}}
      $4 = "s" s; s += 1}
    {print}
  ' $target/$file.conllu
  sed '
    1i# newdoc id = '"${file//\//-}"'
    s@^# sent_id = @# sent_id = '"${file//\//-}"'-@
  ' -i $target/$file.conllu
  if [ "$layer" = m ]; then
    sed '
      s/^\([0-9][0-9]*\t\([^\t]*\t\)\{5\}\)0\(\(\t[^\t]*\)\{3\}\)$/\1_\3/
    ' -i $target/$file.conllu
  fi
done
