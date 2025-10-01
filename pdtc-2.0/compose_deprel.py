#!/usr/bin/env python3
import argparse
import sys

IS_MEMBER, IS_PARENTHESIS_ROOT = 5, 3

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("paths", default=[], nargs="*", help="Input paths")
    parser.add_argument("--is_member", default=False, action="store_true", help="Add is_member")
    parser.add_argument("--is_parenthesis_root", default=False, action="store_true", help="Add is_parenthesis_root")
    args = parser.parse_args()

    for path in args.paths:
        with open(path, "r", encoding="utf-8") as conllu_file:
            block = []
            for line in conllu_file:
                line = line.rstrip("\n")
                if not line:
                    assert block
                    # Process block
                    heads, deps = {}, {}
                    for entry in block:
                        columns = entry.split("\t")
                        if len(columns) == 10:
                            assert int(columns[0]) == len(heads) + 1
                            deps[len(heads) + 1] = columns[7]
                            heads[len(heads) + 1] = columns[6]

                    for i in range(len(block)):
                        columns = block[i].split("\t")
                        if len(columns) == 10:
                            if args.is_member and columns[IS_MEMBER] == "1":
                                parent = int(columns[0])
                                while parent and deps[parent] not in ("Apos", "Coord"):
                                    parent = int(heads[parent])

                                if deps[parent] == "Apos":
                                    columns[7] += "_Ap"
                                elif deps[parent] == "Coord":
                                    columns[7] += "_Co"
                                else:
                                    print("Did not find correct parent of IsMember {} in the below sentence".format(block[i]), *block, file=sys.stderr, sep="\n")
                                columns[IS_MEMBER] = "_"

                            if args.is_parenthesis_root and columns[IS_PARENTHESIS_ROOT] == "1":
                                columns[7] += "_Pa"
                                columns[IS_PARENTHESIS_ROOT] = "_"

                            block[i] = "\t".join(columns)

#                         if args.is_member and columns[IS_MEMBER] == "1":
#                             columns[7] += "_IsMember"
#                         columns[IS_MEMBER] = "_"
# 
#                         if args.is_parenthesis_root and columns[IS_PARENTHESIS_ROOT] == "1":
#                             columns[7] += "_Pa"
#                         columns[IS_PARENTHESIS_ROOT] = "_"
# 
#                         line = "\t".join(columns)
# 
#                     print(line)

                    print(*block, sep="\n", end="\n\n")
                    block = []
                else:
                    block.append(line)
            assert not block
