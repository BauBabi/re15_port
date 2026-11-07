"""Pruefer V3: Gegenprobe Versuch 2 Runde 2 (farbe/K5c_farbe_v2) an den neuen Riegeln H1 (Herkunft, Index-Bloecke gleich G4), K11, K12, K14, K15."""
import sys, json, math, os
import numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/farbe_lesung/P_skripte')
sys.path.insert(0, r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe/A')
import p_zonen as PZ
from lade import modelle
FB = PZ.P.FB if hasattr(PZ.P, 'FB') else None
import form_buehne as FB
def lab(x): return cv2.cvtColor(np.asarray(x, np.float32).reshape(1, -1, 3) / 255.0, cv2.COLOR_RGB2LAB)[0]
V = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/farbe/K5c_farbe_v2'
k = PZ.P.Kandidat(V + '/form.json')
G4, tim, _ = modelle()['G4']; tg = FB.Textur(tim)
# H1: Anteil der V2-Texel (8x8-Bloecke), die bytegleich irgendwo im G4-Atlas vorkommen (Index-Ebene)
gb = set()
for y in range(0, tg.h - 7):
    for x in range(0, tg.w - 7):
        gb.add(tg.px[y:y + 8, x:x + 8].tobytes())
vb = [k.tex.px[y:y + 8, x:x + 8].tobytes() for y in range(0, k.tex.h - 7, 8) for x in range(0, k.tex.w - 7, 8)]
R = dict(H1_v2_bloecke_8x8_gleich_G4_prozent=round(100 * sum(b in gb for b in vb) / len(vb), 2), H1_bloecke=len(vb))
def rot(T, m):
    L = lab(T[m]); C = np.hypot(L[:, 1], L[:, 2]); r = (L[:, 1] >= 12) & (C >= 15) & (L[:, 0] >= 20); Pp = L[r]
    return dict(anteil=round(100 * float(r.mean()), 1), h=round(math.degrees(math.atan2(Pp[:, 2].mean(), Pp[:, 1].mean())), 1) if len(Pp) else None)
F1 = np.load('F1_pruef.npy'); band = np.load('band2_pruef.npy')
for n, reg in ((1, F1), (2, band)):
    sauber, alb, m, kk = k.rendern(n)
    vid = PZ.FV.video_filter(sauber, bild=n, entzerren=False, rauschen=True, saat=15)
    T, c = PZ.rueck(vid, n)
    regs = dict(PZ.regionen(n)); fig = np.zeros((480, 640), bool)
    for g in regs.values(): fig |= g
    f = m & (alb.max(2) > 0)
    R['bild%d_zonen' % n] = {z: rot(T, g & f) for z, g in list(regs.items()) + [('Figur', fig)]}
    mu = lab(T[reg].mean(0))[0]
    R['bild%d_stelle' % n] = dict(lab=[round(float(x), 1) for x in mu], rot=rot(T, reg))
json.dump(R, open('gegen_v2.json', 'w'), indent=1)
print(json.dumps(R, indent=1))
