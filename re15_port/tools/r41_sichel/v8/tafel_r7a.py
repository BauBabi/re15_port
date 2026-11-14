"""R41 Spur SF, V8 Runde 7a (Hand-Rest K3 / Armhaltung K4): Sicht-Tafeln mit den Spalten Vorlage | V7 | Stand 6d | NEU.

    python tafel_r7a.py hand  --s6d EMD [--neu EMD] [--v7 EMD] --aus PNG
        16_hand: linker Arm vom Ellbogen bis zur Krallenspitze in den fuenf Paar-Posen (hand_szene.PAARE), frei gezeichnet,
        roh (naechster Nachbar), mit den acht Stationen des Messblatts (hand_mass.kachel); Zahlenleiste Vorlage | 6d | NEU.
    python tafel_r7a.py stellung --fd-v7 DIR --fd-6d DIR --fd-neu DIR --s6d EMD [--neu EMD] [--v7 EMD] --aus PNG
        18_handstellung: echte Spielfenster-Bilder F368/F369/F371/F373/F375 je Stand mit dem Nutzerbild daneben, darunter die ganze
        Figur im Stand und im Gang von vorn / links / hinten je V7 | Stand 6d | NEU.
    python tafel_r7a.py arm --fd-v7 DIR --fd-6d DIR --fd-neu DIR --aus PNG
        Armhaltung (K4) in echten Framedumps: F311-F324 und F528, F552, F576, F600, je Videobild (f = F + 1108, SF_belege/roh/alle,
        nur bis f1538 = F430 vorhanden) | V7 | Stand 6d | NEU.
Runde 7e (Handfarbe): --mitte NAME benennt die mittlere Spalte (Standard "Stand 6d"; 7e: "Stand 7d" = 5c902d49 per --s6d / --fd-6d).
Kein RE-Wert: reine Sicht-Tafeln, roh (keine Aufhellung, keine Retusche). Bestehende Tafel-Werkzeuge (tafel_hand.py,
tafel_handstellung.py) bleiben unveraendert; diese Datei nutzt nur ihre Hilfsfunktionen.
"""
import os, sys, argparse
import numpy as np
import cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                  # noqa: E402
import hand_mass as HM                 # noqa: E402
import hand_szene as SZ                # noqa: E402
import riegel_hand as RH               # noqa: E402
import tafel_hand as TH                # noqa: E402
import tafel_handstellung as TS        # noqa: E402

VIDEO = os.path.join(os.path.dirname(V.BAUM), 'r41_sichel', 'analysis', 'befunde_runde41', 'SF_belege', 'roh', 'alle')
ARM_F = list(range(311, 325)) + [528, 552, 576, 600]
ARM_SCHNITT = (300, 820, 150, 690)     # x0, x1, y0, y1 im Framedump 960 x 720 (Figur in F311-F324)
ARM_SCHNITT_GANG = (0, 520, 120, 660)  # Seitengang F528-F600 (Kamera 4, Figur am linken Bildrand), gleiche Groesse


def hand(a):
    staende = [('V7', a.v7), (a.mitte, a.s6d), ('NEU', a.neu)]
    mo = {n: V.modell(open(p, 'rb').read()) for n, p in staende}
    vor = RH._vorlage()
    zeilen, leiste = [], []
    for k, h, pose in SZ.PAARE:
        pn = 'h%d' % h
        v = HM.vorlage(k)
        reihe = [HM.kachel(v['_bild'], v, 0.5, TH.KANTE, TH.F, 'Vorlage %s (%s)' % (v['bild'], v['video']))]
        ms = {}
        for n, _ in staende:
            ms[n] = HM.modell(mo[n], pose)
            reihe.append(HM.kachel(ms[n]['_bild'], ms[n], 1.0, TH.KANTE, TH.F, '%s  %s (F%d, Clip %d Bild %d)' % (n, pn, h + 243, pose['clip'], pose['bild'])))
        zeilen.append(np.hstack(reihe))
        leiste.append((v['bild'], pn, v, ms[a.mitte], ms['NEU']))
    bild = np.vstack(zeilen)
    L = np.full((30 + 66 * len(leiste) + 30, bild.shape[1], 3), 24, np.uint8)
    TH.text(L, 'Breitenprofil quer zur Finger-Achse in Videopixeln, Stationen 1-8: ' + ', '.join('%d %s' % (i + 1, s) for i, s in enumerate(HM.STATIONEN))
            + ' | Daumen Gabel / Laenge / Dicke', 8, 20)
    for j, (b, pn, v, m6, mn) in enumerate(leiste):
        y = 30 + 66 * j + 18
        for r, (nm, m, farbe) in enumerate((('%s Vorlage:' % b, v, (200, 200, 200)), ('%s %s:' % (pn, a.mitte.replace('Stand ', '')), m6, None), ('%s NEU:' % pn, mn, None))):
            TH.text(L, nm, 8, y + 20 * r, farbe or (255, 255, 255))
            for i, s in enumerate(HM.STATIONEN):
                ok = farbe is not None or RH._in(m['stationen'][s], vor['st'][s])
                TH.text(L, '%5.1f' % m['stationen'][s], 200 + 130 * i, y + 20 * r, farbe or ((120, 255, 120) if ok else (80, 80, 255)))
            d = m.get('daumen') or {}
            TH.text(L, '%s / %s / %s' % (d.get('gabel_ab_handgelenk'), d.get('laenge'), d.get('dicke_wurzel')), 200 + 130 * 8, y + 20 * r, farbe or (255, 255, 255))
    TH.text(L, 'Streuung der fuenf Vorlagebilder min..max: ' + '  '.join('%d: %.1f..%.1f' % (i + 1, vor['st'][s][0], vor['st'][s][2]) for i, s in enumerate(HM.STATIONEN))
            + '   (gruen = innerhalb +- 1 px, rot = ausserhalb)', 8, L.shape[0] - 10)
    cv2.imwrite(a.aus, np.vstack([bild, L]))
    print('geschrieben', a.aus, bild.shape)


def stellung(a):
    B = 2400
    reihen = []
    h_spiel = int(round((TS.AUSSCHNITT[3] - TS.AUSSCHNITT[2]) * TS.F_SPIEL))
    for name, ordner in (('V7', a.fd_v7), (a.mitte, a.fd_6d), ('NEU', a.fd_neu)):
        teile = []
        for k, hh, _pose in SZ.PAARE:
            F = hh + 243
            teile.append(TS.spielbild(ordner, F, '%s  Spielbild F%d' % (name, F)))
            teile.append(TS.vorlagenbild(k, h_spiel, 'Vorlage birkin%d (f%d)' % (k, hh + 1351)))
        kopf = np.full((24, B, 3), 24, np.uint8)
        TS.text(kopf, 'Echte Spielfenster-Bilder: %s - je daneben das Nutzerbild der Vorlage' % name, 6, 17)
        reihen += [kopf, TS.zeile(teile, B)]
    mods = (('V7', V.modell(open(a.v7, 'rb').read())), (a.mitte, V.modell(open(a.s6d, 'rb').read())), ('NEU', V.modell(open(a.neu, 'rb').read())))
    p125 = dict(SZ.PAARE[0][2])
    for pn, clip, bild in (('Stand (Clip 0 Bild 9)', 0, 9), ('Gang (Clip 1 Bild 35)', 1, 35)):
        teile = []
        seite = None
        for sicht in ('vorn', 'links', 'hinten'):
            for mn, mo in mods:
                pose = dict(p125, clip=clip, bild=bild)
                if sicht == 'hinten':
                    pose['gier'] = (p125['gier'] + 2048) % 4096
                if sicht == 'links':
                    if seite is None:
                        w = {d: TS.figur(mods[2][1], dict(pose, gier=(p125['gier'] + d) % 4096), '')[1] for d in (1024, 3072)}
                        seite = max(w, key=w.get)
                    pose['gier'] = (p125['gier'] + seite) % 4096
                teile.append(TS.figur(mo, pose, '%s  %s' % (mn, sicht))[0])
        kopf = np.full((24, B, 3), 24, np.uint8)
        TS.text(kopf, 'Ganze Figur im %s von vorn (Sicht der Szene), von ihrer linken Seite, von hinten - je V7 | %s | NEU' % (pn, a.mitte), 6, 17)
        reihen += [kopf, TS.zeile(teile, B)]
    bild = TS.stapel(reihen)
    cv2.imwrite(a.aus, bild)
    print('geschrieben', a.aus, bild.shape)


def arm(a):
    f = 0.5
    zeilen = []
    for F in ARM_F:
        x0, x1, y0, y1 = ARM_SCHNITT if F < 500 else ARM_SCHNITT_GANG
        teile = []
        pv = os.path.join(VIDEO, 'f%04d.png' % (F + 1108))
        if os.path.exists(pv):
            im = cv2.imread(pv)
            sx, sy = im.shape[1] / 960.0, im.shape[0] / 720.0
            k = im[int(y0 * sy):int(y1 * sy), int(x0 * sx):int(x1 * sx)]
            k = cv2.resize(k, (int((x1 - x0) * f), int((y1 - y0) * f)), interpolation=cv2.INTER_NEAREST)
            t = 'Video f%d' % (F + 1108)
        else:
            k = np.full((int((y1 - y0) * f), int((x1 - x0) * f), 3), 40, np.uint8)
            t = 'kein Videobild (f%d > f1538)' % (F + 1108)
        TS.text(k, t, 4, 16, 0.45)
        teile.append(k)
        for nm, d in (('V7', a.fd_v7), (a.mitte, a.fd_6d), ('NEU', a.fd_neu)):
            im = cv2.imread(os.path.join(d, 'f%06d.ppm' % F))
            k = cv2.resize(im[y0:y1, x0:x1], None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST)
            TS.text(k, '%s F%d' % (nm, F), 4, 16, 0.45)
            teile.append(k)
        zeilen.append(np.hstack(teile))
    halb = (len(zeilen) + 1) // 2
    li, re_ = zeilen[:halb], zeilen[halb:]
    while len(re_) < len(li):
        re_.append(np.full_like(li[0], 24))
    bild = np.hstack([np.vstack(li), np.full((sum(z.shape[0] for z in li), 12, 3), 24, np.uint8), np.vstack(re_)])
    cv2.imwrite(a.aus, bild)
    print('geschrieben', a.aus, bild.shape)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('was', choices=('hand', 'stellung', 'arm'))
    ap.add_argument('--neu', default=V.ASSET)
    ap.add_argument('--v7', default=os.path.join(V.SFV8, 'emd', 'EM30_V7.EMD'))
    ap.add_argument('--s6d')
    ap.add_argument('--fd-v7'); ap.add_argument('--fd-6d'); ap.add_argument('--fd-neu')
    ap.add_argument('--aus', required=True)
    ap.add_argument('--mitte', default='Stand 6d', help='Runde 7e: Name der mittleren Spalte (Stand zu --s6d / --fd-6d)')
    a = ap.parse_args()
    dict(hand=hand, stellung=stellung, arm=arm)[a.was](a)


if __name__ == '__main__':
    main()
