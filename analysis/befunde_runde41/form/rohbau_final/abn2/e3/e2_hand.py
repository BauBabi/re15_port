#!/usr/bin/env python3
"""E2: Handverhaeltnis aus den Masken, Rechenweg. Kamera Bild 2, Handgelenk aus dem Modell (die Masken zeigen es nicht), Spitzen aus den Masken.
Gemessen wird die Bildlaenge Gelenk -> fernster Maskenpunkt der Hand, mal Tiefe am Gelenk durch Brennweite = Laenge quer zur Blickrichtung (Untergrenze der echten Laenge).
Dasselbe am Modell. Verhaeltnis der Masken geteilt durch Verhaeltnis des Modells = Korrektur auf das 3D-Verhaeltnis des Modells. Kein RE-Wert."""
import math, os, sys
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
RF = os.path.dirname(os.path.dirname(HIER))
sys.path.insert(0, os.path.normpath(os.path.join(RF, '..', '..', '..', '..', 're15_port', 'tools')))
import form_pruefung as P
import form_buehne as FB
sys.path.insert(0, RF)
from rf_kern import HAND_L
k = P.Kandidat(os.path.join(RF, 'form.json'))
lage, gier, clip, bild = k.stand(2)
kam = P.kamera(2)
R, T = k.mo.pose(k.kf(clip, bild))
def welt(q):
    a = 2 * math.pi * gier / 4096.0
    s, c = math.sin(a), math.cos(a)
    Y = np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])
    return q @ Y.T + np.array(lage, float)
def proj(W):
    v = (kam.V.astype(float) / 4096.0) @ W.T + kam.t.astype(float)[:, None]
    u = 160 + kam.h * v[0] / v[2]; w = 120 + kam.h * v[1] / v[2]
    x, y = FB.abbilden(u + FB.PHASE, w + FB.PHASE, kam.abbildung)
    return np.stack([x, y], 1), v[2]
# Bildmassstab: px je Einheit quer zur Blickrichtung bei Tiefe z, aus der Abbildung gemessen (Schritt 100 Einheiten)
def px_je_einheit(W):
    p0, z0 = proj(W)
    a = 2 * math.pi * gier / 4096.0
    p1, _ = proj(W + (kam.V.astype(float)[0] / 4096.0) * 100)
    return np.linalg.norm(p1 - p0) / 100.0, z0[0]
_, sk = None, None
z_all = {}
logo = P.logo_maske()
print('HAND_L im Generator', HAND_L, '-> Bauverhaeltnis rechts/links', round(1 / HAND_L, 3))
erg = {}
for seite, h, maske, fi in (('rechts', 18, 'right_arm_front', (19, 20, 29, 31)), ('links', 13, 'left_arm_front', (14, 15, 28, 30))):
    G = welt(T[h].astype(float)[None])
    p, z = proj(G)
    s, _ = px_je_einheit(G)
    m = P.teil_roh(maske) & ~logo
    ys, xs = np.nonzero(m)
    d = np.hypot(xs - p[0, 0], ys - p[0, 1])
    # Hand = Maskenpixel unterhalb (rechts: links-unterhalb) des Gelenks; fernster Punkt und 98-Perzentil (gegen Ausreisser des Handschnitts)
    i = int(d.argmax())
    l_max, l_98 = float(d.max()), float(np.percentile(d, 98))
    # Modell: fernste projizierte Fingerecke
    best = 0.0
    for b in fi:
        m_ = k.mo.meshes[b]
        vv = np.array([x_[:3] for x_ in (m_.tv + m_.qv)], float)
        q = vv @ (R[b].astype(float).T / 4096.0) + T[b].astype(float)
        pp, _ = proj(welt(q))
        best = max(best, float(np.hypot(pp[:, 0] - p[0, 0], pp[:, 1] - p[0, 1]).max()))
    erg[seite] = dict(gelenk=p[0], tiefe=z[0], s=s, l_max=l_max, l_98=l_98, spitze=(xs[i], ys[i]), modell=best)
    print('%s: Gelenk Bild (%.0f,%.0f), Tiefe %.0f, %.4f px je Einheit; Maske fernster Punkt (%d,%d) %.1f px, 98-Perzentil %.1f px; Modell fernste Spitze %.1f px'
          % (seite, p[0, 0], p[0, 1], z[0], s, xs[i], ys[i], l_max, l_98, best))
r, l = erg['rechts'], erg['links']
for nm, key in (('fernster Punkt', 'l_max'), ('98-Perzentil', 'l_98')):
    Lr, Ll = r[key] / r['s'], l[key] / l['s']
    print('Maske %s: rechts %.0f / links %.0f Einheiten quer zur Sicht -> Verhaeltnis %.3f' % (nm, Lr, Ll, Lr / Ll))
Mr, Ml = r['modell'] / r['s'], l['modell'] / l['s']
print('Modell gleiche Messung: rechts %.0f / links %.0f -> %.3f' % (Mr, Ml, Mr / Ml))
q = (r['l_max'] / r['s'] / (l['l_max'] / l['s'])) / (Mr / Ml)
print('Korrektur Maske/Modell %.3f -> 3D-Verhaeltnis aus den Masken %.3f (Bau %.3f)' % (q, q / HAND_L, 1 / HAND_L))
for dp in (-3, 3):
    qq = ((r['l_max'] + dp) / r['s'] / ((l['l_max'] - dp) / l['s'])) / (Mr / Ml)
    print('  Spitzen je %+d px (rechts) / %+d px (links): %.3f' % (dp, -dp, qq / HAND_L))
