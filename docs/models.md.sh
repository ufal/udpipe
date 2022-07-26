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
  awk '
   /^\| [-a-z0-9_.]*-[0-9][0-9][0-9][0-9][0-9][0-9] *\|/ {
     if (!($2 in models)) {
       models[$2] = 1;
       $2 = "<span id=\"" $2 "\">" $2 "</span>";
     }
   }
   {print}
  ' $model
  echo
done
