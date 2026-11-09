"""Uebersicht: alle Staende in Szene Bild 1/2 (Disc-Licht x k roh und + Videofilter Saat 15) und rundum (Clip 2 Ruhe), Cache je Stand."""
import os, sys, pickle
import numpy as np, cv2
import v4f
AUS = sys.argv[1]
os.makedirs(AUS, exist_ok=True)
for nm, o in v4f.STAENDE:
    p = os.path.join(AUS, 'cache_%s.pkl' % nm.replace(' ', '_'))
    if os.path.exists(p):
        continue
    d = {}
    for n in (1, 2):
        s, K = v4f.szene_id(o, n)
        d[n] = dict(sauber=s, K=K, video={sa: v4f.video(s, n, sa) for sa in v4f.SAATEN})
    pickle.dump(d, open(p, 'wb'))
    print(nm, 'ok', flush=True)
