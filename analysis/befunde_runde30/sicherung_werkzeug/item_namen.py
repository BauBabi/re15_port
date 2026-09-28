#!/usr/bin/env python3
"""Liest die AUSGELIEFERTEN Item-Namen aus BIN/DEBUG.BIN und gibt je Eintrag Adresse +
Rohbytes + Klartext aus. Beleg fuer "Item 0x40 heisst Fuse".

Reader im Original: FUN_80028840 @0x80028840 (andi a0,0xff / sll a0,1 /
lhu v1,0x800c495c(a0) / addu v0,v1,0x800c4a28). DEBUG.BIN mappt nach 0x800c0000.
Glyphen: re15_port/engine/src/msg_common.c:175-199 (A-Z @0x1D, a-z @0x3D, 0x00 = Leerzeichen).

    python analysis/befunde_runde30/sicherung_werkzeug/item_namen.py 0x3e 0x43
"""
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from r30_lib import PSX  # noqa: E402

BASE = 0x800C0000
NAME_OFF = 0x800C495C
NAME_BLOB = 0x800C4A28


def glyph(b):
    if b == 0:
        return " "
    if 0x0C <= b <= 0x15:
        return chr(ord("0") + b - 0x0C)
    if 0x1D <= b <= 0x36:
        return chr(ord("A") + b - 0x1D)
    if 0x3D <= b <= 0x56:
        return chr(ord("a") + b - 0x3D)
    return {0x16: ":", 0x18: ",", 0x1A: "!", 0x1B: "?", 0x38: "/", 0x3A: "'",
            0x3B: "-", 0x3C: ".", 0x57: "."}.get(b, "<%02x>" % b)


def main():
    d = open(os.path.join(PSX, "BIN", "DEBUG.BIN"), "rb").read()
    a = int(sys.argv[1], 0) if len(sys.argv) > 1 else 0x3E
    e = int(sys.argv[2], 0) if len(sys.argv) > 2 else 0x43
    for i in range(a, e + 1):
        tab = NAME_OFF + i * 2
        off = struct.unpack_from("<H", d, tab - BASE)[0]
        p = NAME_BLOB + off
        roh = bytearray()
        q = p - BASE
        while d[q] != 0x07:
            roh.append(d[q])
            q += 1
        print("Item 0x%02X  Tabelle @0x%08X = 0x%04X  Name @0x%08X (Datei 0x%05X)  %s 07  \"%s\""
              % (i, tab, off, p, p - BASE, " ".join("%02x" % b for b in roh),
                 "".join(glyph(b) for b in roh)))


if __name__ == "__main__":
    main()
