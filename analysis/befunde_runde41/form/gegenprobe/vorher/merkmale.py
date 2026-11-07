#!/usr/bin/env python3
"""Zeichnung auf den Koerper legen: jeder Merkmalpunkt -> Knochen, Stelle, Hoehe, Seite.

    python merkmale.py lauf/x.pt --wahl 3 [--aus lauf/merkmale_x.json]
"""
import argparse, json, math, os, sys
import numpy as np, torch

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import fit as FT      # noqa: E402
import rig as RG      # noqa: E402
import ziele as ZI    # noqa: E402
import ansicht as AN  # noqa: E402

GER = FT.GER
M1 = [('Z1', 325, 231), ('Z1o', 331, 223), ('Z1u', 312, 250), ('Z2', 317, 265), ('Z3', 326, 293), ('Z4', 320, 303), ('A05', 302, 280), ('A06', 290.5, 299), ('A10', 291, 208),
      ('Z5sp', 400, 277), ('Z5ba', 398, 296), ('Z6a', 329, 351), ('Z6b', 322, 366), ('Z6c', 314, 384), ('Z7a', 241, 346), ('Z7b', 248, 372), ('Z7k', 249, 381), ('Z7c', 242, 383),
      ('Z8a', 241, 289), ('Z8b', 253, 285), ('Z8c', 272, 284), ('Z9a', 411, 240), ('Z9b', 420, 262), ('X', 268, 450)]
M2 = [('Q1a', 286, 183), ('Q1b', 277, 206), ('Q2', 327, 168), ('Q2u', 330, 190), ('Q3', 375, 205), ('Q4', 411, 217), ('Q5a', 392, 240), ('Q5b', 362, 296),
      ('Zug1', 300, 192), ('Zug2', 345, 189), ('Zug3', 372, 201), ('Zug4', 388, 236), ('Zug5', 374, 278), ('P', 366, 304),
      ('G1', 338, 320), ('G2', 312, 339), ('G3', 285, 358), ('B10', 280, 275), ('B11', 258, 263), ('B12', 217, 274),
      ('H1', 230, 222), ('H2', 244, 236), ('H3', 226, 256)]


def lage(par, st, ger, bi, q, kn, z, name, x, y):
    xi, yi = int(round(x)), int(round(y))
    b = int(kn[yi, xi])
    weg = 0.0
    if b < 0:
        ys, xs = np.nonzero(kn >= 0)
        d = np.hypot(xs - x, ys - y)
        j = int(np.argmin(d))
        weg = float(d[j])
        xi, yi = int(xs[j]), int(ys[j])
        b = int(kn[yi, xi])
    auge, r = AN.FT.FK.strahl(bi.kam, np.array([x, y], float))
    t = float(z[yi, xi]) / float(r @ bi.kam['R'][2])
    X = auge + r * t
    T = st['T'][0].cpu().numpy()
    R = st['R'][0].cpu().numpy()
    wur = T[0]
    v = R[b].T @ (X - T[b])
    k = RG.klasse(RG.NAMEN[b])
    L = np.linalg.norm(np.array(RG.RICHTUNG[k]))
    # Figurraum: x vorn, y unten, z links, ohne Rumpfneigung
    g = float(par.gier[q][0])
    vorn = np.array([math.cos(g), 0, -math.sin(g)])
    links = np.array([math.sin(g), 0, math.cos(g)])
    return dict(name=name, bild=bi.nr, punkt=[x, y], knochen=RG.NAMEN[b], teil=AN.knochen_teil(bi.nr)[b], weg_px=round(weg, 1), hoehe=round(float(-X[1])),
                vorn=round(float((X - wur) @ vorn)), links=round(float((X - wur) @ links)), lokal=[round(float(a)) for a in v])


def main():
    a = argparse.ArgumentParser()
    a.add_argument('pt')
    a.add_argument('--wahl', type=int, default=0)
    a.add_argument('--aus')
    a = a.parse_args()
    ger = RG.Gerippe(GER, FT.DT)
    bilder = [FT.Bild(1, 4, ger), FT.Bild(2, 4, ger)]
    par, ids, d = FT.lade(os.path.join(HIER, a.pt), wahl=[a.wahl])
    with torch.no_grad():
        lae, ver, a3, b3, off, start, ende, st = FT.zustand(par, ger, bilder)
    aus = []
    for q, (bi, liste) in enumerate(zip(bilder, (M1, M2))):
        kn, z = AN.tiefenbild(st[q], ger, 0)
        for name, x, y in liste:
            e = lage(par, st[q], ger, bi, q, kn, z, name, x, y)
            aus.append(e)
            print('%-5s B%d (%3d,%3d) %-15s %-20s Hoehe %5d vorn %6d links %6d weg %4.1f lokal %s' % (name, bi.nr, x, y, e['knochen'], e['teil'], e['hoehe'], e['vorn'], e['links'], e['weg_px'], e['lokal']))
    if a.aus:
        json.dump(aus, open(os.path.join(HIER, a.aus), 'w', encoding='utf-8'), indent=0)


if __name__ == '__main__':
    main()
