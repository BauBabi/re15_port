# -*- coding: utf-8 -*-
"""Messung 30: Wirkung des neuen Objekts ueber ALLE begehbaren Standplaetze.

Fuer jeden Standplatz (Engine-Dump, Band 0, 200er-Raster) wird der Koerperkasten
projiziert und mit ALT und NEU verglichen: wieviele Punkte des Kastens verdeckt die
Maske? ABDECKUNG der Schiene wird mitgemeldet.
"""
import os
import struct
import sys

import numpy as np
from PIL import Image

sys.path.insert(0, "re15_port/tools/maske")
import abnahme                                  # noqa: E402
import geom                                     # noqa: E402
import geometrie                                # noqa: E402
import maskenbild                               # noqa: E402

ALT = "C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/d917ef01-c258-42fe-ac63-7a1e284c7456/scratchpad/m4"
NEU = "re15_port/shared_assets/PSX/MASKS"
rdt, _ = geom.load_rdt(os.path.join("re15_port", "shared_assets", "PSX"), "ROOM10D0")
cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, 7)


def deck_tief(ordner):
    blob = open(os.path.join(ordner, "ROOM10D0.MSK"), "rb").read()
    ms = maskenbild.masken(blob, 7) or []
    idx = maskenbild.lies_tim(os.path.join(ordner, "ROOM10D0_PRI07.TIM"))[0]
    return abnahme.deckung_und_tiefe(ms, idx)


dA, tA = deck_tief(ALT)
dB, tB = deck_tief(NEU)
floor = abnahme.floor_aus_dump("build/p2/floor_p2.txt", 0x10D0)
pts = floor.get(0, [])
print("ABDECKUNG: %d begehbare Standplaetze in Band 0 (200er-Raster, Engine-Dump)" % len(pts))

besser, schlechter, gleich, beruehrt = [], [], 0, 0
for (wx, wz) in pts:
    pf = abnahme.proj(R, t, H, wx, 0, wz)
    pk = abnahme.proj(R, t, H, wx, -abnahme.KOPF, wz)
    if pf is None or pk is None:
        continue
    fsx, fsy, fvz = pf
    ksx, ksy, kvz = pk
    hw = abnahme.HALB * H / fvz
    x0 = int(max(0, fsx - hw)); x1 = int(min(320, fsx + hw))
    y0 = int(max(0, min(ksy, fsy))); y1 = int(min(240, max(ksy, fsy)))
    if x1 <= x0 or y1 <= y0:
        continue
    ys = np.arange(y0, y1)
    vzs, ok = geometrie.profil_spalte(R, t, H, wx, wz, ys)
    vzs = np.where(ok, vzs, fvz)
    vA = (dA[y0:y1, x0:x1] & geometrie.verdeckt(tA[y0:y1, x0:x1], vzs[:, None])).sum()
    vB = (dB[y0:y1, x0:x1] & geometrie.verdeckt(tB[y0:y1, x0:x1], vzs[:, None])).sum()
    if vA or vB:
        beruehrt += 1
    if vB > vA:
        besser.append((wx, wz, int(vA), int(vB)))
    elif vB < vA:
        schlechter.append((wx, wz, int(vA), int(vB)))
    elif vA:
        gleich += 1

print("Standplaetze mit Maskenwirkung: %d" % beruehrt)
print("  MEHR verdeckt (Kasten wirkt jetzt): %d" % len(besser))
print("  WENIGER verdeckt (Splitter faellt weg): %d" % len(schlechter))
print("  unveraendert: %d" % gleich)
if besser:
    b = sorted(besser, key=lambda q: q[3] - q[2], reverse=True)
    print("  groesste Zunahmen (x, z, alt, neu):", b[:8])
    print("  Zunahme gesamt %d Punkte, Median %d"
          % (sum(q[3] - q[2] for q in besser), np.median([q[3] - q[2] for q in besser])))
if schlechter:
    s = sorted(schlechter, key=lambda q: q[2] - q[3], reverse=True)
    print("  groesste Abnahmen (x, z, alt, neu):", s[:8])
    print("  Abnahme gesamt %d Punkte, Median %d"
          % (sum(q[2] - q[3] for q in schlechter), np.median([q[2] - q[3] for q in schlechter])))
