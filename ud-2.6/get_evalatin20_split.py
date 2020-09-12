#!/usr/bin/env python3
import argparse
import os
import random
import sys


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("paths", nargs="+", type=str, help="Input CoNLL-U files")
    parser.add_argument("--ratio", default=0.95, type=float, help="Train/dev ratio")
    parser.add_argument("--train", default=None, type=str, help="Train file to append to")
    parser.add_argument("--dev", default=None, type=str, help="Dev file to append to")
    parser.add_argument("--seed", default=42, type=int, help="Random seed")
    args = parser.parse_args()

    train, dev = [], []

    for path in args.paths:
        print("Loading {}".format(path), file=sys.stderr)

        with open(path, "r", encoding="utf-8") as input_file:
            blocks, block = [], []
            header = "# newdoc id = {}\n".format(os.path.basename(path))
            for line in input_file:
                if line.startswith("# variant "): continue
                block.append(line)
                if not line.rstrip("\n"):
                    block = "".join(block)
                    if "greek.expression" not in block:
                        blocks.append(block)
                    block = []
        assert not block

        random.Random(args.seed).shuffle(blocks)
        train.extend([header] + blocks[:int(args.ratio * len(blocks))])
        if args.ratio < 1:
            dev.extend([header] + blocks[int(args.ratio * len(blocks)):])

    for path, blocks in [(args.train, train), (args.dev, dev)]:
        if not blocks: continue

        with open(path, "a", encoding="utf-8") as output_file:
            print("".join(blocks), end="", file=output_file)
