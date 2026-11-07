#!/usr/bin/env python3
"""Pose in Worten: Gelenke im Bild, Hoehe, Tiefe, wichtige Abstaende. python posen_lesen.py lauf/x.pt --wahl 0"""
import argparse, json, math, os, sys
import numpy as np, torch

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import fit as FT      # noqa: E402
import rig as RG      # noqa: E402


def main():
    a = argparse.ArgumentParser()
    a.add_argument('pt')
    a.add_argument('--wahl', type=int, default=0)
    a = a.parse_args()
    ger = RG.Gerippe(FT.GER, FT.DT)
    bilder = [FT.Bild(1, 4, ger), FT.Bild(2, 4, ger)]
    par, ids, d = FT.lade(os.path.join(HIER, a.pt), wahl=[a.wahl])
    with torch.no_grad():
        lae, ver, a3, b3, off, start, ende, st = FT.zustand(par, ger, bilder)
    S, Ee = start[0].cpu().numpy(), ende[0].cpu().numpy()
    wahl = ['huefte', 'schwanz1', 'schwanz2', 'schwanz3', 'schwanz4', 'schwanz5', 'schwanz6', 'wirbel', 'brust', 'hals', 'kopf', 'kiefer',
            'oberarm_r', 'unterarm_r', 'hand_r', 'oberarm_l', 'unterarm_l', 'hand_l', 'oberschenkel_r', 'unterschenkel_r', 'fuss_r', 'oberschenkel_l', 'unterschenkel_l', 'fuss_l']
    erg = {}
    for q, bi in enumerate(bilder):
        s = st[q]
        T = s['T'][0].cpu().numpy()
        R = s['R'][0].cpu().numpy()
        g = float(par.gier[q][0])
        ang = (par.ang[q][0] * ger.frei).detach().cpu().numpy() * 180 / math.pi
        print('=== Bild', bi.nr, 'Gier %.1f' % (math.degrees(g) % 360))
        top = (s['c'][0, :, 1] - s['ry'][0]).min().item()
        print('Scheitel Hoehe %.0f bei %s' % (-top, RG.NAMEN[int(ger.st_kn[(s['c'][0, :, 1] - s['ry'][0]).argmin()])]))
        z = {}
        for n in wahl:
            i = RG.IDX[n]
            p = T[i]
            b = FT.FK.projiziere(bi.kam, p)
            z[n] = dict(bild=[round(float(b[0])), round(float(b[1]))], hoehe=round(float(-p[1])), tiefe=round(float(FT.FK.tiefe(bi.kam, p))))
            print('  %-16s Bild (%3.0f,%3.0f) Hoehe %5.0f Tiefe %5.0f  Winkel (%5.0f %5.0f %5.0f)' % (n, b[0], b[1], -p[1], FT.FK.tiefe(bi.kam, p), ang[i, 0], ang[i, 1], ang[i, 2]))
        for n in ('hand_r', 'hand_l', 'fuss_r', 'fuss_l', 'schwanz6'):
            i = RG.IDX[n]
            p = T[i] + R[i] @ Ee[i]
            print('  Ende %-10s Bild (%3.0f,%3.0f) Hoehe %5.0f Tiefe %5.0f Welt (%6.0f,%6.0f)' % (n, *FT.FK.projiziere(bi.kam, p), -p[1], FT.FK.tiefe(bi.kam, p), p[0], p[2]))
        hr, hl = T[RG.IDX['hand_r']], T[RG.IDX['hand_l']]
        fr, fl = T[RG.IDX['fuss_r']], T[RG.IDX['fuss_l']]
        print('  Abstand Haende %.0f, Fuesse %.0f, Hand r - Fuss r %.0f, Hand l - Fuss l %.0f' % (np.linalg.norm(hr - hl), np.linalg.norm(fr - fl), np.linalg.norm((hr - fr)[[0, 2]]), np.linalg.norm((hl - fl)[[0, 2]])))
        print('  Laenge Figur vorn-hinten im Boden: Schwanzende bis Kopfende')
        kopf_ende = T[RG.IDX['kopf']] + R[RG.IDX['kopf']] @ Ee[RG.IDX['kopf']]
        sw = T[RG.IDX['schwanz6']] + R[RG.IDX['schwanz6']] @ Ee[RG.IDX['schwanz6']]
        print('  Kopfende Hoehe %.0f, Schwanzende Hoehe %.0f, Abstand waagerecht %.0f' % (-kopf_ende[1], -sw[1], np.linalg.norm((kopf_ende - sw)[[0, 2]])))
        if bi.nr == 2:
            fr0 = np.array([600.0, 0, -25300.0])
            print('  Frau-Spawn: Abstand Hand r %.0f, Huefte %.0f' % (np.linalg.norm((hr - fr0)[[0, 2]]), np.linalg.norm((T[0] - fr0)[[0, 2]])))
        erg[bi.nr] = z
    json.dump(erg, open(os.path.join(HIER, 'lauf', 'posen_lesen.json'), 'w', encoding='utf-8'), indent=0)


if __name__ == '__main__':
    main()
