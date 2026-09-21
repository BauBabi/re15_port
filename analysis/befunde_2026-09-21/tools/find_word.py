#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Search the whole RE2-Leon tree for a word encoded in the RE2 MSG char table.

Char table = repo's own src/main/java/de/re15/extractors/MSGParser.java.
Latin part:  0x00 space, 0x0C-0x15 '0'-'9', 0x1D-0x36 'A'-'Z', 0x3D-0x56 'a'-'z',
             0x01 '.', 0x16 ':', 0x18 ',', 0x1A '!', 0x1B '?', 0x3A "'", 0x3B '-',
             0x38 '/'
Also searches plain ASCII, so EXE strings are covered too.
"""
import os, sys

ROOT = os.environ.get(
    "FXROOT",
    r"C:\workspace\git\reAi_v2\.claude\worktrees\wf_39ff9f8a-8c1-2\info\re2leon")

def enc(s):
    out = bytearray()
    for ch in s:
        if ch == ' ':
            out.append(0x00)
        elif '0' <= ch <= '9':
            out.append(0x0C + ord(ch) - ord('0'))
        elif 'A' <= ch <= 'Z':
            out.append(0x1D + ord(ch) - ord('A'))
        elif 'a' <= ch <= 'z':
            out.append(0x3D + ord(ch) - ord('a'))
        elif ch == '.':
            out.append(0x01)
        elif ch == '-':
            out.append(0x3B)
        else:
            raise ValueError(ch)
    return bytes(out)

DEC = {}
DEC[0x00] = ' '; DEC[0x01] = '.'; DEC[0x16] = ':'; DEC[0x18] = ','
DEC[0x1A] = '!'; DEC[0x1B] = '?'; DEC[0x3A] = "'"; DEC[0x3B] = '-'
DEC[0x38] = '/'; DEC[0x37] = '['; DEC[0x39] = ']'
for i in range(10):
    DEC[0x0C + i] = str(i)
for i in range(26):
    DEC[0x1D + i] = chr(ord('A') + i)
for i in range(26):
    DEC[0x3D + i] = chr(ord('a') + i)

def dec(b):
    r = []
    for x in b:
        if x == 0xFC or x == 0xFD:
            r.append(' / ')
        elif x == 0xFE:
            r.append(' |END|')
        else:
            r.append(DEC.get(x, '<%02X>' % x))
    return ''.join(r)


def main():
    words = sys.argv[1:]
    pats = []
    for w in words:
        pats.append((w, enc(w.lower()), 'msgenc-lower'))
        pats.append((w, enc(w.capitalize()), 'msgenc-cap'))
        pats.append((w, w.lower().encode('ascii'), 'ascii-lower'))
        pats.append((w, w.capitalize().encode('ascii'), 'ascii-cap'))
        pats.append((w, w.upper().encode('ascii'), 'ascii-upper'))
    nfiles = 0
    hits = 0
    for dirpath, dirnames, filenames in os.walk(ROOT):
        for fn in filenames:
            p = os.path.join(dirpath, fn)
            try:
                b = open(p, 'rb').read()
            except Exception:
                continue
            nfiles += 1
            for w, pat, kind in pats:
                start = 0
                while True:
                    i = b.find(pat, start)
                    if i < 0:
                        break
                    hits += 1
                    ctx = b[max(0, i - 70): i + len(pat) + 70]
                    if kind.startswith('msgenc'):
                        txt = dec(ctx)
                    else:
                        txt = ''.join(chr(c) if 32 <= c < 127 else '.' for c in ctx)
                    rel = os.path.relpath(p, ROOT)
                    print('HIT %-12s %-10s %-46s @0x%06X  %s' % (w, kind, rel, i, txt))
                    start = i + 1
    print('--- scanned %d files, %d hits' % (nfiles, hits))


main()
