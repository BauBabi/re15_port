# -*- coding: utf-8 -*-
"""Pose 2: Huefte tief, Rumpf steiler, Brust bleibt, wo sie ist. Die Nutzer-Maske zeigt unten links Oberkoerper bis zur Bodenzeile und oben links keine Masse: Das Hinterteil liegt tiefer als im Gerippe.
python ik2.py DELTA_GRAD  (DELTA < 0: steiler). Beine danach auf Fuss- und Knieziele."""
import json, math, os, sys
import numpy as np
import figur, schau
from pose_ik import Aufgabe, speichern
from schau import FK, WURZEL
sys.path.insert(0, os.path.join(WURZEL, 're15_port', 'tools'))
import form_emd as FE

E = schau.E
d_grad = float(sys.argv[1]) if len(sys.argv) > 1 else -12.0

f = figur.bauen()
fig = figur.fertig(f)
g = figur.lade_gerippe()
p = fig['posen']['bild2']
w = [list(x) for x in g['posen']['bild2']['winkel']] + [[0, 0, 0], [0, 0, 0]]
wurzel_alt = list(g['posen']['bild2']['wurzel'])
R, T = schau.fk(f.eltern, f.versatz, wurzel_alt, w)
ih, iw, ib = f.ix['huefte'], f.ix['wirbel'], f.ix['brust']
Rb_alt, Tb_alt = R[ib].copy(), T[ib].copy()
# Huefte kippen: lokale Drehung um Z vor die alte Huefte
n = w[ih]
Rh = schau.rm(*(np.array(n, float) * E))
dz = math.radians(d_grad)
Rneu = Rh @ schau.rm(0, 0, dz)
h_neu = list(FE.winkel_aus_matrix(Rneu))
w[ih] = h_neu
Rh2 = schau.rm(*(np.array(h_neu, float) * E))
Rw2 = Rh2 @ schau.rm(*(np.array(w[iw], float) * E))
Lb = Rw2.T @ Rb_alt
w[ib] = list(FE.winkel_aus_matrix(Lb))
Rw3 = Rh2 @ schau.rm(*(np.array(w[iw], float) * E))
# Wurzel so, dass der Brustursprung bleibt
T0 = Tb_alt - (Rh2 @ np.array(f.versatz[iw], float) + Rw3 @ np.array(f.versatz[ib], float))
print('Huefte neu Hoehe', -T0[1], 'Versatz', T0.round(), 'alt', wurzel_alt)
d = json.load(open(os.path.join(schau.HIER, 'posen.json'), encoding='utf-8'))
e = d.setdefault('bild2', {})
e['wurzel'] = [int(round(c)) for c in T0]
e.setdefault('winkel', {})
e['winkel']['huefte'] = [int(c) for c in w[ih]]
e['winkel']['brust'] = [int(c) for c in w[ib]]
json.dump(d, open(os.path.join(schau.HIER, 'posen.json'), 'w', encoding='utf-8'), indent=0)
