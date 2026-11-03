"""NB2neu Punkt 1 (Farbe): Birkin-Formen im Spiellicht (echte Laeufe, RE15_FRAMEDUMP) + Video 4, grosse Kacheln, darunter der
Hautton am Bild (Kasten um den Koerper; Haut = nicht Jeans (b > r), nicht Wunde (r > 2,2 g), nicht Feuer (r > 200 und g > 110),
Helligkeit 25..230) und aus der Textur (texel-gewichtet, haut.py)."""
import sys, os, colorsys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from c6 import *
R = RUNS
def ppm(run, F): return cv2.resize(cv2.imread(os.path.join(R, run, 'fd', 'f%06d.ppm' % F)), (640, 480), interpolation=cv2.INTER_AREA)
def hautton(im, box):
    x0, y0, x1, y1 = box; c = im[y0:y1, x0:x1].reshape(-1, 3).astype(float)[:, ::-1]
    r, g, b = c[:, 0], c[:, 1], c[:, 2]; L = c @ np.array([0.299, 0.587, 0.114])
    s = ~(b > r) & ~(r > 2.2 * np.maximum(g, 1)) & ~((r > 200) & (g > 110)) & (L > 25) & (L < 230) & (r > g)
    m = np.median(c[s], axis=0); h, sat, v = colorsys.rgb_to_hsv(*(m / 255))
    return m, h * 360, sat, int(s.sum())
KACHELN = [
    ('NB2 6a1e9430d Hangar 50A0 h40', lambda: port('N2', 40), (300, 150, 470, 300), 'Textur (95,68,67) 3 Grad S 0,29'),
    ('r41/sichel 3f91e5c57 Hangar h40', lambda: port('J', 40), (300, 150, 470, 300), 'Textur (69,45,44) 2 Grad S 0,35'),
    ('EM30 Auslieferung = Mensch D0 (3070/5080/5090)', None, None, 'Textur (118,91,89) 4 Grad S 0,24'),
    ('Riesenform 5090 (Lauf R5090b F1420)', lambda: ppm('R5090b', 1420), (170, 130, 380, 250), 'Bahnsteig-G4 Textur (103,74,65) 14 Grad S 0,36'),
    ('G5 5090 (Lauf R5090 F2050)', lambda: ppm('R5090', 2050), (180, 60, 480, 260), 'G5 EM36 Textur (126,81,58) 20 Grad S 0,53'),
    ('Video 4 h40 (Schwarzwert abgezogen)', lambda: vsw(vid(40)), (300, 150, 470, 300), ''),
]
tiles = []
for titel, src, box, tex in KACHELN:
    if src is None:
        t = np.zeros((480, 640, 3), np.uint8); t[:] = (89, 91, 118)[::-1] if False else (89, 91, 118)
        t[:, :] = np.array([89, 91, 118], np.uint8)          # BGR der Texturfarbe (118,91,89)
        cv2.putText(t, 'kein Spielbild in 900 Bildern 3070', (20, 240), 0, 0.7, (255, 255, 255), 2)
        txt = 'nur Textur'
    else:
        t = src().copy(); m, h, sat, n = hautton(t, box)
        cv2.rectangle(t, box[:2], box[2:], (0, 255, 255), 1)
        txt = 'Bild Haut (%d,%d,%d) %d Grad S %.2f n%d' % (m[0], m[1], m[2], h, sat, n)
    t = cv2.copyMakeBorder(t, 34, 44, 0, 0, cv2.BORDER_CONSTANT)
    cv2.putText(t, titel, (6, 24), 0, 0.62, (0, 255, 255), 2)
    cv2.putText(t, txt, (6, t.shape[0] - 26), 0, 0.55, (255, 255, 255), 1)
    cv2.putText(t, tex, (6, t.shape[0] - 8), 0, 0.55, (200, 200, 255), 1)
    tiles.append(t)
im = np.vstack([np.hstack(tiles[0:3]), np.hstack(tiles[3:6])])
kopf = np.zeros((40, im.shape[1], 3), np.uint8)
cv2.putText(kopf, 'Farbe: Birkin-Formen im Spiellicht (Port) und Video 4 - Hautton am Bild (Median im gelben Kasten) und in der Textur', (8, 27), 0, 0.7, (0, 255, 255), 2)
cv2.imwrite(sys.argv[1], np.vstack([kopf, im]), [cv2.IMWRITE_JPEG_QUALITY, 90])
