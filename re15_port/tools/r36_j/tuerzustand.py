#!/usr/bin/env python3
"""tuerzustand.py - Spur J (Runde 36, Nachbesserung 1): Tuerbild je Framedump-Bild gegen ROOM11B.BSS.

Vergleicht einen Bildausschnitt (Tuerbereich) jedes Framedumps (RE15_FRAMEDUMP, RE15_WINDOW_SCALE=1) mit dem
GESCHLOSSENEN (z.B. Bild 2) und dem OFFENEN Hintergrund (z.B. Bild 8) aus probe_bg_dump.exe <PSX> <bgdir> 1
(mittlere absolute Grauwert-Differenz) und meldet ZU/OFFEN. Tuerbereiche: Ost (Bild 2/8) 128,92,142,112 bzw.
128,90,142,140; West (Bild 0/7) 206,36,231,97 (Unterschied BSS 2/8 bbox (127,86)-(170,145), 0/7 (206,36)-(231,97)).

Aufruf: tuerzustand.py <bgdir> <laufdir> <zu> <offen> <x0,y0,x1,y1> <f0> <f1> [schritt]
"""
import sys, os
from PIL import Image, ImageChops, ImageStat
bg, run, zu, of = sys.argv[1], sys.argv[2], int(sys.argv[3]), int(sys.argv[4])
box = tuple(int(v) for v in sys.argv[5].split(","))
f0, f1 = int(sys.argv[6]), int(sys.argv[7]); st = int(sys.argv[8]) if len(sys.argv) > 8 else 1
Z = Image.open(f"{bg}/ROOM11B{zu:02d}.ppm").convert("L").crop(box)
O = Image.open(f"{bg}/ROOM11B{of:02d}.ppm").convert("L").crop(box)
for f in range(f0, f1 + 1, st):
    p = f"{run}/f_{f:06d}.ppm"
    if not os.path.exists(p): continue
    im = Image.open(p).convert("L").crop(box)
    dz = ImageStat.Stat(ImageChops.difference(im, Z)).mean[0]
    do = ImageStat.Stat(ImageChops.difference(im, O)).mean[0]
    print("F%d  |Bild-%d| %.1f  |Bild-%d| %.1f  -> %s" % (f, zu, dz, of, do, "ZU" if dz < do else "OFFEN"))
