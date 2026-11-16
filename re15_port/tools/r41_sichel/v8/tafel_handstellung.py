"""R41 Spur SF, V8 Runde 6c (P7): Tafel 18 "Handstellung" - die linke Hand in den echten Spielbildern gegen die Nutzerbilder.

    python tafel_handstellung.py --fd-neu ORDNER --fd-alt ORDNER --r6b EMD [--neu EMD] [--v7 EMD] [--aus PNG]

Zeile 1: echte Spielfenster-Bilder (RE15_FRAMEDUMP, 960 x 720, Ausschnitt x 370-770 / y 215-665) des Endstands in den fuenf Paar-Bildern
         F368, F369, F371, F373, F375, je daneben das Nutzerbild der Vorlage (birkin20, 22, 23, 21, 24; Videobild f = F + 1108).
Zeile 2: dieselben Spielbilder vom Stand des Rest-Bauers (95a783a6, Hand um 118 Grad verdreht).
Zeile 3: ganze Figur von vorn, von ihrer linken Seite und von hinten im Stand (Clip 0 Bild 9), je V7 | V8 r6b | NEU.
Zeile 4: dasselbe im Gang (Clip 1 Bild 35 = Paar-Pose h125).
Befund P7 (Nutzer 2026-11-10): "sieht es so aus als waere die linke Hand verkehrt herum gedreht".
Kein RE-Wert: reine Sicht-Tafel, roh (keine Aufhellung, keine Retusche).
"""
import os, sys, argparse
import numpy as np
import cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import hand_mass as HM                # noqa: E402
import hand_szene as SZ               # noqa: E402

AUSSCHNITT = (370, 770, 215, 665)     # x0, x1, y0, y1 im Framedump 960 x 720: die Figur in F368-F375 (gemessen am Lauf sfv8/r6c/p7/lauf_neu)
F_SPIEL = 0.7
GRAU = (64, 64, 64)


def text(img, s, x, y, gr=0.5):
    cv2.putText(img, s, (x, y), cv2.FONT_HERSHEY_SIMPLEX, gr, (0, 0, 0), 3)
    cv2.putText(img, s, (x, y), cv2.FONT_HERSHEY_SIMPLEX, gr, (255, 255, 255), 1)


def spielbild(ordner, F, titel):
    im = cv2.imread(os.path.join(ordner, 'f%06d.ppm' % F))
    x0, x1, y0, y1 = AUSSCHNITT
    k = cv2.resize(im[y0:y1, x0:x1], None, fx=F_SPIEL, fy=F_SPIEL, interpolation=cv2.INTER_AREA)
    text(k, titel, 4, 16)
    return k


def vorlagenbild(k, hoehe, titel):
    im = HM.ki_laden(k)
    a = im[..., 3:4].astype(float) / 255.0
    bild = (im[..., :3] * a + np.array(GRAU, float) * (1 - a)).astype(np.uint8)
    ys, xs = np.nonzero(im[..., 3] > 127)
    bild = bild[ys.min():ys.max() + 1, xs.min():xs.max() + 1]
    f = hoehe / float(bild.shape[0])
    bild = cv2.resize(bild, None, fx=f, fy=f, interpolation=cv2.INTER_AREA)[:hoehe]
    text(bild, titel, 4, 16, 0.42)
    return bild


def figur(mo, pose, titel, b=266, h=350):
    A = HM.FR.zeichne(mo, pose, None, licht=True)
    img = cv2.cvtColor(A['img'][:480], cv2.COLOR_RGB2BGR)
    bone = A['bone'][:480]
    img[bone < 0] = GRAU
    ys, xs = np.nonzero((bone >= 0) & (bone <= 15))             # Mitte = Koerper ohne Schwanz (Knochen 0-15), sonst schiebt der Schwanz die Figur aus der Kachel
    cx, y1 = int((xs.min() + xs.max()) / 2), int(np.nonzero(bone >= 0)[0].max()) + 6
    gross = np.full((480 + 2 * h, 640 + 2 * b, 3), GRAU, np.uint8)
    gross[h:h + 480, b:b + 640] = img
    k = gross[y1:y1 + h, b + cx - b // 2:b + cx - b // 2 + b].copy()
    text(k, titel, 4, 16, 0.45)
    return k, int(np.isin(bone, (10, 11, 12)).sum()) - int(np.isin(bone, (13, 14, 15)).sum())


def zeile(teile, breite):
    z = np.hstack(teile)
    if z.shape[1] < breite:
        z = np.hstack([z, np.full((z.shape[0], breite - z.shape[1], 3), GRAU, np.uint8)])
    return z


def stapel(reihen):
    B = max(r.shape[1] for r in reihen)
    return np.vstack([np.hstack([r, np.full((r.shape[0], B - r.shape[1], 3), r[0, -1], np.uint8)]) if r.shape[1] < B else r for r in reihen])


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--neu', default=V.ASSET)
    ap.add_argument('--v7', default=os.path.join(V.SFV8, 'emd', 'EM30_V7.EMD'))
    ap.add_argument('--r6b', required=True)
    ap.add_argument('--fd-neu', required=True)
    ap.add_argument('--fd-alt', required=True)
    ap.add_argument('--aus', default=os.path.join(V.BAUM, 'analysis', 'befunde_runde41', 'SF_belege', 'v8', '18_handstellung.png'))
    a = ap.parse_args()
    B = 2400
    reihen = []
    h_spiel = int(round((AUSSCHNITT[3] - AUSSCHNITT[2]) * F_SPIEL))
    for name, ordner in (('NEU', a.fd_neu), ('Stand Rest-Bauer 95a783a6 (verdreht)', a.fd_alt)):
        teile = []
        for k, hh, _pose in SZ.PAARE:
            F = hh + 243
            teile.append(spielbild(ordner, F, '%s  Spielbild F%d' % (name if len(name) < 6 else 'ALT', F)))
            teile.append(vorlagenbild(k, h_spiel, 'Vorlage birkin%d (f%d)' % (k, hh + 1351)))
        kopf = np.full((24, B, 3), 24, np.uint8)
        text(kopf, 'Echte Spielfenster-Bilder: %s - je daneben das Nutzerbild der Vorlage' % name, 6, 17)
        reihen += [kopf, zeile(teile, B)]
    mods = (('V7', V.modell(open(a.v7, 'rb').read())), ('V8 r6b', V.modell(open(a.r6b, 'rb').read())), ('NEU', V.modell(open(a.neu, 'rb').read())))
    p125 = dict(SZ.PAARE[0][2])
    for pn, clip, bild in (('Stand (Clip 0 Bild 9)', 0, 9), ('Gang (Clip 1 Bild 35)', 1, 35)):
        teile = []
        # Sicht der Szene = von vorn; linke Seite der Figur = die Vierteldrehung, in der der linke Arm (Knochen 10-12) mehr Bildpunkte zeigt als der Sichel-Arm
        seite = None
        for sicht in ('vorn', 'links', 'hinten'):
            for mn, mo in mods:
                pose = dict(p125, clip=clip, bild=bild)
                if sicht == 'hinten':
                    pose['gier'] = (p125['gier'] + 2048) % 4096
                if sicht == 'links':
                    if seite is None:
                        w = {d: figur(mods[2][1], dict(pose, gier=(p125['gier'] + d) % 4096), '')[1] for d in (1024, 3072)}
                        seite = max(w, key=w.get)
                    pose['gier'] = (p125['gier'] + seite) % 4096
                teile.append(figur(mo, pose, '%s  %s' % (mn, sicht))[0])
        kopf = np.full((24, B, 3), 24, np.uint8)
        text(kopf, 'Ganze Figur im %s von vorn (Sicht der Szene), von ihrer linken Seite, von hinten - je V7 | V8 r6b | NEU' % pn, 6, 17)
        reihen += [kopf, zeile(teile, B)]
    bild = stapel(reihen)
    cv2.imwrite(a.aus, bild)
    print('geschrieben', a.aus, bild.shape)


if __name__ == '__main__':
    main()
