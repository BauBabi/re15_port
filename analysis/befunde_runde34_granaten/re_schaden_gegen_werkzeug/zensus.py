#!/usr/bin/env python3
"""zensus.py - Gegenpruefung Runde 34 (re_schaden_resolver): unabhaengiger Aufrufer-/Schreiber-Zensus.

Sucht in PSX.EXE (t_addr aus Header @0x18, Text ab Datei 0x800), DEBUG.BIN (@0x800C0000) und
STAGE1..6/TITLE (@0x80100000, ohne Kopf):
  ziel <hex>   : jal ZIEL, j ZIEL, Datenwort ZIEL, lui/addiu-Paar (hi/lo) das ZIEL baut
  imm  <hex>   : alle Lade-/Speicher-Instruktionen (lb/lh/lw/lbu/lhu/sb/sh/sw) mit Immediate IMM
                 (signed 16), mit der vorausgehenden lui-Basis (falls in den 8 Instruktionen davor)
Aufruf:
  python zensus.py ziel 0x80012d60
  python zensus.py imm 0x52c4 --store
"""
import os, sys, struct

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, ".claude", "skills", "re15-psx-disasm", "scripts"))
import re15_disasm as R  # noqa: E402

BIN_DIR = os.path.join(REPO, "info", "Re1.5", "PSX", "BIN")


def images():
    exe = open(os.path.join(REPO, "info", "Re1.5", "PSX.EXE"), "rb").read()
    taddr = struct.unpack_from("<I", exe, 0x18)[0]
    yield "PSX.EXE", exe[0x800:], taddr
    yield "DEBUG.BIN", open(os.path.join(BIN_DIR, "DEBUG.BIN"), "rb").read(), 0x800C0000
    for i in range(1, 7):
        yield f"STAGE{i}.BIN", open(os.path.join(BIN_DIR, f"STAGE{i}.BIN"), "rb").read(), 0x80100000
    yield "TITLE.BIN", open(os.path.join(BIN_DIR, "TITLE.BIN"), "rb").read(), 0x80100000


def words(buf):
    for off in range(0, len(buf) - 3, 4):
        yield off, struct.unpack_from("<I", buf, off)[0]


def ziel(t):
    jal = (3 << 26) | ((t >> 2) & 0x3ffffff)
    j = (2 << 26) | ((t >> 2) & 0x3ffffff)
    hi = (t >> 16) & 0xffff
    lo = t & 0xffff
    if lo & 0x8000:
        hi = (hi + 1) & 0xffff
    for name, buf, base in images():
        ws = list(words(buf))
        for idx, (off, w) in enumerate(ws):
            a = base + off
            if w == jal:
                print(f"  {name:10s} jal  @0x{a:08x}")
            elif w == j:
                print(f"  {name:10s} j    @0x{a:08x}")
            elif w == t:
                print(f"  {name:10s} wort @0x{a:08x}")
            op = w >> 26
            if op == 0xF and (w & 0xffff) == hi:
                rt = (w >> 16) & 31
                # addiu/ori rt, rt, lo innerhalb der naechsten 8
                for k in range(1, 9):
                    if idx + k >= len(ws):
                        break
                    w2 = ws[idx + k][1]
                    op2 = w2 >> 26
                    if op2 in (9, 0xD) and ((w2 >> 21) & 31) == rt and (w2 & 0xffff) == lo:
                        print(f"  {name:10s} lui/addiu-Paar @0x{a:08x} +{k}")


def imm(v, only_store):
    LD = {0x20: "lb", 0x21: "lh", 0x23: "lw", 0x24: "lbu", 0x25: "lhu", 0x28: "sb", 0x29: "sh", 0x2b: "sw"}
    for name, buf, base in images():
        ws = list(words(buf))
        for idx, (off, w) in enumerate(ws):
            op = w >> 26
            if op in LD and (w & 0xffff) == (v & 0xffff):
                if only_store and op < 0x28:
                    continue
                a = base + off
                rs = (w >> 21) & 31
                basis = "?"
                for k in range(1, 12):
                    if idx - k < 0:
                        break
                    w0 = ws[idx - k][1]
                    if w0 >> 26 == 0xF and ((w0 >> 16) & 31) == rs:
                        basis = f"lui {R.REGS[rs]},0x{w0 & 0xffff:x} @-{k}"
                        break
                s, _ = R.dis_one(w, a)
                print(f"  {name:10s} @0x{a:08x}: {s:30s} [{basis}]")


def main():
    if len(sys.argv) < 3:
        print(__doc__)
        sys.exit(1)
    mode = sys.argv[1]
    val = int(sys.argv[2], 16)
    if mode == "ziel":
        print(f"Ziel 0x{val:08x}")
        ziel(val)
    elif mode == "imm":
        imm(val, "--store" in sys.argv)
    else:
        print(__doc__)


if __name__ == "__main__":
    main()
