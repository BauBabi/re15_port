"""Bild 1: Aktorlage und Gier fein absuchen am echten Pruefwerkzeug (nur Bild 1), Kandidat-JSON danach setzen."""
import sys, json, os, itertools
sys.path.insert(0, '.')
from b_geom import *
import numpy as np, cv2
sys.path.insert(0, BAUM + '/re15_port/tools')
import form_pruefung as P
ordner = sys.argv[1]
pf = ordner + '/kandidat.json'
J0 = json.load(open(pf))
G = json.load(open(P.daten('grenzen.json'), encoding='utf-8'))
z = cv2.imread(FORM + '/ziel_bild1.png', 0)
PP = [p for p in P.punkte_abschnitt7() if p['bild'] == 1]
zonen = [P.poly_maske(P.zone_b1(i)) & (z == 0) for i in ('L1', 'L2', 'SCH')]


def wert(lage, gier):
    J = dict(J0)
    J['bild1'] = dict(lage=lage, gier=gier)
    json.dump(J, open(ordner + '/kandidat_t.json', 'w'))
    k = P.Kandidat(ordner + '/kandidat_t.json')
    s, a, m, _ = k.rendern(1)
    band = P.material_maske(a, m, 'helle_baender')
    r = [P.punkt_messen(p, G[p['id']], m, band) for p in PP]
    ok = sum(1 for x in r if x['ok'])
    tr = sum(int((m & q).sum()) for q in zonen)
    return ok, tr, int((m & (z == 0)).sum())


base = list(J0['bild1']['lage'])
g0 = J0['bild1']['gier']
best = None
for dx, dz, dg in itertools.product((-24, 0, 24), (-24, 0, 24), (-4, 0, 4)):
    l = [base[0] + dx, 0, base[2] + dz]
    w = wert(l, g0 + dg)
    print(dx, dz, dg, w, flush=True)
    sc = (w[0] - 0.15 * w[1], -w[2])
    if best is None or sc > best[0]:
        best = (sc, l, g0 + dg, w)
print('Bester', best)
json.dump(dict(lage=best[1], gier=best[2]), open(ordner + '/lage1_best.json', 'w'))
os.remove(ordner + '/kandidat_t.json')
