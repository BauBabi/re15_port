"""Bildfolge um die Vorlagen: Ausschnitt x0..x1,y0..y1 je Videobild, x2, nebeneinander."""
import os, sys, numpy as np, cv2
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'v4'))
import v4_gemein as G
V = os.path.join(G.BAUM, 'analysis', 'befunde_runde41', 'SF_belege', 'roh', 'alle', 'f%04d.png')
ns = [int(a) for a in sys.argv[1].split(',')]; x0, y0, x1, y1 = map(int, sys.argv[2:6]); f = float(sys.argv[6]); name = sys.argv[7]
ks = []
for n in ns:
    im = cv2.imread(V % n)[y0:y1, x0:x1]; im = cv2.resize(im, None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST)
    cv2.putText(im, 'n%d' % n, (5, 20), 0, 0.6, (0, 255, 255), 2); ks.append(im)
per = int(sys.argv[8]) if len(sys.argv) > 8 else len(ks)
while len(ks) % per: ks.append(np.zeros_like(ks[0]))
cv2.imwrite(os.path.join(G.SFV4, 'foto', name), np.vstack([np.hstack(ks[i:i + per]) for i in range(0, len(ks), per)]))
