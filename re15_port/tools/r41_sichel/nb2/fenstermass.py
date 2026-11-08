"""Runde 41 SF NB2: Umriss-Mass im ECHTEN Fenster (gdigrab-Aufnahme von szene_lauf.sh) gegen die Video-Umrisse.

    python fenstermass.py <laufordner>@<p0> [...]

Port-Umriss: Fensterbild (320x240) gegen den Engine-Hintergrund BSS 50A Schnitt 12 (build/form_bg/ROOM50A12.ppm, derselbe
Dekoder): Differenz > 24 in einem Kanal, Schattenbildpunkte (Boden y >= 190, alle Kanaele gleich abgedunkelt) und der Spider-Man-Bereich (nur
groesste Komponente im Birkin-Feld) aus; mit der ECC-Homographie SF_belege/kamera_50A0_12.json nach 640x480. Video-Umriss und
Fenster wie messung.py. Ausgabe: IoU (Szenenlage wie gespielt), IoU nach bester Verschiebung, Hoehe x Breite (H/B).
"""
import json, os, sys
import numpy as np
import cv2
from PIL import Image
from scipy import ndimage
HIER = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, HIER)
import umriss as U, messung as MS, anpassen as A   # noqa: E402

BG = None


def port_maske(d, p0, h):
    global BG
    if BG is None:
        BG = np.asarray(Image.open(os.path.join(U.REPO, 're15_port', 'build', 'form_bg', 'ROOM50A12.ppm')).convert('RGB'), float)
    f = os.path.join(d, 'gd_%04d.png' % (p0 + h))
    a = np.asarray(Image.open(f).convert('RGB').resize((320, 240), Image.NEAREST), float)
    diff = np.abs(a - BG).max(2)
    k = (a + 4) / (BG + 4); km = k.mean(2)
    schatten = (np.abs(k - km[..., None]).max(2) < 0.12) & (km < 0.8)
    schatten[:190] = False                      # Schattenfleck nur auf dem Boden unter der Figur (Spielbild-y >= 190)
    m = (diff > 24) & ~schatten
    m = ndimage.binary_opening(m, iterations=1)
    f2 = np.zeros_like(m); f2[55:217, 115:300] = m[55:217, 115:300]      # unter y 217 Letterbox
    # Teile verbinden, die nur durch duenne Luecken getrennt sind (Hose/Beine gegen das Tor), dann groesste Komponente
    lab, n = ndimage.label(ndimage.binary_dilation(f2, iterations=3))
    if n == 0:
        return np.zeros((480, 640), bool)
    gr = ndimage.sum(f2, lab, range(1, n + 1))
    m = f2 & (lab == (1 + int(np.argmax(gr))))
    m = ndimage.binary_fill_holes(ndimage.binary_closing(m, iterations=1))
    M = np.array(json.load(open(os.path.join(U.BEL, 'kamera_50A0_12.json')))['M'], float)
    T1 = np.array([[1, 0, 0.5], [0, 1, 0.5], [0, 0, 1.0]]); T2 = np.array([[1, 0, -0.5], [0, 1, -0.5], [0, 0, 1.0]])
    return cv2.warpPerspective(m.astype(np.uint8), T2 @ M @ T1, (640, 480), flags=cv2.INTER_NEAREST).astype(bool)


def messen(d, p0, ns=None):
    ns = ns or sorted(int(k) for k in json.load(open(MS.UMRISSE)))
    out = []
    for n in ns:
        h = n - 1350
        v = MS.video_maske(n); pm = port_maske(d, p0, h)
        y0 = MS.FENSTER.get(n, 0); v[:y0] = False; pm[:y0] = False
        iv, dx, dy = A.iou_verschoben(v, pm, 40)
        hv, bv = MS.kasten(v); hp, bp = MS.kasten(pm)
        out.append(dict(h=h, iou=MS.iou(v, pm), iouv=iv, dx=dx, dy=dy, v=(hv, bv), p=(hp, bp), pm=pm, vm=v))
    return out


if __name__ == '__main__':
    for arg in sys.argv[1:]:
        d, p0 = arg.split('@')
        r = messen(d, int(p0))
        print(arg[-30:])
        print('  h  | IoU   | IoU versch. (dx,dy) | Video H x B (H/B) | Port H x B (H/B)')
        for e in r:
            print('h%3d | %.3f | %.3f (%3d,%3d)     | %3d x %3d (%.2f)   | %3d x %3d (%.2f)' %
                  (e['h'], e['iou'], e['iouv'], e['dx'], e['dy'], e['v'][0], e['v'][1], e['v'][0] / e['v'][1],
                   e['p'][0], e['p'][1], e['p'][0] / e['p'][1]))
        print('Mittel IoU %.3f, verschoben %.3f' % (np.mean([e['iou'] for e in r]), np.mean([e['iouv'] for e in r])))
