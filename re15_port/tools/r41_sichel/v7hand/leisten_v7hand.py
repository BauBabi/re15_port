"""SF V7 linke Hand (Bauer): Leisten 40-42 (roh, Nearest-Neighbor-Vergroesserung, keine Filter).

    python leisten_v7hand.py --neu EM30_V7.EMD --alt EM30_V6.EMD --aus DIR

40_hand_links_x8.png    Hand (W)-links x8: Foto 1 roh (f1485) | Foto 2 roh (f1435) | f1475 (grauer Keil = Daumenkralle) | f1483 (roter
                        Streifen) | V6 | neu; Zeile 1 Modell in P1 (Fotospalten im Fenster der P1-Hand), Zeile 2 Modell in P2 (Fotospalten im
                        Fenster der P2-Hand; in Foto 2 ist die (W)-linke Hand abgewandt).
41_foto1_vergleich.jpg  ganze Figur x2: Foto 1 roh | V6 | neu; Zeile 1 P1, Zeile 2 Pose 135 (Kopf/Hand von vorn).
42_foto2_vergleich.jpg  ganze Figur x2: Foto 2 roh | V6 | neu (P2).
"""
import os, sys, argparse
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import hand_gemein as H           # noqa: E402
import leisten_v5f as L5          # noqa: E402

G, V, B2 = H.G, H.V, H.b2
BOX = L5.BOX
HAND = {'P1': (466, 330, 516, 380), 'P2': (425, 280, 475, 330)}


def vid(n):
    return cv2.cvtColor(cv2.imread(H.VID % n), cv2.COLOR_BGR2RGB)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--neu', required=True); ap.add_argument('--alt', required=True); ap.add_argument('--aus', required=True)
    a = ap.parse_args()
    os.makedirs(a.aus, exist_ok=True)
    mn, ma = V.modell(a.neu), V.modell(a.alt)
    P = V.posen()
    nn, na = os.path.basename(a.neu), os.path.basename(a.alt)
    f1, f2 = L5.foto_roh(1), L5.foto_roh(2)
    r = {}
    for pn in ('P1', 'P2', '135'):
        r[pn] = (G.zeichne(mn, P[pn])[0], G.zeichne(ma, P[pn])[0])
    # ---------------- 40 Hand x8
    f8 = 8
    zeilen = []
    for pn in ('P1', 'P2'):
        hb = HAND[pn]
        zeilen.append([B2.tafel(f1, 'Foto 1 roh (f1485)', ['Fenster P1-Hand'], f8, HAND['P1']),
                       B2.tafel(f2, 'Foto 2 roh (f1435)', ['Fenster P2-Hand (abgewandt)'], f8, HAND['P2']),
                       B2.tafel(vid(1475), 'f1475 grauer Keil', ['Daumenkralle (Nutzer)'], f8, HAND['P1']),
                       B2.tafel(vid(1483), 'f1483 roter Streifen', ['Fingerkralle'], f8, HAND['P1']),
                       B2.tafel(r[pn][1], 'V6 %s' % pn, [na], f8, hb),
                       B2.tafel(r[pn][0], 'neu %s' % pn, [nn], f8, hb)])
    B2.leiste(zeilen, ['SF V7 linke Hand: 40 Hand (W)-links x8 (roh) - Fingerkrallen rot (Zeile 492), Daumenkralle grau (491), alle auf Foto-Laenge',
                       'Zeile 1 Modell P1 (Foto 1), Zeile 2 Modell P2 (Foto 2); Fotospalten je Zeile gleich'],
              os.path.join(a.aus, '40_hand_links_x8.png'))
    # ---------------- 41 Foto 1
    f = 2
    bx = BOX[1]
    z1 = [B2.tafel(f1, 'Foto 1 roh (f1485)', ['Kamera 50A0 Cut 12'], f, bx),
          B2.tafel(r['P1'][1], 'V6 (P1)', [na], f, bx), B2.tafel(r['P1'][0], 'neu (P1)', [nn], f, bx)]
    z2 = [B2.tafel(f1, 'Foto 1 roh', ['Pose 135 = von vorn'], f, bx),
          B2.tafel(r['135'][1], 'V6 (135)', [na], f, bx), B2.tafel(r['135'][0], 'neu (135)', [nn], f, bx)]
    B2.leiste([z1, z2], ['SF V7 linke Hand: 41 Foto 1 (Vorderansicht) - roh, x2', 'Zeile 1 Pose P1, Zeile 2 Pose 135'],
              os.path.join(a.aus, '41_foto1_vergleich.jpg'))
    # ---------------- 42 Foto 2
    bx = BOX[2]
    z = [B2.tafel(f2, 'Foto 2 roh (f1435)', ['Kamera 50A0 Cut 12'], f, bx),
         B2.tafel(r['P2'][1], 'V6 (P2)', [na], f, bx), B2.tafel(r['P2'][0], 'neu (P2)', [nn], f, bx)]
    B2.leiste([z], ['SF V7 linke Hand: 42 Foto 2 (Seitenansicht) - roh, x2', 'Pose P2'], os.path.join(a.aus, '42_foto2_vergleich.jpg'))
    print('Leisten geschrieben nach', a.aus)


if __name__ == '__main__':
    main()
