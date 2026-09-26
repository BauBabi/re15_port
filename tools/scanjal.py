#!/usr/bin/env python3
"""Find every jal/j to a target address in a raw MIPS image.

usage: scanjal.py <file> <load_base_hex> <target_hex>
"""
import struct, sys, os

def main():
    path = sys.argv[1]
    base = int(sys.argv[2], 16)
    data = open(path, 'rb').read()
    n = len(data) // 4
    words = struct.unpack("<%dI" % n, data[:n * 4])
    for targ in sys.argv[3:]:
        tgt = int(targ, 16)
        field = (tgt >> 2) & 0x3ffffff
        jal = 0x0c000000 | field
        j = 0x08000000 | field
        hits = []
        for i, w in enumerate(words):
            if w == jal:
                hits.append((base + i * 4, "jal"))
            elif w == j:
                hits.append((base + i * 4, "j"))
        # also word-sized pointer references (dispatch tables)
        ptrs = [base + i * 4 for i, w in enumerate(words) if w == tgt]
        print("== %s base=%08x target=%08x : %d jal/j, %d ptr" % (
            os.path.basename(path), base, tgt, len(hits), len(ptrs)))
        for a, k in hits:
            print("   %s @%08x" % (k, a))
        for a in ptrs:
            print("   ptr @%08x" % a)

main()
