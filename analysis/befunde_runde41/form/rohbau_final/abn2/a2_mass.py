#!/usr/bin/env python3
"""Abnahme 2: Formmasse aus FORM.EMD. Rueckenprofil, Fuss, Hand, Haende im Bild, Eindringen. Kein RE-Wert."""
import math, os, sys
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.dirname(os.path.dirname(HIER))
TOOLS = os.path.normpath(os.path.join(FORM, '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS)
import form_pruefung as P
import form_buehne as FB
from a2_netz_lib import netz, NAMEN
from a2_deckel_kern import treffer
K = os.path.join(FORM, 'rohbau_final', 'form.json')
k = P.Kandidat(K)
n = netz(k)
IX = {v: a for a, v in NAMEN.items()}
def pose(clip):
    R, T = k.mo.pose(k.kf(clip, 0))
    G = {}
    for b, (v, f) in enumerate(n):
        G[b] = (v @ (R[b].astype(float).T / 4096.0) + T[b].astype(float), f)
    return G, R, T
TEIL = {'rumpf': (0, 7, 10), 'schwanz': (1, 2, 3, 4, 5, 6)}
print('=== Rueckenlinie: hoechster Punkt je x (Hoehe in Einheiten)')
for clip, nm in ((0, 'Pose 1'), (2, 'Ruhe'), (1, 'Pose 2')):
    G, R, T = pose(clip)
    print('---', nm, '  x: Schwanz / Rumpf / Stufe (Schwanz minus Rumpf)')
    alles = {t: np.vstack([G[b][0] for b in ks]) for t, ks in TEIL.items()}
    z = []
    for x in range(-1400, 2400, 200):
        wert = {}
        for t, A in alles.items():
            m = np.abs(A[:, 0] - x) < 130
            wert[t] = -A[m, 1].min() if m.any() else None
        z.append((x, wert['schwanz'], wert['rumpf']))
    for x, s, r in z:
        if s is None and r is None:
            continue
        print('   x %5d  Schwanz %5s  Rumpf %5s  Stufe %5s' % (x, '-' if s is None else '%.0f' % s, '-' if r is None else '%.0f' % r, '-' if (s is None or r is None) else '%+.0f' % (s - r)))
print()
print('=== Fuss: Hauptachse gegen Boden, Laenge, Zehenlaenge')
for clip, nm in ((0, 'Pose 1'), (1, 'Pose 2'), (2, 'Ruhe')):
    G, R, T = pose(clip)
    for b in (25, 27):
        q = G[b][0]
        c = q - q.mean(0)
        w, v = np.linalg.eigh(c.T @ c)
        a = v[:, 2]
        win = math.degrees(math.asin(abs(a[1])))
        hoehe = (-q[:, 1]).max()
        print('   %-7s %-7s Achse gegen Boden %3.0f Grad  Ausdehnung x %.0f y %.0f z %.0f  hoechster Punkt %.0f  Ring-/Spitzenzahl Ecken %d' % (nm, NAMEN[b], win, *(q.max(0) - q.min(0)), hoehe, len(q)))
print()
print('=== Zehen und Krallen im Knochenraum (Laenge entlang der Knochenachse, Dicke am Ansatz)')
for b in (25, 27, 13, 14, 15, 28, 30, 18, 19, 20, 29, 31):
    v = n[b][0]
    print('   %-7s Ausdehnung x %.0f y %.0f z %.0f Ecken %d' % (NAMEN[b], *(v.max(0) - v.min(0)), len(v)))
# Zehen im Fuss: Punkte vorn
v = n[27][0]
ach = np.argmax(v.max(0) - v.min(0))
print('   fuss_r Laengsachse', 'xyz'[ach], 'Stufen der Ecken:', sorted(set(np.round(v[:, ach] / 50).astype(int) * 50))[-14:])
print()
print('=== Handflaeche: kleinste Hauptachse von hand_r / hand_l gegen die Senkrechte (0 = Hand liegt flach)')
for clip, nm in ((0, 'Pose 1'), (1, 'Pose 2'), (2, 'Ruhe')):
    G, R, T = pose(clip)
    for bs, tx in (((18,), 'hand_r'), ((13,), 'hand_l')):
        q = np.vstack([G[b][0] for b in bs])
        c = q - q.mean(0)
        w, v = np.linalg.eigh(c.T @ c)
        na = v[:, 0]
        print('   %-7s %-6s Normale gegen senkrecht %3.0f Grad' % (nm, tx, math.degrees(math.acos(abs(na[1])))), end='')
        fi = {'hand_r': (19, 20, 29, 31), 'hand_l': (14, 15, 28, 30)}[tx]
        print('   Fingerspitzen hoechste %s' % ' '.join('%s %.0f' % (NAMEN[b], (-G[b][0][:, 1]).max()) for b in fi))
print()
print('=== Haende in Bild 2: Spitzen projiziert, Tiefe am Handgelenk, Laenge Gelenk-Spitze im Bild und im Modell')
lage, gier, clip, bild = k.stand(2)
kam = P.kamera(2)
G, R, T = pose(1)
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
for tx, h, fi in (('rechts', 18, (19, 20, 29, 31)), ('links', 13, (14, 15, 28, 30))):
    pt = T[h].astype(float)[None]
    p, z = proj(welt(pt))
    print('   %s Handgelenk Bild (%.0f,%.0f) Tiefe %.0f' % (tx, p[0, 0], p[0, 1], z[0]))
    for b in fi:
        q = G[b][0]
        d = np.linalg.norm(q - pt, axis=1)
        i = int(d.argmax())
        s, zz = proj(welt(q[i:i + 1]))
        print('      %-4s Spitze Bild (%.0f,%.0f), Modell-Laenge ab Gelenk %.0f, Bild-Laenge %.0f px, Tiefe %.0f' % (NAMEN[b], s[0, 0], s[0, 1], d[i], np.linalg.norm(s[0] - p[0]), zz[0]))
print()
print('=== Soll aus den Masken: Spitzen right_arm_front, left_arm_front, Laenge ab Handgelenk bei Tiefe des Modells')
sp = {'rechts': ((262, 395), [(145, 353), (197, 476), (222, 475), (274, 443)], 5281), 'links': ((405, 365), [(468, 417), (439, 418)], 6685)}
for t, (w, ss, tief) in sp.items():
    for s in ss:
        l = math.hypot(s[0] - w[0], s[1] - w[1])
        print('   %s %s: %.0f px, mal Tiefe %d = %.0f' % (t, s, l, tief, l * tief / 1000))
print()
print('=== Eindringen: Anteil der Ecken eines Teils im Inneren anderer Teile')
for clip, nm in ((0, 'Pose 1'), (1, 'Pose 2'), (2, 'Ruhe')):
    G, R, T = pose(clip)
    tri = {b: G[b][0][G[b][1]] for b in G if len(G[b][0])}
    print('---', nm)
    for b, gegen, tx in ((16, (21, 22, 23), 'oarm_r in Hals/Kopf/Kiefer'), (11, (21, 22, 23), 'oarm_l in Hals/Kopf/Kiefer'), (16, (0, 7, 10), 'oarm_r im Rumpf'), (11, (0, 7, 10), 'oarm_l im Rumpf'), (21, (0, 7, 10), 'hals im Rumpf'), (22, (0, 7, 10), 'kopf im Rumpf'), (9, (0, 7, 10), 'oschenkel_r im Rumpf'), (8, (0, 7, 10), 'oschenkel_l im Rumpf'), (1, (0, 7, 10), 's1 im Rumpf')):
        v = np.unique(G[b][0].round(2), axis=0)
        drin = 0
        for p in v:
            for o in gegen:
                if len(treffer(p, np.array([.4359, .7171, .5436]), tri[o])) % 2 == 1:
                    drin += 1; break
        print('   %-30s %3d von %3d Ecken  %.2f' % (tx, drin, len(v), drin / len(v)))
