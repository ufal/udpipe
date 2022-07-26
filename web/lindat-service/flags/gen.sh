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
  inkscape $svg -o ${svg%.svg}.png -h 32
done
rm *.svg

cp old_east_slavic.png old_russian.png

echo All done
