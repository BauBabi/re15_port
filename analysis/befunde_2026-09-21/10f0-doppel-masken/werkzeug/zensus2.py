# -*- coding: utf-8 -*-
"""Zensus 2: Doppel nach GEOMETRIE-Schluessel (dstX, dstY, w, h, depth).

Der Schluessel enthaelt BEWUSST nicht src: zwei Rechtecke, die an derselben
Bildstelle mit derselben Groesse und derselben Tiefe liegen, belegen zwei
Maskenplaetze und zwei Atlas-Plaetze, obwohl sie EINEN Bildbereich besetzen.
"""
import os
import sys
import json
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from zensus import container, CAP

MASKS = "re15_port/shared_assets/PSX/MASKS"


def analyse():
    rows = []
    for f in sorted(os.listdir(MASKS)):
        if not f.endswith(".MSK"):
            continue
        room = f[:-4]
        res, ncut = container(os.path.join(MASKS, f))
        for c in sorted(res):
            masks, decl, gc, build = res[c]
            n = len(masks)
            geo = {}
            paare = []
            src_gleich = 0
            for i, m in enumerate(masks):
                k = m[2:]           # dstX, dstY, depth, w, h
                if k in geo:
                    paare.append((geo[k], i))
                    if masks[geo[k]][:2] == m[:2]:
                        src_gleich += 1
                else:
                    geo[k] = i
            rows.append(dict(room=room, cut=c, gruppen=gc, gebaut=n, decl=decl,
                             uniq=len(geo), doppel=len(paare), src_gleich=src_gleich,
                             paare=paare[:40]))
    return rows


if __name__ == "__main__":
    rows = analyse()
    tot = sum(r["gebaut"] for r in rows)
    totd = sum(r["doppel"] for r in rows)
    print("Cuts geschrieben: %d (Raeume %d)   Masken %d   Geometrie-Doppel %d (%.2f %%)"
          % (len(rows), len(set(r["room"] for r in rows)), tot, totd, 100.0 * totd / tot))
    print("davon mit GLEICHEM src (bitgleiche Maske): %d"
          % sum(r["src_gleich"] for r in rows))
    print()
    betroffen = [r for r in rows if r["doppel"]]
    print("Cuts MIT Doppeln: %d von %d" % (len(betroffen), len(rows)))
    hdr = "%-9s %3s %5s %5s %5s %6s %6s" % ("Raum", "Cut", "geb.", "uniq", "dopp", "frei", "amLim")
    print(hdr); print("-" * len(hdr))
    for r in sorted(betroffen, key=lambda r: (-r["doppel"], -r["gebaut"])):
        print("%-9s %3d %5d %5d %5d %6d %6s" % (
            r["room"], r["cut"], r["gebaut"], r["uniq"], r["doppel"],
            CAP - r["uniq"], "JA" if r["gebaut"] >= CAP else ""))
    print()
    n_lim = [r for r in rows if r["gebaut"] >= CAP]
    print("Cuts AN der Grenze %d: %d" % (CAP, len(n_lim)))
    print("  davon mit Doppeln: %d" % len([r for r in n_lim if r["doppel"]]))
    n_nah = [r for r in rows if 95 <= r["gebaut"] < CAP]
    print("Cuts NAHE der Grenze (95..104): %d" % len(n_nah))
    json.dump(rows, open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "zensus2.json"), "w"), indent=1)
