#!/usr/bin/env python3
"""R41 Spur SF, V8 Runde 6a (P6): Tafel 16_hand.png - linke Hand und linker Unterarm.

Zeilen P1, P2, PV: Vorlage | V7 | V8 r5d | NEU, gleicher Ausschnitt um den linken Arm (KANTE x KANTE Videopixel um die Mitte von
Ellbogen / Handgelenk / fernstem Handpunkt des neuen Modells), gleicher Zoom, roh (naechster Nachbar), Spielkamera 50A0 mit RDT-Licht.
Vorlagen: P1 = Nutzerbild birkin20 (Version_80%, freigestellt; 2 Bildpunkte je Videopixel wie im Messblatt Hand, deshalb 2-fach statt
4-fach vergroessert = gleicher Zoom je Videopixel; Ausschnitt um Ellbogen / Handgelenk / fernsten Handpunkt der Vorlage-Messung),
Foto 2 (Beschreibung2_Vorlage, P2), Darstellung_Vorlage (PV); in P2 und PV zeigt die Vorlage den linken Arm nicht (hinter dem Rumpf).
Darunter: Bilder aus dem ECHTEN Fenster (gdigrab, runde6a/gdi.sh): V8 r5d und zweimal NEU, gleicher Ausschnitt; daneben die
Helligkeitsmessung der Hand (Mittel der Bildpunkte von Knochen 12, mit und ohne Licht) - Angabe fuer den Punkt "Hand dunkel".

  python tafel_hand.py --r5d <EM30 r5d> --gdi-alt <png> --gdi-neu <png> <png> --aus <16_hand.png>
"""
import os, sys, argparse
import numpy as np, cv2

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import hand_mass as HM                # noqa: E402
import tafeln_v8 as TV                # noqa: E402
import f_rast as FR                   # noqa: E402

KANTE, ZOOM = 120, 4                  # Ausschnitt 120 x 120 Videopixel, 4-fach (Darstellung; kein Messwert)
VORLAGEN = dict(P1='Beschreibung1_Vorlage.png', P2='Beschreibung2_Vorlage.png', PV='Darstellung_Vorlage.png')
VORLAGE_P1, VORLAGE_SKALA = 20, 0.5   # Nutzerbild birkin20; 0.5 Videopixel je Bildpunkt (hand_suche.tafel / Messblatt Hand)
GDI_BOX = (380, 200, 780, 680)        # Fenster 960 x 720: Ausschnitt um die Figur im Takt "steht frontal" (x0, y0, x1, y1)


def hell(mo, pose):
    """Mittlere Helligkeit (0.299 R + 0.587 G + 0.114 B) der Bildpunkte von Knochen 12, mit und ohne Licht; Zahl der Bildpunkte."""
    out = []
    for licht in (True, False):
        A = FR.zeichne(mo, pose, licht=licht)
        m = A['bone'][:480] == 12
        px = A['img'][:480][m].astype(float)
        out.append(float((px @ np.array([0.299, 0.587, 0.114])).mean()) if len(px) else 0.0)
    return out[0], out[1], int(m.sum())


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--neu', default=V.ASSET)
    ap.add_argument('--v7', default=os.path.join(V.SFV8, 'emd', 'EM30_V7.EMD'))
    ap.add_argument('--r5d', required=True)
    ap.add_argument('--gdi-alt'); ap.add_argument('--gdi-neu', nargs='*', default=[])
    ap.add_argument('--aus', required=True)
    a = ap.parse_args()
    blobs = [('V7 319bbd17', open(a.v7, 'rb').read()), ('V8 r5d c4976c62', open(a.r5d, 'rb').read()), ('NEU ' + V.sha(open(a.neu, 'rb').read())[:8], open(a.neu, 'rb').read())]
    mos = [V.modell(b) for _n, b in blobs]
    zeilen, text = [], []
    for pn in ('P1', 'P2', 'PV'):
        pose = HM.POSEN[pn]
        v = HM.modell(mos[2], pose)
        P = np.array([v['E'], v['W'], v['T']], float)
        cx, cy = P[:, 0].mean(), P[:, 1].mean()
        x0 = int(min(max(cx - KANTE / 2, 0), 640 - KANTE)); y0 = int(min(max(cy - KANTE / 2, 0), 480 - KANTE))
        box = (x0, y0, x0 + KANTE, y0 + KANTE)
        vl = TV.bild(V.V80 + VORLAGEN[pn])
        ks = [TV.kachel(vl, box, ZOOM, 'Vorlage %s (Arm verdeckt)' % pn)]
        if pn == 'P1':
            vv = HM.vorlage(VORLAGE_P1)
            Q = np.array([vv['E'], vv['W'], vv['T']], float)
            n = int(KANTE / VORLAGE_SKALA)
            im = vv['_bild']
            qx = int(min(max(Q[:, 0].mean() - n / 2, 0), im.shape[1] - n)); qy = int(min(max(Q[:, 1].mean() - n / 2, 0), im.shape[0] - n))
            aus = im[qy:qy + n, qx:qx + n]
            al = aus[..., 3:4].astype(float) / 255.0
            rgb = (aus[..., 2::-1] * al + 96 * (1 - al)).astype(np.uint8)
            k = np.ascontiguousarray(cv2.resize(rgb, (KANTE * ZOOM, KANTE * ZOOM), interpolation=cv2.INTER_NEAREST))
            TV.schrift(k, 'Vorlage P1: Nutzerbild birkin%d' % VORLAGE_P1, 6, 22)
            ks = [k]
        for (nm, blob), mo in zip(blobs, mos):
            ks.append(TV.kachel(TV.render(blob, pose), box, ZOOM, '%s %s' % (nm, pn)))
            h = hell(mo, pose)
            text.append('%s %s: Hand %4d px, Helligkeit mit Licht %5.1f, ohne Licht %5.1f' % (pn, nm, h[2], h[0], h[1]))
        zeilen.append(TV.reihe(ks))
    g = KANTE * ZOOM
    unten = []
    x0, y0, x1, y1 = GDI_BOX
    for titel, p in [('echtes Fenster V8 r5d', a.gdi_alt)] + [('echtes Fenster NEU %d' % (i + 1), p) for i, p in enumerate(a.gdi_neu)]:
        if not p:
            continue
        im = cv2.cvtColor(cv2.imread(p), cv2.COLOR_BGR2RGB)[y0:y1, x0:x1]
        k = np.zeros((g, im.shape[1], 3), np.uint8)
        k[:im.shape[0]] = im[:g]
        TV.schrift(k, titel, 6, 22)
        unten.append(k)
    breite = zeilen[0].shape[1]
    rest = breite - sum(k.shape[1] for k in unten)
    tk = np.full((g, rest, 3), 24, np.uint8)
    for i, t in enumerate(['Helligkeit der Hand (Knochen 12), Angabe:'] + text):
        cv2.putText(tk, t, (8, 24 + 20 * i), cv2.FONT_HERSHEY_SIMPLEX, 0.42, (255, 255, 255), 1, cv2.LINE_AA)
    unten.append(tk)
    im = np.vstack(zeilen + [np.hstack(unten)])
    TV.speichern(TV.kopf(im, 'SF V8 16: linke Hand / Unterarm (P6) - Vorlage | V7 | V8 r5d | NEU je Pose P1, P2, PV (roh, x%d); unten echtes Fenster (gdigrab)' % ZOOM), a.aus)
    print('\n'.join(text))


if __name__ == '__main__':
    main()
