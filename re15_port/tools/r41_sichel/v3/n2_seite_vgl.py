# SF V3 Runde 2: Kopf seitlich im Spielmassstab - KI birkin12 (1280er Freisteller, H 504) auf 640 verkleinert (INTER_AREA, H 252)
# neben Port-Spielkamera h85 (H 265), Ausschnitt 70 x 60 px um den Kopf, x8 NEAREST. Aufruf: python n2_seite_vgl.py aus.png EMD ...
import os, sys, json
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER); sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'v2'))
import b2
import n2_seite_mass as SM
aus = sys.argv[1]
k = cv2.imread('C:/workspace/git/reAi_v2/Version_80%/birkin12.png', cv2.IMREAD_UNCHANGED)
rgb = k[..., 2::-1].astype(float); a = k[..., 3:4].astype(float) / 255.0
k = (rgb * a + 60 * (1 - a)).astype(np.uint8)
k = cv2.resize(k, None, fx=0.5, fy=0.5, interpolation=cv2.INTER_AREA)
# Schnauzenspitze (388,112) -> (194,56): Ausschnitt rechts buendig
kc = k[56 - 35:56 + 25, 194 - 60:194 + 10]
spalten = [cv2.resize(kc, None, fx=8, fy=8, interpolation=cv2.INTER_NEAREST)]
for p in sys.argv[2:]:
    mo = b2.modell(p)
    im, ids, zl = SM.zeichne_zeilen(mo, SM.PS['85'])
    ys, xs = np.nonzero(ids == 9)
    sx, sy = xs.max(), int(np.median(ys[xs >= xs.max() - 2]))
    c = im[sy - 35:sy + 25, sx - 60:sx + 10]
    spalten.append(cv2.resize(c, None, fx=8, fy=8, interpolation=cv2.INTER_NEAREST))
cv2.imwrite(aus, cv2.cvtColor(np.hstack(spalten), cv2.COLOR_RGB2BGR))
