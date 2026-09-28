# -*- coding: utf-8 -*-
"""Runde 31 / H — Paare aus den zulaessigen Sitzen (r31_suche.py): Granate LINKS, Sicherung RECHTS.
Schirm = Cut 4 (Float-Naeherung der Engine-Projektion, NUR PLANUNG) bei Plattform y=-305 (Start)
und y=-1205 (oben, Stand im Sleep 30 @0x101A). Ausgabe: beste Paare je Ziel."""
import math, os, sys
import numpy as np
H = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, H)
import r31_geo as G  # noqa
import r31_suche as Su  # noqa

f = G.Fach()
V = f.view
R = np.array(V['rot'], float).reshape(3, 3) / 4096.0
T = np.array(V['trans'], float)


def schirm(P, py):
    W = np.stack([-20700 - P[:, 0], py + P[:, 1], -17460 - P[:, 2]], 1)   # rot_y 2048
    C = W @ R.T + T
    return 160 + C[:, 0] * V['H'] / C[:, 2], 120 + C[:, 1] * V['H'] / C[:, 2]


def main():
    sich, gran = G.modelle()
    B = os.path.join(H, '..', '..', '..', 'build', 'r31_hebetisch')
    S_ok = np.load(os.path.join(B, 's_ok.npy'))
    G_ok = np.load(os.path.join(B, 'g_ok.npy'))
    Sd, Gd = {}, {}
    s_info = []
    for s in S_ok:
        ry, xs, zs = int(s[0]), int(s[1]), int(s[2])
        if ry not in Sd: Sd[ry] = Su.dreh(sich, ry)
        P = Sd[ry] + np.array([xs, -1062, zs])
        a0, _ = schirm(P, -305); a1, _ = schirm(P, -1205)
        s_info.append((ry, xs, zs, s[5], a0.mean(), a1.mean(), a0.min(), a1.min()))
    g_info = []
    for g in G_ok:
        ry, xg, yc, zg = int(g[0]), int(g[1]), int(g[2]), int(g[3])
        if ry not in Gd: Gd[ry] = Su.dreh(gran, ry)
        P = Gd[ry] + np.array([xg, yc, zg])
        a0, _ = schirm(P, -305); a1, _ = schirm(P, -1205)
        g_info.append((ry, xg, yc, zg, g[6], a0.mean(), a1.mean(), a0.max(), a1.max()))
    # Kandidaten vorfiltern: Granate ganz in der Oeffnung, Sicherung Mitte rechts der Granate
    gv = [g for g in g_info if g[4] >= 0.999]
    print('Granaten-Sitze ganz in der Oeffnung: %d' % len(gv))
    paare = []
    for s in s_info:
        Ps = None
        for g in gv:
            d0 = s[4] - g[5]; d1 = s[5] - g[6]
            if d0 <= 0 or d1 <= 0:
                continue
            paare.append((min(d0, d1), s, g))
    paare.sort(key=lambda p: -p[0])
    print('Paare (vor Abstandspruefung): %d' % len(paare))
    fertig = []
    gesehen = 0
    for sc, s, g in paare:
        gesehen += 1
        Pg = Gd[g[0]] + np.array([g[1], g[2], g[3]])
        ab = Su.abstand(Pg, (s[1], -1062, s[2]), s[0])
        if ab >= 1:
            fertig.append((sc, s, g, ab))
        if len(fertig) >= 40 or gesehen > 400000:
            break
    for sc, s, g, ab in fertig[:40]:
        print('Abstand Schirm-Mitten min(Start,oben) %.1f | Sicherung ry %d (%d,-1062,%d) in Oeffnung %.2f Mitte %.1f/%.1f | '
              'Granate ry %d (%d,%d,%d) Mitte %.1f/%.1f | Abstand %.1f' % (sc, s[0], s[1], s[2], s[3], s[4], s[5], g[0], g[1], g[2], g[3], g[5], g[6], ab))


if __name__ == '__main__':
    main()
