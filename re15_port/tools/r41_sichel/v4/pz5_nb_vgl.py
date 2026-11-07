"""Zahlen-Pruefer V4 r5: Variante mit nachgebauter Schwanztextur (r5_schwanz_textur.py aus HEAD) gegen die gemeldete Variante - TN/TR/TQ/T8-Kern."""
import os, sys, numpy as np, cv2
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from scipy import ndimage as nd
import v4_gemein as G, b2
from pz5_mess import vraum, lab, SP, de, EA_, EB_
yy, xx = np.mgrid[:480, :640]; BOX = (xx >= 284) & (xx <= 356) & (yy < 262)
def tn(V, m):
    L = lab(V); a = []
    for x in range(280, 377):
        ys = np.nonzero(m[:262, x])[0]
        if len(ys) < 5: continue
        y = int((ys.min() + ys.max()) / 2); a.append((x, L[y - 1:y + 2, x - 1:x + 2].reshape(-1, 3).mean(0)))
    c = np.array([q[1] for q in a]); d = np.linalg.norm(c[3:] - c[:-3], axis=1); return round(float(d.max()), 1), a[int(np.argmax(d)) + 1][0]
def tr(V, m):
    me = cv2.erode(m.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool) & BOX; return round(float((lab(V[me])[:, 1] > 12).mean()), 3)
P = G.posen()['85']
for nm, pf in (('gemeldet a762bd66', SP + 'emd/EM30_V4.EMD'), ('Nachbau ad293f22', SP + 'emd_nb/EM30_V4.EMD')):
    img, ids = G.zeichne(b2.modell(pf), P); sw = (ids >= 16) & (ids <= 22); V = vraum(img)
    q = []
    for x in (310, 320, 330, 340):
        ys = np.nonzero(sw[:262, x])[0]; p = lab(V)[ys.min():ys.max() + 1, x, 0]; q.append(round(float((np.abs(np.diff(p)) < 0.3).mean()), 2))
    L = lab(V)[sw]
    print(nm, 'TN', tn(V, sw), 'TR', tr(V, sw), 'TQ Plateau', q, 'Schwanz Mittel Lab', np.round(L.mean(0), 1))
