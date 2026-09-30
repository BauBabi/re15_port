#!/usr/bin/env python3
"""Spur C (Runde 34 Nacht, BAU-Abnahme): je Framedump (echte exe, RE15_WINDOW_SCALE=3) die Lage des
Auswahl-Cursors, des roten Zeigers und den Zustand der ZWEI GRUENEN LAMPEN — gemessen am GERENDERTEN Bild,
neben die Zeile des RE15_PANEL_LOG des VORBILDS gestellt (das Bild F zeigt den Zustand des Logs F-1:
Hintergrund/Zeiger/Lampen werden am Anfang der Hauptschleife gezeichnet, VM + Spielschritt laufen danach).

Lampen (Auflage 7e: RELATIV zum Hintergrund, keine Absolutschwelle): mittleres G der Glasflaeche im Bild
minus mittleres G derselben Pixel im Original-Hintergrund ROOM11F10.bmp (oben x 218..228 / y 72..80, unten
x 218..229 / y 131..140, C_generator.md §3.6). AN: dG >= 40, AUS: |dG| <= 4, sonst "?".
Zelle (Abnahme 6/7): das Lampenquadrat (22x22 ab (212,65) bzw. (212,125)) gegen die Soll-Vorschau
vorschau_k22_z3.png / _z4.png (dieselbe Regel, re15_lampen_vorschau.py) — mittlere Kanaldifferenz je Zelle,
die kleinere gewinnt.
Cursor (Auflage 7c): hellgruene Eckwinkel der Cursor-Textur (G >= 200, R <= 80, B <= 80) im Schalterfeld
x 40..200 / y 50..200 — steht der Cursor ueber einer Lampe, wuerde er die Lampenmessung verfaelschen; die
Spalte cur_x muss < 200 sein.
Aufruf: lampen_spur.py <laufverzeichnis> [belege-verzeichnis]"""
import os, sys, glob, re
from PIL import Image
HERE = os.path.dirname(os.path.abspath(__file__))
BAUM = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
BG = "C:/workspace/git/reAi_v2/extracted/PSX/STAGE1/ROOM11F/ROOM11F10.bmp"
BELEGE = sys.argv[2] if len(sys.argv) > 2 else os.path.join(BAUM, "analysis", "befunde_runde34_nacht", "C_belege")
d = sys.argv[1]
bg = Image.open(BG).convert("RGB"); bgp = bg.load()
v3 = Image.open(os.path.join(BELEGE, "vorschau_k22_z3.png")).convert("RGB").load()
v4 = Image.open(os.path.join(BELEGE, "vorschau_k22_z4.png")).convert("RGB").load()
GLAS = {"o": (218, 228, 72, 80), "u": (218, 229, 131, 140)}
QUAD = {"o": (212, 65), "u": (212, 125)}
log = {}
lp = os.path.join(d, "panel.log")
if os.path.exists(lp):
    for z in open(lp):
        m = re.match(r"F(\d+) ", z)
        if not m: continue
        log[int(m.group(1))] = dict(t.split("=", 1) for t in z.split()[1:] if "=" in t)
def klein(f):
    im = Image.open(f).convert("RGB")
    s = im.width // 320
    if s > 1:
        im = im.resize((320, 240), Image.NEAREST)
    return im
def mittel_g(p, box):
    x0, x1, y0, y1 = box; n = 0; s = 0
    for y in range(y0, y1 + 1):
        for x in range(x0, x1 + 1):
            s += p[x, y][1]; n += 1
    return s / float(n)
def diff_quad(p, ref, ecke):
    x0, y0 = ecke; s = 0
    for y in range(y0, y0 + 22):
        for x in range(x0, x0 + 22):
            a = p[x, y]; b = ref[x, y]
            s += abs(a[0] - b[0]) + abs(a[1] - b[1]) + abs(a[2] - b[2])
    return s / (22 * 22 * 3.0)
print("%5s %6s %6s %6s | %6s %6s %5s %5s | %4s %4s %5s %4s %5s %4s %4s %3s %3s" % (
    "F", "cur_x", "cur_y", "zg_y", "dG_o", "dG_u", "Z_o", "Z_u",
    "cut", "sper", "maske", "wert", "ruhe", "lo", "lu", "zo", "zu"))
for f in sorted(glob.glob(os.path.join(d, "fd_*.ppm"))):
    n = int(re.search(r"fd_(\d+)", f).group(1))
    im = klein(f); p = im.load()
    xs = []; ys = []; zy = []
    for y in range(50, 200):
        for x in range(40, 200):
            r, g, b = p[x, y]
            if g >= 200 and r <= 80 and b <= 80: xs.append(x); ys.append(y)
    for y in range(40, 200):
        for x in range(281, 292):
            r, g, b = p[x, y]
            if r >= 150 and g <= 60 and b <= 80: zy.append(y)
    cx = sum(xs) / len(xs) if xs else -1; cy = sum(ys) / len(ys) if ys else -1
    zz = sum(zy) / len(zy) if zy else -1
    dg = {}; zell = {}
    for k in ("o", "u"):
        dg[k] = mittel_g(p, GLAS[k]) - mittel_g(bgp, GLAS[k])
        if dg[k] >= 40:
            a3 = diff_quad(p, v3, QUAD[k]); a4 = diff_quad(p, v4, QUAD[k])
            zell[k] = "3:%.1f" % a3 if a3 < a4 else "4:%.1f" % a4
        else:
            zell[k] = "aus" if abs(dg[k]) <= 4 else "?"
    kv = log.get(n - 1, {})
    print("%5d %6.1f %6.1f %6.1f | %6.1f %6.1f %5s %5s | %4s %4s %5s %4s %5s %4s %4s %3s %3s" % (
        n, cx, cy, zz, dg["o"], dg["u"], zell["o"], zell["u"],
        kv.get("cut", "?"), kv.get("panelsperre", "?"), kv.get("maske", "?"), kv.get("wert", "?"),
        kv.get("ruhe", "?"), kv.get("lampe_o", "?"), kv.get("lampe_u", "?"),
        kv.get("lzelle_o", "?"), kv.get("lzelle_u", "?")))
