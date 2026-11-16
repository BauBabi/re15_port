"""Zacke + V-Kerbe: je Kerben-Stueck (wie szene.mass: im Kopf+Rumpf-Huellpolygon, beruehrt Kopf und Ruecken) die Mesh-8-Wand jenseits
des Stuecks (Kopf-abgewandt, 4 px breit um das Stueck): mittlere Helligkeit L (0..255) im echten Bild (plain-Lauf), Seite, Groesse.
python zacke2.py N F0 F1   (Klassen N_kl.npz, plain-Bilder laeufe/N/fd)"""
import sys, numpy as np
from scipy import ndimage
from skimage.morphology import convex_hull_image
import szene as SZ
S8 = np.ones((3, 3), bool)
def stuecke(k, plain):
    S = k >= 0; H = k >= 3; R = k == 2; m8 = (k == 1) | (k == 2)
    if not H.any(): return []
    dH = ndimage.distance_transform_cdt(~H, metric='chessboard'); Hn = dH <= SZ.NAH; Hd = dH <= 1
    hull = convex_hull_image(H | (m8 & Hn), offset_coordinates=False)
    lab, nn = ndimage.label(hull & ~S, S8)
    hx = np.nonzero(H)[1].mean()
    L = plain @ np.array([0.299, 0.587, 0.114]) if plain is not None else None
    out = []
    for i in range(1, nn + 1):
        c = lab == i; cd = ndimage.binary_dilation(c, S8)
        if not ((cd & Hd).any() and (cd & R).any()): continue
        ys, xs = np.nonzero(c); cx = xs.mean(); seite = 'r' if cx > hx else 'l'
        w = ndimage.binary_dilation(c, S8, iterations=4) & m8
        if seite == 'r': w[:, :int(np.ceil(cx))] = False
        else: w[:, int(cx):] = False
        lw = float(L[w].mean()) if (L is not None and w.any()) else -1
        ln = float(L[c].mean()) if L is not None else -1
        dd = ndimage.distance_transform_edt(~((~S) & ~c))[c]
        tf, mt = int((dd >= 1.5).sum()), round(float(dd.max()), 1)
        out.append(dict(px=int(c.sum()), seite=seite, L_wand=round(lw, 1), L_kerbe=round(ln, 1), ort=(int(cx), int(ys.min())), tief=tf,
                        maxtiefe=mt, ueber=not SZ.soll(dict(summe=int(c.sum()), tief=tf, maxtiefe=mt))))
    return out
if __name__ == '__main__':
    n, f0, f1 = sys.argv[1], int(sys.argv[2]), int(sys.argv[3])
    K = np.load(SZ.L_ + n + '_kl.npz')['K']
    for f in range(f0, f1 + 1):
        p = SZ.ppm(n, f)
        st = stuecke(SZ.klein(K[f - SZ.F0]), SZ.klein(p).astype(float) if p is not None else None)
        if st: print('F%d' % f, [(s['px'], s['tief'], s['maxtiefe'], s['seite'], s['L_wand'], 'UEBER' if s['ueber'] else '') for s in st if s['seite'] == 'r'])
