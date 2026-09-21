# -*- coding: utf-8 -*-
"""Zensus 3 — OBJEKT-Redundanz ueber ALLE Cuts, die einen Atlas haben.

Ein Rechteck-Doppel ist nur die Spitze: redundant ist ein ganzer OBJEKTBLOCK,
dessen undurchsichtige Punkte schon ein anderer Block liefert. Gemessen wird
je Cut mit Atlas: Bloecke, Masken je Block, und Blockpaare mit Jaccard >= 0,99.
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

rows = []
for f in sorted(os.listdir(MASKS)):
    if not f.endswith(".MSK"):
        continue
    room = f[:-4]
    res, _ = container(os.path.join(MASKS, f))
    for cut in sorted(res):
        tim = os.path.join(MASKS, "%s_PRI%02d.TIM" % (room, cut))
        masks = res[cut][0]
        pbm = os.path.exists(os.path.join(MASKS, "%s_PRI%02d.PBM" % (room, cut)))
        if not os.path.exists(tim):
            rows.append(dict(room=room, cut=cut, n=len(masks), atlas=False, p2=pbm,
                             bloecke=0, dopp_obj=0, dopp_masken=0))
            continue
        idx, _c = lies_tim_bytes(open(tim, "rb").read())
        bl = bloecke(masks)
        decks = []
        for (s, e, k, ax, ay) in bl:
            d = np.zeros((240, 320), bool)
            for m in masks[s:e]:
                d |= deckung(idx, m)
            decks.append(d)
        dopp_obj, dopp_masken, paare = 0, 0, []
        erledigt = set()
        for i in range(len(decks)):
            for j in range(i + 1, len(decks)):
                if j in erledigt:
                    continue
                a, b = decks[i], decks[j]
                u = int((a | b).sum())
                if u == 0:
                    continue
                jac = int((a & b).sum()) / float(u)
                if jac >= 0.99:
                    dopp_obj += 1
                    erledigt.add(j)
                    ni = bl[i][1] - bl[i][0]
                    nj = bl[j][1] - bl[j][0]
                    dopp_masken += min(ni, nj)
                    paare.append((i, j, round(jac, 4), ni, nj))
        rows.append(dict(room=room, cut=cut, n=len(masks), atlas=True, p2=pbm,
                         bloecke=len(bl), dopp_obj=dopp_obj, dopp_masken=dopp_masken,
                         paare=paare))

mit = [r for r in rows if r["atlas"]]
print("Sektionen gesamt %d;  mit Atlas (zeichnen etwas) %d;  Phase-2-Bau (.PBM) %d"
      % (len(rows), len(mit), sum(1 for r in rows if r["p2"])))
print("Masken gesamt %d" % sum(r["n"] for r in rows))
bet = [r for r in mit if r["dopp_obj"]]
print("Cuts mit REDUNDANTEM OBJEKTBLOCK (Jaccard >= 0,99): %d von %d" % (len(bet), len(mit)))
for r in sorted(bet, key=lambda r: -r["dopp_masken"]):
    print("  %-9s C%-2d  %3d Masken, %2d Bloecke, %d Doppelpaare, %d redundante Masken (%.0f %%)  %s"
          % (r["room"], r["cut"], r["n"], r["bloecke"], r["dopp_obj"], r["dopp_masken"],
             100.0 * r["dopp_masken"] / r["n"], r["paare"]))
print()
lim = [r for r in rows if r["n"] >= CAP]
print("Cuts AN der Engine-Grenze %d Masken: %d" % (CAP, len(lim)))
for r in sorted(lim, key=lambda r: (r["room"], r["cut"])):
    print("  %-9s C%-2d  Atlas %s  Phase2 %s  Bloecke %2d  redundant %d"
          % (r["room"], r["cut"], "ja" if r["atlas"] else "NEIN",
             "ja" if r["p2"] else "nein", r["bloecke"], r["dopp_masken"]))
nah = [r for r in rows if 95 <= r["n"] < CAP]
print("Cuts 95..104 Masken: %d" % len(nah))
json.dump([{k: v for k, v in r.items() if k != "paare"} for r in rows],
          open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "zensus3.json"), "w"), indent=1)
