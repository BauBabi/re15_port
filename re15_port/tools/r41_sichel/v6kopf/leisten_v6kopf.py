"""SF V6 Kopf (Bauer): Leisten k_kopf_foto1.jpg, k_kopf_foto2.jpg, k_textur.jpg.

    python leisten_v6kopf.py --neu EM30_V5k.EMD --orig EM30_V6orig.EMD --aus DIR

k_kopf_foto1: Zeilen Pose 135 (Kopf von vorn, Messblatt) und P1 (Foto-1-Pose); Spalten Foto 1 roh | Foto 1 entblendet (Messblatt V4:
              I' = 17 + (I - 17) / 0,766) | ORIGINAL RE1.5-Kopf unveraendert | neu. Foto-Spalten um die Foto-Kopfbox (Messblatt V4 x 368-416
              y 139-191), Modell-Spalten um die Modell-Kopfbox (gleiche Groesse, Mitte = Kopfmitte des neuen Modells); x4, roh (keine Filter).
k_kopf_foto2: Pose P2 (Foto-2-Pose); Spalten Foto 2 roh (nicht abgeblendet) | ORIGINAL | neu; Foto um die Kopfbox x 412-467 y 190-263.
k_textur:     Kopftextur (TPage 128 u 50-127 v 130-255, Original-Palette) Original | neu, x6, roh; dritte Spalte neu mit Zonen-Umriss
              (A = grauer Fleck erweitert, blau; B = Hellgrau ueber dem Mund, gruen; Quelle Fleck gelb, Quelle Hellgrau weiss).
"""
import os, sys, argparse
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v6_gemein as V
import texel_erweitern as TE

B2 = V.b2
GROESSE = 80          # Ausschnitt 80 x 80 Videopunkte


def ausschnitt(bild, mitte, g=GROESSE):
    cx, cy = int(round(mitte[0])), int(round(mitte[1]))
    x0, y0 = max(min(cx - g // 2, 640 - g), 0), max(min(cy - g // 2, 480 - g), 0)
    return (x0, y0, x0 + g, y0 + g)


def kopfmitte(ids):
    ys, xs = np.nonzero(ids == 9)
    return ((xs.min() + xs.max()) / 2, (ys.min() + ys.max()) / 2), (int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max()))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--neu', required=True); ap.add_argument('--orig', required=True); ap.add_argument('--aus', required=True)
    a = ap.parse_args()
    mn, mo = V.modell(a.neu), V.modell(a.orig)
    P = V.posen()
    f1r = V.G.vorlage(1)
    f1roh = np.asarray(__import__('PIL.Image', fromlist=['Image']).open(V.G.V80 + 'Beschreibung1_Vorlage.png').convert('RGB'))
    f2 = V.G.vorlage(2).astype(np.uint8)
    kb1, kb2 = V.G.KB_ORIG[1], V.G.KB_ORIG[2]
    zeilen = []
    for pn in ('135', 'P1'):
        imn, idn = V.render(mn, P[pn]); imo, ido = V.render(mo, P[pn])
        mitte, boxn = kopfmitte(idn)
        bm = ausschnitt(imn, mitte)
        bf = ausschnitt(f1roh, ((kb1[0] + kb1[2]) / 2, (kb1[1] + kb1[3]) / 2))
        zeilen.append([B2.tafel(f1roh, 'Foto 1 roh (n1485)', ['Box %s' % (bf,)], 4, bf),
                       B2.tafel(np.clip(f1r, 0, 255).astype(np.uint8), 'Foto 1 entblendet', ['k 0,766 / SW 17'], 4, bf),
                       B2.tafel(imo, 'ORIGINAL RE1.5-Kopf (%s)' % pn, ['EM30 Mesh 9 + Textur unveraendert'], 4, bm),
                       B2.tafel(imn, 'neu V5k (%s)' % pn, ['Kopfbox %s' % (boxn,)], 4, bm)])
    B2.leiste(zeilen, ['SF V6 Kopf aus dem Original RE1.5-Birkin - Foto 1 (vorn), Spielkamera 50A0 Cut 12',
                       'Zeile 1 Pose 135 (Kopf von vorn, Messblatt), Zeile 2 Pose P1 (Clip %d Bild %d Gier %d); Ausschnitt %d x %d px x4, roh' % (
                           P['P1']['clip'], P['P1']['bild'], P['P1']['gier'], GROESSE, GROESSE)],
              os.path.join(a.aus, 'k_kopf_foto1.jpg'))
    imn, idn = V.render(mn, P['P2']); imo, ido = V.render(mo, P['P2'])
    mitte, boxn = kopfmitte(idn)
    bm = ausschnitt(imn, mitte); bf = ausschnitt(f2, ((kb2[0] + kb2[2]) / 2, (kb2[1] + kb2[3]) / 2))
    B2.leiste([[B2.tafel(f2, 'Foto 2 roh (n1435)', ['nicht abgeblendet', 'Box %s' % (bf,)], 4, bf),
                B2.tafel(imo, 'ORIGINAL RE1.5-Kopf (P2)', ['EM30 Mesh 9 + Textur unveraendert'], 4, bm),
                B2.tafel(imn, 'neu V5k (P2)', ['Kopfbox %s' % (boxn,)], 4, bm)]],
              ['SF V6 Kopf aus dem Original RE1.5-Birkin - Foto 2 (seitlich), Spielkamera 50A0 Cut 12',
               'Pose P2 (Clip %d Bild %d Gier %d); Ausschnitt %d x %d px x4, roh' % (P['P2']['clip'], P['P2']['bild'], P['P2']['gier'], GROESSE, GROESSE)],
              os.path.join(a.aus, 'k_kopf_foto2.jpg'))
    # Textur
    u0, u1, v0, v1 = 50, 128, 130, 256
    to = mo.lut[6][mo.tex][v0:v1, u0:u1, :3]; tn = mn.lut[6][mn.tex][v0:v1, u0:u1, :3]
    S = 6
    big = lambda x: np.ascontiguousarray(cv2.resize(x, None, fx=S, fy=S, interpolation=cv2.INTER_NEAREST))
    ko = big(to); kn = big(tn); kz = kn.copy()
    for poly, col in ((TE.PARAM['zone_a'], (80, 160, 255)), (TE.PARAM['zone_b'], (0, 255, 0))):
        pts = (np.array(poly, float) - [u0, v0]) * S
        cv2.polylines(kz, [pts.astype(np.int32)], True, col, 2)
    for bx, col in ((TE.PARAM['fleck_box'], (255, 255, 0)), (TE.PARAM['hell_box'], (255, 255, 255))):
        cv2.rectangle(kz, ((bx[0] - u0) * S, (bx[1] - v0) * S), ((bx[2] + 1 - u0) * S, (bx[3] + 1 - v0) * S), col, 1)
    for k_ in (ko, kn, kz):
        for u in range(56, 128, 8):
            B2.schrift(k_, str(u), (u - u0) * S + 2, 12, (0, 255, 255), 0.35)
        for v in range(136, 256, 8):
            B2.schrift(k_, str(v), 2, (v - v0) * S + 10, (0, 255, 255), 0.35)
    def kachel(img, titel):
        k = np.full((28, img.shape[1], 3), 16, np.uint8); B2.schrift(k, titel, 6, 20, (255, 255, 0), 0.55)
        return np.vstack([k, img])
    B2.leiste([[kachel(ko, 'ORIGINAL (EM30 TPage 128, Zeile 0)'), kachel(kn, 'neu V5k'), kachel(kz, 'neu + Zonen A/B, Quellen')]],
              ['SF V6 Kopftextur Original | neu (u %d-%d, v %d-%d, x%d, Original-Palette)' % (u0, u1 - 1, v0, v1 - 1, S),
               'A blau: grauer Fleck erweitert (Bild-links = (W)-rechts, bis zur Nase); B gruen: Hellgrau ueber dem Mund; Quelle Fleck gelb, Hellgrau weiss'],
              os.path.join(a.aus, 'k_textur.jpg'))
    print('Leisten geschrieben nach', a.aus)


if __name__ == '__main__':
    main()
