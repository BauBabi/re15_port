"""Runde 8c: Felder in Original-Zeile 480 (Asset 493), Seite 0, in Original-Nutzung, Asset-Texel = Disc-Original, sortiert nach der Vorhersage im
Spiellicht mit dem am Kandidaten K4 GEMESSENEN Lichtfaktor (Spielbild-Median / Texel-Median, x16_messen.py) - nur Auswahl, keine Verschiebung."""
import sys, colorsys, numpy as np
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import v8_gemein as V
import kopf_orig_bau as KB, birkin_formen as BF, adern_r4 as A4
import v4_gemein as G
o = BF.lade_em30(); _d, _oo, _mso, timo = KB.teile(o); to = KB.tim_teile(timo)
A = open(r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD', 'rb').read()
_d, _o, _ms, tim = KB.teile(A); t = KB.tim_teile(tim)
z = int(sys.argv[1]); seite = int(sys.argv[2])
c = to['clut'][z - 480].astype(int); rgb = np.stack([(c & 31) * 255 / 31., ((c >> 5) & 31) * 255 / 31., ((c >> 10) & 31) * 255 / 31.], -1)
nz = A4.nutzung(None, z)[:, 128 * seite:128 * seite + 128]; P = to['pix'][:, 128 * seite:128 * seite + 128]
gl = (t['pix'][:, 128 * seite:128 * seite + 128] == to['pix'][:, 128 * seite:128 * seite + 128])
k4 = rgb[P[46:66, 26:58]].reshape(-1, 3)
fak = np.array([86.0, 56.0, 54.0]) / np.median(k4, 0)
ziel = G.lab(np.array([[[98.5, 58.5, 59.5]]]))[0, 0]
print('Lichtfaktor aus K4 (Spielbild (86, 56, 54) / Texel %s) = %s' % (np.round(np.median(k4, 0), 1), np.round(fak, 3)))
best = []
for h in (8, 10, 12, 14, 16, 20, 24):
    for w in (12, 16, 20, 24, 28, 32):
        for v0 in range(0, 256 - h + 1, 2):
            for u0 in range(0, 128 - w + 1, 2):
                if not (nz[v0:v0 + h, u0:u0 + w].all() and gl[v0:v0 + h, u0:u0 + w].all()):
                    continue
                cc = rgb[P[v0:v0 + h, u0:u0 + w]].reshape(-1, 3); med = np.median(cc, 0)
                pr = np.clip(med * fak, 0, 255)
                lab = G.lab(pr[None, None])[0, 0]; dE = float(np.linalg.norm(lab - ziel))
                st = float(G.lab(cc[None])[0][:, 0].std())
                _h, s_, v_ = colorsys.rgb_to_hsv(*(pr / 255.0))
                if st <= 22.5 and w * h >= 160:
                    best.append((round(dE, 2), -w * h, u0, u0 + w - 1, v0, v0 + h - 1, round(st, 1), [round(x) for x in pr], round(s_, 2), round(v_, 2)))
best.sort()
gew = []
for b in best:
    if any(not (b[3] < g[2] or b[2] > g[3] or b[5] < g[4] or b[4] > g[5]) for g in gew):
        continue
    gew.append(b)
    if len(gew) >= 12:
        break
for b in gew:
    print('dE %.2f n %d u %d-%d v %d-%d streu %.1f Vorhersage %s S %.2f V %.2f' % (b[0], -b[1], b[2], b[3], b[4], b[5], b[6], b[7], b[8], b[9]))
