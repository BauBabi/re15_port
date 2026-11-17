"""Runde 8c: groesstes ruhiges Bauchfeld in Original-Zeile 481 (Asset 494), Seite 1, in Original-Nutzung, Asset-Texel = Disc-Original;
Vorhersage Spielbild = Texel-Median x Lichtfaktor (x12_vorrat.py: 0,95 / 0,831 / 0,81). Ziel Video-Mitte RGB (98,5, 58,5, 59,5)."""
import sys, numpy as np
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import v8_gemein as V
import kopf_orig_bau as KB, birkin_formen as BF, adern_r4 as A4
import v4_gemein as G
o = BF.lade_em30(); _d, _oo, _mso, timo = KB.teile(o); to = KB.tim_teile(timo)
A = open(r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD', 'rb').read()
_d, _o, _ms, tim = KB.teile(A); t = KB.tim_teile(tim)
fak = np.array([0.95, 0.831, 0.81]); ziel = G.lab(np.array([[[98.5, 58.5, 59.5]]]))[0, 0]
c = to['clut'][1].astype(int); rgb = np.stack([(c & 31) * 255 / 31., ((c >> 5) & 31) * 255 / 31., ((c >> 10) & 31) * 255 / 31.], -1)
nz = A4.nutzung(None, 481)[:, 128:256]; P = to['pix'][:, 128:256]; gl = (t['pix'][:, 128:256] == to['pix'][:, 128:256])
best = []
for h in range(8, 33, 2):
    for w in range(12, 41, 2):
        for v0 in range(0, 72 - h + 1):
            for u0 in range(0, 128 - w + 1):
                if not (nz[v0:v0 + h, u0:u0 + w].all() and gl[v0:v0 + h, u0:u0 + w].all()):
                    continue
                cc = rgb[P[v0:v0 + h, u0:u0 + w]].reshape(-1, 3); med = np.median(cc, 0)
                lab = G.lab(np.clip(med * fak, 0, 255)[None, None])[0, 0]; dE = float(np.linalg.norm(lab - ziel))
                st = float(G.lab(cc[None])[0][:, 0].std())
                if dE <= 2.5 and st <= 12.5:
                    best.append((w * h, u0, u0 + w - 1, v0, v0 + h - 1, round(dE, 2), round(st, 1), [round(x) for x in med * fak]))
best.sort(reverse=True)
for b in best[:12]:
    print(b)
Lm = G.lab(np.clip(rgb[P] * fak, 0, 255).reshape(1, -1, 3))[0][:, 0].reshape(256, 128)
print('Karte vorhergesagtes Spiel-L* Zeile 481 Seite 1 (je 2x2), u 20-79, v 0-47; . = nicht in Nutzung, * = Asset-Texel != Original')
for v in range(0, 48, 2):
    print('v%3d ' % v + ''.join((('%2d' % int(Lm[v:v + 2, u:u + 2].mean())) if nz[v:v + 2, u:u + 2].all() else ' .') + ('*' if not gl[v:v + 2, u:u + 2].all() else ' ') for u in range(20, 80, 2)))
