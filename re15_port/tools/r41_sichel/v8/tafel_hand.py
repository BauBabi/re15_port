"""R41 Spur SF, V8 Runde 6c (P7): Tafel 16_hand.png - linker Arm vom Ellbogen bis zur Krallenspitze in den ECHTEN Szenenposen.

Je Paar eine Zeile: Vorlage (Nutzerbild birkin20/22/23/21/24, Version_80%, freigestellt, 2 Bildpunkte je Videopixel) | V7 | V8 r5d |
V8 r6b (Hand um 118 Grad verdreht, Beuge 60) | NEU - jedes Modell in der Pose SEINES Spielbilds (hand_szene.PAARE: h125, h126, h128,
h130, h132), frei gezeichnet, roh (naechster Nachbar, 2,5 Bildpunkte je Videopixel). Eingezeichnet: Ellbogen (gelb), Handgelenk (gruen),
Spitze (magenta) und die acht Stationslinien des Messblatts mit ihrer Breite in Videopixeln (hand_mass.kachel).
Darunter als Zahlenleiste das Breitenprofil je Paar: Vorlage gegen NEU an den acht Stationen, Zahlen ausserhalb der Streuung der fuenf
Vorlagebilder (+- 1 px, Regel S17 a) rot.

Aufruf: python tafel_hand.py --r5d <EMD> --r6b <EMD> [--neu <EMD>] [--v7 <EMD>] [--aus <png>]
PORT-WAHL: nur Darstellung, keine Verhaltens-Zahl.
"""
import os, sys, argparse
import numpy as np
import cv2
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import v8_gemein as V                  # noqa: E402
import hand_mass as HM                 # noqa: E402
import hand_szene as SZ                # noqa: E402
import riegel_hand as RH               # noqa: E402

KANTE, F = 136, 2.5                    # Ausschnitt 136 Videopixel, 2,5-fach


def text(img, s, x, y, farbe=(255, 255, 255), gr=0.45):
    cv2.putText(img, s, (x, y), cv2.FONT_HERSHEY_SIMPLEX, gr, (0, 0, 0), 3, cv2.LINE_AA)
    cv2.putText(img, s, (x, y), cv2.FONT_HERSHEY_SIMPLEX, gr, farbe, 1, cv2.LINE_AA)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--neu', default=V.ASSET)
    ap.add_argument('--v7', default=os.path.join(V.SFV8, 'emd', 'EM30_V7.EMD'))
    ap.add_argument('--r5d', required=True)
    ap.add_argument('--r6b', required=True)
    ap.add_argument('--aus', default=os.path.join(V.BAUM, 'analysis', 'befunde_runde41', 'SF_belege', 'v8', '16_hand.png'))
    a = ap.parse_args()
    staende = [('V7', a.v7), ('V8 r5d', a.r5d), ('V8 r6b (verdreht)', a.r6b), ('NEU', a.neu)]
    mo = {n: V.modell(open(p, 'rb').read()) for n, p in staende}
    vor = RH._vorlage()
    zeilen, leiste = [], []
    for k, h, pose in SZ.PAARE:
        pn = 'h%d' % h
        v = HM.vorlage(k)
        reihe = [HM.kachel(v['_bild'], v, 0.5, KANTE, F, 'Vorlage %s (%s)' % (v['bild'], v['video']))]
        for n, _ in staende:
            m = HM.modell(mo[n], pose)
            reihe.append(HM.kachel(m['_bild'], m, 1.0, KANTE, F, '%s  %s (Clip %d Bild %d)' % (n, pn, pose['clip'], pose['bild'])))
        zeilen.append(np.hstack(reihe))
        leiste.append((v['bild'], pn, v['stationen'], m['stationen'], v['hand_laenge'], m['hand_laenge'], v['unterarm_laenge'], m['unterarm_laenge']))
    bild = np.vstack(zeilen)
    L = np.full((30 + 44 * len(leiste) + 30, bild.shape[1], 3), 24, np.uint8)
    text(L, 'Breitenprofil quer zur Finger-Achse in Videopixeln, Stationen 1-8: ' + ', '.join('%d %s' % (i + 1, s) for i, s in enumerate(HM.STATIONEN))
         + ' | Hand-Laenge | Unterarm-Laenge', 8, 20)
    for j, (b, pn, sv, sm, hv, hm, uv, um) in enumerate(leiste):
        y = 30 + 44 * j + 18
        text(L, '%s Vorlage:' % b, 8, y, (200, 200, 200))
        text(L, '%s NEU:' % pn, 8, y + 20, (255, 255, 255))
        for i, s in enumerate(HM.STATIONEN):
            ok = RH._in(sm[s], vor['st'][s])
            text(L, '%5.1f' % sv[s], 200 + 150 * i, y, (200, 200, 200))
            text(L, '%5.1f' % sm[s], 200 + 150 * i, y + 20, (120, 255, 120) if ok else (80, 80, 255))
        text(L, '%5.1f' % hv, 200 + 150 * 8, y, (200, 200, 200))
        text(L, '%5.1f' % hm, 200 + 150 * 8, y + 20)
        text(L, '%5.1f' % uv, 200 + 150 * 9, y, (200, 200, 200))
        text(L, '%5.1f' % um, 200 + 150 * 9, y + 20)
    text(L, 'Streuung der fuenf Vorlagebilder min..max: ' + '  '.join('%d: %.1f..%.1f' % (i + 1, vor['st'][s][0], vor['st'][s][2]) for i, s in enumerate(HM.STATIONEN))
         + '   (gruen = innerhalb +- 1 px, rot = ausserhalb)', 8, L.shape[0] - 10)
    cv2.imwrite(a.aus, np.vstack([bild, L]))
    print('geschrieben', a.aus, bild.shape)


if __name__ == '__main__':
    main()
