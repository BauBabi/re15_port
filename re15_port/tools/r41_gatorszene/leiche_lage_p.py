#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Runde 41 Spur P "alligator2" (Nachtrag 31 Punkt 2 "nicht vor der Kamera"): Vorauswahl fuer `test_r41_alligator2 lagesuche2 <datei>`.

  python re15_port/tools/r41_gatorszene/leiche_lage_p.py <clip6.txt> [rand_wurzel=1500] [kamera_min=3500] [raster=100] > kand.txt

Form wie leiche_lage_n5.py --kandidaten (Leiche ganz im Bild der Ankunftskamera Cut 0, Licht >= 168 moeglich, Kette Durchbruch + Biss im Wasser mit
Wurzel-Rand 1500 und Kopf-Rand 500; clip6.txt = Ausgabe von probe_clip6_n5.c), dazu die Kampfbeginn-Schaetzung: Kau-Lage + 700 von der Leiter weg (Schrei-
Rueckstoss, leiche_lage_n4.py WENDE_SCHREI) in Zone 0 (z <= -18100), Abstand zur Leiter im RE2-Band, mindestens kamera_min von der Kamera Cut 0 (RID @0x060).
Der echte Ablauf entscheidet: lagesuche2 misst je Kandidat Szene, Spur-B-Kamerawahl, Kampfbeginn und 150 Bilder Abstand der Gator-Teile zur Kamera.
Zeilen "x z gier_leiche anlauf_gier durchbruch_bis".
"""
import math, os, re, sys
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import leiche_lage as L
import leiche_lage_n4 as N4


def main():
    clip6 = sys.argv[1]
    rand_w = float(sys.argv[2]) if len(sys.argv) > 2 else 1500
    kam_min = float(sys.argv[3]) if len(sys.argv) > 3 else 3500
    schritt = int(sys.argv[4]) if len(sys.argv) > 4 else 100
    rest = {}
    for line in open(clip6):
        m = re.match(r"f\s+(\d+) .*Rest ab f \(2/3\) \(\s*(-?\d+),\s*(-?\d+)\)", line)
        if m:
            rest[int(m.group(1))] = (float(m.group(2)), float(m.group(3)))
    tot = rest[0]
    cams, lichter, zonen, vs = L.lade()
    H = N4.huelle(np.array(vs, float))
    p0, t0, sd0 = cams[0]
    f = np.array(L.norm([t0[i] - p0[i] for i in range(3)]))
    r = np.array(L.norm([f[2], 0, -f[0]]))
    u = np.cross(f, r)
    p0a = np.array(p0, float)
    pos = []
    for ry in range(0, 4096, 128):
        R = np.array(L.M(0, ry, 0)).reshape(3, 3)
        Hw = H @ R.T
        kl = L.mv(L.M(0, ry, 0), L.KEY_LOKAL)
        for x in range(-3000, 3001, schritt):
            for z in range(-26500, -20500, schritt):
                if any(N4.draussen(x + (R @ np.array(e, float))[0], z + (R @ np.array(e, float))[2], 250) > 0
                       for e in ((-444, 0, 0), (2626, 0, 0), (1090, 0, -1049), (1090, 0, 822))):
                    continue
                kw = [x + kl[0], L.LEICHE_Y + kl[1], z + kl[2]]
                if 180 * sd0 / math.dist(kw, p0) < 7.5:
                    continue
                ls = lichter[0]
                lmax = ls[3][1] + sum(ls[2][i][1] * max(0.0, 1 - math.hypot((kw[0] - ls[4][i][0]) / 16, (kw[2] - ls[4][i][2]) / 16) / (ls[5][i] / 16))
                                      for i in range(3))
                if lmax < 168:
                    continue
                Q = Hw + np.array([x, L.LEICHE_Y, z]) - p0a
                Zc = Q @ f
                if Zc.min() <= 300:
                    continue
                bx = 160 + sd0 * (Q @ r) / Zc
                by = 120 + sd0 * (Q @ u) / Zc
                if bx.min() < 24 or bx.max() > 296 or by.min() < 40 or by.max() > 205:
                    continue
                pos.append((x, z, ry))
    print("Lagen Cut 0:", len(pos), file=sys.stderr)
    out = []
    le = np.array(N4.LEITER, float)
    for (x, z, ry) in pos:
        mx = x + L.MITTE_X * L.C(ry)
        mz = z - L.MITTE_X * L.S(ry)
        for th in range(1200, 2305, 32):
            h = (L.C(th), -L.S(th))
            k13 = N4.welt(th, N4.KOPF13_R)
            w4 = N4.welt(th, N4.WEG4_R)
            r13 = (mx - k13[0], mz - k13[1])
            a = (r13[0] - w4[0], r13[1] - w4[1])
            kau = (r13[0] + 220 * h[0], r13[1] + 220 * h[1])
            wg = np.array(kau) - le
            wg /= np.linalg.norm(wg)
            ende = np.array(kau) + N4.WENDE_SCHREI * wg
            ab_e = np.linalg.norm(ende - le)
            if not (11300 <= ab_e <= 13950) or ende[1] > -18100:
                continue
            if math.hypot(ende[0] - p0[0], ende[1] - p0[2]) < kam_min:
                continue
            for k in (22, 26, 30, 34, 40, 18):
                w6 = (tot[0] - rest[k][0], tot[1] - rest[k][1])
                w6w = N4.welt(th, w6)
                s = (a[0] - w6w[0], a[1] - w6w[1])
                t = np.linspace(0.0, 1.0, 60)
                X = s[0] + (kau[0] - s[0]) * t
                Z = s[1] + (kau[1] - s[1]) * t
                if (N4.draussen(X, Z, rand_w) > 0).any() or (N4.draussen(X + k13[0], Z + k13[1], 500) > 0).any():
                    continue
                out.append((x, z, ry, th, k))
                break
    print("Kandidaten:", len(out), file=sys.stderr)
    for o in out:
        print("%d %d %d %d %d" % o)
    return 0


if __name__ == "__main__":
    sys.exit(main())
