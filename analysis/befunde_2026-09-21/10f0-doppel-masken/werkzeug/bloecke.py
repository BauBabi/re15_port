# -*- coding: utf-8 -*-
"""Bloecke (= Objekte) aus der GESCHRIEBENEN Sektion zurueckgewinnen und ihre
Deckungen vergleichen.

Ein Objekt wird von anwenden.bau_objektweise mit EINER Kantenlaenge gekachelt
(geom.gitter_mit_kante, anwenden.py:461-463) und alle seine Kacheln stehen
zusammenhaengend in der Maskenliste. Der Block wechselt also genau dort, wo
Gitterursprung (x mod k, y mod k) oder Kante k wechselt.

Gemessen wird je Block die DECKUNG aus dem Atlas (Palettenindex != 0) —
das ist genau die Flaeche, die die Maske im Bild undurchsichtig macht.
"""
import os
import struct
import sys
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from zensus import container
from paare import lies_tim_bytes

MASKS = "re15_port/shared_assets/PSX/MASKS"


def deckung(idx, m):
    sx, sy, dx, dy, dep, w, h = m
    out = np.zeros((240, 320), bool)
    x0, x1 = max(0, dx), min(320, dx + w)
    y0, y1 = max(0, dy), min(240, dy + h)
    if x1 <= x0 or y1 <= y0:
        return out
    sub = idx[sy + (y0 - dy):sy + (y1 - dy), sx + (x0 - dx):sx + (x1 - dx)]
    out[y0:y1, x0:x1] = sub != 0
    return out


def bloecke(masks):
    """-> [(start, ende_exklusiv, kante, ux, uy)] nach Gitter-Signatur."""
    sig = []
    for (sx, sy, dx, dy, dep, w, h) in masks:
        k = max(w, h)
        sig.append((k, dx % k, dy % k))
    out = []
    s = 0
    for i in range(1, len(masks) + 1):
        if i == len(masks) or sig[i] != sig[s]:
            out.append((s, i) + sig[s])
            s = i
    return out


if __name__ == "__main__":
    for cut in (4, 5):
        res, _ = container(os.path.join(MASKS, "ROOM10F0.MSK"))
        masks, decl, gc, build = res[cut]
        idx, clut = lies_tim_bytes(open(os.path.join(MASKS, "ROOM10F0_PRI%02d.TIM" % cut), "rb").read())
        bl = bloecke(masks)
        print("=== ROOM10F0 C%d: %d Masken -> %d Bloecke" % (cut, len(masks), len(bl)))
        decks = []
        for (s, e, k, ux, uy) in bl:
            d = np.zeros((240, 320), bool)
            for m in masks[s:e]:
                d |= deckung(idx, m)
            deps = sorted(set(m[4] for m in masks[s:e]))
            decks.append(d)
            print("  Block %2d: Masken %3d..%-3d  n=%2d  Kante %2d Gitter(%2d,%2d)  Deckung %5d px  t=%d..%d"
                  % (len(decks) - 1, s, e - 1, e - s, k, ux, uy, int(d.sum()), deps[0], deps[-1]))
        print("  --- Blockpaare mit gleicher Deckung ---")
        for i in range(len(decks)):
            for j in range(i + 1, len(decks)):
                a, b = decks[i], decks[j]
                inter = int((a & b).sum())
                if inter == 0:
                    continue
                u = int((a | b).sum())
                print("    Block %2d (%d px, %d Masken) vs %2d (%d px, %d Masken): "
                      "gemeinsam %d, nur A %d, nur B %d, Jaccard %.4f"
                      % (i, int(a.sum()), bl[i][1] - bl[i][0], j, int(b.sum()),
                         bl[j][1] - bl[j][0], inter, int((a & ~b).sum()), int((b & ~a).sum()),
                         inter / float(u)))
        ges = np.zeros((240, 320), bool)
        for d in decks:
            ges |= d
        print("  Gesamtdeckung des Cuts: %d px (%.1f %% des Bildes)" % (int(ges.sum()), 100.0 * ges.mean()))
        print()
