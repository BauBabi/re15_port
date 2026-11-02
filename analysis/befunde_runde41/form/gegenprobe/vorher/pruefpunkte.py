#!/usr/bin/env python3
"""Gemessene Pruefpunkte aus FORMBLATT 7 gegen das Gerippe. python pruefpunkte.py lauf/x.pt --wahl 0 [--aus lauf/pp.json]"""
import argparse, json, os, sys
import numpy as np, cv2, torch

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import fit as FT      # noqa: E402
import rig as RG      # noqa: E402
import ziele as ZI    # noqa: E402
import ansicht as AN  # noqa: E402

F = FT.F


def main():
    a = argparse.ArgumentParser()
    a.add_argument('pt')
    a.add_argument('--wahl', type=int, default=0)
    a.add_argument('--aus')
    a = a.parse_args()
    fb = json.load(open(os.path.join(F, 'formblatt.json'), encoding='utf-8'))
    ger = RG.Gerippe(FT.GER, FT.DT)
    bilder = [FT.Bild(1, 4, ger), FT.Bild(2, 4, ger)]
    par, ids, d = FT.lade(os.path.join(HIER, a.pt), wahl=[a.wahl])
    _, m = ZI.lade()
    erg, karten, _ = AN.tabelle(par, ger, bilder, 0, m)
    aus = []
    # Bild 1
    kn = karten[1][0]
    sil = kn >= 0
    kont, _ = cv2.findContours(sil.astype(np.uint8), cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)
    rand = np.vstack([c.reshape(-1, 2) for c in kont]).astype(float)

    def d_rand(p):
        return float(np.min(np.hypot(rand[:, 0] - p[0], rand[:, 1] - p[1])))

    def spalte_oben(x):
        y = np.nonzero(sil[:, x])[0]
        return int(y.min()) if y.size else -1
    def zeile(y, xlo, xhi):
        xs = np.nonzero(sil[y, xlo:xhi])[0]
        return (int(xs.min()) + xlo, int(xs.max()) + xlo) if xs.size else None
    aus.append(('B1 Scheitel y, Soll 187', [spalte_oben(x) for x in (300, 310, 322)]))
    for p in ((348, 205), (366, 215), (378, 233)):
        aus.append(('B1 Flanke %s Abstand px' % (p,), round(d_rand(p), 1)))
    aus.append(('B1 Rand links x bei y=232, Soll 247', zeile(232, 200, 340)))
    aus.append(('B1 Arm x bei y=324, Soll 381-414', zeile(324, 360, 450)))
    aus.append(('B1 Arm x bei y=344, Soll 381-418', zeile(344, 360, 450)))
    aus.append(('B1 Bein rechts x bei y=322, Soll 300-347', zeile(322, 290, 355)))
    aus.append(('B1 Bein rechts x bei y=346, Soll 308-344', zeile(346, 296, 352)))
    aus.append(('B1 Sohle Hand y unter x=398, Soll 374', int(np.nonzero(sil[:, 398])[0].max())))
    for t in fb['nicht_figur_bild1']:
        if 'polygon' not in t:
            continue
        q = np.zeros(sil.shape, np.uint8)
        cv2.fillPoly(q, [np.array(t['polygon'], np.int32)], 1)
        aus.append(('B1 frei %s: Figurpixel' % t['id'], '%d von %d' % ((sil & (q > 0)).sum(), (q > 0).sum())))
    # Bild 2
    kn = karten[2][0]
    sil = kn >= 0
    kont, _ = cv2.findContours(sil.astype(np.uint8), cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)
    rand = np.vstack([c.reshape(-1, 2) for c in kont]).astype(float)
    r = [t for t in fb['teile_bild2'] if t['id'] == 'R'][0]
    belegt = [p for p in r['kante'] if p[0] >= 354]
    gelesen = [p for p in r['kante'] if p[0] < 354]
    aus.append(('B2 Kante belegt Abstand px', [round(d_rand(p), 1) for p in belegt]))
    aus.append(('B2 Kante gelesen Abstand px', [round(d_rand(p), 1) for p in gelesen]))
    for t in fb['verdeckungen_bild2']:
        (x0, y0), (x1, y1) = t['stelle']
        n = 40
        xs = np.linspace(x0, x1, n).round().astype(int)
        ys = np.linspace(y0, y1, n).round().astype(int)
        aus.append(('B2 %s Anteil auf Figur' % t['id'], round(float(sil[ys, xs].mean()), 2)))
    for z in aus:
        print(z)
    if a.aus:
        json.dump(aus, open(os.path.join(HIER, a.aus), 'w', encoding='utf-8'), indent=0, ensure_ascii=False)


if __name__ == '__main__':
    main()
