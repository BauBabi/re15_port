import os, sys, numpy as np, cv2
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'v4'))
import v4_gemein as G
V = os.path.join(G.BAUM, 'analysis', 'befunde_runde41', 'SF_belege', 'roh', 'alle', 'f%04d.png')
a, b, st = int(sys.argv[1]), int(sys.argv[2]), int(sys.argv[3])
ks = []
for n in range(a, b, st):
    im = cv2.imread(V % n); im = cv2.resize(im, (320, 240)); cv2.putText(im, 'n%d' % n, (5, 20), 0, 0.6, (0, 255, 255), 2); ks.append(im)
while len(ks) % 5: ks.append(np.zeros_like(ks[0]))
rows = [np.hstack(ks[i:i + 5]) for i in range(0, len(ks), 5)]
cv2.imwrite(os.path.join(G.SFV4, 'foto', 'kontakt_%d_%d.jpg' % (a, b)), np.vstack(rows))
