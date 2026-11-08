"""Runde 41 SF NB2: Kopflage gegen den Umriss (640x480, gleiche Kamera, Lage/Gier/Clip-Bild aus stoer.json).

Video (von Hand am vergroesserten Bild abgelesen, spur.py/kopfblatt.py; KOPF_VIDEO): Oberkante und Mitte des Kopfes
(h0/h3: rote Kappe oben auf der Masse; h100: rote Kuppe oben; h116/h124: helle Kuppe oben Mitte mit rotem Gesicht darunter)
und die Oberkante des Umrisses in derselben Spalte.
Port: Kopf = sichtbare Bildpunkte von Mesh 9 (Differenz mit/ohne Mesh 9), Umriss = alle Knochen. Ausgabe je Bild:
Kopf-Oberkante unter Umriss-Oberkante (dy), Kopfmitte x, sichtbare Kopfflaeche, Kopfbreite.
"""
import json, os, sys
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, HIER)
import portmaske as PM, umriss as U   # noqa: E402
import birkin_buehne as BB            # noqa: E402

# n: (Kopf oben y, Kopfmitte x, Kopfbreite, Umriss oben y)  -- abgelesen, +-4 Bildpunkte
KOPF_VIDEO = {1350: (165, 370, 30, 163), 1450: (177, 405, 22, 177), 1466: (159, 393, 20, 156), 1474: (152, 394, 22, 147)}


def port_kopf(blob, s):
    mo = BB.modell(blob)
    bg = U.hintergrund().astype(np.uint8)
    mit, m = PM.maske(mo, s['clip'], s['bild'], s['gier'], s['x'], s['z'], hintergrund=bg, licht=False)
    ohne, _ = PM.maske(mo, s['clip'], s['bild'], s['gier'], s['x'], s['z'], hintergrund=bg, licht=False,
                       nur=set(range(len(mo.meshes))) - {9})
    k = np.abs(mit.astype(int) - ohne.astype(int)).sum(2) > 0
    if k.sum() == 0:
        return None
    ys, xs = np.where(k)
    xm = int(round(xs.mean()))
    spalte = m[:, max(0, xm - 6):xm + 7].any(1)
    oben = int(np.where(spalte)[0].min())
    return dict(oben=int(ys.min()), mitte_x=xm, breite=int(np.percentile(xs, 95) - np.percentile(xs, 5)),
                flaeche=int(k.sum()), umriss_oben=oben, dy=int(ys.min()) - oben)


if __name__ == '__main__':
    St = json.load(open(sys.argv[2]))[sys.argv[3]]
    blob = open(sys.argv[1], 'rb').read()
    for n, (vo, vx, vb, vu) in KOPF_VIDEO.items():
        p = port_kopf(blob, St[str(n)])
        print('h%3d Video: Kopf oben %d (dy %d unter Umriss), Mitte x %d, Breite %d | Port: %s' % (n - 1350, vo, vo - vu, vx, vb, p))
