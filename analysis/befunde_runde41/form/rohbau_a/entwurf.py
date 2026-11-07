#!/usr/bin/env python3
"""Entwurf Rohbau A: Fleisch. Alle Teile als Ringe. Masse in Spieleinheiten. Tafeln (t, o, r, u, l, p): o/u = Radius bei 0/180 Grad, r/l = bei 90/270 Grad, p = Eckigkeit.
Glieder: 0 vorn, 90 aussen, 180 hinten, 270 innen. Rumpf und Kopf: 0 oben, 90 rechts, 180 unten, 270 links.
Tafel = Stuetzstellen der Form, ts = Lage der Ringe. So sitzen Rippen und Grate auf Ringen, nicht auf Stuetzstellen."""
import math
import numpy as np
from ringe import Teil
from kn import INDEX as K


def wert(tab, t):
    """Tafel an t lesen, linear zwischen Stuetzstellen."""
    ts = [z[0] for z in tab]
    i = int(np.searchsorted(ts, t))
    i = min(max(i, 1), len(ts) - 1)
    a, b = tab[i - 1], tab[i]
    f = 0.0 if b[0] == a[0] else (t - a[0]) / (b[0] - a[0])
    f = min(max(f, 0.0), 1.0)
    z = []
    for k in range(1, 6):
        av = a[k] if k < len(a) else 2.0
        bv = b[k] if k < len(b) else 2.0
        z.append(av + (bv - av) * f)
    return z


def reihe(tab, ts, extra=None):
    """Ringe an ts aus der Tafel. extra(i, t) -> dict (k, cu, cv, dreh, ko, kr)."""
    aus = []
    for i, t in enumerate(ts):
        o, r, u, l, p = wert(tab, t)
        s = dict(t=t, o=o, r=r, u=u, l=l, p=p)
        if extra:
            s.update(extra(i, t))
        aus.append(s)
    return aus


def rumpf():
    T = []
    # Huefte: breites Becken, Beckenkamm als Grat, nach vorn zur Taille eingezogen
    tab = [(-580, 240, 400, 240, 400, 2.0), (-470, 500, 690, 430, 690, 2.2), (-290, 640, 880, 560, 880, 2.6), (-80, 700, 980, 620, 980, 2.5),
           (140, 760, 960, 660, 960, 2.3), (360, 820, 880, 680, 880, 2.1), (620, 860, 800, 690, 800, 2.0)]

    def hk(i, t):
        return dict(f={7: 0.985, 8: 0.90}.get(i, 1.0))
    T.append(Teil('huefte', K['huefte'], 'x', 10, reihe(tab, (-580, -470, -340, -170, 0, 170, 340, 480, 620), hk), dreh=18.0,
                  relief=[(-200, 60, 190, 30, 0.10), (-200, 300, 190, 30, 0.10), (120, 104, 120, 16, 0.08), (120, 256, 120, 16, 0.08), (40, 75, 90, 14, -0.05), (40, 285, 90, 14, -0.05)]))
    # Taille und Flanke: Rippen auf beiden Seiten, links stehen sie frei (offene Stelle)
    tab = [(-200, 840, 830, 700, 830, 2.0), (30, 870, 790, 660, 790, 2.1), (250, 900, 760, 650, 760, 2.2), (470, 940, 750, 680, 750, 2.2), (690, 980, 740, 760, 740, 2.2),
           (900, 1020, 735, 860, 735, 2.2), (1090, 1040, 720, 930, 720, 2.1)]

    def wk(i, t):
        k = {}
        if 2 <= i <= 8:
            f = 1.05 if i % 2 == 0 else 0.98
            k.update({7: f, 6: 1 + (f - 1) * 0.5, 8: 1 + (f - 1) * 0.4})      # Rippen links, offene Flanke
        return dict(k=k, f={0: 0.90, 1: 0.985, 8: 0.985, 9: 0.90}.get(i, 1.0))
    T.append(Teil('wirbel', K['wirbel'], 'x', 10, reihe(tab, (-200, 0, 170, 300, 430, 560, 690, 820, 950, 1090), wk), dreh=18.0,
                  relief=[(120, 145, 160, 18, -0.08), (120, 215, 160, 18, -0.08), (400, 180, 300, 16, 0.06)]))
    # Brust: Nackenbuckel, Schultergurt, schwerer Brustkorb
    tab = [(-200, 1000, 740, 900, 740, 2.1), (60, 1060, 790, 960, 790, 2.7), (300, 1110, 850, 960, 850, 2.9), (520, 1090, 920, 880, 920, 2.9), (730, 980, 900, 760, 900, 2.6),
           (950, 800, 790, 660, 790, 2.0), (1130, 650, 640, 560, 640, 1.9), (1290, 420, 430, 380, 430, 2.0), (1370, 200, 200, 180, 200, 2.0)]

    def bk(i, t):
        return dict(f={0: 0.90, 1: 0.985}.get(i, 1.0))
    T.append(Teil('brust', K['brust'], 'x', 10, reihe(tab, (-200, 60, 300, 520, 730, 950, 1130, 1290, 1370), bk), dreh=18.0,
                  relief=[(420, 148, 180, 26, 0.10), (420, 212, 180, 26, 0.10), (520, 86, 150, 22, 0.05), (520, 274, 150, 22, 0.05)]))
    return T


def hals_kopf():
    T = []
    T.append(Teil('hals', K['hals'], 'x', 8, [
        (-240, 400, 430, 430, 430, 2.0), (120, 480, 490, 490, 490, 2.2), (480, 520, 520, 500, 520, 2.1), (680, 470, 480, 470, 480, 2.0)],
        relief=[(250, 52, 200, 30, 0.08), (250, 308, 200, 30, 0.08)]))
    # Schaedel: Knochenkamm wie ein Hufeisen um das Gesicht, vorn zurueckgesetzt das Gesicht, Schnauze tief und breit.
    tab = [(-260, 330, 320, 360, 320), (-60, 470, 480, 500, 480, 2.2), (200, 525, 540, 520, 540, 2.3), (440, 565, 595, 510, 595, 2.3), (640, 610, 635, 470, 635, 2.0),
           (740, 655, 655, 440, 655, 1.6), (790, 525, 545, 430, 545, 1.9), (900, 435, 445, 425, 445, 2.2), (1040, 385, 365, 420, 365, 2.3), (1180, 360, 315, 400, 315, 2.5), (1320, 330, 295, 350, 295, 2.6),
           (1420, 180, 190, 230, 190, 2.2)]
    T.append(Teil('kopf', K['kopf'], 'x', 10, reihe(tab, (-260, -60, 200, 440, 640, 740, 790, 900, 1040, 1180, 1320, 1420)),
                  relief=[(770, 40, 50, 22, 0.05), (770, 320, 50, 22, 0.05),        # Brauen
                          (900, 58, 60, 22, -0.13), (900, 302, 60, 22, -0.13),      # Augenhoehle
                          (1300, 26, 50, 20, 0.09), (1300, 334, 50, 20, 0.09),      # Nuestern
                          (1180, 90, 70, 26, 0.07), (1180, 270, 70, 26, 0.07)],     # Lefzen
                  ende='spitz', spitze=(1480, -20, 0), kante=40))
    T.append(Teil('kiefer', K['kiefer'], 'x', 8, [
        (-240, 170, 300, 200, 300, 2.0), (0, 225, 390, 265, 390, 2.2), (220, 205, 395, 250, 395, 2.5), (450, 170, 360, 225, 360, 2.5), (640, 150, 320, 215, 320, 2.5), (770, 110, 250, 200, 250, 2.2),
        (850, 40, 110, 90, 110, 2.0)], kante=40))

    # Zaehne: kantige Kegel, oben am Kopf, unten am Kiefer
    def zahn(name, kn, x, y, z, lg, r, nach, mat='klaue'):
        return Teil(name, K[kn], None, 4, [dict(t=0, o=r, r=r * 0.8, u=r, l=r * 0.8, p=1.2)], ursprung=(x, y, z), achse=nach, oben=(1, 0, 0), ende='spitz', spitze=(lg, 0, 0), mat=mat, kante=20)
    for i, (x, z, lg) in enumerate(((1340, 150, 230), (1200, 225, 190), (1040, 270, 180))):
        for sg, s in ((1, 'l'), (-1, 'r')):
            T.append(zahn('zahn_o%d%s' % (i, s), 'kopf', x, 330, sg * z, lg, 62, (0.1, 1, 0)))
    for i, (x, z, lg) in enumerate(((760, 150, 200), (600, 245, 170), (430, 285, 160))):
        for sg, s in ((1, 'l'), (-1, 'r')):
            T.append(zahn('zahn_u%d%s' % (i, s), 'kiefer', x, -110, sg * z, lg, 56, (0.1, -1, 0)))
    # Hauer am Ende des Kamms, knoechern. Rechts im Bild 2 sitzt die Spitze auf P.
    for sg, s in ((1, 'l'), (-1, 'r')):
        T.append(Teil('hauer_' + s, K['kopf'], None, 6, [
            dict(t=0, o=120, r=105, u=120, l=105, p=1.8), dict(t=170, o=120, r=100, u=125, l=100, p=1.7), dict(t=340, o=85, r=70, u=90, l=70, p=1.6)],
            ursprung=(800, 240, sg * 500), achse=(0.62, 0.62, -sg * 0.32), oben=(0, -1, 0), ende='spitz', spitze=(560, 0, 0), mat='klaue', kante=35))
    return T


def schwanz():
    T = []
    daten = [('schwanz1', 592, 312, 322, 333), ('schwanz2', 598, 340, 342, 338), ('schwanz3', 625, 337, 273, 224), ('schwanz4', 662, 224, 187, 166), ('schwanz5', 678, 169, 176, 187)]
    for i, (nm, L, d0, d1, d2) in enumerate(daten):
        d = 16.0 * i

        def ring(t, r, f=1.0, gz=1.0):
            return dict(t=t, o=r * 1.1 * f, r=r * 1.0 * f, u=r * 0.9 * f, l=r * 1.0 * f, p=1.7, dreh=d + 8.0 * t / L, k={0: gz})
        st = [ring(-100, d0, 0.90), ring(0.14 * L, 0.5 * (d0 + d1), 1.05, 1.06), ring(0.5 * L, d1, 1.0), ring(0.82 * L, 0.5 * (d1 + d2), 1.02, 1.05), ring(L + 80, d2, 0.92)]
        if i >= 2:
            st = [ring(-100, d0, 0.90), ring(0.3 * L, 0.6 * d0 + 0.4 * d1, 1.05, 1.06), ring(0.75 * L, 0.5 * (d1 + d2), 1.02, 1.05), ring(L + 80, d2, 0.92)]
        T.append(Teil(nm, K[nm], 'mx', 8, st,
            relief=[(0.3 * L, 90, 0.4 * L, 36, 0.05), (0.3 * L, 270, 0.4 * L, 36, 0.05)]))
    T.append(Teil('schwanz6', K['schwanz6'], 'mx', 8, [
        dict(t=-100, o=190, r=190, u=180, l=190, p=1.8), dict(t=120, o=200, r=230, u=190, l=230, p=1.8), dict(t=330, o=200, r=330, u=190, l=330, p=1.8),
        dict(t=560, o=190, r=430, u=180, l=430, p=1.7), dict(t=780, o=150, r=400, u=150, l=400, p=1.6), dict(t=960, o=90, r=240, u=90, l=240, p=1.6)],
        ende='spitz', spitze=(1120, 0, 0), mat='rosa'))
    return T


def arm(seite, sp):
    s = '_' + seite
    T = []
    tab = [(-170, 150, 170, 170, 150), (-100, 230, 250, 250, 210), (-20, 300, 310, 320, 270, 1.9), (160, 380, 370, 400, 320, 1.8), (360, 380, 340, 410, 300, 1.8), (550, 320, 305, 370, 270, 1.9),
           (700, 270, 280, 310, 250, 2.0), (890, 235, 270, 280, 230, 2.0)]

    def ok(i, t):
        # Trizepskopf aussen als Dorn: Bild 1 zeigt rechts neben dem Arm eine Kante bei (429,272)
        return dict(k={2: {3: 1.16, 4: 1.38, 5: 1.14}.get(i, 1.0)})
    T.append(Teil('oberarm' + s, K['oberarm' + s], 'y', 8, reihe(tab, (-170, -100, -20, 170, 330, 470, 620, 760, 890), ok), anf='spitz', anf_spitze=(-235, 0, 0),
                  relief=[(340, 0, 170, 40, 0.08), (300, 190, 190, 36, 0.08), (560, 95, 100, 15, -0.09), (420, 135, 170, 13, -0.10), (420, 45, 170, 13, -0.08)], spiegel=sp))
    tab = [(-150, 220, 250, 270, 230), (0, 295, 305, 330, 280, 1.8), (190, 345, 335, 350, 305, 1.9), (430, 335, 340, 320, 315, 1.9), (670, 285, 320, 270, 295), (880, 235, 285, 225, 265), (1060, 205, 260, 195, 245),
           (1200, 195, 245, 185, 230)]

    def ak(i, t):
        # Sehnenrippen auf der Innenseite vorn: Ring fuer Ring vor und zurueck, Bild 2 zeigt sie als gerippten Streifen
        if 1 <= i <= 7:
            f = 1.11 if i % 2 else 0.985
            return dict(k={6: 1 + (f - 1) * 0.7, 7: f})
        return {}
    T.append(Teil('unterarm' + s, K['unterarm' + s], 'y', 8, reihe(tab, (-150, 0, 150, 300, 450, 600, 750, 900, 1200), ak),
                  relief=[(240, 55, 200, 30, 0.10), (280, 305, 220, 26, 0.05), (40, 180, 170, 24, 0.20)], spiegel=sp))
    T.append(Teil('hand' + s, K['hand' + s], 'x', 8, [
        (-140, 220, 250, 240, 250, 2.0), (50, 245, 350, 245, 330, 2.4), (260, 225, 470, 215, 410, 2.6), (470, 190, 580, 180, 480, 2.8), (640, 160, 620, 160, 510, 3.0), (760, 100, 500, 105, 430, 3.0)],
        spiegel=sp))
    for nm, L in (('klaue%s1', 1.0), ('klaue%s2', 1.05), ('klaue%s3', 1.25), ('klaue%s4', 0.86)):
        n = nm % s
        T.append(Teil(n, K[n], 'x', 6, [
            (-90, 150, 185, 170, 185, 2.0), (120 * L, 200, 210, 210, 210, 2.0), (330 * L, 180, 190, 190, 190, 1.8, -10, 0), (510 * L, 135, 130, 145, 130, 1.6, -60, 0)],
            ende='spitz', spitze=(720 * L, -170, 0), spiegel=sp, mat='klaue', kante=35))
    return T


def bein(seite, sp):
    s = '_' + seite
    T = []
    T.append(Teil('oberschenkel' + s, K['oberschenkel' + s], 'y', 8, [
        (-350, 170, 230, 200, 190, 2.0), (-210, 380, 340, 400, 280, 2.2), (-20, 480, 380, 520, 320, 2.2), (200, 500, 360, 500, 300, 2.1), (430, 430, 320, 420, 270, 2.0), (640, 320, 270, 300, 240, 2.0),
        (800, 250, 235, 240, 215, 2.0), (920, 230, 215, 205, 200, 2.2), (1050, 215, 205, 200, 190, 2.0)],
        relief=[(150, 20, 200, 36, 0.06), (150, 180, 220, 40, 0.06), (420, 100, 150, 15, -0.08), (350, 225, 200, 14, -0.10)], spiegel=sp))
    T.append(Teil('unterschenkel' + s, K['unterschenkel' + s], 'y', 8, [
        (-150, 200, 190, 200, 180, 2.0), (0, 235, 205, 230, 190, 2.1), (140, 230, 215, 340, 195, 2.0), (300, 210, 215, 390, 195, 1.9), (460, 190, 200, 320, 175, 1.9), (620, 170, 175, 210, 160, 2.0),
        (790, 155, 160, 165, 145, 2.0), (930, 150, 160, 150, 145, 2.0)], relief=[(300, 180, 170, 12, -0.09)], spiegel=sp))
    T.append(Teil('fuss' + s, K['fuss' + s], 'x', 8, [
        (-210, 130, 170, 180, 170, 2.0), (-60, 180, 200, 250, 190, 2.2), (150, 200, 225, 285, 215, 2.4), (400, 215, 250, 295, 235, 2.6), (650, 215, 280, 295, 265, 2.8), (850, 190, 300, 280, 285, 3.0),
        (980, 125, 250, 195, 235, 2.6)], spiegel=sp))
    for i, (z, g) in enumerate(((-190, -16), (0, 0), (190, 16))):
        c, sn = math.cos(math.radians(g)), math.sin(math.radians(g))
        T.append(Teil('zeh%d%s' % (i, s), K['fuss' + s], None, 6, [
            (-100, 110, 110, 150, 110, 2.0), (120, 110, 100, 125, 100, 1.8, -20, 0)],
            ursprung=(930, 170, z), achse=(c, 0.05, sn), ende='spitz', spitze=(420, -90, 0), spiegel=sp, mat='klaue', kante=35))
    return T


# Masse aus der Passung in den Entwurf zurueckgeholt, damit das Korrekturfeld klein bleibt (Mittelwerte q aus Durchgang 4)
SK = {'huefte': 0.90, 'wirbel': 0.90, 'brust': 0.86, 'schwanz1': 0.82, 'schwanz2': 0.90, 'schwanz3': 0.88, 'schwanz4': 0.82, 'schwanz5': 0.76,
      'oberarm_l': 1.18, 'oberschenkel_r': 0.72, 'oberschenkel_l': 0.72, 'unterschenkel_r': 0.82, 'unterschenkel_l': 0.95, 'fuss_r': 0.85, 'fuss_l': 0.93}


def alle():
    T = rumpf() + hals_kopf() + schwanz()
    T += arm('r', False) + arm('l', True) + bein('r', False) + bein('l', True)
    for t in T:
        t.R = t.R * SK.get(t.name, 1.0)
    return T


if __name__ == '__main__':
    tt = alle()
    print(len(tt), 'Teile, Vierecke zwischen Ringen', sum((len(t.st) - 1) * t.n for t in tt))
