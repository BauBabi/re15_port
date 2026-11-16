"""Zacke + V-Kerbe neben dem Kopf (E9: in jeder Ansicht verboten in >= 2 Bildern in Folge). Aus Klassenkarten (szene.py klassen, 640x480):
je Kerben-Stueck (beruehrt Kopf und Ruecken, wie szene.mass) die Rueckenspitze daneben: hoechster Ruecken-Bildpunkt (Klasse 2) in den
Spalten des Stuecks +- 3 px auf der Kopf-abgewandten Seite; Zacke = dieser Punkt liegt hoeher als die Oberkante des Stuecks (V geschlossen
von Kopf und Spitze).  python zacke.py N [F0 F1]"""
import sys, numpy as np
from scipy import ndimage
from skimage.morphology import convex_hull_image
import szene as SZ
n = sys.argv[1]; f0, f1 = (int(sys.argv[2]), int(sys.argv[3])) if len(sys.argv) > 3 else (235, 378)
K = np.load(SZ.L_ + n + '_kl.npz')['K']
S8 = np.ones((3, 3), bool)
for f in range(f0, f1 + 1):
    k = SZ.klein(K[f - SZ.F0]); S = k >= 0; H = k >= 3; R = k == 2; m8 = (k == 1) | (k == 2)
    if not H.any(): continue
    dH = ndimage.distance_transform_cdt(~H, metric='chessboard'); Hn = dH <= SZ.NAH; Hd = dH <= 1
    hull = convex_hull_image(H | (m8 & Hn), offset_coordinates=False)
    lab, nn = ndimage.label(hull & ~S, S8)
    hx = np.nonzero(H)[1].mean()
    out = []
    for i in range(1, nn + 1):
        c = lab == i; cd = ndimage.binary_dilation(c, S8)
        if not ((cd & Hd).any() and (cd & R).any()): continue
        ys, xs = np.nonzero(c); top = ys.min(); cx = xs.mean()
        seite = 'r' if cx > hx else 'l'
        band = np.zeros_like(c); x0, x1 = xs.min() - 3, xs.max() + 4
        band[:, max(0, x0):x1] = True
        # Kopf-abgewandte Seite: Ruecken-Bildpunkte in den Spalten jenseits der Stueck-Mitte
        if seite == 'r': band[:, :int(cx)] = False
        else: band[:, int(cx) + 1:] = False
        rr = np.nonzero(R & band)[0]
        spitze = (top - rr.min()) if rr.size else -99
        out.append((int(c.sum()), seite, int(spitze), (int(cx), int(top))))
    if out:
        print('F%d' % f, out)
