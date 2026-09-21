# -*- coding: utf-8 -*-
"""Bloecke = Objekte, robust gegen die ANGESCHNITTENEN Randkacheln.

geom.gitter_mit_kante legt ein Gitter der Kante k ueber die Bounding-Box der
Objektregion; die Kacheln am rechten/unteren Rand sind schmaler. Ein Block
wird deshalb ueber ANKER + Kante erkannt: Kachel j gehoert dazu, wenn
(dx_j - dx_0) % k == 0 und (dy_j - dy_0) % k == 0 und w_j <= k und h_j <= k.
"""
import os
import sys
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from zensus import container
from paare import lies_tim_bytes
from bloecke import deckung

MASKS = "re15_port/shared_assets/PSX/MASKS"


def bloecke(masks):
    out, i = [], 0
    n = len(masks)
    while i < n:
        k = max(masks[i][5], masks[i][6])
        ax, ay = masks[i][2], masks[i][3]
        j = i + 1
        while j < n:
            sx, sy, dx, dy, dep, w, h = masks[j]
            if w > k or h > k or (dx - ax) % k or (dy - ay) % k:
                break
            j += 1
        out.append((i, j, k, ax, ay))
        i = j
    return out


def main(room, cuts):
    res, _ = container(os.path.join(MASKS, "%s.MSK" % room))
    for cut in cuts:
        masks, decl, gc, build = res[cut]
        idx, clut = lies_tim_bytes(open(os.path.join(MASKS, "%s_PRI%02d.TIM" % (room, cut)), "rb").read())
        bl = bloecke(masks)
        decks, info = [], []
        for (s, e, k, ax, ay) in bl:
            d = np.zeros((240, 320), bool)
            for m in masks[s:e]:
                d |= deckung(idx, m)
            decks.append(d)
            deps = sorted(set(m[4] for m in masks[s:e]))
            info.append((s, e, k, ax, ay, int(d.sum()), deps[0], deps[-1], len(deps)))
        print("=== %s C%d: %d Masken -> %d Bloecke (= Objekte)" % (room, cut, len(masks), len(bl)))
        for b, (s, e, k, ax, ay, px, d0, d1, nd) in enumerate(info):
            print("  Obj %2d: Masken %3d..%-3d n=%2d  Kante %2d Anker(%3d,%3d)  Deckung %5d px  "
                  "Tiefen %d..%d (%d Stufen)" % (b, s, e - 1, e - s, k, ax, ay, px, d0, d1, nd))
        print("  --- identische / enthaltene Deckungen ---")
        verlust = 0
        for i in range(len(decks)):
            for j in range(i + 1, len(decks)):
                a, b_ = decks[i], decks[j]
                inter = int((a & b_).sum())
                if inter == 0:
                    continue
                u = int((a | b_).sum())
                jac = inter / float(u)
                kenn = "IDENTISCH" if jac == 1.0 else ("%.4f" % jac)
                print("    Obj %2d (%2d Masken, %5d px) <-> Obj %2d (%2d Masken, %5d px): "
                      "Jaccard %s  nurA=%d nurB=%d"
                      % (i, info[i][1] - info[i][0], info[i][5], j,
                         info[j][1] - info[j][0], info[j][5], kenn,
                         int((a & ~b_).sum()), int((b_ & ~a).sum())))
                if jac == 1.0:
                    verlust += min(info[i][1] - info[i][0], info[j][1] - info[j][0])
        ges = np.zeros((240, 320), bool)
        for d in decks:
            ges |= d
        print("  Gesamtdeckung: %d px (%.2f %% des Bildes);  redundante Masken (billigere Seite): %d"
              % (int(ges.sum()), 100.0 * ges.mean(), verlust))
        print()


if __name__ == "__main__":
    main("ROOM10F0", (4, 5))
