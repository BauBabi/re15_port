"""Suche (Messung, keine Eichung): Rechtecke in der Original-Nutzung der Original-Zeilen 480/481/482 (Asset 493/494/495), deren Median-Ton dem
Bauch der Probe a nahe ist (Textur: L* 26,8, C* 20,3, Farbton ~24) und die gross sind; dazu Fleckung (L*-Streuung im Feld). Und fuer die Baender:
hellrot/rosa Felder (L* 40-56, C* 12-28, h 5-35), groesste Rechtecke mit >= 70 % solcher Texel."""
import os, sys
import numpy as np
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
sys.path.insert(0, W); sys.dont_write_bytecode = True
import v8_gemein as V, adern_r4 as A4, kopf_orig_bau as KB, birkin_formen as BF, v4_gemein as G
import schwanz_r8 as Q
o = BF.lade_em30(); to = KB.tim_teile(KB.teile(o)[3])
def lab_zeile(z):
    c = to['clut'][z - 480].astype(int)
    rgb = np.stack([(c & 31) * 255 / 31., ((c >> 5) & 31) * 255 / 31., ((c >> 10) & 31) * 255 / 31.], -1)
    return G.lab(rgb[None])[0]
def alle_rechtecke(mask, min_b=6, min_h=6):
    """alle maximalen Rechtecke (grob): fuer jede Hoehe h das breiteste Rechteck je Zeilenende - Kandidatenliste."""
    H, W_ = mask.shape
    hoch = np.zeros(W_, int); out = []
    for v in range(H):
        hoch = np.where(mask[v], hoch + 1, 0)
        st = []
        for u in range(W_ + 1):
            h = hoch[u] if u < W_ else 0
            anf = u
            while st and st[-1][1] >= h:
                anf, hh = st.pop()
                if hh >= min_h and u - anf >= min_b:
                    out.append((anf, u - 1, v - hh + 1, v))
            st.append((anf, h))
    return out
for z in (482, 481, 480):
    lab = lab_zeile(z)
    nutz = A4.nutzung(None, z)
    Lm = lab[to['pix']]
    C = np.hypot(Lm[..., 1], Lm[..., 2]); h = np.degrees(np.arctan2(Lm[..., 2], Lm[..., 1]))
    best = []
    for (u0, u1, v0, v1) in alle_rechtecke(nutz):
        sel = (slice(v0, v1 + 1), slice(u0, u1 + 1))
        Lmed, Cmed, hmed = float(np.median(Lm[sel][..., 0])), float(np.median(C[sel])), float(np.median(h[sel]))
        if 22 <= Lmed <= 32 and 15 <= Cmed <= 24 and 12 <= hmed <= 36:
            best.append(((u1 - u0 + 1) * (v1 - v0 + 1), u0, u1, v0, v1, Lmed, Cmed, hmed, float(np.std(Lm[sel][..., 0]))))
    best.sort(reverse=True)
    print('Zeile', z, 'Bauch-Kandidaten (Flaeche, Seite, u, v, L, C, h, L-Streuung):')
    for b in best[:6]:
        print('   %4d  Seite %d u %d-%d v %d-%d  L %.1f C %.1f h %.0f  sL %.1f' % (b[0], b[1] // 128, b[1] % 128, b[2] % 128, b[3], b[4], b[5], b[6], b[7], b[8]))
    pink = nutz & (Lm[..., 0] >= 40) & (Lm[..., 0] <= 56) & (C >= 12) & (C <= 28) & (h >= 5) & (h <= 35)
    from scipy import ndimage as ndi
    best = []
    for (u0, u1, v0, v1) in alle_rechtecke(nutz, 3, 3):
        sel = (slice(v0, v1 + 1), slice(u0, u1 + 1))
        a = float(pink[sel].mean())
        if a >= 0.7:
            best.append(((u1 - u0 + 1) * (v1 - v0 + 1), u0, u1, v0, v1, a, float(np.median(Lm[sel][..., 0])), float(np.median(C[sel])), float(np.median(h[sel]))))
    best.sort(reverse=True)
    print('Zeile', z, 'Band-Kandidaten hellrot/rosa (Flaeche, Seite, u, v, Anteil, L, C, h):')
    for b in best[:6]:
        print('   %4d  Seite %d u %d-%d v %d-%d  %.2f  L %.1f C %.1f h %.0f' % (b[0], b[1] // 128, b[1] % 128, b[2] % 128, b[3], b[4], b[5], b[6], b[7], b[8]))
