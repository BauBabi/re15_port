"""Runde 41 SF NB2: Umriss-Mass Port gegen Video 4 (Hangar, gleiche Kamera ROOM50A0 Schnitt 12).

    python re15_port/tools/r41_sichel/nb2/messung.py <emd> <debug.log> [--blatt aus.png]

Video-Umriss: von Hand am vergroesserten Bild nachgezeichnet (SF_belege/nb2/umrisse_video.json, Bild n -> Polygon in
640x480; Hilfe tools/r41_sichel/nb2/spur.py: Bild + Hintergrund-Differenz). Unsicherheit der Beine im Schatten ~5 Bildpunkte.
Port-Umriss: Buehne (tools/birkin_buehne.py) mit der Pose der Messzeile "[hangar-takt] hN ... B clip/bild/gier (x,z)" des
Laufs. Fenster: in den Haltebildern (h40, h64) nur y >= 160 (oben haengt der Spider-Man in der Hand, das Video-Polygon
endet dort). Ausgabe je Bild: IoU, Hoehe, Breite, Hoehe/Breite (Video | Port) und IoU nach bester Verschiebung
(+-40 Bildpunkte, trennt Haltung von der Lage der Szene).
"""
import json, os, sys
import numpy as np
import cv2
HIER = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, HIER)
import portmaske as PM, umriss as U   # noqa: E402
import birkin_buehne as BB            # noqa: E402

UMRISSE = os.path.join(U.BEL, 'nb2', 'umrisse_video.json')
FENSTER = {1390: 160, 1414: 160}


def video_maske(n):
    m = np.zeros((480, 640), np.uint8)
    for p in json.load(open(UMRISSE))[str(n)]:
        cv2.fillPoly(m, [np.array(p, np.int32)], 1)
    return m.astype(bool)


def kasten(m):
    ys, xs = np.where(m)
    return int(ys.max() - ys.min() + 1), int(xs.max() - xs.min() + 1)


def iou(a, b):
    return float((a & b).sum()) / max(1, float((a | b).sum()))


def beste_verschiebung(v, p, r=40, s=2):
    best = (iou(v, p), 0, 0)
    for dy in range(-r, r + 1, s):
        for dx in range(-r, r + 1, s):
            q = np.roll(np.roll(p, dy, 0), dx, 1)
            i = iou(v, q)
            if i > best[0]:
                best = (i, dx, dy)
    return best


def messen(blob, log, ns=None):
    mo = BB.modell(blob); P = PM.posen(log)
    ns = ns or sorted(int(k) for k in json.load(open(UMRISSE)))
    out = []
    for n in ns:
        h = n - 1350
        c, b, g, x, z = P[h]
        _bild, pm = PM.maske(mo, c, b, g, x, z, licht=False)
        vm = video_maske(n)
        y0 = FENSTER.get(n, 0)
        pm = pm.copy(); pm[:y0] = False; vm = vm.copy(); vm[:y0] = False
        hv, bv = kasten(vm); hp, bp = kasten(pm)
        i0 = iou(vm, pm); ib, dx, dy = beste_verschiebung(vm, pm)
        out.append(dict(n=n, h=h, iou=round(i0, 3), iou_verschoben=round(ib, 3), dx=dx, dy=dy,
                        video=(hv, bv, round(hv / bv, 2)), port=(hp, bp, round(hp / bp, 2))))
    return out


if __name__ == '__main__':
    emd, log = sys.argv[1], sys.argv[2]
    r = messen(open(emd, 'rb').read(), log)
    print('  h  | IoU   | IoU verschoben (dx,dy) | Video H x B (H/B) | Port H x B (H/B)')
    for e in r:
        print('h%3d | %.3f | %.3f (%3d,%3d)        | %3d x %3d (%.2f)   | %3d x %3d (%.2f)' %
              (e['h'], e['iou'], e['iou_verschoben'], e['dx'], e['dy'], *e['video'], *e['port']))
    print('Mittel IoU %.3f, verschoben %.3f' % (np.mean([e['iou'] for e in r]), np.mean([e['iou_verschoben'] for e in r])))
