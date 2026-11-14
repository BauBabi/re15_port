"""SF V8 Runde 7d (Abgabe): Tafel 19_nacken_p5.png - Nacken / Uebergang Ruecken-Kopf (Nutzer-Befund P5) am Endstand.

    python tafel_r7d.py --v7 EMD --r4 EMD --r6 EMD [--neu EMD] --fd-v7 FD --fd-r4 FD --fd-r6 FD --fd-neu FD --ko FD_KO_NEU --aus PNG

Spalten je Zeile: Vorlage | V7 | V8 r4 | V8 r6 | NEU, gleicher Ausschnitt, vergroessert ohne Glaettung (naechster Nachbar).
Zeile 1: Pose PV offline (Spielkamera 50A0/12, RDT-Licht) neben dem Ausschnitt aus Version_80%/Darstellung.png (Nutzerlinien).
Zeile 2: dieselben Bilder, die Nutzerlinien (Bildpunkte, in denen Darstellung.png von Darstellung_Vorlage.png um > 40 je Kanal abweicht,
         wie tafeln_v8.py Tafel 01) ueber das Modellbild gelegt - die weisse Linie "smooth straight to head without further hills".
Zeile 3: Pose P2 offline neben Foto 2 (Beschreibung2_Vorlage.png = n1435).
Darunter die echten Framedumps (lauf_v4.sh RE15_FRAMEDUMP, 960 x 720) der Griffphase F262, F278, F294, F310, F311, F312 und die
Vorderbilder F346, F354, F362, F372, erste Spalte das Videobild f = F + 1108 (SF_belege/roh/alle, 640 x 480 = Framedump / 1,5) im selben
Ausschnitt. Ausschnitt je Bild: 270 x 200 Framedump-Bildpunkte um den Schwerpunkt des Gesichts im Marker-Lauf KO des Endstands
(szene_marker.py ko: Gesichtsflaechen magenta), fuer alle Spalten gleich.
Kein RE-Wert, keine Verhaltens-Zahl: reine Sicht-Tafel, roh (keine Aufhellung, keine Retusche). Bestehende Tafel-Werkzeuge bleiben
unveraendert; diese Datei nutzt nur ihre Hilfsfunktionen.
"""
import os, sys, argparse
import numpy as np
import cv2
from PIL import Image
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import tafeln_v8 as T                 # noqa: E402

VIDEO = os.path.join(os.path.dirname(V.BAUM), 'r41_sichel', 'analysis', 'befunde_runde41', 'SF_belege', 'roh', 'alle')
BILDER = (262, 278, 294, 310, 311, 312, 346, 354, 362, 372)
BOX_FOTO = (330, 115, 542, 274)        # Ausschnitt 640 x 480 um Huegel, Nacken und Kopf in PV/P2 (weisse Linie etwa x 447-462, y 207-245)
F_FOTO = 510.0 / 212                   # gleiche Kachelbreite wie die Framedump-Zeilen
W_FD, H_FD = 270, 200                  # Ausschnitt im Framedump


def fd(d, F):
    p = os.path.join(d, 'f%06d.ppm' % F)
    return np.asarray(Image.open(p).convert('RGB')) if os.path.exists(p) else None


def box_kopf(ko, F):
    m = fd(ko, F)
    if m is None:
        return None
    ys, xs = np.nonzero((m[..., 0] > 200) & (m[..., 1] < 40) & (m[..., 2] > 200))
    if len(xs) < 10:
        return None
    cx, cy = int(xs.mean()), int(ys.mean())
    x0 = min(max(cx - W_FD // 2, 0), 960 - W_FD)
    y0 = min(max(cy - H_FD // 2 + 20, 0), 720 - H_FD)
    return x0, y0, x0 + W_FD, y0 + H_FD


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--v7', required=True); ap.add_argument('--r4', required=True); ap.add_argument('--r6', required=True)
    ap.add_argument('--neu', default=V.ASSET)
    for k in ('fd-v7', 'fd-r4', 'fd-r6', 'fd-neu', 'ko'):
        ap.add_argument('--' + k, required=True)
    ap.add_argument('--aus', required=True)
    a = ap.parse_args()
    neu = open(a.neu, 'rb').read()
    st = [('V7 ' + V.sha(open(a.v7, 'rb').read())[:8], open(a.v7, 'rb').read()),
          ('V8 r4 ' + V.sha(open(a.r4, 'rb').read())[:8], open(a.r4, 'rb').read()),
          ('V8 r6 ' + V.sha(open(a.r6, 'rb').read())[:8], open(a.r6, 'rb').read()),
          ('NEU ' + V.sha(neu)[:8], neu)]
    vl = T.bild(V.V80 + 'Darstellung_Vorlage.png'); dar = T.bild(V.V80 + 'Darstellung.png')
    lin = np.abs(dar.astype(int) - vl.astype(int)).max(2) > 40
    f2 = T.bild(V.V80 + 'Beschreibung2_Vorlage.png')
    zs = []
    pv = [(n, T.render(b, V.PV)) for n, b in st]
    zs.append(T.reihe([T.kachel(dar, BOX_FOTO, F_FOTO, 'Darstellung.png (Nutzer)')] + [T.kachel(r, BOX_FOTO, F_FOTO, n + ' PV') for n, r in pv]))
    ueber = []
    for n, r in pv:
        o = r.copy(); o[lin] = dar[lin]
        ueber.append(T.kachel(o, BOX_FOTO, F_FOTO, n + ' PV + Linien'))
    zs.append(T.reihe([T.kachel(vl, BOX_FOTO, F_FOTO, 'Darstellung_Vorlage (n1439)')] + ueber))
    zs.append(T.reihe([T.kachel(f2, BOX_FOTO, F_FOTO, 'Foto 2 (n1435)')] + [T.kachel(T.render(b, T.P2), BOX_FOTO, F_FOTO, n + ' P2') for n, b in st]))
    breite = zs[0].shape[1]
    ordner = [a.fd_v7, a.fd_r4, a.fd_r6, a.fd_neu]
    for F in BILDER:
        bx = box_kopf(a.ko, F)
        if bx is None:
            bx = (345, 200, 345 + W_FD, 200 + H_FD)
        x0, y0, x1, y1 = bx
        f = 510.0 / W_FD
        pvid = os.path.join(VIDEO, 'f%04d.png' % (F + 1108))
        if os.path.exists(pvid):
            vb = T.bild(pvid)
            vbox = tuple(int(round(c / 1.5)) for c in bx)
            k0 = cv2.resize(np.ascontiguousarray(vb[vbox[1]:vbox[3], vbox[0]:vbox[2]]), (510, int(round(H_FD * f))), interpolation=cv2.INTER_NEAREST)
            k0 = np.ascontiguousarray(k0); T.schrift(k0, 'Video f%d (F%d)' % (F + 1108, F), 6, 22)
        else:
            k0 = np.zeros((int(round(H_FD * f)), 510, 3), np.uint8); T.schrift(k0, 'kein Videobild f%d' % (F + 1108), 6, 22)
        ks = [k0]
        for (n, _b), o in zip(st, ordner):
            im = fd(o, F)
            if im is None:
                k = np.zeros_like(k0); T.schrift(k, '%s F%d fehlt' % (n, F), 6, 22)
            else:
                k = T.kachel(im, bx, f, '%s F%d' % (n, F))
            ks.append(k)
        z = T.reihe(ks)
        if z.shape[1] < breite:
            z = np.hstack([z, np.zeros((z.shape[0], breite - z.shape[1], 3), np.uint8)])
        zs.append(z[:, :breite])
    im = np.vstack(zs)
    T.speichern(T.kopf(im, 'SF V8 19: Nacken P5 - Vorlage | V7 | V8 r4 | V8 r6 | NEU; PV/P2 offline (x2,4), echte Framedumps F262-F312 '
                           'Griffphase und F346-F372 vorn (Ausschnitt um das Gesicht, x1,9), roh'), a.aus)


if __name__ == '__main__':
    main()
