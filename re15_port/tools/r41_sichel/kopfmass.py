"""Runde 41 Spur SF N1.10: Kopffarbe Birkin im Gang zur Kamera, Video 4 gegen Port-Fensteraufnahme.

    python re15_port/tools/r41_sichel/kopfmass.py <h,h,...> <laufordner>@<p0> [...]

Video: Kopfbereich x 372-402, y 165-215 (Bild 640x480; Kopf = rotes Fleisch oben in der Mitte, h108-h132 per Augenschein
abgegrenzt, Blatt SF_belege/nb1/kopf_*.jpg). Port: Kopfmaske = nur Mesh 9 an der Pose der Messzeile "[hangar-takt] hN B clip
.. bild .. gier .. (x,z)" offline gezeichnet (birkin_buehne, Kamera Schnitt 12), auf 320x240 gebracht, um 1 erodiert, auf
die Fensteraufnahme gelegt. Je Bild Median, p90, r/g,
Anteil r > 1,6 g, Luma L; dazu Rumpf-L (haut.py-Kasten) fuer das Verhaeltnis Kopf/Rumpf.
"""
import os, sys
import numpy as np
from PIL import Image
V = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', 'analysis', 'befunde_runde41', 'SF_belege',
                 'roh', 'alle', 'f%04d.png')


def werte(reg):
    rg = reg[:, 0] / np.maximum(reg[:, 1], 1)
    return (np.median(reg, 0).astype(int).tolist(), np.percentile(reg, 90, axis=0).astype(int).tolist(),
            round(float(np.median(rg)), 2), round(float((rg > 1.6).mean()), 2), round(float((reg @ [.299, .587, .114]).mean())))


def rumpf_L(a320):
    c = a320[95:150, 150:240].reshape(-1, 3); r, g, b = c[:, 0], c[:, 1], c[:, 2]
    c = c[(r > g + 8) & (r > b + 4)]
    return float((c @ [.299, .587, .114]).mean())


sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
import re                                    # noqa: E402
import birkin_buehne as BB                   # noqa: E402
from scipy import ndimage                    # noqa: E402
_MO = {}


def kopfmaske(d, h):
    log = open(os.path.join(d, 'debug.log'), encoding='latin-1').read()
    mm = re.search(r'\[hangar-takt\] h%d F\d+ B clip (\d+) bild (\d+) gier (\d+) \((-?\d+),(-?\d+)\)' % h, log)
    if not mm:
        return None
    clip, bild, gier, x, z = map(int, mm.groups())
    if 'mo' not in _MO:
        _MO['mo'] = BB.modell(open(os.path.join(BB.PSX, 'EMD', 'EM30_HANGAR.EMD'), 'rb').read())
        for b in range(len(_MO['mo'].meshes)):
            if b != 9:
                _MO['mo'].meshes[b].tf, _MO['mo'].meshes[b].qf = [], []
    r = BB.Raum('50A0'); s = r.schnitt(12); kam = BB.Kamera(s['pos'], s['ziel'], s['fov'] >> 7)
    z480 = np.zeros((480, 640, 3), np.uint8)
    mk = BB.rastern(BB.dreiecke(_MO['mo'], BB.kf_von_clip(_MO['mo'], clip, bild), (x, 0, z), gier, kam, None), _MO['mo'],
                    z480, np.diag([2.0, 2.0, 1.0]))
    mk = np.asarray(Image.fromarray(mk.astype(np.uint8) * 255).resize((320, 240), Image.NEAREST)) > 0
    return ndimage.binary_erosion(mk, iterations=1)


hs = [int(x) for x in sys.argv[1].split(',')]
for h in hs:
    a = np.asarray(Image.open(V % (1350 + h)).convert('RGB'), dtype=float)
    p = np.asarray(Image.new('RGB', (320, 240)), dtype=float)
    v320 = Image.open(V % (1350 + h)).convert('RGB').resize((320, 240), Image.BILINEAR)
    vp = Image.new('RGB', (320, 240)); vp.paste(v320, (-1, -3))
    m, p90, rg, ant, L = werte(a[165:215, 372:402].reshape(-1, 3))
    print('h%d Video Kopf median %s p90 %s r/g %.2f kraeftig rot %.2f L %d | Kopf/Rumpf %.2f' %
          (h, m, p90, rg, ant, L, L / rumpf_L(np.asarray(vp, dtype=float))))
    for arg in sys.argv[2:]:
        d, p0 = arg.split('@')
        im = Image.open(os.path.join(d, 'gd_%04d.png' % (int(p0) + h))).convert('RGB')
        a = np.asarray(im, dtype=float)
        mk = kopfmaske(d, h)
        if mk is None or mk.sum() < 10:
            print('   %-8s keine Kopfmaske' % d[-8:]); continue
        m, p90, rg, ant, L = werte(a[mk])
        print('   %-8s Kopf n%d median %s p90 %s r/g %.2f kraeftig rot %.2f L %d | Kopf/Rumpf %.2f' %
              (d[-8:], mk.sum(), m, p90, rg, ant, L, L / rumpf_L(a)))
