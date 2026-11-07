"""Zahlen-Pruefer V4 r2: Boden im Lauf sfv4/r/V4r2 - Kettenpunkte (debug.log) und die Dornspitze von Mesh 22 (Mesh-Ausdehnung ueber
das Gliedende hinaus, eigener MD1-Parser) gegen den Boden y = a->y = 0 ("Schwanz B y 0"); y > 0 = unter dem Boden."""
import sys, os, numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from pz2_lauf import K, B
from pz2_g1 import secs, md1, S
M = md1(secs(open(S + 'EM30_V4.EMD', 'rb').read())[7])
v = np.vstack([M[22]['tv'][:, :3], M[22]['qv'][:, :3]]).astype(float)
ymax = v[:, 1].max()
r_end = np.hypot(v[:, 0], v[:, 2])[v[:, 1] > 200].max()
print('Mesh 22: y max %.0f (Knochenraum), Radius am Ende %.0f' % (ymax, r_end))
unter = []
for h in sorted(K):
    P = K[h]; L = np.linalg.norm(P[7] - P[6]); d = (P[7] - P[6]) / L
    dorn = P[6] + d * ymax                              # Spitze des Dorns (Mesh-y max entlang der Gliedachse)
    tief = max(dorn[1], P[7][1] + r_end * np.sqrt(max(0.0, 1 - d[1] ** 2)))   # Ring am Gliedende quer zur Achse
    unter.append((h, round(P[:, 1].max()), round(dorn[1]), round(tief), B.get(h, {}).get('clip')))
z = [u for u in unter if u[3] > 0]
print('Takte mit Mesh unter dem Boden (Dorn oder Endring, y > 0):', len(z), 'von', len(unter))
for u in z: print('  h%d: Kette tiefster Punkt y %d, Dornspitze y %+d, tiefster Mesh-Punkt y %+d, clip %s' % u)
print('tiefster Kettenpunkt ueber alle Takte:', max(u[1] for u in unter), '; tiefste Dornspitze:', max(u[2] for u in unter))
