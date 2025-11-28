#!/usr/bin/env python3
import argparse
import os
import sys

headers = ["Model", "Mode", "Words", "Sents", "UPOS", "XPOS", "UFeats", "AllTags", "Lemma", "UAS", "LAS", "MLAS", "BLEX"]

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("datadir", type=str, help="UD data directory to use")
    args = parser.parse_args()

    # Load UD data and find out used features
    print("Loading UD data and finding out used features...", file=sys.stderr, end=" ", flush=True)
    names_iso = {}
    with open(f"{args.datadir}/iso_names.txt") as iso_names_file:
        for line in iso_names_file:
            iso, name = line.rstrip("\r\n").split()
            name = name.removeprefix("UD_").lower()
            names_iso[name] = iso

    features = {}
    for iso in names_iso.values():
        features[iso] = {"XPOS": False, "UFeats": False, "Lemmas": False}
        with open(f"{args.datadir}/{iso}/{iso}-ud-train.conllu") as train_file:
            for line in train_file:
                line = line.rstrip("\r\n")
                if not line or line.startswith("#") or not line.split("\t", maxsplit=1)[0].isdecimal():
                    continue
                columns = line.split("\t")
                for column, name in [(4, "XPOS"), (5, "UFeats"), (2, "Lemmas")]:
                    if columns[column] not in ["", "_"]:
                        features[iso][name] = True
    print("done", file=sys.stderr)

    results = []
    for line in sys.stdin:
        line = line.rstrip("\r\n")
        model, mode, *metrics = line.split()
        iso = names_iso[model.split("-ud-", maxsplit=1)[0]]
        metrics = {name: value for name, value in [metric.split(":") for metric in metrics]}

        results.append([])
        for header in headers:
            header = "Sentences" if header == "Sents" else "Lemmas" if header == "Lemma" else header
            if header == "Model":
                results[-1].append(model)
            elif header == "Mode":
                results[-1].append({"raw_text": "Raw text", "gold_tok": "Gold tokenization"}[mode])
            elif header in ["Words", "Sentences"] and mode == "gold_tok":
                results[-1].append("—")
            elif header in features[iso] and not features[iso][header]:
                results[-1].append("—")
            else:
                results[-1].append(metrics[header])

    max_lengths = [max(map(len, values)) for values in zip(headers, *results)]
    for i, values in enumerate([headers] + results):
        print("|", " | ".join([value.ljust(maxlen) for value, maxlen in zip(values, max_lengths)]), "|")
        if not i:
            print("|", "|".join((str.ljust if j < 2 else str.rjust)(":", maxlen + 2, "-") for j, maxlen in enumerate(max_lengths)), "|", sep="")
