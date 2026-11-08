"""Sichtkontrolle der Posen-Kandidaten: 50 %-Ueberblendung Foto/Modell mit Foto-Umriss (gruen) und Modell-Kopf (orange)."""
import os, sys, json, numpy as np, cv2
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'v4'))
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import v4_gemein as G, b2
from f_tafeln import kachel, kontur, BOX
S = os.path.join(G.SFV4, 'foto')
b = int(sys.argv[1]); idx = [int(i) for i in sys.argv[2].split(',')]
mo = b2.modell(G.EMD_V4)
P = json.load(open(os.path.join(S, 'pose_%d.json' % b)))
cand = [('alt', P['pose_alt'])] + [('f%d' % i, P['fein'][i]) for i in idx]
v = G.vorlage(b); T = np.load(os.path.join(S, 'trimap_%d.npy' % b))
ks = []
for n, p in cand:
    pose = {k: p[k] for k in ('clip', 'bild', 'gier', 'x', 'z')}
    img, ids = G.zeichne(mo, pose)
    t = kachel(0.5 * v + 0.5 * img, BOX[b])
    kontur(t, T == 1, BOX[b], (0, 255, 0)); kontur(t, ids == 9, BOX[b], (255, 140, 0)); kontur(t, np.isin(ids, G.SCHWANZ), BOX[b], (255, 0, 255))
    ks.append(b2.tafel(t, '%s c%d b%d g%d iou %s' % (n, pose['clip'], pose['bild'], pose['gier'], p.get('iou', '')), f=1))
b2.leiste([ks], ['Kandidaten Foto %d' % b], os.path.join(S, 'kand_%d.jpg' % b))
