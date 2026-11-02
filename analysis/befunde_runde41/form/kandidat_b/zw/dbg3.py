import sys, json
sys.path.insert(0, '.')
from b_geom import *
import b_teile as T
import numpy as np, cv2
sys.path.insert(0, BAUM + '/re15_port/tools')
import form_pruefung as P
import form_buehne as FB
k = P.Kandidat(sys.argv[1])
n = int(sys.argv[2])
x0, y0, x1, y1 = [int(v) for v in sys.argv[3:7]]
kam = P.kamera(n)
lage, gier, clip, bild = k.stand(n)
kf = k.kf(clip, bild)
for b in range(len(T.KNOCHEN)):
    a = np.zeros((480, 640, 3), np.uint8)
    m = FB.rastern(FB.dreiecke(k.mo, kf, lage, gier, kam, None, teile={b}), k.tex, a, kam.abbildung)
    c = m[y0:y1 + 1, x0:x1 + 1].sum()
    if c:
        print(b, T.KNOCHEN[b][0], c)
