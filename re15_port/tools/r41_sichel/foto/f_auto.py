"""Automatische Vor-Maske je Foto: dE76 gegen BSS 50A0/12 (Videoraum), Gauss 1,5, mehrere Schwellen; Umriss ueber Foto x2 mit Gitter."""
import os, sys, numpy as np, cv2
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'v4'))
import v4_gemein as G
from f_zoom import zoom
S = os.path.join(G.SFV4, 'foto')
bg = G.videoraum(G.hintergrund())
for b in (1, 2):
    v = G.vorlage(b)
    d = np.sqrt(((G.lab(v) - G.lab(bg)) ** 2).sum(2)).astype(np.float32)
    d = cv2.GaussianBlur(d, (0, 0), 1.5)
    np.save(os.path.join(S, 'de15_%d.npy' % b), d)
    for t in (5, 8):
        m = (d > t).astype(np.uint8)
        m = cv2.morphologyEx(m, cv2.MORPH_CLOSE, np.ones((5, 5), np.uint8))
        z = zoom(v, 260, 120, 520, 450, 3, False, maske=m > 0)
        cv2.imwrite(os.path.join(S, 'auto_%d_t%d.png' % (b, t)), z)
