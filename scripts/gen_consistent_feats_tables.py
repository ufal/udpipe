#!/usr/bin/env python3
import argparse
import gzip
import json
import os
import sys
import pickle

import numpy as np

sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
import udpipe2_dataset

def load_consistency():
    path = os.path.join(os.path.dirname(__file__), "docs-automation")
    if not os.path.exists(path):
        os.system("git -C '{}' clone --depth=1 https://github.com/UniversalDependencies/docs-automation".format(os.path.dirname(__file__)))

    with open(os.path.join(path, "valrules", "feats.json"), "r", encoding="utf-8") as feats_file:
        return json.load(feats_file)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("lang", type=str, help="Language to generate")
    parser.add_argument("model", type=str, help="Model path")
    args = parser.parse_args()

    consistency = load_consistency()["features"][args.lang]
    mappings = udpipe2_dataset.UDPipe2Dataset.load_mappings(os.path.join(args.model, "mappings.pickle"))
    upos = mappings.factors[mappings.UPOS].words
    feats = mappings.factors[mappings.FEATS].words

    allowed = np.ones([len(upos), len(feats)], dtype=bool)
    for i, feat in enumerate(feats):
        if feat in ["<pad>", "<unk>", "<root>", "_"]:
            continue
        for feature_value in feat.split("|"):
            feature, values = feature_value.split("=")
            if feature not in consistency:
                print("Did not find feature '{}' in consistency table!".format(feature), file=sys.stderr)
            else:
                for j in range(len(upos)):
                    if upos[j] not in consistency[feature]["byupos"] or \
                            any(value not in consistency[feature]["byupos"][upos[j]] for value in values.split(",")) or \
                            any(consistency[feature]["byupos"][upos[j]][value] == 0 for value in values.split(",")):
                        allowed[j][i] = False

    with gzip.open(os.path.join(args.model, "consistent_feats.table"), "wb") as consistent_feats_file:
        np.save(consistent_feats_file, allowed, allow_pickle=False)
