"""Runde 8c: ruhige Original-Felder (Zeilen 480/481/482 = Asset 493/494/495, Original-Nutzung, Asset-Texel = Disc-Original), Vorhersage im Spiellicht
= Texel-MITTEL x (0,92, 0,86, 0,86): an den ruhigen Kandidaten K1 / K2 / K5 / K6 und dem heutigen Feld gemessen (Spielbild F265 / Texel-Mittel
0,89-0,95 / 0,85-0,89 / 0,84-0,93; x16_messen.py). Nur Auswahl, keine Verschiebung. Rang = Lab-Abstand zur Video-Mitte (98,5, 58,5, 59,5)."""
import sys, colorsys, numpy as np
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import v8_gemein as V
import kopf_orig_bau as KB, birkin_formen as BF, adern_r4 as A4
import v4_gemein as G
o = BF.lade_em30(); _d, _oo, _mso, timo = KB.teile(o); to = KB.tim_teile(timo)
A = open(r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD', 'rb').read()
_d, _o, _ms, tim = KB.teile(A); t = KB.tim_teile(tim)
fak = np.array([0.92, 0.86, 0.86]); ziel = G.lab(np.array([[[98.5, 58.5, 59.5]]]))[0, 0]
STREU = float(sys.argv[1]) if len(sys.argv) > 1 else 12.0
gew_alle = []
for z, seite in ((480, 0), (481, 1), (482, 2), (480, 1), (481, 0), (482, 0), (482, 1), (480, 2), (481, 2)):
    c = to['clut'][z - 480].astype(int); rgb = np.stack([(c & 31) * 255 / 31., ((c >> 5) & 31) * 255 / 31., ((c >> 10) & 31) * 255 / 31.], -1)
    nz = A4.nutzung(None, z)[:, 128 * seite:128 * seite + 128]
    if nz.sum() < 100:
        continue
    P = to['pix'][:, 128 * seite:128 * seite + 128]
    gl = (t['pix'][:, 128 * seite:128 * seite + 128] == P)
    Lt = G.lab(rgb[P].reshape(1, -1, 3))[0][:, 0].reshape(256, 128)
    best = []
    for h in (10, 12, 14, 16, 20):
        for w in (14, 16, 18, 20, 24):
            for v0 in range(0, 256 - h + 1, 2):
                for u0 in range(0, 128 - w + 1, 2):
                    if not (nz[v0:v0 + h, u0:u0 + w].all() and gl[v0:v0 + h, u0:u0 + w].all()):
                        continue
                    st = float(Lt[v0:v0 + h, u0:u0 + w].std())
                    if st > STREU:
                        continue
                    cc = rgb[P[v0:v0 + h, u0:u0 + w]].reshape(-1, 3)
                    pr = np.clip(cc.mean(0) * fak, 0, 255)
                    lab = G.lab(pr[None, None])[0, 0]; dE = float(np.linalg.norm(lab - ziel))
                    _h, s_, v_ = colorsys.rgb_to_hsv(*(pr / 255.0))
                    best.append((round(dE, 2), -w * h, z, seite, u0, u0 + w - 1, v0, v0 + h - 1, round(st, 1), [round(x) for x in pr], round(s_, 2), round(v_, 2)))
    best.sort()
    gew = []
    for b in best:
        if any(not (b[5] < g[4] or b[4] > g[5] or b[7] < g[6] or b[6] > g[7]) for g in gew):
            continue
        gew.append(b)
        if len(gew) >= 5:
            break
    gew_alle += gew
gew_alle.sort()
for b in gew_alle[:16]:
    print('dE %.2f n %d Zeile %d Seite %d u %d-%d v %d-%d L*-Streuung %.1f Vorhersage %s S %.2f V %.2f' % (b[0], -b[1], b[2], b[3], b[4], b[5], b[6], b[7], b[8], b[9], b[10], b[11]))
