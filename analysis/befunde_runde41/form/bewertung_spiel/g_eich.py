# -*- coding: utf-8 -*-
"""Pruefer Spielmodell: Durchblick je Blickhoehe (Spielkameras liegen ueber dem Bauch), G4 in echten Animationsbildern als Eichmass,
Elternbezug der Rumpfteile, Normalenstauchung, Pfeffer in der Textur."""
import sys
import numpy as np
from scipy.ndimage import median_filter
from scipy.spatial import cKDTree
from g_gelenk import *
from g_biege import BAU, posen

ELS = (-35, 0, 20, 40, 65)

print('--- Durchblick je Blickhoehe, Summe ueber 12 Azimute, Oeffnung 3 x 3, Massstab rund 0,175 px je Einheit')
for k in ('A', 'B', 'C'):
    form = lade(k)
    n = len(form['knochen'])
    R, T = pose(form, 2)
    tris = [dreiecke(m) for m in form['meshes'][:n]]
    P = dict(ruhe={})
    P.update(posen(k))
    for nm, D in P.items():
        Rn, Tn = gebeugt(form, R, T, D)
        W = welt(form, Rn, Tn)
        z = []
        for e in ELS:
            s, b, _ = durchblick(form, W, tris, els=(e,))
            z.append(s)
        print(k, '%-11s' % nm, dict(zip(ELS, z)), 'ab 0 Grad:', sum(z[1:]))

print('--- G4 echte Animationsbilder, gleiche Messung, je Clip Bild 0, Mitte, Ende')
form = lade('G4')
n = len(form['knochen'])
tris = [dreiecke(m) for m in form['meshes'][:n]]
gs = []
for c in (0, 3, 4, 5, 8, 9, 10, 26, 27):
    bl = form['clips'][c]['bilder']
    for i in sorted({0, len(bl) // 2, len(bl) - 1}):
        R, T = pose(form, c, i)
        W = welt(form, R, T)
        s0, _, _ = durchblick(form, W, tris, els=(-35,))
        s1, b, _ = durchblick(form, W, tris, els=(0, 20, 40, 65))
        gs.append((c, i, s0, s1, b[0]))
for g in gs:
    print(' Clip %2d Bild %3d  unten %5d  ab 0 Grad %5d  schlechtester Blick %4d' % g)
a = np.array([g[3] for g in gs])
print(' G4 ab 0 Grad: Median', int(np.median(a)), 'Max', int(a.max()))

print('--- Woran haengen die Schlagglieder wirklich: Abstand erstes Armteil zu jedem Rumpfstueck (Ruhe)')
for k, arme, rumpf in (('A', (11, 16), (0, 9, 10)), ('B', (11, 16), (0, 9, 10)), ('C', (16, 11), (0, 9, 10))):
    form = lade(k)
    n = len(form['knochen'])
    R, T = pose(form, 2)
    W = welt(form, R, T)
    tris = [dreiecke(m) for m in form['meshes'][:n]]
    S = {i: proben(W[i], tris[i]) for i in set(arme) | set(rumpf)}
    for ar in arme:
        z = {r: round(float(cKDTree(S[r]).query(S[ar], k=1)[0].min()), 1) for r in rumpf}
        print(k, BAU[k]['namen'][ar], 'Eltern', form['knochen'][ar]['eltern'], 'Abstand zu Rumpfstuecken', z)

print('--- Normalen: Stauchung des Hoehenanteils (Steigung gespeichert gegen Geometrie, 1 = unverbogen)')
for k in ('A', 'B', 'C', 'G4'):
    form = lade(k)
    n = len(form['knochen'])
    xs, ys = [], []
    for i, m in enumerate(form['meshes'][:n]):
        V = np.array(m['ecken'], float)
        N = np.array(m['normalen'], float) / 4096.0
        acc = np.zeros_like(V)
        fl = [(tuple(f['e']), tuple(f['n'])) for f in m['dreiecke']]
        for f in m['vierecke']:
            e, nn = f['e'], f['n']
            fl.append(((e[0], e[1], e[3]), (nn[0], nn[1], nn[3])))
            fl.append(((e[0], e[3], e[2]), (nn[0], nn[3], nn[2])))
        for e, nn in fl:
            p = V[list(e)]
            c = -np.cross(p[1] - p[0], p[2] - p[0])
            for x in e:
                acc[x] += c
        acc /= np.maximum(np.linalg.norm(acc, axis=1, keepdims=True), 1e-6)
        for e, nn in fl:
            for x, y in zip(e, nn):
                xs.append(abs(acc[x][1]))
                ys.append(abs(N[y][1]))
    xs, ys = np.array(xs), np.array(ys)
    st = float((xs * ys).sum() / (xs * xs).sum())
    print(k, 'Steigung |Ny| gespeichert/geometrisch %.2f, Mittel gespeichert %.2f, geometrisch %.2f' % (st, ys.mean(), xs.mean()))

print('--- Texturkoernung: Anteil Texel mit mehr als 18 Y Abstand vom 3 x 3-Median, nur belegte Texel; Y-Perzentile')
for k in ('A', 'B', 'C', 'G4'):
    t = FE.tim_lesen(open(K[k][1], 'rb').read())
    c = np.array(t['cluts'], dtype=np.int64)
    lut = np.stack([(c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3], axis=-1).astype(float)
    rgb = lut[0][t['pixel']]
    y = rgb @ np.array([.299, .587, .114])
    belegt = t['pixel'] != 0
    d = np.abs(y - median_filter(y, 3))
    print(k, 'Koernung %.3f  Y p10/50/90 %s  Farben %d' % (float((d[belegt] > 18).mean()), np.round(np.percentile(y[belegt], [10, 50, 90]), 0), len(np.unique(t['pixel']))))
