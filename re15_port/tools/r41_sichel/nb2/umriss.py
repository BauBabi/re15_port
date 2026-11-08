"""Runde 41 Spur SF Nachbesserung 2: Hintergrund/Videobild-Helfer und der VERWORFENE automatische Video-Umriss.

Gemeinsam genutzt: hintergrund() (BSS 50A Schnitt 12 nach 640x480), video(n), REPO/BEL. figur() (GrabCut) ist der Versuch,
den Birkin-Umriss automatisch zu trennen - verworfen (Dossier N2.1: Beine im Schatten vor dem dunklen Tor nicht trennbar);
die Messung nutzt die von Hand nachgezeichneten Umrisse SF_belege/nb2/umrisse_video.json.

Birkin-Umriss im Video 4 (Hangar, ROOM50A0 Schnitt 12).

    python re15_port/tools/r41_sichel/nb2/umriss.py <h,h,...> <aus.png>

Hintergrund = Engine-dekodiertes BSS 50A Schnitt 12 (build/form_bg/ROOM50A12.ppm, 320x240), mit der ECC-Homographie
SF_belege/kamera_50A0_12.json auf 640x480 gebracht und auf die Videounschaerfe weichgezeichnet (Farbe ~ Identitaet,
farbe_50A0_12.json). Figur = GrabCut auf dem Videobild, Startmaske aus der Hintergrund-Differenz: sicher Figur d > 40,
wahrscheinlich Figur d > 14, Schatten (alle Kanaele gleich abgedunkelt) wahrscheinlich Hintergrund, ausserhalb des
Birkin-Felds sicher Hintergrund. Danach groesste Komponente. Nur Werkzeug der Nachbildung (keine RE-Werte).
"""
import json, os, sys
import numpy as np
import cv2
from PIL import Image
from scipy import ndimage
HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, '..', '..', '..', '..'))
BEL = os.path.join(REPO, 'analysis', 'befunde_runde41', 'SF_belege')
V = os.path.join(BEL, 'roh', 'alle', 'f%04d.png')
_BG = {}


def hintergrund():
    if 'bg' not in _BG:
        bg = np.asarray(Image.open(os.path.join(REPO, 're15_port', 'build', 'form_bg', 'ROOM50A12.ppm')).convert('RGB'))
        M = np.array(json.load(open(os.path.join(BEL, 'kamera_50A0_12.json')))['M'], float)
        T1 = np.array([[1, 0, 0.5], [0, 1, 0.5], [0, 0, 1.0]]); T2 = np.array([[1, 0, -0.5], [0, 1, -0.5], [0, 0, 1.0]])
        _BG['bg'] = cv2.warpPerspective(bg, T2 @ M @ T1, (640, 480), flags=cv2.INTER_LINEAR).astype(float)
        _BG['M'] = M
    return _BG['bg']


def video(n):
    return np.asarray(Image.open(V % n).convert('RGB'), dtype=float)


FELD = (240, 600, 120, 470)          # x0, x1, y0, y1 im 640x480-Bild: Birkin-Feld (Spider-Man oben links bleibt draussen)


def figur(bild, feld=FELD, bg=None, iter_=6):
    bg = hintergrund() if bg is None else bg
    bb = cv2.GaussianBlur(bg, (0, 0), 2.2); fb = cv2.GaussianBlur(bild, (0, 0), 1.0)
    d = np.sqrt(((fb - bb) ** 2).sum(2))
    k = (fb + 4) / (bb + 4); km = k.mean(2)
    schatten = (np.abs(k - km[..., None]).max(2) < 0.10) & (km < 0.80)
    gc = np.full(d.shape, cv2.GC_BGD, np.uint8)
    x0, x1, y0, y1 = feld
    inn = np.zeros(d.shape, bool); inn[y0:y1, x0:x1] = True
    gc[inn] = cv2.GC_PR_BGD
    gc[inn & (d > 14)] = cv2.GC_PR_FGD
    gc[inn & (d > 14) & schatten] = cv2.GC_PR_BGD
    gc[inn & (d > 40) & ~schatten] = cv2.GC_FGD
    bgm = np.zeros((1, 65), np.float64); fgm = np.zeros((1, 65), np.float64)
    cv2.grabCut(np.ascontiguousarray(bild.astype(np.uint8)[..., ::-1]), gc, None, bgm, fgm, iter_, cv2.GC_INIT_WITH_MASK)
    m = (gc == cv2.GC_FGD) | (gc == cv2.GC_PR_FGD)
    m = ndimage.binary_opening(m, iterations=1)
    m = ndimage.binary_fill_holes(ndimage.binary_closing(m, iterations=2))
    lab, n = ndimage.label(m)
    if n == 0:
        return m
    gr = ndimage.sum(m, lab, range(1, n + 1))
    return lab == (1 + int(np.argmax(gr)))


def masse(m):
    ys, xs = np.where(m)
    if len(xs) == 0:
        return None
    return dict(x0=int(xs.min()), x1=int(xs.max()), y0=int(ys.min()), y1=int(ys.max()),
                h=int(ys.max() - ys.min() + 1), b=int(xs.max() - xs.min() + 1), n=int(m.sum()))


if __name__ == '__main__':
    hs = [int(x) for x in sys.argv[1].split(',')]
    tafeln = []
    for h in hs:
        b = video(1350 + h)
        m = figur(b)
        print('h%d' % h, masse(m))
        r = b.copy(); rand = m & ~ndimage.binary_erosion(m, iterations=2)
        r[rand] = (0, 255, 255)
        tafeln.append(r[100:470, 200:600].astype(np.uint8))
    W = np.concatenate(tafeln, 1)
    Image.fromarray(W).save(sys.argv[2])
