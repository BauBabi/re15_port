"""Bauer V3 R4: Szenen-ID-Raster. Je Pixel (CLUT-Zeile, Index, Gouraud-Farbe) wie FB.rastern; jede CLUT ergibt dann 'sauber' exakt neu."""
import sys, os, json, math
import numpy as np, cv2
WT = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form'
K5 = WT + '/analysis/befunde_runde41/form/varianten/K5'
for p in (WT + '/re15_port/tools', K5 + '/farbe_lesung/P_skripte', K5 + '/generator', K5 + '/farbe_lesung/V3R4_skripte'):
    if p not in sys.path:
        sys.path.insert(0, p)
import p_zonen as PZ
import form_buehne as FB
import form_pruefung as P
import form_emd as FE

def rastern_ids(tris, tex, H=480, W=640, abbildung=None):
    zei = np.full((H, W), -1, np.int16); idx = np.zeros((H, W), np.int16); farb = np.zeros((H, W, 3), np.float64)
    for _t, p, uv, zeile, farbe in sorted(tris, key=lambda t: -t[0]):
        x, y = FB.abbilden(np.array([q[0] + FB.PHASE for q in p], float), np.array([q[1] + FB.PHASE for q in p], float), abbildung)
        fl = (x[1] - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (y[1] - y[0])
        if abs(fl) < 1e-9:
            continue
        x0 = max(int(np.floor(x.min())), 0); x1 = min(int(np.ceil(x.max())) + 1, W)
        y0 = max(int(np.floor(y.min())), 0); y1 = min(int(np.ceil(y.max())) + 1, H)
        if x0 >= x1 or y0 >= y1:
            continue
        PX, PY = np.meshgrid(np.arange(x0, x1) + 0.5, np.arange(y0, y1) + 0.5)
        w1 = ((PX - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (PY - y[0])) / fl
        w2 = ((x[1] - x[0]) * (PY - y[0]) - (PX - x[0]) * (y[1] - y[0])) / fl
        w0 = 1.0 - w1 - w2
        innen = (w0 >= -1e-9) & (w1 >= -1e-9) & (w2 >= -1e-9)
        if not innen.any():
            continue
        u = np.array([q[0] for q in uv], float); v = np.array([q[1] for q in uv], float)
        uu = np.clip((w0 * u[0] + w1 * u[1] + w2 * u[2]).astype(int), 0, tex.w - 1)
        vv = np.clip((w0 * v[0] + w1 * v[1] + w2 * v[2]).astype(int), 0, tex.h - 1)
        z = zeile - tex.clut_y
        z = z if 0 <= z < tex.lut.shape[0] else 0
        ii = tex.px[vv, uu]
        rgba = tex.lut[z][ii]
        tref = innen & (rgba[..., 3] > 0)
        if not tref.any():
            continue
        c0, c1, c2 = (np.asarray(k, float) for k in farbe)
        f = w0[..., None] * c0 + w1[..., None] * c1 + w2[..., None] * c2
        s = zei[y0:y1, x0:x1]; s[tref] = z
        s = idx[y0:y1, x0:x1]; s[tref] = ii[tref]
        s = farb[y0:y1, x0:x1]; s[tref] = f[tref]
    return zei, idx, farb

def szene(kand, n):
    lage, gier, clip, bild = kand.stand(n)
    hg = P.lade_rgb(P.daten('hintergrund_bild%d.png' % n))
    kam = P.kamera(n)
    kf = kand.kf(clip, bild)
    licht, k = P.licht_fuer(n, kand.licht)
    z, i, f = rastern_ids(FB.dreiecke(kand.mo, kf, lage, gier, kam, licht), kand.tex, abbildung=kam.abbildung)
    za, ia, fa = rastern_ids(FB.dreiecke(kand.mo, kf, lage, gier, kam, None), kand.tex, abbildung=kam.abbildung)
    return dict(hg=hg, z=z, i=i, f=f, za=za, ia=ia, fa=fa)

def lut_aus(cluts):
    c = np.array(cluts, dtype=np.int64)
    return np.stack([(c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3, (c != 0) * 255], axis=-1).astype(np.uint8)

def sauber_aus(S, lut):
    out = S['hg'].copy()
    m = S['z'] >= 0
    rgb = lut[S['z'][m], S['i'][m], :3].astype(float)
    out[m] = np.clip(rgb * S['f'][m] / 255.0, 0, 255).astype(np.uint8)
    alb = np.zeros_like(out)
    ma = S['za'] >= 0
    alb[ma] = np.clip(lut[S['za'][ma], S['ia'][ma], :3].astype(float) * S['fa'][ma] / 255.0, 0, 255).astype(np.uint8)
    return out, alb, m

if __name__ == '__main__':
    var = sys.argv[1]; aus = sys.argv[2]
    k = P.Kandidat(os.path.join(var, 'form.json'))
    t = FE.tim_lesen(k.tim)
    lut = lut_aus(t['cluts'])
    for n in (1, 2):
        S = szene(k, n)
        s1, a1, m1, _ = k.rendern(n)
        s2, a2, m2 = sauber_aus(S, lut)
        print(n, 'sauber gleich', np.array_equal(s1, s2), 'alb gleich', np.array_equal(a1, a2), 'diff px', int((s1 != s2).any(2).sum()))
        np.savez_compressed(os.path.join(aus, 'ids%d.npz' % n), **{kk: v for kk, v in S.items()})
