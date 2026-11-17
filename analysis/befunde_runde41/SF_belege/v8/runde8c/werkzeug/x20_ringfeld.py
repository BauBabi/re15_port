"""Runde 8c: Ring-Feld (weicher Verlauf) nur aus Original-Texeln: Rechtecke in der Original-Nutzung (Zeilen 480/481/482, Asset-Texel = Original),
Profil der vorhergesagten Spiel-Helligkeit (Texel x (0,92, 0,86, 0,86), gemessen x16/x19) entlang der Ring-Richtung (v+ / v- / u+ / u-).
Gesucht: helles Ende L* 42-52 (Video helle Stellen 41-53), dunkles Ende L* 27-33 (Bauch C1 im Spiel L* ~31), dazwischen fallend, KEIN Texel
unter L* 18 (Runde-8c-Probe R1: Ader mit L* 4 am dunklen Ende = dunkle Flecken an den Ringraendern, Messbild mb_ring_nah)."""
import sys, json
import numpy as np
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import v8_gemein as V
import kopf_orig_bau as KB, birkin_formen as BF, adern_r4 as A4
import v4_gemein as G
o = BF.lade_em30(); _d, _oo, _mso, timo = KB.teile(o); to = KB.tim_teile(timo)
A = open(r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD', 'rb').read()
_d, _o, _ms, tim = KB.teile(A); t = KB.tim_teile(tim)
fak = np.array([0.92, 0.86, 0.86])
erg = []
for z in (480, 481, 482):
    c = to['clut'][z - 480].astype(int); rgb = np.stack([(c & 31) * 255 / 31., ((c >> 5) & 31) * 255 / 31., ((c >> 10) & 31) * 255 / 31.], -1)
    nutz = A4.nutzung(None, z)
    for seite in range(3):
        nz = nutz[:, 128 * seite:128 * seite + 128]
        if nz.sum() < 100:
            continue
        P = to['pix'][:, 128 * seite:128 * seite + 128]
        gl = t['pix'][:, 128 * seite:128 * seite + 128] == P
        lab = G.lab((rgb[P] * fak).reshape(1, -1, 3))[0].reshape(256, 128, 3)
        for w in (8, 10, 12, 16):
            for h in (8, 10, 12, 14, 16):
                for v0 in range(0, 256 - h + 1):
                    for u0 in range(0, 128 - w + 1):
                        if not (nz[v0:v0 + h, u0:u0 + w].all() and gl[v0:v0 + h, u0:u0 + w].all()):
                            continue
                        blk = lab[v0:v0 + h, u0:u0 + w]
                        if blk[..., 0].min() < 18:
                            continue
                        for richt in ('v+', 'v-', 'u+', 'u-'):
                            b2 = blk if richt[0] == 'v' else blk.transpose(1, 0, 2)
                            if richt[1] == '-':
                                b2 = b2[::-1]
                            pr = np.median(b2[..., 0], axis=1)
                            k = max(2, len(pr) // 5)
                            anf, end = float(pr[:k].mean()), float(pr[-k:].mean())
                            if not (42 <= anf <= 52 and 27 <= end <= 33):
                                continue
                            gl3 = np.convolve(pr, np.ones(3) / 3, 'valid')
                            steig = float(np.diff(gl3).max())          # Anstieg gegen den Verlauf
                            ca = b2[:k]
                            C_anf = float(np.median(np.hypot(ca[..., 1], ca[..., 2]))); h_anf = float(np.median(np.degrees(np.arctan2(ca[..., 2], ca[..., 1]))))
                            sc = abs(anf - 47) + abs(end - 30) + 0.3 * abs(C_anf - 21) + 0.1 * abs(h_anf - 20) + 1.0 * max(0.0, steig - 2)
                            erg.append(dict(zeile=z, seite=seite, u=[u0, u0 + w - 1], v=[v0, v0 + h - 1], richt=richt, anf=round(anf, 1), ende=round(end, 1),
                                            C_anf=round(C_anf, 1), h_anf=round(h_anf), steig=round(steig, 1), min_L=round(float(blk[..., 0].min()), 1),
                                            score=round(sc, 2), profil=[round(float(x)) for x in pr]))
erg.sort(key=lambda e: e['score'])
gew = []
for e in erg:
    if any(g_['zeile'] == e['zeile'] and g_['seite'] == e['seite'] and not (e['u'][1] < g_['u'][0] or e['u'][0] > g_['u'][1] or e['v'][1] < g_['v'][0] or e['v'][0] > g_['v'][1]) for g_ in gew):
        continue
    gew.append(e)
    if len(gew) >= 12:
        break
for e in gew:
    print(e)
json.dump(gew, open(sys.argv[1], 'w'), indent=1)
