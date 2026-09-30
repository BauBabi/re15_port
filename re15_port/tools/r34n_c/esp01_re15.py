#!/usr/bin/env python3
"""Spur C (Runde 34 Nacht, Auflage 2 der Gegenpruefung): den RE1.5-Kandidaten ESP 0x01 (gruene
Glanz-/Sternleuchte) aus einer RE1.5-RDT ziehen und beschreiben.

Format = Port-Parser engine/src/re15_esp.c (FUN_80019354/FUN_8001945c/FUN_800194f8):
  RDT+0x4C Id-Kopf (u8[8]), RDT+0x50 EFF-Zeigertabelle ENDE (abwaerts), RDT+0x54 TIM-Basis,
  RDT+0x58 TIM-Tabelle ENDE (abwaerts, tim = basis + *(ende - 4*(i+1))).
  EFF-Kopf: u16 count_a (Anim-Saetze, je 8 B) / u16 count_b (Zellen, je 4 B), ab +8;
  Zeilenblock = body + 8 + count_a*8 + count_b*4: 8 x u16 Sub-Offsets, base = rowblk + off*4,
  u16 Stroeme (+2), je Strom u16 nrows (+2), dann nrows x 40-B-Zeilen (re15_esp.c:180-196).
Aufruf: esp01_re15.py <ROOMxxxx.RDT> [id]"""
import sys, struct
p = sys.argv[1]; want = int(sys.argv[2], 0) if len(sys.argv) > 2 else 1
d = open(p, "rb").read()
idh, pe, tb, te = struct.unpack_from("<4I", d, 0x4C)
ids = []
for i in range(8):
    if d[idh + i] == 0xFF: break
    ids.append(d[idh + i])
print("RDT %s: Id-Kopf @0x%05X = %s, EFF-Ende 0x%05X, TIM-Basis 0x%05X, TIM-Ende 0x%05X"
      % (p.split('/')[-1], idh, " ".join("%02x" % x for x in ids), pe, tb, te))
k = ids.index(want)
ent = struct.unpack_from("<i", d, pe - 4 * k)[0]
body = idh + ent
ca, cb = struct.unpack_from("<HH", d, body)
print("Id 0x%02X = Eintrag %d: EFF-Koerper @0x%05X, Anim-Saetze %d, Zellen %d" % (want, k, body, ca, cb))
for i in range(ca):
    print("  Satz %d @0x%05X: %s" % (i, body + 8 + 8 * i, " ".join("%02x" % x for x in d[body + 8 + 8 * i: body + 16 + 8 * i])))
cells = []
for i in range(cb):
    a = body + 8 + 8 * ca + 4 * i
    u, v, ox, oy = struct.unpack_from("<BBbb", d, a)
    cells.append((u, v, ox, oy))
    print("  Zelle %d @0x%05X: u %d v %d Versatz %d/%d" % (i, a, u, v, ox, oy))
rb = body + 8 + 8 * ca + 4 * cb
subs = struct.unpack_from("<8H", d, rb)
print("  Zeilenblock @0x%05X, Sub-Offsets %s" % (rb, subs))
for s in range(8):
    base = rb + subs[s] * 4
    if base + 4 > len(d): continue
    nst = struct.unpack_from("<H", d, base)[0]
    if nst == 0 or nst > 8: continue
    a = base + 4
    print("  Strom-Gruppe sub&7=%d @0x%05X: %d Strom(e)" % (s, base, nst))
    for st in range(nst):
        nr = struct.unpack_from("<H", d, a)[0]
        for r in range(min(nr, 4)):
            z = d[a + 4 + 40 * r: a + 4 + 40 * r + 40]
            f = struct.unpack_from("<20h", z)
            print("    Strom %d Zeile %d @0x%05X: %s" % (st, r, a + 4 + 40 * r, " ".join("%02x" % x for x in z)))
            print("        selA %d selB %d w %d h %d | +0e %d | +26 %d" % (f[0], f[1], f[2], f[3], f[7], f[19]))
        a += 4 + 40 * nr
tim = tb + struct.unpack_from("<I", d, te - 4 * (k + 1))[0]
magic, flags = struct.unpack_from("<II", d, tim)
bl, cx, cy, cw, ch = struct.unpack_from("<IHHHH", d, tim + 8)
bl2, px, py, pw, ph = struct.unpack_from("<IHHHH", d, tim + 8 + bl)
print("TIM @0x%05X: Magic 0x%x Flags 0x%x, CLUT %dx%d bei (%d,%d), Pixel %dx%d Halbworte bei (%d,%d), Laenge %d"
      % (tim, magic, flags, cw, ch, cx, cy, pw, ph, px, py, 8 + bl + bl2))
for r in range(ch):
    row = [struct.unpack_from("<H", d, tim + 20 + (r * cw + i) * 2)[0] for i in range(cw)]
    print("  CLUT-Zeile %d: %s" % (r, " ".join("%04x" % x for x in row)))
