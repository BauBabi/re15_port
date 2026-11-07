"""Pruefer V3: Rundlauf des Originals (T x Figurlicht -> video_filter, Saat 15/12345/7 -> rueck) fuer Farbton und Anteil der Bild-1-Rotklasse."""
import sys, json, math
import numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/farbe_lesung/P_skripte')
import p_zonen as PZ
def lab(x): return cv2.cvtColor(np.asarray(x, np.float32).reshape(1, -1, 3) / 255.0, cv2.COLOR_RGB2LAB)[0]
def rot(T, m):
    L = lab(T[m]); C = np.hypot(L[:, 1], L[:, 2]); r = (L[:, 1] >= 12) & (C >= 15) & (L[:, 0] >= 20); Pp = L[r]
    return dict(anteil=round(100 * float(r.mean()), 1), h=round(math.degrees(math.atan2(Pp[:, 2].mean(), Pp[:, 1].mean())), 1))
R = {}
for n in (1, 2):
    v = PZ.P.original(n); To, co = PZ.rueck(v, n); regs = dict(PZ.regionen(n)); fig = np.zeros((480, 640), bool)
    for g in regs.values(): fig |= g
    regs['Figur'] = fig
    R['bild%d|Original' % n] = {z: rot(To, g) for z, g in regs.items()}
    e = np.clip(To * PZ.LICHT[n], 0, 255).astype(np.uint8)
    for s in (15, 12345, 7):
        vr = PZ.FV.video_filter(e, bild=n, entzerren=False, rauschen=True, saat=s); Tr, c = PZ.rueck(vr, n)
        R['bild%d|Rundlauf %d' % (n, s)] = {z: rot(Tr, g) for z, g in regs.items()}
json.dump(R, open('rundlauf_ton.json', 'w'), indent=1)
for k, v in R.items(): print(k, v)
