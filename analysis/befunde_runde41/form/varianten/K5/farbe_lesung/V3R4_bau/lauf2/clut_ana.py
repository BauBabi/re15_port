import sys, os, json, math
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import ids as I
import schnell as SN
K5 = I.K5
sys.path.insert(0, K5 + '/generator')
import kf_g4 as KG
var = K5 + '/farbe/K5c_farbe_v3'
t = I.FE.tim_lesen(open(var + '/FORM.TIM', 'rb').read())
cl = t['cluts']
S = SN.szenen()
M = SN.M
for n in (1, 2):
    reg = M['reg%d' % n].astype(int)
    basis = np.unpackbits(M['basis%d' % n])[:480 * 640].reshape(480, 640).astype(bool)
    z = S[n]['z']
    print('Bild', n)
    for ri, rn in enumerate(SN.RR.REGIONEN):
        mm = basis & (reg == ri) & (z >= 0)
        if mm.sum() < 150: continue
        zz, c = np.unique(z[mm], return_counts=True)
        print('  %-12s %6d px  Zeilen %s' % (rn, mm.sum(), {int(a): int(b) for a, b in zip(zz, c)}))
# Eintraege je Zeile 0-5: Texel-gewichtet ueber beide Szenen
for zr in range(9):
    rgb = KG.wort_rgb(cl[zr]); L = KG.lab(rgb)
    C = np.hypot(L[:, 1], L[:, 2]); h = np.degrees(np.arctan2(L[:, 2], L[:, 1]))
    w = np.clip((C - KG.ROT_C0) / (KG.ROT_C1 - KG.ROT_C0), 0, 1) * ((h > KG.ROT_H0) & (h < KG.ROT_H1))
    cnt = np.zeros(256)
    for n in (1, 2):
        m = S[n]['z'] == zr
        np.add.at(cnt, S[n]['i'][m], 1)
    if cnt.sum() == 0:
        print('Zeile', zr, 'ungenutzt in Szenen'); continue
    g = cnt / cnt.sum()
    ader = w > 0
    def st(sel):
        if cnt[sel].sum() == 0: return 'leer'
        r = lambda x: np.repeat(x[sel], cnt[sel].astype(int))
        return 'L p10/50/90 %.1f/%.1f/%.1f  a50 %.1f C50 %.1f h50 %.1f' % (*np.percentile(r(L[:, 0]), [10, 50, 90]), np.median(r(L[:, 1])), np.median(r(C)), np.median(r(h)))
    print('Zeile %d: px %d; Ader-Anteil %.1f %%' % (zr, cnt.sum(), 100 * g[ader].sum()))
    print('   Haut', st(~ader))
    print('   Ader', st(ader))
