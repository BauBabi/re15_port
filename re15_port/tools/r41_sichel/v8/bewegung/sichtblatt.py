"""Sichtblaetter fuer den Augen-Agenten: je Zeile Video | vorher | nachher (Ausschnitt x 140-600, y 150-470 im Videoraum 640x480),
jedes 2. Bild h16..h104, 6 Zeilen je Blatt, Kopfzeile "Messbild - kein Ergebnis".

    python sichtblatt.py <vorher-lauf> <nachher-lauf> <ausgabeordner> [Titel nachher]
"""
import sys, os
import numpy as np, cv2
AN = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/schwanz_v8/analyse'
vl, nl, aus = sys.argv[1], sys.argv[2], sys.argv[3]
tn = sys.argv[4] if len(sys.argv) > 4 else 'nachher'
os.makedirs(aus, exist_ok=True)
M = np.array([[2.0 / 3.0, 0, 1.97], [0, 2.079 / 3.0, 1.3]], np.float32)
X0, X1, Y0, Y1 = 140, 600, 150, 470


def port(lauf, h):
    im = cv2.imread(os.path.join(lauf, 'fd', 'f%06d.ppm' % (h + 243)))
    if im is None:
        return np.zeros((480, 640, 3), np.uint8)
    return cv2.warpAffine(im, M, (640, 480), flags=cv2.INTER_AREA)


def schrift(im, s, x, y, sc=0.55, col=(255, 255, 255)):
    cv2.putText(im, s, (x, y), cv2.FONT_HERSHEY_SIMPLEX, sc, (0, 0, 0), 3, cv2.LINE_AA)
    cv2.putText(im, s, (x, y), cv2.FONT_HERSHEY_SIMPLEX, sc, col, 1, cv2.LINE_AA)


hs = list(range(16, 105, 2))
blaetter = [hs[i:i + 6] for i in range(0, len(hs), 6)]
for bi, gruppe in enumerate(blaetter):
    zeilen = []
    for h in gruppe:
        v = cv2.imread(AN + '/v4/n%04d.png' % (1349 + h))
        kach = []
        for name, im in (('Video h%d' % h, v), ('vorher h%d' % h, port(vl, h)), ('%s h%d' % (tn, h), port(nl, h))):
            c = np.ascontiguousarray(im[Y0:Y1, X0:X1])
            schrift(c, name, 6, 20)
            kach.append(c)
            kach.append(np.full((Y1 - Y0, 4, 3), 40, np.uint8))
        zeilen.append(np.hstack(kach[:-1]))
        zeilen.append(np.full((4, zeilen[-1].shape[1], 3), 40, np.uint8))
    im = np.vstack(zeilen[:-1])
    kopf = np.zeros((36, im.shape[1], 3), np.uint8)
    schrift(kopf, 'Messbild - kein Ergebnis | Video | vorher (bb19de74c) | %s, h%d-h%d, Ausschnitt x %d-%d y %d-%d' % (tn, gruppe[0], gruppe[-1], X0, X1, Y0, Y1), 8, 25, 0.6, (0, 255, 255))
    p = os.path.join(aus, 'blatt_%02d_h%d-h%d.png' % (bi + 1, gruppe[0], gruppe[-1]))
    cv2.imwrite(p, np.vstack([kopf, im]))
    print(p)
