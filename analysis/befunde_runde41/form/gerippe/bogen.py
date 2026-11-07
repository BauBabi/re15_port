#!/usr/bin/env python3
"""Zug Q gegen den Kopfumriss: Abstand der Zugpunkte zum Rand der Kopf-Primitive in Bild 2. python bogen.py lauf/x.pt --wahl 0"""
import argparse, json, os, sys
import numpy as np, cv2, torch

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import fit as FT      # noqa: E402
import rig as RG      # noqa: E402

F = FT.F


def main():
    a = argparse.ArgumentParser()
    a.add_argument('pt')
    a.add_argument('--wahl', type=int, default=0)
    a = a.parse_args()
    ger = RG.Gerippe(FT.GER, FT.DT)
    bilder = [FT.Bild(1, 4, ger), FT.Bild(2, 4, ger)]
    par, ids, d = FT.lade(os.path.join(HIER, a.pt), wahl=[a.wahl])
    with torch.no_grad():
        st = FT.zustand(par, ger, bilder)[-1][1]
        ys, xs = np.mgrid[0:480, 0:640]
        px = torch.tensor(xs.ravel(), dtype=FT.DT, device=FT.GER)
        py = torch.tensor(ys.ravel(), dtype=FT.DT, device=FT.GER)
        sel = torch.nonzero(torch.isin(ger.st_kn, torch.tensor([RG.IDX['kopf'], RG.IDX['kiefer']], device=FT.GER)))[:, 0]
        sd, _ = RG.abstand(px, py, st['u'][:, sel], st['v'][:, sel], st['q'][0][:, sel], st['q'][1][:, sel], st['q'][2][:, sel], st['reff'][:, sel])
        m = (sd.min(1).values[0] < 0).reshape(480, 640).cpu().numpy()
    kont, _ = cv2.findContours(m.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    rand = np.vstack([c.reshape(-1, 2) for c in kont]).astype(float)
    ys, xs = np.nonzero(m)
    print('Kopf+Kiefer im Bild: x %d-%d, y %d-%d' % (xs.min(), xs.max(), ys.min(), ys.max()))
    fb = json.load(open(os.path.join(F, 'formblatt.json'), encoding='utf-8'))
    zug = fb['zeichnung']['zug_q']['linie']
    linke = [[262, 225], [268, 212], [277, 206], [286, 183]]
    for name, pts in (('Zug Q', zug), ('Bogen links Q1', linke)):
        d = [float(np.min(np.hypot(rand[:, 0] - p[0], rand[:, 1] - p[1]))) for p in pts]
        print(name, 'Abstand zum Kopfrand px:', [round(x, 1) for x in d], 'Median %.1f' % np.median(d))


if __name__ == '__main__':
    main()
