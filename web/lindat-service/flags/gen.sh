#!/bin/sh

set -e

git clone --depth=1 --branch=pages-source https://github.com/UniversalDependencies/docs
git clone --depth=1 https://github.com/UniversalDependencies/docs-automation

cat docs-automation/codes_and_flags.yaml | grep -e "^[^ ]" -e "flag:" | sed '
  s/:$//; s/ /_/g; s/.*/\L&/; N; s/\n\s*flag://; s/["'"'"']//g;
' | while read name code; do
  cp docs/flags/svg/$code.svg $name.svg
done

rm -rf docs docs-automation

for svg in *.svg; do
  echo $svg
  inkscape $svg -e ${svg%.svg}.png -h 32
done
rm *.svg

sed 's/#.*$//; /^$/d' ../../../releases/models.txt | while read code name rest; do
  [ -f "$name".png ] && continue

  generic_name="${name%-*}"
  echo Flag for $name not found, trying to use $generic_name >&2
  [ -f "$generic_name".png ] || { echo Missing flag even for $generic_name, aborting >&2; exit 1; }
  cp "$generic_name".png "$name".png
done

cp ancient_greek.png ancient-greek.png
cp ancient_greek-proiel.png ancient-greek-proiel.png
cp old_church_slavonic.png old-church-slavonic.png
cp latin-ittb.png latin-itt.png

echo All done
