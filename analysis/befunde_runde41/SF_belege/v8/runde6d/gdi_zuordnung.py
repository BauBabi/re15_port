"""Runde 6d: ordnet die Bilder des gdigrab-Videos (echtes Fenster) den Spielbildern zu - je Framedump F (RE15_FRAMEDUMP desselben Stands,
Lauf sfv8/r6d/szene) das gdigrab-Bild mit der kleinsten mittleren Abweichung (Grauwert, 320 x 240). Keine Zeit geraten: gemessen.
Zwei Durchgaenge (kein Sprung im Video: der Sprung per CAP_PROP_POS_FRAMES traf nicht das gesuchte Bild).
    python gdi_zuordnung.py VIDEO FD_ORDNER AUS_ORDNER F1 F2 ...
"""
import sys, os
import numpy as np, cv2
video, fdo, aus = sys.argv[1:4]
ziele = [int(x) for x in sys.argv[4:]]
cap = cv2.VideoCapture(video)
klein = []
while True:
    ok, im = cap.read()
    if not ok:
        break
    klein.append(cv2.resize(cv2.cvtColor(im, cv2.COLOR_BGR2GRAY), (320, 240), interpolation=cv2.INTER_AREA).astype(np.float32))
K = np.stack(klein)
fps = cap.get(cv2.CAP_PROP_FPS)
cap.release()
print('gdigrab-Bilder', len(klein), 'fps (Datei)', fps)
os.makedirs(aus, exist_ok=True)
wahl = {}
for F in ziele:
    fd = cv2.imread(os.path.join(fdo, 'f%06d.ppm' % F))
    g = cv2.resize(cv2.cvtColor(fd, cv2.COLOR_BGR2GRAY), (320, 240), interpolation=cv2.INTER_AREA).astype(np.float32)
    d = np.abs(K - g).mean(axis=(1, 2))
    i = int(d.argmin())
    # naechstes fremdes Spielbild: kleinste Abweichung ausserhalb des zusammenhaengenden Laufs um i
    a = i
    while a > 0 and d[a - 1] < d[i] + 0.3: a -= 1
    b = i
    while b < len(d) - 1 and d[b + 1] < d[i] + 0.3: b += 1
    rest = np.concatenate([d[:a], d[b + 1:]])
    wahl.setdefault(i, []).append((F, fd, float(d[i]), float(rest.min()), a, b))
cap = cv2.VideoCapture(video)
n = 0
while True:
    ok, im = cap.read()
    if not ok:
        break
    for F, fd, di, dr, a, b in wahl.get(n, []):
        voll = float(np.abs(im.astype(np.float32) - fd.astype(np.float32)).mean())
        p = os.path.join(aus, 'gdi_fenster_F%d.png' % F)
        cv2.imwrite(p, im)
        print('F%d -> gdigrab-Bild %d (t %.3f s; gleiche Bilder %d-%d), mittlere Abweichung 320x240 %.2f (naechstes anderes Bild %.2f), 960x720 %.2f -> %s'
              % (F, n, n / fps, a, b, di, dr, voll, os.path.basename(p)))
    n += 1
