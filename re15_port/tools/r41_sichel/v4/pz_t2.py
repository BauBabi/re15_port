"""Zahlen-Pruefer: T2-Drehung fein (max. Richtungsaenderung in 0,10 H Bogenlaenge), Rueckenkontur Ref2, Pixelvergleich mit den Nutzerpunkten."""
import json, numpy as np, sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import v4_gemein as G, b2
SP = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/pz/'
d = json.load(open(SP + 'mess_V4_V3_REF2.json'))
for nm in ('V4', 'V3'):
    T = d[nm]['seite']['T']; P = np.array(T['_P']); s = np.array(T['_s']); H = d[nm]['seite']['H']
    def richt(j, hw=0.025):
        a = max(0, np.searchsorted(s, s[j] - hw * H)); b = min(len(P) - 1, np.searchsorted(s, s[j] + hw * H))
        dd = P[b] - P[a]; return np.degrees(np.arctan2(dd[1], -dd[0]))
    R = np.array([richt(j) for j in range(len(P))])
    beg = T['beginn']
    best = (0, None)
    for j in range(len(P)):
        if P[j][0] > beg[0] - 0.10 * H: continue
        j2 = np.searchsorted(s, s[j] + 0.10 * H)
        if j2 >= len(P): break
        if R[j2] - R[j] > best[0]: best = (R[j2] - R[j], j)
    j = best[1]
    print(nm, 'max Drehung in 0,10 H (Fenster +-0,025 H):', round(best[0], 1), 'von', np.round(P[j], 1), 'bis', np.round(P[np.searchsorted(s, s[j] + 0.10 * H)], 1))
    print('   Richtungsprofil (s/H, Grad):', [(round(s[k] / H, 3), int(R[k])) for k in range(0, len(P), 2)])
# Rueckenkontur Ref2-Koerper (Knochen 0-15) in Pose 85 auf Hoehe 0,767 H
P85 = G.posen()['85']
mo = b2.modell(G.REF2)
img, ids, idv = G.zeichne(mo, P85, voll=True, knochen=list(range(16)))
m = (idv >= 0) & (idv < 16)
ys = np.nonzero(m.any(1))[0]; ok, so = ys.min(), ys.max(); H = so - ok
top = np.nonzero(m[ok])[0].mean(); band = np.nonzero(m[ok:ok + 3].any(0))[0].mean()
for f in (0.70, 0.727, 0.75, 0.767, 0.80):
    y = int(round(so - f * H)); xs = np.nonzero(m[y])[0]
    print('Koerper ohne Schwanz: Hoehe %.3f H (y %d): hinterste Kante x %d -> hinter Scheitel(oberste Zeile %.1f) %.3f H, (3 Zeilen %.1f) %.3f H' % (f, y, xs.min(), top, (top - xs.min()) / H, band, (band - xs.min()) / H))
print('H', H, 'ok', ok, 'so', so)
# Oberkontur (Scheitel-Plateau): oberste Zeile je Spalte
cols = {x: np.nonzero(m[:, x])[0].min() for x in range(330, 470) if m[:, x].any()}
mn = min(cols.values()); plat = [x for x, y in cols.items() if y <= mn + 3]
print('Plateau (<= 3 px unter OK): x %d-%d, Mitte %.1f' % (min(plat), max(plat), np.mean(plat)))
print('--- Variante "Richtung je 0,05 H": Winkel zwischen zwei aufeinanderfolgenden 0,05-H-Sehnen (zusammen 0,10 H)')
for nm in ('V4', 'V3'):
    T = d[nm]['seite']['T']; P = np.array(T['_P']); s = np.array(T['_s']); H = d[nm]['seite']['H']; beg = T['beginn']
    def at(x): 
        return np.array([np.interp(x, s, P[:, 0]), np.interp(x, s, P[:, 1])])
    best = (0, 0)
    for x in np.arange(0, s[-1] - 0.10 * H, 0.5):
        if at(x)[0] > beg[0] - 0.10 * H: continue
        a = at(x + 0.05 * H) - at(x); b = at(x + 0.10 * H) - at(x + 0.05 * H)
        w = np.degrees(np.arctan2(b[1], -b[0]) - np.arctan2(a[1], -a[0]))
        if w > best[0]: best = (w, x)
    print(nm, 'max', round(best[0], 1), 'Grad ab', np.round(at(best[1]), 1), 'Mitte', np.round(at(best[1] + 0.05 * H), 1))
