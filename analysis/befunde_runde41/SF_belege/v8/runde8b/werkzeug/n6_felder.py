"""Suche (Messung): (1) schraeges Fleischfeld (Parallelogramm) in der Original-Nutzung der Zeile 482, Seite 2, Zeilen v0..v1, groesste Breite;
(2) Band-Feld hellrot/rosa in der Original-Nutzung der Zeile 480 (Asset 493), Seite 0: groesstes Rechteck mit >= 60 % solcher Texel."""
import os, sys
import numpy as np
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
sys.path.insert(0, W); sys.dont_write_bytecode = True
import v8_gemein as V, adern_r4 as A4, kopf_orig_bau as KB, birkin_formen as BF, v4_gemein as G
o = BF.lade_em30(); to = KB.tim_teile(KB.teile(o)[3])
def lab_zeile(z):
    c = to['clut'][z - 480].astype(int)
    rgb = np.stack([(c & 31) * 255 / 31., ((c >> 5) & 31) * 255 / 31., ((c >> 10) & 31) * 255 / 31.], -1)
    return G.lab(rgb[None])[0]
nutz = A4.nutzung(None, 482)[:, 256:384]
lab = lab_zeile(482); Lm = lab[to['pix'][:, 256:384]]
best = None
for v0 in range(110, 117):
    for v1 in range(v0 + 6, 126):
        for a in range(0, 30):
            for b in range(a, 40):
                for w in range(40, 5, -1):
                    ok = True
                    for v in range(v0, v1 + 1):
                        u = int(round(a + (b - a) * (v - v0) / (v1 - v0)))
                        if u < 0 or u + w > 128 or not nutz[v, u:u + w].all():
                            ok = False; break
                    if ok:
                        area = w * (v1 - v0 + 1)
                        if best is None or area > best[0]:
                            sel = np.concatenate([Lm[v, int(round(a + (b - a) * (v - v0) / (v1 - v0))):int(round(a + (b - a) * (v - v0) / (v1 - v0))) + w] for v in range(v0, v1 + 1)])
                            C = np.hypot(sel[:, 1], sel[:, 2])
                            best = (area, v0, v1, a, b, w, float(np.median(sel[:, 0])), float(np.median(C)), float(np.std(sel[:, 0])))
                        break
print('Fleisch schraeg (Flaeche, v0, v1, u0 bei v0, u0 bei v1, Breite, L, C, sL):', best)
# Band
lab = lab_zeile(480); nz = A4.nutzung(None, 480)[:, 0:128]; Lm = lab[to['pix'][:, 0:128]]
C = np.hypot(Lm[..., 1], Lm[..., 2]); h = np.degrees(np.arctan2(Lm[..., 2], Lm[..., 1]))
pink = nz & (Lm[..., 0] >= 40) & (Lm[..., 0] <= 56) & (C >= 12) & (C <= 28) & (h >= 5) & (h <= 35)
bb = []
for v0 in range(0, 250):
    for v1 in range(v0 + 3, min(v0 + 30, 256)):
        for u0 in range(0, 125):
            for u1 in range(u0 + 3, min(u0 + 16, 128)):
                if not nz[v0:v1 + 1, u0:u1 + 1].all():
                    break
                a = pink[v0:v1 + 1, u0:u1 + 1].mean()
                if a >= 0.6:
                    bb.append(((u1 - u0 + 1) * (v1 - v0 + 1), u0, u1, v0, v1, round(float(a), 2)))
bb.sort(reverse=True)
for x in bb[:8]:
    u0, u1, v0, v1 = x[1:5]
    sel = Lm[v0:v1 + 1, u0:u1 + 1].reshape(-1, 3)
    print('Band Zeile 480 Seite 0 (Flaeche, u0, u1, v0, v1, Anteil rosa):', x, 'L %.1f C %.1f h %.0f' % (np.median(sel[:, 0]), np.median(np.hypot(sel[:, 1], sel[:, 2])), np.median(np.degrees(np.arctan2(sel[:, 2], sel[:, 1])))))
