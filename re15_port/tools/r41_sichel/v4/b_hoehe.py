# Leser B: H (Oberkante Buckel bis Sohle) fuer Bild 1 (n1485) und Bild 2 (n1435). n 1-basiert (Formblatt).
# Oberkante: je Spalte erste Zeile, ab der 5 Zeilen in Folge R-G >= 8 gelten (Wesen roetlich, Balken R-G -0,8, Tuerrahmen 8,6 -> Grenzfall, daher
#   nur Spalten ueber dem Kopf/Buckel und Minimum); Sohle: Unterkante roter Fussbloecke (R-G>18, R-B>10.8, >=10 px) in Nachbarbildern,
#   weil die Sohle in Bild 1 im Schatten/Abblende und in Bild 2 hinter dem Vordergrund (gruen) liegt.
import sys, json, numpy as np
from scipy import ndimage as ndi
import b_common as C
S = sys.argv[1]
def oberkante(a, xs, y0, y1, thr=8, run=5):
    RG = a[..., 0] - a[..., 1]; out = {}
    for x in xs:
        col = RG[y0:y1, x] >= thr
        for y in range(len(col) - run):
            if col[y:y + run].all(): out[x] = y0 + y; break
    return out
def fuesse(a):
    R, G, B = a[..., 0], a[..., 1], a[..., 2]
    m = (R - G > 18) & (R - B > 10.8); m[:395] = False; m[458:] = False; m[:, :260] = False; m[:, 520:] = False
    lab, k = ndi.label(m); out = []
    for i in range(1, k + 1):
        ys, xs = np.nonzero(lab == i)
        if len(xs) >= 10: out.append(dict(x=int(xs.mean()), y0=int(ys.min()), y1=int(ys.max()), n=int(len(xs))))
    return out
res = {}
for nm, n, xs, (y0, y1), nb in (('bild1', 1485, range(370, 411), (110, 220), range(1475, 1478)), ('bild2', 1435, range(380, 421), (150, 260), range(1438, 1446))):
    a = C.bild(S, n); ok = oberkante(a, xs, y0, y1); xm = min(ok, key=ok.get)
    fu = {m: fuesse(C.bild(S, m)) for m in nb}
    sohlen = [b['y1'] for m in fu for b in fu[m]]
    so = int(np.median(sohlen))
    res[nm] = dict(n=n, oberkante_y=ok[xm], oberkante_x=xm, oberkante_profil={str(k): v for k, v in ok.items() if k % 5 == 0},
                   fussbloecke_nachbarbilder={str(m): v for m, v in fu.items()}, sohle_y_median=so, H_px=so - ok[xm])
    print(nm, 'Oberkante', ok[xm], '@x', xm, 'Sohle(Median Nachbarn)', so, 'H', so - ok[xm]); print('  Fuesse', fu)
# Gegenprobe Tiefe: Oberkante in den Sohlen-Nachbarbildern (gleiche Spalten)
for nm, n2 in (('bild1', 1475), ('bild1', 1476), ('bild2', 1441)):
    a = C.bild(S, n2); ok = oberkante(a, range(370, 421), 110, 260); xm = min(ok, key=ok.get)
    res[nm].setdefault('oberkante_nachbar', {})[str(n2)] = [ok[xm], xm]; print(nm, 'Oberkante Nachbar', n2, ok[xm], '@x', xm)
json.dump(res, open(f'{S}/B_H.json', 'w'), indent=1)
