# -*- coding: utf-8 -*-
"""Was gewinnt man, wenn die doppelten Objekte entfallen?

Die Regionen kommen aus der GESCHRIEBENEN Maske (Atlas-Deckung je Objektblock) —
sie sind die Sollflaeche punktgenau (raum.py:668-680 "Treue: ... punktgenau").
Die Kachelwahl wird aus anwenden.py:417-465 woertlich nachgestellt und ZUERST
gegen das geschriebene Ergebnis geeicht.
"""
import os
import sys
import numpy as np
sys.path.insert(0, "re15_port/tools/maske")
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import geom
from zensus import container
from paare import lies_tim_bytes
from bloecke import deckung
from bloecke2 import bloecke

MASKS = "re15_port/shared_assets/PSX/MASKS"
KANTEN = (8, 10, 12, 16, 20, 24, 32, 40, 48, 64)
KAP = 256 * 256
BUDGET = geom.MAX_MASKS_PER_CUT     # 105, geom.py:23


def waehle(kosten, rest, kap=KAP):
    """anwenden.py:432-465 woertlich."""
    kanten_je = [[k for k in KANTEN if k in ko] for ko in kosten]
    wahl = [len(kj) - 1 for kj in kanten_je]

    def summen(w):
        n = sum(kosten[i][kanten_je[i][j]][0] for i, j in enumerate(w))
        f = sum(kosten[i][kanten_je[i][j]][1] for i, j in enumerate(w))
        return n, f
    n, f = summen(wahl)
    if n > rest or f > kap:
        return tuple(kanten_je[i][j] for i, j in enumerate(wahl)), n, f, True
    verbessert = True
    while verbessert:
        verbessert = False
        beste = None
        for i in range(len(wahl)):
            if wahl[i] == 0:
                continue
            probe = list(wahl); probe[i] -= 1
            pn, pf = summen(probe)
            if pn > rest or pf > kap:
                continue
            spr = kanten_je[i][wahl[i]] - kanten_je[i][probe[i]]
            if beste is None or spr > beste[0]:
                beste = (spr, i, pn, pf)
        if beste is not None:
            wahl[beste[1]] -= 1; n, f = beste[2], beste[3]; verbessert = True
    return tuple(kanten_je[i][j] for i, j in enumerate(wahl)), n, f, False


def regionen(room, cut):
    res, _ = container(os.path.join(MASKS, "%s.MSK" % room))
    masks = res[cut][0]
    idx, _c = lies_tim_bytes(open(os.path.join(MASKS, "%s_PRI%02d.TIM" % (room, cut)), "rb").read())
    out = []
    for (s, e, k, ax, ay) in bloecke(masks):
        d = np.zeros((240, 320), bool)
        for m in masks[s:e]:
            d |= deckung(idx, m)
        out.append((d, k, e - s))
    return out, masks


def stufen(region, k):
    """Wieviele UNTERSCHIEDLICHE Tiefen traegt eine Kachelung? (Aufloesung der Tiefe)"""
    return len(geom.gitter_mit_kante(region, k))


def lauf(room, cut, doppel_idx):
    regs, masks = regionen(room, cut)
    kosten = [geom.gitter_kosten(r, KANTEN) for (r, k, n) in regs]
    ist_kante = [k for (r, k, n) in regs]
    ist_n = [n for (r, k, n) in regs]
    print("=== %s C%d — %d Objekte, %d Masken geschrieben" % (room, cut, len(regs), sum(ist_n)))

    komb, n, f, notnagel = waehle(kosten, BUDGET)
    ok = (list(komb) == ist_kante)
    print("  EICHUNG der nachgestellten Wahl gegen die geschriebene Maske:")
    print("    geschrieben: Kanten %s -> %d Masken" % (ist_kante, sum(ist_n)))
    print("    nachgestellt: Kanten %s -> %d Masken, %d Atlaspunkte%s"
          % (list(komb), n, f, "  [Notnagel]" if notnagel else ""))
    print("    %s" % ("UEBEREIN — das Modell ist die gebaute Wahl" if ok else "ABWEICHEND"))

    behalten = [i for i in range(len(regs)) if i not in doppel_idx]
    k2 = [kosten[i] for i in behalten]
    komb2, n2, f2, nn2 = waehle(k2, BUDGET)
    print("  OHNE die %d doppelten Objekte %s:" % (len(doppel_idx), sorted(doppel_idx)))
    print("    Kanten %s -> %d Masken, %d Atlaspunkte" % (list(komb2), n2, f2))
    print("    frei: %d von %d Maskenplaetzen (%.0f %%), %d von %d Atlaspunkten"
          % (BUDGET - n2, BUDGET, 100.0 * (BUDGET - n2) / BUDGET, KAP - f2, KAP))
    print("    Kachel-Verfeinerung je behaltenem Objekt:")
    for a, i in enumerate(behalten):
        alt = ist_kante[i]
        neu = komb2[a]
        px = int(regs[i][0].sum())
        print("      Obj %2d (%5d px): Kante %2d -> %2d  %s  Kacheln %d -> %d"
              % (i, px, alt, neu, "FEINER" if neu < alt else ("gleich" if neu == alt else "groeber"),
                 kosten[i][alt][0], kosten[i][neu][0]))
    return n2


if __name__ == "__main__":
    # Doppel-Objekte = die aus dem quader/nur_kunst-Zweig, deren Zelle schon
    # ein szene-Lasso hat (siehe _r22/herkunft.py): C4 Obj 7,8,9 ; C5 Obj 4,5,6
    lauf("ROOM10F0", 4, {7, 8, 9})
    print()
    lauf("ROOM10F0", 5, {4, 5, 6})
