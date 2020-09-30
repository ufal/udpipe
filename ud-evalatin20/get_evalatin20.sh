#!/bin/sh

# Add EvaLatin tagger-only data
mkdir -p la_evalatin20
git -C la_evalatin20 clone --depth=1 https://github.com/CIRCSE/LT4HALA

echo "# variant = la_evalatin20" >la_evalatin20/la_evalatin20-train.conllu
echo "# variant = la_evalatin20" >la_evalatin20/la_evalatin20-dev.conllu

python3 get_evalatin20_split.py la_evalatin20/LT4HALA/data_and_doc/training_data_10-12-2019/*.conllu \
  --train=la_evalatin20/la_evalatin20-train.conllu --dev=la_evalatin20/la_evalatin20-dev.conllu

echo "# variant = la_ittb" >>la_evalatin20/la_evalatin20-train.conllu
python3 get_evalatin20_split.py ../ud-2.6/la_ittb/la_ittb-ud-train.conllu --ratio 0.6 --train=la_evalatin20/la_evalatin20-train.conllu --dev=/dev/null

echo "# variant = la_perseus" >>la_evalatin20/la_evalatin20-train.conllu
python3 get_evalatin20_split.py ../ud-2.6/la_perseus/la_perseus-ud-train.conllu --ratio 1 --train=la_evalatin20/la_evalatin20-train.conllu --dev=/dev/null

echo "# variant = la_proiel" >>la_evalatin20/la_evalatin20-train.conllu
python3 get_evalatin20_split.py ../ud-2.6/la_proiel/la_proiel-ud-train.conllu --ratio 1 --train=la_evalatin20/la_evalatin20-train.conllu --dev=/dev/null

sed '
  s#^14\tnescio\tnescio\tnescio\tVERB#14\tnescio\tnescio\tVERB#
  s#^9\tne\tPART#9\tne\tne\tPART#
  s#\(\t[^\t]*\)\{6\}$#\t_\t_\t_\t_\t_\t_#
' -i la_evalatin20/la_evalatin20-train.conllu

echo "# variant = la_evalatin20" >la_evalatin20/la_evalatin20-test.conllu
echo "# variant = la_evalatin20" >la_evalatin20/la_evalatin20-test-crossgenre.conllu
echo "# variant = la_evalatin20" >la_evalatin20/la_evalatin20-test-crosstime.conllu
for f in la_evalatin20/LT4HALA/data_and_doc/gold_EvaLatin/*.conllu; do
  case "$(basename $f)" in
    Horatius-Carmina_GOLD.conllu) name=test-crossgenre;;
    SummaContraGentiles_IV_GOLD.conllu) name=test-crosstime;;
    *) name=test;;
  esac
  (echo "# newdoc id = $(basename $f)"; cat $f) >>la_evalatin20/la_evalatin20-$name.conllu
done

for conllu in la_evalatin20/la_evalatin20-dev.conllu la_evalatin20/la_evalatin20-test*.conllu; do
  perl ../ud-2.6/conllu_to_text.pl --language=la <$conllu >${conllu%.conllu}.txt
done

rm -rf la_evalatin20/LT4HALA
