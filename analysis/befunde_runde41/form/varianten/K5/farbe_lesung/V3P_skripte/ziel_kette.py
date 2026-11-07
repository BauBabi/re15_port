"""Pruefer V3: Riegel-Zielwerte der neuen Ketten-Riegel im Messraum von p_zonen.rueck (T = e/Figurlicht, Gauss 1,0), Original + Rundlauf 3 Saaten,
und dieselben Masse fuer K5c heute und Leser-A-(b2)-Probe (aus kette_b2-Videos, Saat 15)."""
import sys, json, math, os
import numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/farbe_lesung/P_skripte')
import p_zonen as PZ
def lab(x): return cv2.cvtColor(np.asarray(x, np.float32).reshape(1, -1, 3) / 255.0, cv2.COLOR_RGB2LAB)[0]
F1 = np.load('F1_pruef.npy'); band = np.load('band2_pruef.npy')
def mass(T, m):
    L = lab(T[m]); C = np.hypot(L[:, 1], L[:, 2]); r = (L[:, 1] >= 12) & (C >= 15) & (L[:, 0] >= 20)
    mu = lab(T[m].mean(0))[0]
    return dict(lab_mittel=[round(float(x), 1) for x in mu], rotklasse_px=int(r.sum()), rotklasse_anteil=round(100 * float(r.mean()), 1),
                ton_mittel=round(math.degrees(math.atan2(mu[2], mu[1])), 1))
R = {}
for n, m, nm in ((1, F1, 'F1'), (2, band, 'Band')):
    v = PZ.P.original(n); To, c = PZ.rueck(v, n); R['%s|Original' % nm] = mass(To, m)
    e = np.clip(To * PZ.LICHT[n], 0, 255).astype(np.uint8)
    for s in (15, 12345, 7):
        Tr, c = PZ.rueck(PZ.FV.video_filter(e, bild=n, entzerren=False, rauschen=True, saat=s), n); R['%s|Rundlauf %d' % (nm, s)] = mass(Tr, m)
    for var in ('K5c_heute', 'b2_probe_A'):
        vid = cv2.cvtColor(cv2.imread('kette_b2_bild%d_%s.png' % (n, var)), cv2.COLOR_BGR2RGB)
        Tr, c = PZ.rueck(vid, n); R['%s|%s' % (nm, var)] = mass(Tr, m)
json.dump(R, open('ziel_kette.json', 'w'), indent=1)
for k, v in R.items(): print(k, v)
