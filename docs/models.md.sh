#!/bin/sh

models="models_ud210.md models_ud26.md models_evalatin20.md"

cat <<EOF
# UDPipe 2 Models

EOF

for model in $models; do
  sed -n '/^## .* #[^ ]*$/ {s/^## /1. [/; s/ #/](#/; s/$/)/; p}' $model
done
echo

for model in $models; do
  cat $model
  echo
done
