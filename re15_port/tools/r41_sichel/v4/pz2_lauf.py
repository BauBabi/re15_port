"""Zahlen-Pruefer V4 r2: Spiellauf V4r2 (sfv4/r/V4r2, RE15_FRAMEDUMP + debug.log): Gelenkwinkel der Schwanzkette je Takt, Bodenabstand."""
import re, numpy as np, json
S = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/r/V4r2/'
txt = open(S + 'debug.log', encoding='latin-1').read().splitlines()
K, B, BE = {}, {}, {}
for l in txt:
    m = re.match(r'\[hangar-takt\] h(-?\d+) Schwanz B y \S+ (.*)', l)
    if m:
        pts = re.findall(r'\d\((-?\d+),(-?\d+),(-?\d+)\)', m.group(2)); K[int(m.group(1))] = np.array(pts, float)
    m = re.match(r'\[hangar-takt\] h(-?\d+) F(\d+) B clip (\d+) bild (\d+) gier (\d+)', l)
    if m: B[int(m.group(1))] = dict(F=int(m.group(2)), clip=int(m.group(3)), bild=int(m.group(4)), gier=int(m.group(5)))
for l in open(S + 'bein.log', encoding='latin-1'):
    m = re.match(r'\[bein\] F(\d+) slot35 .*?((?:\d\(-?\d+,-?\d+,-?\d+\) ?)+)', l)
    if m: BE[int(m.group(1))] = np.array(re.findall(r'\d\((-?\d+),(-?\d+),(-?\d+)\)', m.group(2)), float)
print('Takte mit Kette:', min(K), '..', max(K), len(K))
ymax = max((k[:, 1].max(), h) for h, k in K.items())
print('tiefster Kettenpunkt (y gross = unten; Boden y 0):', ymax)
fuss = min(b[:, 1].max() for b in BE.values()); print('Fussgelenke y max (bein.log, tiefster Wert ueber alle F):', max(b[:, 1].max() for b in BE.values()))
def winkel(P):
    w = []
    for i in range(1, len(P) - 1):
        a, b = P[i] - P[i - 1], P[i + 1] - P[i]
        w.append(np.degrees(np.arccos(np.clip(a @ b / np.linalg.norm(a) / np.linalg.norm(b), -1, 1))))
    return np.round(w, 1)
for h in sorted(K):
    if h in (-8, 0, 20, 40, 60, 80, 85, 90, 100, 110, 118, 125, 135, 140, 150) or h == max(K):
        P = K[h]; seg = np.round(np.linalg.norm(np.diff(P, axis=0), axis=1)).astype(int)
        print('h%4d %s Winkel je Gelenk 1-6: %s  Glieder %s  Spitze y %d' % (h, B.get(h, {}), winkel(P).tolist(), seg.tolist(), P[-1][1]))
W = np.array([winkel(K[h]) for h in sorted(K)])
print('Gelenk 3 (Biegung) Winkel ueber den Lauf: min %.1f max %.1f median %.1f' % (W[:, 2].min(), W[:, 2].max(), np.median(W[:, 2])))
print('Max-Winkel je Gelenk ueber den Lauf:', W.max(0).round(1).tolist(), ' Min:', W.min(0).round(1).tolist())
print('--- Rueckfaltung: Takte mit einem Gelenkwinkel > 90 Grad')
hs = sorted(K)
f90 = [h for h in hs if max(winkel(K[h])) > 90]
print('Anzahl', len(f90), 'von', len(hs), ':', f90)
gang = [h for h in hs if B.get(h, {}).get('clip') == 1]
print('davon im Gang (clip 1):', [h for h in f90 if h in gang], 'von', len(gang), 'Gang-Takten', gang[:3], '..', gang[-3:])
for h in (100, 135):
    print('h%d Punkte:' % h, K[h].astype(int).tolist())
