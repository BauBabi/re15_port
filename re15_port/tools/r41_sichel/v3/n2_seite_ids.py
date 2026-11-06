# SF V3 Runde 2: Seitenkamera h83/h85/h92 - welche Bildpunkte gehoeren zu Mesh 9 (Kopf) / 8 (Brust), Lage im Bild.
import os, sys, json
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER); sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'v2'))
import b2
SC = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad'
OUT = SC + '/sfv3/n2/'
emd = sys.argv[1]; name = sys.argv[2]
mo = b2.modell(emd)
P = json.load(open(SC + '/sfv2/posen_seite.json'))
for h in ('83', '85', '92'):
    im, ids = b2.zeichne(mo, P[h], None)
    ok, so = b2.ok_sohle(ids, ausser=tuple(range(16, 23)))
    Hs = so - ok
    m9 = ids == 9
    ys, xs = np.nonzero(m9)
    print(h, 'OK', ok, 'Sohle', so, 'H', Hs, 'Mesh9 px', m9.sum(), 'bbox x', xs.min(), xs.max(), 'y', ys.min(), ys.max(),
          'y unter OK H %.3f-%.3f' % ((ys.min() - ok) / Hs, (ys.max() - ok) / Hs))
    vis = im.copy(); vis[m9] = (vis[m9] * 0.4 + np.array([0, 255, 0]) * 0.6).astype(np.uint8)
    vis[ids == 8] = (vis[ids == 8] * 0.6 + np.array([0, 0, 255]) * 0.4).astype(np.uint8)
    big = cv2.resize(vis[150:330, 300:520], None, fx=4, fy=4, interpolation=cv2.INTER_NEAREST)
    cv2.imwrite(OUT + 'ids_%s_h%s.png' % (name, h), cv2.cvtColor(big, cv2.COLOR_RGB2BGR))
    cv2.imwrite(OUT + 'im_%s_h%s.png' % (name, h), cv2.cvtColor(cv2.resize(im[150:330, 300:520], None, fx=4, fy=4, interpolation=cv2.INTER_NEAREST), cv2.COLOR_RGB2BGR))
