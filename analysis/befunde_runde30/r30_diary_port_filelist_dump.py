#!/usr/bin/env python3
"""r30_diary_port_filelist_dump.py - MESSUNG: womit fuellt RE1.5 (und damit der Port) den
FILE-Reiter? Liest die Tabellen DIREKT aus shared_assets/PSX/BIN/DEBUG.BIN (Modul liegt
@0x800c0000, Datei == RAM) und dekodiert sie mit dem Zeichendekoder FUN_80013160
(Digraph-Paare @0x800c44b8, Bias 0xa0) - dieselben Adressen, die
re15_port/engine/src/gen/inv_file_doc.inc und gen/inv_name_bank.inc im Kopf nennen.

Adressen (alle DEBUG.BIN, RAM = 0x800c0000 + Datei-Offset):
  0x800c6c98  u16[3]   Sichtbarkeitsmaske je Listenseite          (lhu @0x800c72f0)
  0x800c7370  u8[3]    erste Namens-Id je Listenseite             (Zeile = base + row)
  0x800c495c  u16[]    Namens-Offsets, Blob @0x800c4a28           (FUN_80028840)
  0x800c78e4  u32[3]   Zeiger auf die Seitentitel                 (@0x800c7284-9c)
  0x800ccd34  Dokument: u16[0] = 2*Seitenzahl, u16[i] = Offset der Seite i
"""
import struct, sys, os
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
BIN = os.path.join(REPO, "re15_port", "shared_assets", "PSX", "BIN", "DEBUG.BIN")
BASE = 0x800c0000
d = open(BIN, "rb").read()
def u8(a):  return d[a - BASE]
def u16(a): return struct.unpack_from("<H", d, a - BASE)[0]

def glyph(c):
    if c == 0x00: return " "
    if c == 0x3a: return "'"
    if c == 0x57: return "."
    if c == 0x3b: return "_"
    if c == 0x38: return "/"
    ch = c + 0x24
    if 0x30 <= ch <= 0x39 or 0x41 <= ch <= 0x5a or 0x61 <= ch <= 0x7a: return chr(ch)
    return "<%02x>" % c

def decode(addr, limit=4096):
    """FUN_80013160-Strom ab addr bis Code 7 oder 1. Liefert (text, rohbytes)."""
    out = []; p = addr; pend = None; n = 0
    while n < limit:
        n += 1
        if pend is not None:
            c = pend; pend = None; p += 1
        else:
            c = u8(p)
            if ((c + 0xa0) & 0xff) > 0x58:
                p += 1
            else:                       # Digraph: Paar @0x800c44b8 + (c-0xa0)*2
                i = 0x800c44b8 + (c - 0xa0) * 2
                pend = u8(i + 1); c = u8(i)
        if c in (7, 1): break
        if c == 8:  out.append("\n"); continue
        if c == 9:  op = u16(p); p += 2; out.append("<mitte %d>" % op); continue
        if c == 10: op = u16(p); p += 2; out.append("<rechts %d>" % op); continue
        if c == 5:  p += 1; continue
        if c == 0xfb: continue
        if c == 0xfc: out.append(" "); continue
        out.append(glyph(c))
    return "".join(out), d[addr - BASE:p - BASE]

print("== FILE-LISTE (RE1.5 DEBUG.BIN, FUN_800c6ca0) ==")
masks = [u16(0x800c6c98 + i * 2) for i in range(3)]
bases = [u8(0x800c7370 + i) for i in range(3)]
for pg in range(3):
    t_ptr = struct.unpack_from("<I", d, 0x800c78e4 + pg * 4 - BASE)[0]   # Zeigertabelle u32[3]
    title, _ = decode(t_ptr)
    print("Seite %d  Titel=%r  Maske=0x%04x @0x%08x  Basis-Id=0x%02x @0x%08x"
          % (pg, title, masks[pg], 0x800c6c98 + pg * 2, bases[pg], 0x800c7370 + pg))
    for row in range(10):
        nid = bases[pg] + row
        off = u16(0x800c495c + nid * 2)
        name, raw = decode(0x800c4a28 + off)
        vis = (masks[pg] >> row) & 1
        print("   Zeile %d  id=0x%02x  @0x%08x  %-28r %s"
              % (row, nid, 0x800c4a28 + off, name, "SICHTBAR" if vis else "unterstrichen (Maske 0)"))

print()
print("== DOKUMENT @0x800ccd34 (der EINE Text, den der Leser fuer JEDE Zeile zeigt) ==")
n = u16(0x800ccd34) >> 1
print("Seiten: %d  (u16 @0x800ccd34 = 0x%04x, >>1 @0x800c7544-50)" % (n, u16(0x800ccd34)))
for i in range(n):
    off = u16(0x800ccd34 + i * 2)
    text, raw = decode(0x800ccd34 + off)
    print("-- Seite %d  @0x%08x (Offset 0x%x, %d Rohbytes)" % (i + 1, 0x800ccd34 + off, off, len(raw)))
    for ln in text.split("\n"):
        print("   | " + ln)
