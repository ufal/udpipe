#!/usr/bin/env python3
import argparse
import glob
import os
import sys

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("root", default="", nargs="?", type=str, help="Root directory to check")
    args = parser.parse_args()

    nonascii = False
    for path in glob.iglob(os.path.join(args.root, "**"), recursive=True):
        if path.endswith((".c", ".cpp", ".h", ".i")) or "Makefile" in path:
            try:
                with open(path, "r", encoding="ascii") as path_file:
                    path_file.read()
            except:
                print("Non-ascii characters in {}".format(path), flush=True)
                nonascii = True

    sys.exit(1 if nonascii else 0)
