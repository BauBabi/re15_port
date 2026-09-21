#!/usr/bin/env python3
"""re15_items.py - RE1.5-Itemnamen aus DEBUG.BIN + Platzierungen aus den RDTs.

NAMEN: DEBUG.BIN traegt den Namensblock als Folge `0x07 <Text>` ab Datei-0x4A2B.
Der Zeichensatz ist derselbe wie in RE2 (Buchstaben ASCII-0x24, Ziffern ab 0x0C,
0x00 = Leerzeichen).

ORDNUNG -> ITEM-ID: id = (Platz in der 0x07-Liste) + 1. Das ist an ZWEI
unabhaengigen, im Port bereits gemessenen Punkten geprueft
(re15_port/engine/src/scd_vm.c:3829-3831, Kommentar zum Item_aot_set-Handler):
    * "ROOM1050.RDT @0xb9a ... = handgun ammo (0x15)"  -> Liste Platz 20 = "H. Gun Bullets"
    * "ROOM1000 @0xc24 = 0x31 x1"                      -> Liste Platz 48 = "Fire Extinguisher"
      (und test_item_name_census.c:42 definiert ITEM_FIRE_EXTINGUISHER 0x31)

PLATZIERUNGEN: Item_aot_set ist in RE1.5 Opcode 0x50, 22 Byte, Feldlage nach
re15_port/engine/src/scd_vm.c:3800-3817 (Kurzform; pc[3]&0x80 = Langform, +8):
    +0 op  +1 slot  +2 sce  +3 sat  +4 floor  +5 super
    +6 x  +8 z  +10 w  +12 d   (alle s16 LE)
    +14 item_type(u16 LE, Typ = Low-Byte)  +16 amount  +18 taken-bit  +20 prop-slot

Aufruf:
    re15_items.py names
    re15_items.py find <id> [<id> ...]      Platzierungen dieser Item-Ids
"""
import sys, os, struct, glob

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
DEBUG_BIN = os.path.join(REPO, "info", "Re1.5", "PSX", "BIN", "DEBUG.BIN")
STAGES = os.path.join(REPO, "info", "Re1.5", "PSX", "STAGE*", "ROOM*.RDT")
NAME_BLOCK = 0x4A28          # erstes 0x07, direkt davor endet eine u16-Tabelle;
                             # danach folgt 1f 4b 49 3e 3d 50 = "Combat"
SEP = 0x07


def dec(b):
    if b == 0x00: return " "
    if b == 0x01: return "."
    if 0x0C <= b <= 0x15: return chr(ord("0") + b - 0x0C)
    if 0x1D <= b <= 0x36: return chr(ord("A") + b - 0x1D)
    if b == 0x3A: return "'"
    if b == 0x3B: return "-"
    if 0x3D <= b <= 0x56: return chr(ord("a") + b - 0x3D)
    return "{%02X}" % b


def names(limit=120):
    """[(id, Datei-Offset, Text)] — Eintraege sind `0x07 <Text>` hintereinander."""
    d = open(DEBUG_BIN, "rb").read()
    out = []
    p = NAME_BLOCK
    while p < len(d) and d[p] == SEP and len(out) < limit:
        p += 1
        start = p
        s = []
        while p < len(d) and d[p] != SEP and len(s) < 40:
            s.append(dec(d[p])); p += 1
        txt = "".join(s).rstrip()
        out.append((len(out) + 1, start, txt))     # id = Platz + 1
    return out


LONG = 0x80


def placements(want):
    hits = []
    for f in sorted(glob.glob(STAGES)):
        d = open(f, "rb").read()
        for p in range(len(d) - 32):
            if d[p] != 0x50:
                continue
            sat = d[p + 3]
            lf = bool(sat & LONG)
            n = 30 if lf else 22
            if p + n > len(d):
                continue
            it = d[p + (22 if lf else 14)]
            hi = d[p + (23 if lf else 15)]
            amt = d[p + (24 if lf else 16)]
            tk = d[p + (26 if lf else 18)]
            prop = d[p + (28 if lf else 20)]
            if it not in want or hi != 0:
                continue
            x, z, w, dd = struct.unpack_from("<hhhh", d, p + 6)
            # Plausibilitaet: sce in 0..15, amount 1..250, w/d positiv und < 20000
            ok = (d[p + 2] <= 15 and 1 <= amt <= 250 and 0 < w < 20000 and 0 < dd < 20000)
            hits.append((os.path.basename(f), p, it, amt, d[p + 2], sat, x, z, w, dd,
                         tk, prop, ok, d[p:p + n]))
    return hits


if __name__ == "__main__":
    if sys.argv[1] == "names":
        for i, off, t in names():
            print("id 0x%02X (%3d)  DEBUG.BIN+0x%04X  %s" % (i, i, off, t))
    elif sys.argv[1] == "find":
        want = set(int(a, 0) for a in sys.argv[2:])
        nm = {i: t for i, _, t in names()}
        for (f, off, it, amt, sce, sat, x, z, w, dd, tk, prop, ok, raw) in placements(want):
            print("%-14s +0x%04X  id=0x%02X %-20s n=%-3d sce=%-2d sat=0x%02X "
                  "x=%-7d z=%-7d w=%-5d d=%-5d tk=%-3d prop=%-3d %s  %s"
                  % (f, off, it, nm.get(it, "?"), amt, sce, sat, x, z, w, dd, tk, prop,
                     "plausibel" if ok else "UNPLAUSIBEL",
                     " ".join("%02x" % b for b in raw)))
