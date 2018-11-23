#!/bin/sh

set -e

git clone --depth=1 --branch=pages-source https://github.com/UniversalDependencies/docs

cat docs/gen_index/flags.json | perl -nle '/^"([^"]*)":"([^"]*)",?$/ and print lc($1) . " $2"' | while read name code; do
  cp docs/flags/svg/$code.svg $name.svg
done

rm -rf docs

cp ancient_greek.svg ancient-greek.svg
cp ancient_greek-proiel.svg ancient-greek-proiel.svg
cp old_church_slavonic.svg old-church-slavonic.svg

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

echo All done
