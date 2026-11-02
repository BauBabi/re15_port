#!/usr/bin/env python3
"""Abnahme 1: Gelenke im Bild und Kopfrand gegen Zug Q. Kein RE-Wert."""
import os, sys, math
import cv2
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
from abn_netz import K, P, netz, pose
from abn_kamton import welt, bildpunkte
k = P.Kandidat(K)
n = netz(k)
aus = []
def sag(t=''):
    print(t); aus.append(t)
NAMEN = {0: 'Huefte', 10: 'Brust', 16: 'Schulter r', 17: 'Ellbogen r', 18: 'Handgelenk r', 11: 'Schulter l', 12: 'Ellbogen l', 13: 'Handgelenk l', 9: 'Huefte r', 26: 'Knie r', 27: 'Knoechel r', 8: 'Huefte l', 24: 'Knie l', 25: 'Knoechel l', 21: 'Hals', 22: 'Kopf', 23: 'Kiefer'}
for nr in (1, 2):
    lage, gier, clip, bild = k.stand(nr)
    kam = P.kamera(nr)
    R, T = k.mo.pose(k.kf(clip, bild))
    W = welt(np.array(T[:len(n)], float), lage, gier)
    px, z = bildpunkte(W, kam)
    sag('Bild %d Gelenke (Bildlage, Tiefe)' % nr)
    for b, t in NAMEN.items():
        sag('  %-14s (%4.0f,%4.0f)  Tiefe %6.0f  Hoehe %5.0f' % (t, px[b, 0], px[b, 1], z[b], -T[b][1]))
# Kopf gegen Zug Q in Bild 2: Abstand der Bogenpunkte zum Rand von Kopf allein (22, 23), Hals dazu, Brust dazu
Q = [(262, 225), (290, 172), (345, 189), (372, 201), (388, 236)]
lage, gier, clip, bild = k.stand(2)
kam = P.kamera(2)
V, F, B = pose(k, n, clip, bild)
W = welt(V, lage, gier)
px, z = bildpunkte(W, kam)
for t, kn in (('Kopf 22,23', (22, 23)), ('Kopf und Hals 21,22,23', (21, 22, 23)), ('Brust 10', (10,))):
    M = np.zeros((480, 640), np.uint8)
    for f in F[np.isin(B, kn)]:
        cv2.fillConvexPoly(M, np.round(px[f]).astype(np.int32), 1)
    ko, _ = cv2.findContours(M, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    c = np.vstack([x.reshape(-1, 2) for x in ko])
    z2 = []
    for q in Q:
        d = np.hypot(*(c - np.array(q)).T).min()
        z2.append('%s %s %.0f' % (q, 'innen' if M[q[1], q[0]] else 'aussen', d))
    x, y, w, h = cv2.boundingRect(M)
    sag('%s: Rahmen x %d-%d y %d-%d; Bogenpunkte: %s' % (t, x, x + w, y, y + h, '; '.join(z2)))
with open(os.path.join(HIER, 'mess', 'gelenk.txt'), 'w', encoding='utf-8') as f:
    f.write('\n'.join(aus))
