#!/usr/bin/env python3
"""Spur C (Runde 34 Nacht): Lage des Auswahl-Cursors (obj 0x00) und des roten Zeigers je Framedump,
neben die Zeilen des RE15_PANEL_LOG gestellt — der Nachweis "Cursor friert ein / faehrt frei" am
ARTEFAKT (gerendertes Bild), nicht an einem Zustandswert.

Cursor: die hellgruenen Eckwinkel der Cursor-Textur (Palette-Index 3 = 0x03E1 = (8,248,0), ROOM11F0.RDT
TIM @0x18DAC, Runde 26 §2.6) -> Pixel mit G >= 200, R <= 80, B <= 80 im Schalterfeld (x 40..200 /
y 50..200 bei 320x240). Zeiger: gesaettigtes Rot (R >= 150, G <= 60, B <= 80) rechts der Skala
(x >= 281). Ausgabe je Bild: F, Cursor-Mitte (x, y) in 320x240-Pixeln, Zeiger-y, panelsperre, maske,
wert (aus panel.log).
Aufruf: cursor_spur.py <laufverzeichnis>"""
import os, sys, glob, re
from PIL import Image
d = sys.argv[1]
log = {}
lp = os.path.join(d, "panel.log")
if os.path.exists(lp):
    for z in open(lp):
        m = re.match(r"F(\d+) ", z)
        if not m: continue
        kv = dict(t.split("=", 1) for t in z.split()[1:] if "=" in t)
        log[int(m.group(1))] = kv
print("%5s %7s %7s %6s %5s %5s %4s %5s" % ("F", "cur_x", "cur_y", "zg_y", "sperr", "maske", "wert", "ruhe"))
for f in sorted(glob.glob(os.path.join(d, "fd_*.ppm"))):
    n = int(re.search(r"fd_(\d+)", f).group(1))
    im = Image.open(f).convert("RGB")
    sx = im.width / 320.0; sy = im.height / 240.0
    px = im.load()
    xs = []; ys = []; zy = []
    for y in range(int(50 * sy), int(200 * sy)):
        for x in range(int(40 * sx), int(200 * sx)):
            r, g, b = px[x, y]
            if g >= 200 and r <= 80 and b <= 80:
                xs.append(x / sx); ys.append(y / sy)
    for y in range(int(40 * sy), int(200 * sy)):
        for x in range(int(281 * sx), int(292 * sx)):
            r, g, b = px[x, y]
            if r >= 150 and g <= 60 and b <= 80:
                zy.append(y / sy)
    cx = sum(xs) / len(xs) if xs else -1; cy = sum(ys) / len(ys) if ys else -1
    zz = sum(zy) / len(zy) if zy else -1
    kv = log.get(n, {})
    print("%5d %7.1f %7.1f %6.1f %5s %5s %4s %5s" % (n, cx, cy, zz, kv.get("panelsperre", "?"),
          kv.get("maske", "?"), kv.get("wert", "?"), kv.get("ruhe", "?")))
