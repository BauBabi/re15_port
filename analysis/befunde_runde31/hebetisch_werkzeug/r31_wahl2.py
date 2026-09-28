# -*- coding: utf-8 -*-
"""Runde 31 / H — Feinsuche (NUR PLANUNG). Wie r31_wahl.py, aber:
 * Granate nur ry 1024..2048: die in der Hand offene Ecke der Granate (Modell +x-Ende, -z-Seite: dort
   fehlen Kegelflaeche und Eckdreieck, gen/granate_prop.inc) zeigt dann von der Kamera WEG
   (Kamera liegt bei Plattform-x +1242, r31_geo/achse.py).
 * dichtes Raster um die Sieger der Grobsuche."""
import os, sys, itertools
import numpy as np
H = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, H)
import r31_raster as RS  # noqa
import r31_suche as Su  # noqa
import r31_geo as G  # noqa

sch = Su.Schnell()
sz = RS.Szene()
sich, gran = G.modelle()


def zulaessig(pts, sitz):
    x, y, z, ry = sitz
    P = Su.dreh(pts, ry) + np.array([x, y, z])
    lz, lo, raus = sch.luft(P)
    return lz >= 0 and lo >= 0 and raus == 0, P


def einzeln(sitz, art):
    r = []
    for py in (-305, -1205):
        idb = sz.render(py, 150, s_sitz=sitz if art == 'S' else None, g_sitz=sitz if art == 'G' else None)
        r.append(RS.Szene.auswerten(idb)[art])
    return r


s_kand = []
for ry in list(range(1280, 1537, 32)) + list(range(640, 897, 32)) + [1024]:
    for xs in range(-320, -179, 10):
        for zs in range(1250, 1381, 10):
            ok, P = zulaessig(sich, (xs, -1062, zs, ry))
            if ok:
                s_kand.append((xs, -1062, zs, ry))
g_kand = []
for ry in (1024, 1280, 1536, 1792, 2048):
    for xg in range(-340, -179, 10):
        for zg in range(1120, 1211, 5):
            for yc in (-1091, -1088):
                ok, P = zulaessig(gran, (xg, yc, zg, ry))
                if ok:
                    g_kand.append((xg, yc, zg, ry))
print('zulaessig: %d Sicherung, %d Granate' % (len(s_kand), len(g_kand)))
se = [(s, einzeln(s, 'S')) for s in s_kand]
ge = [(g, einzeln(g, 'G')) for g in g_kand]
se = [e for e in se if all(r[0] for r in e[1])]
ge = [e for e in ge if all(r[0] for r in e[1])]
se_r = sorted([e for e in se if min(r[0] for r in e[1]) >= 300], key=lambda e: -sum(r[1] for r in e[1]))[:30]
ge_l = sorted([e for e in ge if min(r[0] for r in e[1]) >= 230], key=lambda e: sum(r[1] for r in e[1]))[:30]
print('Stufe 1: %d/%d Sicherungen, %d/%d Granaten in der Auswahl' % (len(se_r), len(se), len(ge_l), len(ge)))
erg = []
for (s, rs), (g, rg) in itertools.product(se_r, ge_l):
    Pg = Su.dreh(gran, g[3]) + np.array(g[:3])
    ab = Su.abstand(Pg, (s[0], -1062, s[2]), s[3])
    if ab < 1:
        continue
    r = [RS.Szene.auswerten(sz.render(py, 150, s, g)) for py in (-305, -1205)]
    if not all(x['S'][0] and x['G'][0] for x in r):
        continue
    trenn = min(x['S'][1] - x['G'][1] for x in r)
    fl = min(min(x['S'][0], x['G'][0]) for x in r)
    erg.append((min(trenn, 30) * 10 + fl, trenn, fl, s, g, ab, r))
erg.sort(key=lambda e: -e[0])
for e in erg[:25]:
    r = e[6]
    print('Wert %.0f Trennung %.1f kleinste Flaeche %d | S %s G %s | Abstand %.1f | Start S %d x%.1f G %d x%.1f | oben S %d x%.1f G %d x%.1f'
          % (e[0], e[1], e[2], e[3], e[4], e[5], r[0]['S'][0], r[0]['S'][1], r[0]['G'][0], r[0]['G'][1],
             r[1]['S'][0], r[1]['S'][1], r[1]['G'][0], r[1]['G'][1]))
