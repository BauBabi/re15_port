# -*- coding: utf-8 -*-
"""Zensus 4 — BEITRAGSLOSE Objektbloecke, allgemein und ohne Formvergleich.

Kriterium (exakt, kein Mass): ein Block j ist beitragslos, wenn das Tiefenfeld
T(p) = min ueber alle deckenden Masken  OHNE j an KEINEM Bildpunkt groesser wird.
Dann aendert sein Wegfall weder die gedeckte Flaeche noch ein einziges
Verdeckungsurteil der Engine (re15_pri_mask_occludes: depth < bucket(vz)).
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
        felder = []
        for (s, e, k, ax, ay) in bl:
            T = np.full((240, 320), INF, np.int64)
            for m in masks[s:e]:
                d = deckung(idx, m)
                T = np.where(d, np.minimum(T, m[4]), T)
            felder.append(T)
        ges = np.full((240, 320), INF, np.int64)
        for T in felder:
            ges = np.minimum(ges, T)
        frei = []
        for j in range(len(bl)):
            ohne = np.full((240, 320), INF, np.int64)
            for i, T in enumerate(felder):
                if i == j:
                    continue
                ohne = np.minimum(ohne, T)
            if np.array_equal(ohne, ges):
                frei.append((j, bl[j][1] - bl[j][0]))
        rows.append(dict(room=room, cut=cut, n=len(masks), bloecke=len(bl),
                         frei=frei, freimasken=sum(x[1] for x in frei)))

print("Cuts mit Atlas: %d" % len(rows))
bet = [r for r in rows if r["frei"]]
print("Cuts mit BEITRAGSLOSEN Bloecken: %d" % len(bet))
print("%-9s %3s %5s %7s %9s  %s" % ("Raum", "Cut", "Mask", "Bloecke", "beitr.los", "Bloecke (Index, Masken)"))
for r in sorted(bet, key=lambda r: -r["freimasken"]):
    print("%-9s %3d %5d %7d %9d  %s"
          % (r["room"], r["cut"], r["n"], r["bloecke"], r["freimasken"], r["frei"][:12]))
json.dump(rows, open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "zensus4.json"), "w"), indent=1)
