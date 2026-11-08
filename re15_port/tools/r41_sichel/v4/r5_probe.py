"""SF V4 Runde 5 (Bauer): schnelle Probe eines Stands ohne Spiellauf - Oberkoerper-Rotverteilung und kraeftiges Rot in den Laufposen
(b2-Render der Posen h85/h100/h118/h135 aus dem Lauf sfv4/r/V4r4, debug.log; Lichtgegenprobe b2 gegen FRAMEDUMP dE00 <= 0,9, Pruefung
V4 r4 Zahlen 3) und gelbe Bildpunkte. Nur Werkzeug fuer die Laeufe; die Riegel misst riegel_v4.py am echten Spielbild.

    python r5_probe.py <EMD> [<EMD> ...]
"""
import os, sys
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'nb2neu'))
import v4_gemein as G
import b2
import r4_mass as R4
import riegel_v3 as R3

LAUF = os.path.join(G.SFV4, 'r', 'V4r4', 'debug.log')
FEN = {85: (300, 150, 480, 380), 100: (300, 150, 480, 330), 118: (262, 162, 475, 280), 135: (262, 148, 492, 268)}


def probe(emd):
    PO = R3.lauf_posen(LAUF)
    mo = b2.modell(emd)
    out = {}
    for h in (85, 100, 118, 135):
        img, ids = G.zeichne(mo, PO[h])
        V = G.videoraum(img)
        Lab = G.lab(V)
        x0, y0, x1, y1 = FEN[h]
        sub = Lab[y0:y1, x0:x1].reshape(-1, 3)
        r = dict(a22=int((sub[:, 1] > 22).sum()), a15=int((sub[:, 1] > 15).sum()))
        gelb = (Lab[..., 2] > 20) & (Lab[..., 1] < 12) & (Lab[..., 0] > 25) & (ids >= 0) & (ids < 16)
        r['gelb'] = int(gelb.sum())
        if h in R4.PS_SPIEL:
            d = R4.drittel(V, R4.poly_maske(R4.PS_SPIEL[h]))
            r.update(fo=round(d['rot_m_a'], 2), drittel=[round(d[k]['rot'], 2) for k in ('B_links', 'Mitte', 'B_rechts')],
                     schulter=round(d['schulter_a12'], 3))
        out[h] = r
    return out


if __name__ == '__main__':
    for e in sys.argv[1:]:
        print(os.path.basename(os.path.dirname(e)) + '/' + os.path.basename(e), probe(e))
