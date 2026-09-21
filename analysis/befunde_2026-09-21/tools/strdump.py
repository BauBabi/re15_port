#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Decode a byte range of any RE1.5/RE2 binary with the RE-MSG char table.

usage: strdump.py <file> <start hex> <len hex> [<ram base hex> <data off hex>]
"""
import sys

DEC = {0x00: ' ', 0x01: '.', 0x16: ':', 0x18: ',', 0x1A: '!', 0x1B: '?',
       0x3A: "'", 0x3B: '-', 0x38: '/', 0x37: '[', 0x39: ']', 0x02: '>',
       0x05: '(', 0x06: ')', 0x09: '"', 0x0A: '"'}
for i in range(10):
    DEC[0x0C + i] = str(i)
for i in range(26):
    DEC[0x1D + i] = chr(ord('A') + i)
for i in range(26):
    DEC[0x3D + i] = chr(ord('a') + i)


def dec1(x):
    if x == 0xFC:
        return '{LF}'
    if x == 0xFD:
        return '{PAGE}'
    if x == 0xFE:
        return '{END}'
    return DEC.get(x, '<%02X>' % x)


def main():
    path = sys.argv[1]
    start = int(sys.argv[2], 16)
    length = int(sys.argv[3], 16)
    ram = int(sys.argv[4], 16) if len(sys.argv) > 4 else 0
    doff = int(sys.argv[5], 16) if len(sys.argv) > 5 else 0
    seps = set([0xF7, 0x07, 0xFE])
    b = open(path, 'rb').read()
    seg = b[start:start + length]
    cur = []
    off = start
    n = 0
    for i, x in enumerate(seg):
        if x in seps:
            print('%3d  file@0x%06X  ram@0x%08X  sep=%02X  %r'
                  % (n, off, ram + (off - doff) if ram else 0, x, ''.join(cur)))
            n += 1
            cur = []
            off = start + i + 1
        else:
            cur.append(dec1(x))
    if cur:
        print('%3d  file@0x%06X  ram@0x%08X  sep=--  %r'
              % (n, off, ram + (off - doff) if ram else 0, ''.join(cur)))


main()
