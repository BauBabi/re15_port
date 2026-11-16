"""Runde 7e: Hand-Ausschnitte aus den ECHTEN Fensterbildern (gdigrab, gdi_fenster_F*.png, 960 x 720) neben dem Nutzerbild, roh
(naechster Nachbar, gleicher Massstab je Zeile), dazu dieselben Ausschnitte aus den Framedumps von Stand 7d (5c902d49) als Vergleich.
Ausschnitt: Marker-Masken Mesh 11 + 12 des HF-Laufs (riegel_handfarbe.py) + 12 Bildpunkte Rand. Kein RE-Wert, reine Sicht.
    python gdi_hand.py BELEG_ORDNER FD_ALT HF_LAUF AUS.png
"""
import os, sys
import numpy as np, cv2
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
sys.path.insert(0, W)
import riegel_handfarbe as RF          # noqa: E402
import hand_mass as HM                 # noqa: E402

beleg, fd_alt, hf, aus = sys.argv[1:5]
H = 320
PAARE = ((367, 20), (368, 20), (369, 22), (371, 23), (373, 21), (375, 24))


def text(img, s):
    cv2.putText(img, s, (3, 15), cv2.FONT_HERSHEY_SIMPLEX, 0.45, (0, 0, 0), 3)
    cv2.putText(img, s, (3, 15), cv2.FONT_HERSHEY_SIMPLEX, 0.45, (255, 255, 255), 1)


def gross(img, t):
    k = cv2.resize(img, (int(img.shape[1] * H / img.shape[0]), H), interpolation=cv2.INTER_NEAREST)
    text(k, t)
    return k


zeilen = []
for f, b in PAARE:
    h = RF.ppm(hf, f)
    a = RF.ppm(fd_alt, f)
    k = RF.klassen(h, a)
    m = np.zeros(a.shape[:2], bool)
    for n in ('unterarm', 'hand', 'finger', 'daumenkralle', 'kastenfinger'):
        m |= k[n]
    ys, xs = np.nonzero(m)
    y0, y1, x0, x1 = max(ys.min() - 12, 0), ys.max() + 12, max(xs.min() - 12, 0), xs.max() + 12
    v = HM.vorlage(b)
    im = v['_bild']
    al = im[..., 3:4] / 255.0
    vb = (im[..., :3] * al + 64 * (1 - al)).astype(np.uint8)
    my, mx = np.nonzero(v['_maske'])
    teile = [gross(vb[my.min() - 10:my.max() + 10, mx.min() - 10:mx.max() + 10], 'Vorlage birkin%d' % b),
             gross(a[..., ::-1].astype(np.uint8)[y0:y1, x0:x1], 'Stand 7d FD F%d' % f),
             gross(cv2.imread(os.path.join(beleg, 'gdi_fenster_F%d.png' % f))[y0:y1, x0:x1], 'NEU Fenster F%d' % f)]
    zeilen.append(np.hstack(teile))
B = max(z.shape[1] for z in zeilen)
bild = np.vstack([np.hstack([z, np.full((z.shape[0], B - z.shape[1], 3), 24, np.uint8)]) for z in zeilen])
halb = (len(zeilen) + 1) // 2
li, re_ = bild[:H * halb], bild[H * halb:]
if re_.shape[0] < li.shape[0]:
    re_ = np.vstack([re_, np.full((li.shape[0] - re_.shape[0], B, 3), 24, np.uint8)])
cv2.imwrite(aus, np.hstack([li, np.full((li.shape[0], 10, 3), 24, np.uint8), re_]))
print('geschrieben', aus)
