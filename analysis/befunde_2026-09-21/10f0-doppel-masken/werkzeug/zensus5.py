# -*- coding: utf-8 -*-
"""Zensus 5 — gieriger Abbau: wieviele Masken je Cut lassen sich entfernen, OHNE
dass sich das Tiefenfeld an einem einzigen Bildpunkt aendert?

Bei einem gegenseitig redundanten PAAR ist jede Seite fuer sich entfernbar, aber
nur EINE darf gehen. Deshalb wird wiederholt der entfernbare Block mit den
MEISTEN Masken gestrichen und das Feld neu gerechnet, bis keiner mehr geht.
"""
import os
import sys
import json
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from zensus import container, CAP
from paare import lies_tim_bytes
from bloecke import deckung
from bloecke2 import bloecke

MASKS = "re15_port/shared_assets/PSX/MASKS"
INF = 1 << 30

rows = []
for f in sorted(os.listdir(MASKS)):
    if not f.endswith(".MSK"):
        continue
    room = f[:-4]
    res, _ = container(os.path.join(MASKS, f))
    for cut in sorted(res):
        tim = os.path.join(MASKS, "%s_PRI%02d.TIM" % (room, cut))
        masks = res[cut][0]
        if not os.path.exists(tim):
            continue
        idx, _c = lies_tim_bytes(open(tim, "rb").read())
        bl = bloecke(masks)
        felder, groesse = [], []
        for (s, e, k, ax, ay) in bl:
            T = np.full((240, 320), INF, np.int64)
            for m in masks[s:e]:
                d = deckung(idx, m)
                T = np.where(d, np.minimum(T, m[4]), T)
            felder.append(T); groesse.append(e - s)
        aktiv = set(range(len(bl)))

        def feld(menge):
            g = np.full((240, 320), INF, np.int64)
            for i in menge:
                g = np.minimum(g, felder[i])
            return g
        soll = feld(aktiv)
        weg = []
        while True:
            kand = []
            for j in sorted(aktiv, key=lambda j: -groesse[j]):
                if np.array_equal(feld(aktiv - {j}), soll):
                    kand.append(j); break
            if not kand:
                break
            aktiv.discard(kand[0]); weg.append((kand[0], groesse[kand[0]]))
        rows.append(dict(room=room, cut=cut, n=len(masks), bloecke=len(bl),
                         weg=weg, sparbar=sum(x[1] for x in weg)))

bet = [r for r in rows if r["sparbar"]]
print("Cuts mit Atlas: %d;  davon mit entfernbaren Masken: %d" % (len(rows), len(bet)))
print("Entfernbare Masken gesamt: %d von %d (%.2f %%)"
      % (sum(r["sparbar"] for r in rows), sum(r["n"] for r in rows),
         100.0 * sum(r["sparbar"] for r in rows) / sum(r["n"] for r in rows)))
print()
print("%-9s %4s %6s %8s %9s %7s  %s" % ("Raum", "Cut", "Masken", "Bloecke", "entfernb.", "Anteil", "an der Grenze"))
for r in sorted(bet, key=lambda r: -r["sparbar"]):
    print("%-9s %4d %6d %8d %9d %6.0f %%  %s"
          % (r["room"], r["cut"], r["n"], r["bloecke"], r["sparbar"],
             100.0 * r["sparbar"] / r["n"], "JA (105)" if r["n"] >= CAP else ""))
json.dump(rows, open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "zensus5.json"), "w"), indent=1)
