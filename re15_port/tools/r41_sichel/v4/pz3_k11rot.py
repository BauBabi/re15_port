"""Zahlen-Pruefer V4 r3: O4 'rot-fleckige Oberflaeche' - Rotanteil (R-G >= 15) und Streuung von a* (nach Abzug Gauss sigma 3) in den
Anteils-Fenstern der Kopfbox, Original (Vorlage 1 entblendet) gegen Port (Videoraum, auf Originalmassstab skaliert, nur Id 9)."""
import sys, os, json, numpy as np, cv2
from scipy import ndimage as nd
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from pz3_mess import vraum, lab, orig, SP
from pz3_k11 import FEN, fenster, boxm, KB
def mass(V, m):
    A = lab(V)[..., 1]; d = A - cv2.GaussianBlur(A.astype(np.float32), (0, 0), 3)
    return round(float(((V[..., 0] - V[..., 1])[m] >= 15).mean()), 2), round(float(d[m].std()), 2), round(float(A[m].mean()), 1)
O = orig(1)
print('Original', {k: mass(O, boxm(fenster(KB, f))) for k, f in FEN.items()})
M = json.load(open(SP + 'mess_V4_V4R2_V3.json'))
for nm in ('V4', 'V4R2'):
    d = np.load(SP + 'r_%s_135.npz' % nm); f = 302.0 / M[nm]['vorn']['H']
    V = cv2.resize(vraum(d['img']).astype(np.float32), None, fx=f, fy=f, interpolation=cv2.INTER_AREA)
    ids = cv2.resize(d['ids'].astype(np.uint8), None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST)
    ys, xs = np.nonzero(ids == 9); kb = (xs.min(), ys.min(), xs.max(), ys.max()); k9 = nd.binary_erosion(ids == 9)
    print(nm, {k: mass(V, boxm(fenster(kb, fr), ids.shape) & k9) for k, fr in FEN.items()})
