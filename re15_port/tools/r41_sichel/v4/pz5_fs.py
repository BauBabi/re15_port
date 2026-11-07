"""Zahlen-Pruefer V4 r4: FS-Zeilen (Sicht-Boxen) mit eigenem Code - Unterarm (W)-rechts Id 14 (O5) und Oberarm (B)-rechts unten Id 10,
Original = Vorlage 1 entblendet in der Box, Port = Id (2 px erodiert, beim Oberarm untere 44 % der Zeilen) im Videoraum, Pose 135;
dazu V4 r2 / V3 r2 als Gegenprobe."""
import sys, os, numpy as np
from scipy import ndimage as nd
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from pz5_mess import vraum, lab, de, orig, SP
O = orig(1); LO = lab(O)
def box(b): m = np.zeros((480, 640), bool); m[b[1]:b[3] + 1, b[0]:b[2] + 1] = True; return m
def unten(m, f0):
    ys = np.nonzero(m.any(1))[0]; y0, y1 = ys.min(), ys.max() + 1; b = np.zeros_like(m); b[int(round(y0 + f0 * (y1 - y0))):y1] = True; return m & b
def stat(V, L, m):
    l = lab(V[m].mean(0)); rot = float(((V[m][:, 0] - V[m][:, 1]) >= 15).mean()); return l, rot, float((L[m][:, 0] > 30).mean()), float(np.percentile(L[m][:, 0], 90)), int(m.sum())
for nm_r, b, idm in (('Unterarm (W)-rechts Id 14', (298, 262, 325, 340), lambda ids: nd.binary_erosion(ids == 14, iterations=2)),
                     ('Oberarm (B)-rechts unten Id 10', (455, 228, 500, 262), lambda ids: unten(nd.binary_erosion(ids == 10, iterations=2), 0.56)),
                     ('Oberarm (B)-links unten Id 13', (300, 213, 345, 250), lambda ids: unten(nd.binary_erosion(ids == 13, iterations=2), 0.58))):
    lo, ro, ho, po, no = stat(O, LO, box(b))
    print('%s: Original %s Rot %.2f L*>30 %.3f p90 %.1f (%d px)' % (nm_r, np.round(lo, 1), ro, ho, po, no))
    for nm in ('V4', 'V4R4', 'V3'):
        d = np.load(SP + 'r_%s_135.npz' % nm); V = vraum(d['img']); L = lab(V)
        l, r, h, p, n = stat(V, L, idm(d['ids']))
        print('   %-5s %s dE00 %.2f dL %+.1f Rot %.2f L*>30 %.3f p90 %.1f (%d px)' % (nm, np.round(l, 1), de(l, lo), l[0] - lo[0], r, h, p, n))
