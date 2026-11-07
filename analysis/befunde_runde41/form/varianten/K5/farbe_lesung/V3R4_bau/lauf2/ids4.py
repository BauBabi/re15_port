"""Bauer V3 R4: ID-Raster mit Flaechencode. Wie ids.py, zusaetzlich je Pixel der Flaechenschluessel (Mesh, t/q, Nr.), damit eine neue
Zeilenzuweisung je Flaeche ohne Neurendern ausgewertet werden kann. dreiecke() ist eine Textkopie von form_buehne.dreiecke mit
Flaechencode in der Zeile (Zeile + code * 1024); das Werkzeug selbst bleibt unveraendert."""
import sys, os, inspect
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import ids as I
FB = I.FB
_src = inspect.getsource(FB.dreiecke)
_src = _src.replace('def dreiecke(', 'def dreiecke_k(')
_src = _src.replace('for g, uv in zip(faces, uvs):', 'for fi_, (g, uv) in enumerate(zip(faces, uvs)):')
_src = _src.replace('zeile = (uv[2] >> 6) & 0x1FF', 'zeile = ((uv[2] >> 6) & 0x1FF) + (((b * 2 + viereck) * 4096 + fi_) + 1) * 1024')
assert _src.count('fi_') == 2
_ns = dict(FB.__dict__)
exec(compile(_src, 'dreiecke_k', 'exec'), _ns)
dreiecke_k = _ns['dreiecke_k']

def code_key(c):
    c = int(c) - 1
    f = c % 4096; bv = c // 4096
    return (bv // 2, 'q' if bv % 2 else 't', f)

def rastern_ids(tris, tex, H=480, W=640, abbildung=None):
    zei = np.full((H, W), -1, np.int16); idx = np.zeros((H, W), np.int16); farb = np.zeros((H, W, 3), np.float64)
    fc = np.zeros((H, W), np.int32)
    for _t, p, uv, zeile_c, farbe in sorted(tris, key=lambda t: -t[0]):
        zeile, code = zeile_c % 1024, zeile_c // 1024
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
        s = fc[y0:y1, x0:x1]; s[tref] = code
    return zei, idx, farb, fc

def szene(kand, n):
    lage, gier, clip, bild = kand.stand(n)
    hg = I.P.lade_rgb(I.P.daten('hintergrund_bild%d.png' % n))
    kam = I.P.kamera(n)
    kf = kand.kf(clip, bild)
    licht, k = I.P.licht_fuer(n, kand.licht)
    z, i, f, c = rastern_ids(dreiecke_k(kand.mo, kf, lage, gier, kam, licht), kand.tex, abbildung=kam.abbildung)
    za, ia, fa, ca = rastern_ids(dreiecke_k(kand.mo, kf, lage, gier, kam, None), kand.tex, abbildung=kam.abbildung)
    return dict(hg=hg, z=z, i=i, f=f, c=c, za=za, ia=ia, fa=fa, ca=ca)

if __name__ == '__main__':
    var = sys.argv[1]; aus = sys.argv[2]
    k = I.P.Kandidat(os.path.join(var, 'form.json'))
    t = I.FE.tim_lesen(k.tim)
    lut = I.lut_aus(t['cluts'])
    for n in (1, 2):
        S = szene(k, n)
        s1, a1, m1, _ = k.rendern(n)
        s2, a2, m2 = I.sauber_aus(S, lut)
        print(n, 'sauber gleich', np.array_equal(s1, s2), 'alb gleich', np.array_equal(a1, a2), 'flaechen', len(np.unique(S['c'][S['z'] >= 0])))
        np.savez_compressed(os.path.join(aus, 'fid%d.npz' % n), **S)
