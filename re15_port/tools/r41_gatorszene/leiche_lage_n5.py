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
mit der GERADEN Kette (Durchbruch Clip 6 ganz, Fressen am Ende der Kette) zugleich gehen (Ergebnis: (1)+(2) KEINE Lage).

  python leiche_lage_n5.py --m2 [--raster N]
      Punkt (2) allein ueber ALLE Lagen und alle 13 Kameras mit der vollen Pruefung (ganze Leiche im Bild, groesstes cos(Blick) bei
      Licht in [168, Puls-Grenze] ueber alle Normalen <= 60 Grad, Schluessel >= 10,4 x 9,5): Ergebnis nur Cut 1 bei x -5500..-5200 /
      z -24700..-23700 Gier 384 = die Fassung-1-Stelle (2700..3300 von der Leiter).
  python leiche_lage_n5.py --kandidaten <clip6.txt> > kand.txt
      Vorauswahl fuer `test_r41_gatorszene lagesuche kand.txt` (der ECHTE Ablauf entscheidet): Leiche ganz im Bild der Ankunftskamera
      Cut 0, Licht >= 168 moeglich, Kette Durchbruch (Clip 6 Bild 1..k, k <= 40, Weg aus probe_clip6_n5.c) + Biss im Wasser mit
      Wurzel-Rand 1500 (ueber der Klemme hit_radius_min 1466 des Boss-Takts) und Kopf-Rand 500; Raster 200, Leichen-Gier 128,
      Anlauf-Gier 1408..2304 (32), je Lage/Gier das groesste k. Zeilen "x z gier_leiche anlauf_gier k".
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


def m2_voll(raster):
    """Punkt (2) allein: alle Lagen, alle Kameras, volle Pruefung (Normalen-Raster 2 x 4 Grad)."""
    cams, lichter, zonen, vs = L.lade()
    V = np.array(vs, float); H = N4.huelle(V)
    grenze = 255 * 0x80 / 0xBE
    ns = []
    for a in range(0, 61, 2):
        for b in range(0, 360, 4):
            t = math.radians(a); p = math.radians(b)
            ns.append((math.sin(t) * math.cos(p), -math.cos(t), math.sin(t) * math.sin(p)))
    NN = np.array(ns)

    def licht_vec(ls, P, k):
        gs, typ, farbe, umg, lage, hell = ls
        w = np.full(len(NN), float(umg[k]))
        for i in range(3):
            d = np.array([(P[j] - lage[i][j]) / 16 for j in range(3)])
            dist = math.hypot(d[0], d[2]); rw = hell[i] / 16
            ab = max(0.0, (rw - dist) / rw)
            if ab <= 0:
                continue
            n = d / np.linalg.norm(d)
            w += farbe[i][k] * ab * np.maximum(0.0, NN @ (-gs * n))
        return w
    B = [cam_basis(c) for c in cams]
    ok = []
    for ry in range(0, 4096, 64):
        R = np.array(L.M(0, ry, 0)).reshape(3, 3); Hw = H @ R.T
        kl = L.mv(L.M(0, ry, 0), L.KEY_LOKAL)
        for x in range(-8000, 6401, raster):
            for z in range(-26400, -5800, raster):
                if any(N4.draussen(x + (R @ np.array(e, float))[0], z + (R @ np.array(e, float))[2], 250) > 0
                       for e in ((-444, 0, 0), (2626, 0, 0), (1090, 0, -1049), (1090, 0, 822))):
                    continue
                kw = [x + kl[0], L.LEICHE_Y + kl[1], z + kl[2]]
                for cut, (p, f, r, u, sd) in enumerate(B):
                    gr = 180 * sd / math.dist(kw, p)
                    if gr < KEY_MIN[0]:
                        continue
                    Q = Hw + np.array([x, L.LEICHE_Y, z]) - p
                    Zc = Q @ f
                    if Zc.min() <= 300:
                        continue
                    bx = 160 + sd * (Q @ r) / Zc; by = 120 + sd * (Q @ u) / Zc
                    if bx.min() < 24 or bx.max() > 296 or by.min() < 40 or by.max() > 205:
                        continue
                    l = np.max([licht_vec(lichter[cut], kw, k) for k in range(3)], axis=0)
                    dv = np.array(L.norm([p[i] - kw[i] for i in range(3)]))
                    cv = NN @ dv
                    m = (l >= LICHT_MIN) & (l <= grenze) & (cv > 0)
                    if m.any() and gr * cv[m].max() >= KEY_MIN[1]:
                        ok.append((cut, x, z, ry, gr, cv[m].max()))
    print("Punkt (2) wie abgenommen moeglich: %d Lagen" % len(ok))
    for o in sorted(ok, key=lambda o: -o[4] * o[5])[:20]:
        print("  Cut %d Leiche (%d,%d) Gier %d: %.1f x %.1f Bildpunkte" % (o[0], o[1], o[2], o[3], o[4], o[4] * o[5]))
    return 0


def kandidaten(clip6):
    """Vorauswahl fuer test_r41_gatorszene lagesuche (s. Kopf)."""
    import re
    rest = {}
    for line in open(clip6):
        m = re.match(r"f\s+(\d+) .*Rest ab f \(2/3\) \(\s*(-?\d+),\s*(-?\d+)\)", line)
        if m:
            rest[int(m.group(1))] = (float(m.group(2)), float(m.group(3)))
    tot = rest[0]
    cams, lichter, zonen, vs = L.lade()
    V = np.array(vs, float); H = N4.huelle(V)
    p, f, r, u, sd = cam_basis(cams[0])
    best = {}
    for ry in range(0, 4096, 128):
        R = np.array(L.M(0, ry, 0)).reshape(3, 3); Hw = H @ R.T
        kl = L.mv(L.M(0, ry, 0), L.KEY_LOKAL)
        for x in range(-1400, 2001, 200):
            for z in range(-26400, -22599, 200):
                if any(N4.draussen(x + (R @ np.array(e, float))[0], z + (R @ np.array(e, float))[2], 250) > 0
                       for e in ((-444, 0, 0), (2626, 0, 0), (1090, 0, -1049), (1090, 0, 822))):
                    continue
                kw = [x + kl[0], L.LEICHE_Y + kl[1], z + kl[2]]
                if 180 * sd / math.dist(kw, p) < 9.5:
                    continue
                ls = lichter[0]
                lmax = ls[3][1] + sum(ls[2][i][1] * max(0.0, 1 - math.hypot((kw[0] - ls[4][i][0]) / 16, (kw[2] - ls[4][i][2]) / 16)
                                                           / (ls[5][i] / 16)) for i in range(3))
                if lmax < LICHT_MIN:
                    continue
                Q = Hw + np.array([x, L.LEICHE_Y, z]) - p
                Zc = Q @ f
                if Zc.min() <= 300:
                    continue
                bx = 160 + sd * (Q @ r) / Zc; by = 120 + sd * (Q @ u) / Zc
                if bx.min() < 24 or bx.max() > 296 or by.min() < 40 or by.max() > 205:
                    continue
                mx = x + L.MITTE_X * L.C(ry); mz = z - L.MITTE_X * L.S(ry)
                for th in range(1408, 2305, 32):
                    h = (L.C(th), -L.S(th))
                    for k in range(40, 15, -1):
                        w6 = (tot[0] - rest[k][0], tot[1] - rest[k][1])
                        k13 = N4.welt(th, N4.KOPF13_R); w4 = N4.welt(th, N4.WEG4_R); w6w = N4.welt(th, w6)
                        r13 = (mx - k13[0], mz - k13[1]); a = (r13[0] - w4[0], r13[1] - w4[1]); s0 = (a[0] - w6w[0], a[1] - w6w[1])
                        kau = (r13[0] + 220 * h[0], r13[1] + 220 * h[1])
                        t = np.linspace(0.0, 1.0, 60)
                        X = s0[0] + (kau[0] - s0[0]) * t; Z = s0[1] + (kau[1] - s0[1]) * t
                        if (N4.draussen(X, Z, 1500) > 0).any() or (N4.draussen(X + k13[0], Z + k13[1], 500) > 0).any():
                            continue
                        best[(x, z, ry, th)] = k
                        break
    for (x, z, ry, th), k in sorted(best.items()):
        print("%d %d %d %d %d" % (x, z, ry, th, k))
    print("Kandidaten: %d" % len(best), file=sys.stderr)
    return 0


def main():
    global RY, TH
    if "--m2" in sys.argv:
        return m2_voll(int(sys.argv[sys.argv.index("--raster") + 1]) if "--raster" in sys.argv else 100)
    if "--kandidaten" in sys.argv:
        return kandidaten(sys.argv[sys.argv.index("--kandidaten") + 1])
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
