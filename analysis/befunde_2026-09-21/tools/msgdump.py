#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Dump all extracted RE2-Leon room .msg files as text.

Char table taken verbatim from the repo's own decoder
src/main/java/de/re15/extractors/MSGParser.java (RE1.5/RE2 share it).
"""
import os, sys, io

ROOT = r"C:\workspace\git\reAi_v2\.claude\worktrees\wf_39ff9f8a-8c1-2\info\re2leon\PL0\RDT"

T = {}
T[0x00] = " "; T[0x01] = "."; T[0x02] = ">"; T[0x03] = "<"
T[0x04] = ">"; T[0x05] = "("; T[0x06] = ")"; T[0x07] = "#"
T[0x08] = "#"; T[0x09] = '"'; T[0x0A] = '"'; T[0x0B] = "v"
for i in range(10):
    T[0x0C + i] = str(i)
T[0x16] = ":"; T[0x17] = "\u3001"; T[0x18] = ","; T[0x19] = '"'
T[0x1A] = "!"; T[0x1B] = "?"; T[0x1C] = "!?"
for i in range(26):
    T[0x1D + i] = chr(ord('A') + i)
T[0x37] = "["; T[0x38] = "/"; T[0x39] = "]"; T[0x3A] = "'"
T[0x3B] = "-"; T[0x3C] = "."
for i in range(26):
    T[0x3D + i] = chr(ord('a') + i)


def decode(b):
    out = []
    for by in b:
        if by == 0xFE:
            out.append("|END|")
        elif by == 0xFC:
            out.append("\n")
        elif by == 0xFD:
            out.append("|FD|")
        elif by in T:
            out.append(T[by])
        else:
            out.append("<%02X>" % by)
    return "".join(out)


def main():
    outp = sys.argv[1]
    n = 0
    with io.open(outp, "w", encoding="utf-8") as f:
        for room in sorted(os.listdir(ROOT)):
            md = os.path.join(ROOT, room, "msg")
            if not os.path.isdir(md):
                continue
            for name in sorted(os.listdir(md)):
                p = os.path.join(md, name)
                b = open(p, "rb").read()
                f.write("### %s/%s  (%d bytes)\n" % (room, name, len(b)))
                f.write(decode(b).replace("\n", " \\n "))
                f.write("\n")
                n += 1
    print("dumped %d msg files -> %s" % (n, outp))


main()
