#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Runde 41 Spur F "gatorszene", Nachbesserung 5 (Abnahme 4 M1-M3 = Orchestrator-Entscheid F-E2): ZULAESSIGKEIT der Lagen.

  python re15_port/tools/r41_gatorszene/leiche_lage_n5.py [--raster N]

Prueft fuer JEDE Lage der Leiche (Rumpfmitte x/z, Raster), jede Leichen-Gier und jede Anlauf-Gier des Gators die drei Punkte von
F-E2 mit dem Kettenmodell der Nachbesserung 4 (leiche_lage_n4.py: Kopf13 / Weg4 / Weg6 / Kau aus der Buehnen-Messzeile, Wasser =
gb_becken_draussen, Wurzel Rand 900, Kopf Rand 500):
  (1) Kampfbeginn = RE2: Gator am Kampfbeginn in Leons Beckenzone 0 (gb_zone: z <= -18100, RE2: kein Zonenwechsel vor dem Kontakt,
      Zonenbyte +0x21A @0x80100990-ec, probe_re2_band_n5.c) und Abstand zur Leiter-Ankunft im RE2-Band [11430, 13924]
      (probe_re2_band_n5.c: Intro-Ende .. fruehester Kampfbeginn Cut 1 + 113), Kampfbeginn ~ Kau-Lage +- 700 (Wende + Schrei);
  (2) Schluessel in einer Ankunftskamera (alle 13 Cuts): ganze Leiche im Bild, Licht (beste Neigung <= 60 Grad unter der Puls-
      Grenze) >= 168 (Primitivfarbe Ruhe >= 84 = abgenommene Fassung 1), Schluessel >= 10,4 x 9,5 Bildpunkte (Fassung 1);
  (3) Fressen von vorn: eine Zonenkamera (Cut 0..7) mit Kopf (= Rumpfmitte im Koppelbild) im Bild, Tiefe >= 4804 (Massstab-Grenze
      1892: 208 * 2731 * 16 / Tiefe, gator_re2_r41.c gr2_massstab), Frontalwert cos(Anlauf, Kopf -> Kamera) > 0.
Ausgabe: Zahl der Lagen je Kombination der erfuellten Punkte, die besten Lagen fuer (1)+(2), (1)+(3), (2)+(3) - zeigt, ob alle drei
mit der GERADEN Kette (Durchbruch Clip 6 ganz, Fressen am Ende der Kette) zugleich gehen.
"""
import math, os, sys
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import leiche_lage as L
import leiche_lage_n4 as N4

BAND = (11430, 13924)
KEY_MIN = (10.4, 9.5)
LICHT_MIN = 168.0
TIEFE_MIN = 208 * 2731 * 16 / 1892.0
RY = 256
TH = 64


def cam_basis(cam):
    p, t, sd = cam
    f = np.array(L.norm([t[i] - p[i] for i in range(3)])); r = np.array(L.norm([f[2], 0, -f[0]])); u = np.cross(f, r)
    return np.array(p, float), f, r, u, sd


def main():
    global RY, TH
    raster = int(sys.argv[sys.argv.index("--raster") + 1]) if "--raster" in sys.argv else 200
    if "--grob" in sys.argv:
        RY, TH = 512, 128
    cams, lichter, zonen, vs = L.lade()
    V = np.array(vs, float)
    H = N4.huelle(V)
    basen = [cam_basis(c) for c in cams]
    grenze = 255 * 0x80 / 0xBE
    zaehl = {}
    beste = {}
    xs = np.arange(-8000, 6400, raster); zs = np.arange(-26400, -5800, raster)
    for ry in range(0, 4096, RY):
        R = np.array(L.M(0, ry, 0)).reshape(3, 3)
        Hw = H @ R.T
        kl = L.mv(L.M(0, ry, 0), L.KEY_LOKAL)
        for x in xs:
            for z in zs:
                # Leiche im Wasser (vier Aussenpunkte, Rand 250 wie Fassung 1)
                ok = True
                for e in ((-444, 0, 0), (2626, 0, 0), (1090, 0, -1049), (1090, 0, 822)):
                    w = R @ np.array(e, float)
                    if N4.draussen(x + w[0], z + w[2], 250) > 0:
                        ok = False; break
                if not ok:
                    continue
                mx = x + L.MITTE_X * L.C(ry); mz = z - L.MITTE_X * L.S(ry)
                kw = [x + kl[0], L.LEICHE_Y + kl[1], z + kl[2]]
                # (2) Ankunftskamera
                p2 = False; p2_info = None
                for cut, (p, f, r, u, sd) in enumerate(basen):
                    Q = Hw + np.array([x, L.LEICHE_Y, z]) - p
                    Zc = Q @ f
                    if Zc.min() <= 300:
                        continue
                    bx = 160 + sd * (Q @ r) / Zc; by = 120 + sd * (Q @ u) / Zc
                    if bx.min() < 24 or bx.max() > 296 or by.min() < 40 or by.max() > 205:
                        continue
                    gr = 180 * sd / math.dist(kw, p)
                    if gr < KEY_MIN[0]:
                        continue
                    # obere Schranke des Lichts (flach, bester Kanal) - die Neigung kann nur bis zur Grenze heben
                    l0 = max(L.licht(lichter[cut], kw, (0, -1, 0), k) for k in range(3))
                    lmax = lichter[cut][3][1] + sum(lichter[cut][2][i][1] * max(0.0, 1 - math.hypot(
                        (kw[0] - lichter[cut][4][i][0]) / 16, (kw[2] - lichter[cut][4][i][2]) / 16) / (lichter[cut][5][i] / 16))
                        for i in range(3))
                    if lmax < LICHT_MIN:
                        continue
                    bn, _ = N4.neigung(cams, lichter, cut, ry, kw)
                    if bn is None:
                        continue
                    _, rx, rz, kipp, cv, l = bn
                    if l >= LICHT_MIN and gr * cv >= KEY_MIN[1]:
                        p2 = True; p2_info = (cut, l, gr, gr * cv); break
                # Ketten je Anlauf-Gier
                for th in range(0, 4096, TH):
                    s, r13, kau, k13 = N4.kette(th, mx, mz)
                    if not N4.weg_ok(s, kau, k13):
                        continue
                    ab = math.dist(kau, N4.LEITER)
                    p1 = kau[1] <= -18100 - 700 and BAND[0] - 700 <= ab <= BAND[1] + 700
                    # (3) Fress-Kamera von vorn
                    h = (L.C(th), -L.S(th))
                    p3 = False
                    for cut in range(8):
                        p, f, rr, u, sd = basen[cut]
                        q = np.array([mx, L.LEICHE_Y, mz]) - p
                        tiefe = q @ f
                        if tiefe < TIEFE_MIN:
                            continue
                        bx = 160 + sd * (q @ rr) / tiefe; by = 120 + sd * (q @ u) / tiefe
                        if not (0 <= bx < 320 and 0 <= by < 240):
                            continue
                        d = (p[0] - mx, p[2] - mz); n = math.hypot(*d)
                        if (h[0] * d[0] + h[1] * d[1]) / n > 0:
                            p3 = True; break
                    key = (p1, p2, p3)
                    zaehl[key] = zaehl.get(key, 0) + 1
                    for paar in ((0, 1), (0, 2), (1, 2), (0, 1, 2)):
                        if all(key[i] for i in paar):
                            if paar not in beste or ab < beste[paar][0]:
                                beste[paar] = (ab, x, z, ry, th, kau, p2_info)
    print("Lagen x Anlauf-Gieren mit Kette im Wasser (Raster %d, Leichen-Gier 256, Anlauf-Gier 64):" % raster)
    for k in sorted(zaehl):
        print("  (1) Kampfbeginn %-5s (2) Schluessel %-5s (3) Fressen von vorn %-5s : %d" % (k[0], k[1], k[2], zaehl[k]))
    for paar in ((0, 1), (0, 2), (1, 2), (0, 1, 2)):
        b = beste.get(paar)
        print("Punkte %s zugleich: %s" % ("+".join(str(i + 1) for i in paar),
              "KEINE Lage" if b is None else "z.B. Leiche (%d,%d) Gier %d, Anlauf-Gier %d, Kau-Lage (%.0f,%.0f) Abstand %.0f, "
              "Ankunft %s" % (b[1], b[2], b[3], b[4], b[5][0], b[5][1], b[0], b[6])))
    return 0


if __name__ == "__main__":
    sys.exit(main())
