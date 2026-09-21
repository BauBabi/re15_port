# -*- coding: utf-8 -*-
"""Frage 4: Aendern die Doppel das BILD / die VERDECKUNG?

Das Urteil der Engine je Bildpunkt ist  min(depth ueber alle deckenden Masken)
(re15_pri_mask_occludes, re15_pri.h:120: verdeckt gdw. depth < bucket(vz); ein
Punkt ist also verdeckt, sobald IRGENDEINE deckende Maske klein genug ist).

Gemessen wird das Feld  T(p) = min depth  ueber
  (a) alle 105 geschriebenen Masken,
  (b) nur die Masken der NICHT doppelten Objekte.
Sind (a) und (b) gleich, tragen die Doppel zur Verdeckung NICHTS bei.
"""
import os
import sys
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from zensus import container
from paare import lies_tim_bytes
from bloecke import deckung
from bloecke2 import bloecke

MASKS = "re15_port/shared_assets/PSX/MASKS"
INF = 1 << 30


def felder(room, cut, doppel):
    res, _ = container(os.path.join(MASKS, "%s.MSK" % room))
    masks = res[cut][0]
    idx, _c = lies_tim_bytes(open(os.path.join(MASKS, "%s_PRI%02d.TIM" % (room, cut)), "rb").read())
    bl = bloecke(masks)
    A = np.full((240, 320), INF, np.int64)
    B = np.full((240, 320), INF, np.int64)
    for o, (s, e, k, ax, ay) in enumerate(bl):
        for m in masks[s:e]:
            d = deckung(idx, m)
            A = np.where(d, np.minimum(A, m[4]), A)
            if o not in doppel:
                B = np.where(d, np.minimum(B, m[4]), B)
    return A, B, masks, bl, idx


def bericht(room, cut, doppel, figur_kasten=None, figur_eimer=None):
    A, B, masks, bl, idx = felder(room, cut, doppel)
    gl = int((A == B).sum())
    un = np.nonzero(A != B)
    print("=== %s C%d — Doppel-Objekte %s" % (room, cut, sorted(doppel)))
    print("  Tiefenfeld gleich auf %d von 76800 Punkten (%.4f %%); ungleich: %d"
          % (gl, 100.0 * gl / 76800.0, len(un[0])))
    da = A < INF
    db = B < INF
    print("  gedeckte Flaeche: mit Doppeln %d px, ohne %d px, Unterschied %d px"
          % (int(da.sum()), int(db.sum()), int((da ^ db).sum())))
    if len(un[0]):
        for y, x in list(zip(*un))[:20]:
            print("    (%d,%d): mit=%s ohne=%s" % (x, y, A[y, x], B[y, x]))
    if figur_kasten and figur_eimer:
        x0, x1, y0, y1 = figur_kasten
        for name, eimer in figur_eimer:
            va = int(((A[y0:y1 + 1, x0:x1 + 1] < eimer)).sum())
            vb = int(((B[y0:y1 + 1, x0:x1 + 1] < eimer)).sum())
            print("  Figurkasten x%d..%d y%d..%d, Eimer %d (%s): verdeckte Punkte "
                  "mit Doppeln %d, ohne Doppel %d" % (x0, x1, y0, y1, eimer, name, va, vb))
    return A, B, masks, bl, idx


if __name__ == "__main__":
    # Leon an der Nutzer-Marke F335: Kasten x99..121 y81..118, Eimer Fuss 173 /
    # Mitte 169 / Kopf 165 (Auftrag, (1023*vz)>>16 mit vz 11118/10876/10634).
    bericht("ROOM10F0", 4, {7, 8, 9}, (99, 121, 81, 118),
            [("Fuss", 173), ("Mitte", 169), ("Kopf", 165)])
    print()
    bericht("ROOM10F0", 5, {4, 5, 6})
