#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Close the language gap: search the RE2 tree for the JAPANESE word for fire
extinguisher, since some RE2-US room messages are still kana.

Codes from the repo's own MSGParser.java table:
  hiragana block starts 0x57 (a i u e o ka ki ...), katakana block starts 0xA7.
  shi=0x62, small yo=0x8C, u=0x59, ka=0x5C, ki=0x5D   -> shoukaki (hiragana)
  SHI=0xB2, small YO=0xDC, U=0xA9, KA=0xAC, KI=0xAD   -> SHOUKAKI (katakana)
"""
import os, sys

ROOT = sys.argv[1]
PATS = [
    ('hiragana shoukaki', bytes([0x62, 0x8C, 0x59, 0x5C, 0x5D])),
    ('katakana SHOUKAKI', bytes([0xB2, 0xDC, 0xA9, 0xAC, 0xAD])),
    ('hiragana shouka',   bytes([0x62, 0x8C, 0x59, 0x5C])),
    ('katakana SHOUKA',   bytes([0xB2, 0xDC, 0xA9, 0xAC])),
]
n = 0
hits = 0
for dp, dn, fn in os.walk(ROOT):
    for f in fn:
        p = os.path.join(dp, f)
        try:
            b = open(p, 'rb').read()
        except Exception:
            continue
        n += 1
        for name, pat in PATS:
            i = b.find(pat)
            while i >= 0:
                hits += 1
                print('HIT %-20s %-50s @0x%06X' % (name, os.path.relpath(p, ROOT), i))
                i = b.find(pat, i + 1)
print('--- %d files scanned, %d hits' % (n, hits))
