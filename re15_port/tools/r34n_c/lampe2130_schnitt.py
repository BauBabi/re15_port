#!/usr/bin/env python3
"""Spur C (Runde 34 Nacht): die gruene Generator-Lampe — RE2 ROOM2130 ESP-TIM byte-gleich ausschneiden.

Quelle:  info/re2leon/PL0/RDT/ROOM2130.RDT, ESP-TIM-Basis = RDT-Kopfwort [20] (Datei 0x58) = 0x0E398,
         TIM-Offset-Tabelle bis Kopfwort [21] = 0x0F458 (abwaerts gelesen wie FUN_8001bd38 @0x8001bd64..
         0x8001bd7c: tim = basis + *(ende - 4)). Laenge = 8 + CLUT-Block + Pixelblock.
Pruefung: gegen die Einzeldatei info/re2leon/PL0/RDT/room2130/esp16.tim; Kopf Magic 0x10, Flags 0x08
         (4 bpp + CLUT), CLUT 16x4 bei (0,480), Pixel 64x32 Halbworte; Zellen 3/4 (u 96/128) mit
         CLUT-Zeile 2 nicht leer; alle Nicht-Null-Eintraege der Zeile 2 mit Bit 15.
Aufruf:  lampe2130_schnitt.py              -> nur pruefen (Standard)
         lampe2130_schnitt.py --schreiben  -> zusaetzlich re15_port/shared_assets/RE2/LAMPE2130.TIM
         (erst in der BAU-Stufe benutzen)."""
import os, sys, struct
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
RDT = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM2130.RDT")
EINZEL = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "room2130", "esp16.tim")
ZIEL = os.path.join(REPO, "re15_port", "shared_assets", "RE2", "LAMPE2130.TIM")

d = open(RDT, "rb").read()
offs = struct.unpack_from("<23I", d, 8)
basis, ende = offs[20], offs[21]
tim_rel = struct.unpack_from("<I", d, ende - 4)[0]
a = basis + tim_rel
magic, flags = struct.unpack_from("<II", d, a)
bl_c, cx, cy, cw, ch = struct.unpack_from("<IHHHH", d, a + 8)
bl_p, px, py, pw, ph = struct.unpack_from("<IHHHH", d, a + 8 + bl_c)
laenge = 8 + bl_c + bl_p
tim = d[a:a + laenge]
fehler = 0
def pruef(text, ok):
    global fehler
    print(("OK    " if ok else "FEHLER"), text)
    if not ok: fehler += 1
pruef("Kopfwort[20] Basis = 0x%05X, Tabelle bis 0x%05X, erster Eintrag %d -> TIM @0x%05X" % (basis, ende, tim_rel, a),
      a == 0x0E398)
pruef("Magic 0x%X Flags 0x%X (4bpp+CLUT)" % (magic, flags), magic == 0x10 and flags == 0x08)
pruef("CLUT %dx%d bei (%d,%d)" % (cw, ch, cx, cy), (cw, ch, cx, cy) == (16, 4, 0, 480))
pruef("Pixel %dx%d Halbworte (= %dx%d Texel)" % (pw, ph, pw * 4, ph), (pw, ph) == (64, 32))
pruef("Laenge %d B" % laenge, laenge == 4256)
e = open(EINZEL, "rb").read()
pruef("byte-gleich mit room2130/esp16.tim (%d B)" % len(e), e == tim)
clut2 = [struct.unpack_from("<H", tim, 8 + 12 + (2 * 16 + k) * 2)[0] for k in range(16)]
pruef("CLUT-Zeile 2: Index 0 = 0x0000, alle anderen mit Bit 15: %s" % " ".join("%04x" % v for v in clut2),
      clut2[0] == 0 and all(v & 0x8000 for v in clut2[1:]))
pix = tim[8 + bl_c + 12:8 + bl_c + bl_p]
for zelle, u0 in ((3, 96), (4, 128)):
    n = sum(1 for v in range(32) for u in range(32)
            if (pix[v * pw * 2 + (u0 + u) // 2] >> (4 * ((u0 + u) & 1))) & 15)
    pruef("Zelle %d (u %d..%d): %d von 1024 Texeln belegt" % (zelle, u0, u0 + 31, n), n > 500)
if "--schreiben" in sys.argv:
    os.makedirs(os.path.dirname(ZIEL), exist_ok=True)
    open(ZIEL, "wb").write(tim)
    print("geschrieben:", ZIEL)
sys.exit(1 if fehler else 0)
