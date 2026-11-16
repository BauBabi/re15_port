"""Spielbild Griff 5090 (gdigrab): Farbe an den Bildpunkten der sechs Kopf-Unterseiten-Flaechen gegen die Haut daneben (Hals 21, Kopf-Haut)."""
import json
import cv2
from s_basis import *  # noqa: F401,F403

SECHS = {(22, 1, 1), (22, 1, 8), (22, 1, 9), (22, 1, 194), (22, 1, 201), (22, 1, 209)}
d = dict(np.load(AUS + '/hals_griff.npz'))
tags = d['tags']
g = cv2.cvtColor(cv2.imread(AUS + '/spiel/griff_5090/hwnd_08.png'), cv2.COLOR_BGR2RGB)
lab = v4f.lab(g.reshape(-1, 3).astype(np.float32)).reshape(g.shape)
i = d['maler_i']
gueltig = i >= 0
sechs = np.zeros(i.shape, bool)
for k_, t in enumerate(tags):
    if (int(t[0]), int(t[2]), int(t[1])) in SECHS:
        sechs |= i == k_
kern = cv2.erode(sechs.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0
ring = (cv2.dilate(sechs.astype(np.uint8), np.ones((9, 9), np.uint8)) > 0) & ~sechs & gueltig & np.isin(d['tm'][np.maximum(i, 0)], (21, 22))
a = lab[kern]; b = lab[ring]
h = lambda x: float(np.degrees(np.arctan2(x[:, 2], x[:, 1])).mean())
rot = (a[:, 1] > 6) & (np.degrees(np.arctan2(a[:, 2], a[:, 1])) > -10)
erg = dict(sechs_flaechen_px=int(sechs.sum()), kern_px=int(kern.sum()), kern_lab_mittel=[round(float(v), 1) for v in a.mean(0)],
           ring_px=int(ring.sum()), ring_lab_mittel=[round(float(v), 1) for v in b.mean(0)], kern_warmrot_anteil=float(rot.mean()),
           ring_warmrot_anteil=float(((b[:, 1] > 6) & (np.degrees(np.arctan2(b[:, 2], b[:, 1])) > -10)).mean()))
ys, xs = np.nonzero(sechs)
erg['bbox_fenster'] = [int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())]
json.dump(erg, open(AUS + '/hals_spiel.json', 'w', encoding='utf-8'), indent=1)
print(json.dumps(erg, indent=1))
