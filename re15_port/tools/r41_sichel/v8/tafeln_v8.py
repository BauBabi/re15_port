"""SF V8: Tafeln (roh) nach analysis/befunde_runde41/SF_belege/v8/ (Auftrag Punkt 6).

    python tafeln_v8.py --v7 EMD_V7 --v8 EMD_V8 --szene7 DIR --szene8 DIR [--aus DIR]

01_darstellung.png    Darstellung_Vorlage | Nutzerlinien ueber V7 | Nutzerlinien ueber V8 (PV, Spielkamera 50A0/12, RDT-Licht)
02_foto2_seite.jpg    Foto 2 (Beschreibung2_Vorlage = n1435) | V7 | V8 in P2 (Clip 12 Bild 48 Gier 861)
03_foto1_vorn.jpg     Foto 1 (Beschreibung1_Vorlage = n1485) | V7 | V8 in P1 (Clip 20 Bild 139 Gier 1422)
04_rundum.jpg         V8 und V7 rundum (birkin_buehne.rundum, Clip 17 Bild 47, 8 Richtungen)
05_adern_rechts.png   Vorlage | V7 | V8 | Original-EM30 (CDEMD0.EMS Typ 0x30, gleiche Pose PV), Ausschnitt (W)-rechte Flanke / Ruecken
06_spiel.jpg          Spielbilder der Szene (lauf_v4.sh RE15_FRAMEDUMP) V7 | V8 an gleichen Bildnummern
07_bogen_jedes7.jpg   Bogen: jedes 7. Spielbild F225-F450 der V8-Szene (320 x 240)
08_ruecken_hinten.jpg Runde 2 (Sicht-Pruefer V8 r1: "keine Tafel vergroessert den Ruecken von hinten in der Szene"): Spielbilder
                      F322-F346 (Birkin von hinten), Ausschnitt um den Ruecken x3, V7 | V8
Nutzerlinien = Pixel, in denen Darstellung.png von Darstellung_Vorlage.png um mehr als 40 (Kanal) abweicht (wie Messblatt m2_linien).
"""
import os, sys, argparse
import numpy as np, cv2
from PIL import Image
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import f_rast as FR                   # noqa: E402
import birkin_formen as BF            # noqa: E402

P1 = dict(clip=20, bild=139, gier=1422, x=-8420, z=-21908)      # Messblatt Foto (v6_gemein.posen P1)
P2 = dict(clip=12, bild=48, gier=861, x=-8182, z=-21330)        # Messblatt Foto (v6_gemein.posen P2)


def bild(p):
    return np.array(Image.open(p).convert("RGB"))


def render(blob, pose):
    mo = V.modell(blob)
    return FR.mit_bg(mo, pose)['img'][:480]


def schrift(im, s, x, y, sc=0.6):
    cv2.putText(im, s, (x, y), cv2.FONT_HERSHEY_SIMPLEX, sc, (0, 0, 0), 3, cv2.LINE_AA)
    cv2.putText(im, s, (x, y), cv2.FONT_HERSHEY_SIMPLEX, sc, (255, 255, 0), 1, cv2.LINE_AA)


def kachel(im, box, f, titel):
    x0, y0, x1, y1 = box
    t = cv2.resize(np.ascontiguousarray(im[y0:y1, x0:x1]), None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST)
    t = np.ascontiguousarray(t)
    schrift(t, titel, 6, 22)
    return t


def reihe(ks):
    h = max(k.shape[0] for k in ks)
    return np.hstack([np.vstack([k, np.zeros((h - k.shape[0], k.shape[1], 3), np.uint8)]) for k in ks])


def kopf(im, z):
    k = np.full((34, im.shape[1], 3), 16, np.uint8)
    schrift(k, z, 8, 24, 0.55)
    return np.vstack([k, im])


def speichern(im, p):
    os.makedirs(os.path.dirname(p), exist_ok=True)
    if p.endswith('.jpg'):
        cv2.imwrite(p, cv2.cvtColor(im, cv2.COLOR_RGB2BGR), [cv2.IMWRITE_JPEG_QUALITY, 90])
    else:
        cv2.imwrite(p, cv2.cvtColor(im, cv2.COLOR_RGB2BGR))
    print(p, im.shape)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--v7', required=True); ap.add_argument('--v8', default=V.ASSET)
    ap.add_argument('--szene7'); ap.add_argument('--szene8')
    ap.add_argument('--aus', default=os.path.join(V.BAUM, 'analysis', 'befunde_runde41', 'SF_belege', 'v8'))
    a = ap.parse_args()
    v7, v8 = open(a.v7, 'rb').read(), open(a.v8, 'rb').read()
    n8 = 'V8 ' + V.sha(v8)[:8]
    vl = bild(V.V80 + 'Darstellung_Vorlage.png'); dar = bild(V.V80 + 'Darstellung.png')
    lin = np.abs(dar.astype(int) - vl.astype(int)).max(2) > 40
    r7, r8 = render(v7, V.PV), render(v8, V.PV)
    o7, o8 = r7.copy(), r8.copy(); o7[lin] = dar[lin]; o8[lin] = dar[lin]
    box = (230, 140, 500, 450)
    im = reihe([kachel(vl, box, 2, 'Darstellung_Vorlage (n1439)'), kachel(o7, box, 2, 'V7 319bbd17 + Nutzerlinien'),
                kachel(o8, box, 2, n8 + ' + Nutzerlinien')])
    speichern(kopf(im, 'SF V8 01: Vorlage | V7 | V8, Pose PV Clip 17 Bild 47 Gier 587, Spielkamera 50A0/12, RDT-Licht (roh, x2)'),
              os.path.join(a.aus, '01_darstellung.png'))
    # 02 / 03
    for nr, foto, pose, name, bx in ((2, 'Beschreibung2_Vorlage.png', P2, '02_foto2_seite.jpg', (230, 130, 500, 450)),
                                     (1, 'Beschreibung1_Vorlage.png', P1, '03_foto1_vorn.jpg', (250, 110, 560, 450))):
        f = bild(V.V80 + foto)
        im = reihe([kachel(f, bx, 2, 'Foto %d' % nr), kachel(render(v7, pose), bx, 2, 'V7'), kachel(render(v8, pose), bx, 2, n8)])
        speichern(kopf(im, 'SF V8 %s: Foto %d | V7 | V8, Pose Clip %d Bild %d Gier %d (roh, x2)' % (name[:2], nr, pose['clip'], pose['bild'],
                                                                                              pose['gier'])), os.path.join(a.aus, name))
    # 04 rundum
    import birkin_buehne as BB
    tmp = os.path.join(V.SFV8, 'tafeln_tmp'); os.makedirs(tmp, exist_ok=True)
    BB.rundum(v8, 17, 47, os.path.join(tmp, 'r8.png'), n8 + ' rundum (Clip 17 Bild 47)', schritte=8, groesse=300)
    BB.rundum(v7, 17, 47, os.path.join(tmp, 'r7.png'), 'V7 319bbd17 rundum (Clip 17 Bild 47)', schritte=8, groesse=300)
    i8, i7 = bild(os.path.join(tmp, 'r8.png')), bild(os.path.join(tmp, 'r7.png'))
    w = max(i8.shape[1], i7.shape[1])
    pad = lambda x: np.hstack([x, np.full((x.shape[0], w - x.shape[1], 3), 16, np.uint8)])
    speichern(np.vstack([pad(i8), pad(i7)]), os.path.join(a.aus, '04_rundum.jpg'))
    # 05 Adern
    orig = BF.lade_em30()
    ro = render(orig, V.PV)
    bx = (320, 150, 480, 330)
    im = reihe([kachel(vl, bx, 3, 'Vorlage n1439'), kachel(r7, bx, 3, 'V7'), kachel(r8, bx, 3, n8 + ' (Adern X1-X3)'),
                kachel(ro, bx, 3, 'Original-EM30 (Quelle)')])
    speichern(kopf(im, 'SF V8 05: (W)-rechte Flanke/Ruecken in PV - Vorlage | V7 | V8 | Original-RE1.5-Birkin EM30 gleiche Pose (roh, x3)'),
              os.path.join(a.aus, '05_adern_rechts.png'))
    # 06 Spiel
    if a.szene7 and a.szene8:
        nrs = [235, 260, 290, 320, 345, 380, 420, 450]
        zeilen = []
        for n in nrs:
            p7 = os.path.join(a.szene7, 'fd', 'f%06d.ppm' % n); p8 = os.path.join(a.szene8, 'fd', 'f%06d.ppm' % n)
            if not (os.path.exists(p7) and os.path.exists(p8)):
                continue
            k7, k8 = bild(p7), bild(p8)
            k7 = np.array(k7, copy=True); k8 = np.array(k8, copy=True)
            schrift(k7, 'V7 F%d' % n, 6, 20); schrift(k8, 'V8 F%d' % n, 6, 20)
            zeilen.append(np.hstack([k7, k8]))
        par = [np.hstack(zeilen[i:i + 2]) if i + 1 < len(zeilen) else np.hstack([zeilen[i], np.zeros_like(zeilen[i])])
               for i in range(0, len(zeilen), 2)]
        speichern(kopf(np.vstack(par), 'SF V8 06: Spielbilder Hangar 50A0 (lauf_v4.sh FRAMEDUMP, je Paar V7 | V8, gleiche Bildnummer)'),
                  os.path.join(a.aus, '06_spiel.jpg'))
        # 07 Bogen jedes 7. Bild (V8)
        ks = []
        for n in range(225, 451, 7):
            pp = os.path.join(a.szene8, 'fd', 'f%06d.ppm' % n)
            if os.path.exists(pp):
                k = cv2.resize(bild(pp), (320, 240), interpolation=cv2.INTER_AREA)
                k = np.ascontiguousarray(k); schrift(k, 'F%d' % n, 6, 20)
                ks.append(k)
        if ks:
            while len(ks) % 6:
                ks.append(np.zeros_like(ks[0]))
            speichern(kopf(np.vstack([np.hstack(ks[i:i + 6]) for i in range(0, len(ks), 6)]),
                           'SF V8 07: %s Szene Hangar 50A0, jedes 7. Bild F225-F450 (lauf_v4.sh FRAMEDUMP)' % n8),
                      os.path.join(a.aus, '07_bogen_jedes7.jpg'))
        # 08 Ruecken von hinten (Spiel)
        zeilen = []
        for n in (322, 328, 334, 340, 346):
            p7 = os.path.join(a.szene7, 'fd', 'f%06d.ppm' % n); p8 = os.path.join(a.szene8, 'fd', 'f%06d.ppm' % n)
            if not (os.path.exists(p7) and os.path.exists(p8)):
                continue
            ks = []
            for nm, pp in (('V7', p7), (n8, p8)):
                b_ = bild(pp)
                h_, w_ = b_.shape[:2]
                x0, y0 = int(w_ * 0.31), int(h_ * 0.25)        # Birkin von hinten in F322-F346 (960 x 720: x 300-720, y 180-560)
                ks.append(kachel(b_, (x0, y0, x0 + int(w_ * 0.44), y0 + int(h_ * 0.53)), 2, '%s F%d' % (nm, n)))
            zeilen.append(np.hstack(ks))
        if zeilen:
            speichern(kopf(np.vstack(zeilen), 'SF V8 08: Ruecken von hinten im Spiel (FRAMEDUMP 960x720, Ausschnitt x2), V7 | V8'),
                      os.path.join(a.aus, '08_ruecken_hinten.jpg'))


if __name__ == '__main__':
    main()
