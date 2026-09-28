#!/usr/bin/env python3
"""BAU-ABNAHME (Runde 30, Thema H): Kontaktboegen VORHER/BAU aus den Framedumps.

  bau_abnahme_szene.png  Tuerweg + Ladeweg, F100/F140/F200, Zoom auf den Hebetisch
  bau_abnahme_item.png   Modal F240, Inventar-Raster F50, CHECK F140

Laeufe unter build/r30_sicherung/: bestand_tuer_mit, bestand_laden (vorher),
r30b_tuer_mit, r30b_laden_mit, r30b_inv_grid, r30b_inv_check (Bau; lauf.sh,
lauf_laden.sh, lauf_inventar.sh mit BAUVERZ=build). Die Inventar-Laeufe VORHER
(inv_bestand_grid/_check) stammen aus der Ermittlung; HAUPT=<ordner> zeigt auf
deren build/r30_sicherung, falls es ein anderer Baum ist.

    python bau_uebersicht.py [<ziel-ordner>]
"""
import os
import sys
from PIL import Image, ImageDraw
REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
W = os.path.join(REPO, "build", "r30_sicherung")
M = os.environ.get("HAUPT", W)
AUS = sys.argv[1] if len(sys.argv) > 1 else W

def lade(p): return Image.open(p).convert("RGB")

def kachel(p, text, crop=None, size=(480, 360)):
    im = lade(p)
    if crop:
        im = im.crop(crop)
    im = im.resize(size, Image.NEAREST)
    k = Image.new("RGB", (size[0], size[1] + 18), (0, 0, 0))
    k.paste(im, (0, 18))
    ImageDraw.Draw(k).text((4, 3), text, fill=(255, 255, 0))
    return k

def bogen(zeilen, aus):
    h = sum(max(k.size[1] for k in z) for z in zeilen) + 4 * len(zeilen)
    w = max(sum(k.size[0] for k in z) + 4 * len(z) for z in zeilen)
    b = Image.new("RGB", (w, h), (255, 0, 255))
    y = 0
    for z in zeilen:
        x = 0
        for k in z:
            b.paste(k, (x, y)); x += k.size[0] + 4
        y += max(k.size[1] for k in z) + 4
    b.save(aus)
    print(aus, b.size)

# Szene: Zoom auf den Tisch (320x240-Ausschnitt x120..260 y20..200 -> 960er x3)
Z = (360, 60, 780, 600)
zeilen = []
for f in (100, 140, 200):
    n = "f_%06d.ppm" % f
    zeilen.append([
        kachel(os.path.join(W, "bestand_tuer_mit", n), "VORHER Tuerweg F%d" % f, Z, (315, 405)),
        kachel(os.path.join(W, "r30b_tuer_mit", n), "BAU Tuerweg F%d" % f, Z, (315, 405)),
        kachel(os.path.join(W, "bestand_laden", n), "VORHER Ladeweg F%d" % f, Z, (315, 405)),
        kachel(os.path.join(W, "r30b_laden_mit", n), "BAU Ladeweg F%d" % f, Z, (315, 405)),
    ])
bogen(zeilen, os.path.join(AUS, "bau_abnahme_szene.png"))

zeilen = [
    [kachel(os.path.join(W, "bestand_tuer_mit", "f_000240.ppm"), "VORHER Modal F240"),
     kachel(os.path.join(W, "r30b_tuer_mit", "f_000240.ppm"), "BAU Modal F240")],
    [kachel(os.path.join(M, "inv_bestand_grid", "f_000050.ppm"), "VORHER Raster F50"),
     kachel(os.path.join(W, "r30b_inv_grid", "f_000050.ppm"), "BAU Raster F50")],
    [kachel(os.path.join(M, "inv_bestand_check", "f_000140.ppm"), "VORHER CHECK F140"),
     kachel(os.path.join(W, "r30b_inv_check", "f_000140.ppm"), "BAU CHECK F140")],
]
bogen(zeilen, os.path.join(AUS, "bau_abnahme_item.png"))
