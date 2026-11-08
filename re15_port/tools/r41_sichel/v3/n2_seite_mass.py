# SF V3 Runde 2: Kopf in der SEITENKAMERA (Spielkamera 50A0/12, Posen sfv2/posen_seite.json h83/h85/h92, RDT-Licht wie b2.zeichne).
# Je Bildpunkt Knochen und CLUT-Zeile (Nachbau von b2.rastern_ids): Mesh-9-Bildpunkte mit Zeile 485 = EM30-Wundfleisch
# ("lachsrosa Platte", Sicht-Pruefer A4), Zeile 486 = Kopf V3. Haut = Figur ohne Schwanz und ohne Kopf im Band OK..+0,45 H,
# b* >= -3, L* >= 5 (Messblatt 2). Kopf: L* oben/unten (Haelfte der Mesh-9-Zeilen), dunkler Fleck (L* < 0,5 x Kopf-Median,
# 8er-Komponenten >= 2 px), rote Bildpunkte (a* > 15), helle Stummel (L* > Kopf-Median + 15).
# Aufruf: python n2_seite_mass.py EMD [EMD ...]  -> JSON auf stdout und sfv3/n2/seite_mass_<name>.json, Bilder seite_k_<name>_h*.png
import os, sys, json
import numpy as np, cv2
from scipy import ndimage as nd
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER); sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'v2'))
import b2, birkin_buehne as BB
from P_farbe import lab, r1
SC = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad'
OUT = SC + '/sfv3/n2/'
PS = json.load(open(SC + '/sfv2/posen_seite.json'))


def zeichne_zeilen(mo, pose):
    r, kam, M, li = b2.raum()
    kf = BB.kf_von_clip(mo, pose['clip'], pose['bild'])
    lage = (pose['x'], pose.get('y', 0), pose['z'])
    ziel = np.full((480, 640, 3), 60, np.uint8)
    tris = b2.dreiecke_ids(mo, kf, lage, int(pose['gier']), kam, li)
    H, W = 480, 640
    ids = np.full((H, W), -1, np.int16); zl = np.full((H, W), -1, np.int16)
    for _t, p, uv, zeile, farbe, b in sorted(tris, key=lambda t: -t[0]):
        x, y = BB.abbilden(np.array([q[0] + BB.PHASE for q in p], float), np.array([q[1] + BB.PHASE for q in p], float), M)
        fl = (x[1] - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (y[1] - y[0])
        if abs(fl) < 1e-9:
            continue
        x0, x1 = max(int(np.floor(x.min())), 0), min(int(np.ceil(x.max())) + 1, W)
        y0, y1 = max(int(np.floor(y.min())), 0), min(int(np.ceil(y.max())) + 1, H)
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
        uu = np.clip((w0 * u[0] + w1 * u[1] + w2 * u[2]).astype(int), 0, mo.texw - 1)
        vv = np.clip((w0 * v[0] + w1 * v[1] + w2 * v[2]).astype(int), 0, mo.texh - 1)
        z = zeile - mo.clut_y
        rgba = mo.lut[z if 0 <= z < mo.lut.shape[0] else 0][mo.tex[vv, uu]]
        tref = innen & (rgba[..., 3] > 0)
        if not tref.any():
            continue
        c0, c1, c2 = (np.asarray(k, float) for k in farbe)
        farb = w0[..., None] * c0 + w1[..., None] * c1 + w2[..., None] * c2
        rgb = np.clip(rgba[..., :3].astype(float) * farb / 255.0, 0, 255).astype(np.uint8)
        s = ziel[y0:y1, x0:x1]; s[tref] = rgb[tref]
        ids[y0:y1, x0:x1][tref] = b; zl[y0:y1, x0:x1][tref] = zeile
    return ziel, ids, zl


def messen(emd, name):
    mo = b2.modell(emd)
    erg = {}
    for h in ('83', '85', '92'):
        im, ids, zl = zeichne_zeilen(mo, PS[h])
        A = lab(im.astype(float)); L = A[..., 0]
        body = (ids >= 0) & (ids <= 15)
        ys = np.nonzero(body.any(1))[0]; ok, so = int(ys.min()), int(ys.max()); Hs = so - ok + 1
        band = np.zeros_like(body); band[ok:int(ok + 0.45 * Hs) + 1] = True
        haut = nd.binary_erosion(body & (ids != 9) & band & (A[..., 2] >= -3) & (L >= 5), iterations=2)
        k9 = ids == 9
        n9 = int(k9.sum())
        e = dict(H=Hs, kopf_px=n9, wundfleisch_px=int((k9 & (zl == 485)).sum()), v3_px=int((k9 & (zl == 486)).sum()),
                 L_haut=round(float(L[haut].mean()), 1))
        if n9:
            yk = np.nonzero(k9.any(1))[0]; mitte = (yk.min() + yk.max()) / 2.0
            oben = k9.copy(); oben[int(mitte):] = False; unten = k9 & ~oben
            Lm = float(np.median(L[k9]))
            dk = k9 & (L < 0.5 * Lm)
            lb, n = nd.label(dk, structure=np.ones((3, 3)))
            fl = [int(x) for x in nd.sum(dk, lb, range(1, n + 1))] if n else []
            e.update(L_kopf=round(float(L[k9].mean()), 1), L_kopf_std=round(float(L[k9].std()), 1),
                     L_oben_haut=round(float(L[oben].mean()) / e['L_haut'], 3) if oben.any() else None,
                     L_unten_haut=round(float(L[unten].mean()) / e['L_haut'], 3) if unten.any() else None,
                     dunkel_flecken=[f for f in fl if f >= 2], rot_px=int((k9 & (A[..., 1] > 15)).sum()),
                     hell_px=int((k9 & (L > Lm + 15)).sum()), lab_kopf=r1(A[k9].mean(0)))
            ys9, xs9 = np.nonzero(k9)
            x0, x1, y0, y1 = xs9.min() - 6, xs9.max() + 7, ys9.min() - 6, ys9.max() + 7
            vis = cv2.resize(im[y0:y1, x0:x1], None, fx=8, fy=8, interpolation=cv2.INTER_NEAREST)
            cv2.imwrite(OUT + 'seite_k_%s_h%s.png' % (name, h), cv2.cvtColor(vis, cv2.COLOR_RGB2BGR))
        erg['h' + h] = e
    return erg


if __name__ == '__main__':
    alle = {}
    for p in sys.argv[1:]:
        name = os.path.basename(os.path.dirname(p)) + '_' + os.path.basename(p)[:-4]
        alle[name] = messen(p, name)
        print(name, json.dumps(alle[name]))
    json.dump(alle, open(OUT + 'seite_mass.json', 'w'), indent=1)
