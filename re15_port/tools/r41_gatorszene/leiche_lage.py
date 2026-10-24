#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Runde 41 Spur F "gatorszene": Herleitung der Leichen-Lage, der Szenenkamera und der Schluessel-Neigung (PORT-WAHL).

  python re15_port/tools/r41_gatorszene/leiche_lage.py

NUTZER-VORGABE (AUFTRAG.md Nachtrag 6): "Du darfst auch die Startposition des Aligators und die Position der schwimmenden
Zombie Leiche, die sichtbar den Schluessel hat aendern". Der Schluessel ist ein RE2-Bodenmodell (flaches Quadrat 180 x 180,
ROOM7040.RDT MD1 @0x354C, Ring + Bartschluessel), und alle Zonenkameras von ROOM2090 stehen flach ueber dem Wasser
(y -3132): an der ersten Szenen-Lage (-5300,-21000) zeigte Cut 0 ihn als Strich (5,6 x 0,6 Bildpunkte), in Cut 7 war er
bei Lichtwert 78 (nur Umgebungslicht) im Bild nur 33..47 hell (Messlauf k7b). Darum je Zonenkamera (Cut 0..7):
  - Lage: die GANZE Leiche (MD1 @0x2054, alle Ecken) liegt mit Rand im Bild (x 24..296, y 40..205),
  - Buehne: Kopf im Koppelbild 13 auf der Rumpfmitte, Start S = Mitte - Kopf13 - Weg4 - Weg6 (Gier 1024, aus dem Norden
    durch den Westkanal) - S liegt mit Rand 900 im Wasser, die Mitte im Kanal-Streifen (x -8900..-1700, Rand 900),
  - Wert = Bildgroesse^2 x Licht des Schluessels (NCCT-Modell re15_light.h mit dem Lichtsatz DES Cuts, RDT +0x2C -> @0x468,
    40 B je Cut; positionales Licht: Reichweite = Helligkeit >> 4, Abfall nur in XZ).
Danach die Neigung des Schluessels (lokal rx/rz im Elternraum der Leiche, Drehfolge main.c pc_prop_rot_q12): hoechstens
60 Grad gekippt, groesster Wert cos(Blick) x Licht.
Quellen (alle aus der AUSGELIEFERTEN RDT): Kameras RID @0x060 (32 B je Cut: Flag, H, Lage, Ziel; H >> 7 = Bildabstand 208
wie "[F60] cut=5 sd=208"), Lichtsaetze @0x468, Wasserzonen main00 @0x009D6/@0x009EA/@0x009FE (Aot_set sce 8),
Modelltabelle @0x200 (TIM, MD1), Buehnen-Versaetze aus der Messzeile "[gatorszene] Buehne: ... Kopf13-Versatz (45,-3055)
Weg4 (0,-1328) Weg6 (-16,-6332)" (gatorszene_r41.c ga_buehne, Gier 1024, Zeichen-Skala 2/3).
"""
import math, os, struct, sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(os.path.dirname(HIER))
RDT = os.path.join(PORT, "shared_assets", "PSX", "STAGE2", "ROOM2090.RDT")

KOPF13 = (45, -3055); WEG4 = (0, -1328); WEG6 = (-16, -6332)   # Buehnen-Messzeile (Gier 1024)
MITTE_X = 1090                                                # Rumpfmitte im Leichen-Objektraum (gatorszene_r41.c)
KEY_LOKAL = (1090, -420, -100)                                # hoechste Stelle des Rueckens (MD1 y -341 bei x 800..1100, z -100)
LEICHE_Y = -1740                                              # Obj_model_set obj 2 @0x00A6C pc[12..13] = f9 34


def lade():
    d = open(RDT, "rb").read()
    cams = []
    for c in range(d[1]):
        o = 0x60 + c * 32
        flag, h, px, py, pz, tx, ty, tz, pri = struct.unpack_from("<HHiiiiiiI", d, o)
        cams.append(((px, py, pz), (tx, ty, tz), h >> 7))
    lichter = []
    basis = struct.unpack_from("<I", d, 0x2C)[0]
    for c in range(d[1]):
        o = basis + c * 40
        lichter.append((d[o], list(d[o + 1:o + 4]), [list(d[o + 4 + 3 * i:o + 7 + 3 * i]) for i in range(3)],
                        list(d[o + 13:o + 16]), [struct.unpack_from("<3h", d, o + 16 + 6 * i) for i in range(3)],
                        struct.unpack_from("<3H", d, o + 34)))
    zonen = []
    for o in (0x9D6, 0x9EA, 0x9FE):
        assert d[o] == 0x2C and d[o + 2] == 0x08, "keine sce-8-Zone @0x%X" % o
        x, z, w, t = struct.unpack_from("<hhHH", d, o + 6)
        zonen.append((x, z, x + w, z + t))
    tim, md1 = struct.unpack_from("<II", d, 0x200 + 2 * 8)
    assert md1 == 0x2054
    n = struct.unpack_from("<I", d, md1 + 8)[0]
    vs = []
    for k in range(n):
        f = struct.unpack_from("<7I", d, md1 + 12 + k * 28)
        for j in range(f[1]):
            vs.append(struct.unpack_from("<3h", d, md1 + 12 + f[0] + j * 8))
    return cams, lichter, zonen, vs


def norm(v):
    l = math.sqrt(sum(a * a for a in v)); return [a / l for a in v]


def licht(ls, P, N):
    """Gruen-Kanal des NCCT-Werts (re15_light.h): Umgebung + Summe Farbe x Abfall x max(0, L.N)."""
    gs, typ, farbe, umg, lage, hell = ls
    wert = umg[1]
    for i in range(3):
        if typ[i] == 0:
            d = [(P[k] - lage[i][k]) / 16 for k in range(3)]
            dist = math.hypot(d[0], d[2]); rw = hell[i] / 16
            ab = max(0.0, (rw - dist) / rw)
            n = norm(d); L = [-gs * a for a in n]
        else:
            ab = 1.0; n = norm(lage[i]); L = [-gs * a for a in n]
        wert += farbe[i][1] * ab * max(0.0, sum(L[k] * N[k] for k in range(3)))
    return wert


def S(a): return math.sin(a * 2 * math.pi / 4096)
def C(a): return math.cos(a * 2 * math.pi / 4096)
def M(rx, ry, rz):   # main.c pc_prop_rot_q12 (Gleitkomma)
    sx, cx, sy, cy, sz, cz = S(rx), C(rx), S(ry), C(ry), S(rz), C(rz)
    return [cz * cy, -sz * cy, sy, sz * cx + cz * sy * sx, cz * cx - sz * sy * sx, -cy * sx,
            sz * sx - cz * sy * cx, cz * sx + sz * sy * cx, cy * cx]
def mul(a, b): return [sum(a[r * 3 + k] * b[k * 3 + c] for k in range(3)) for r in range(3) for c in range(3)]
def mv(a, v): return [sum(a[r * 3 + k] * v[k] for k in range(3)) for r in range(3)]


def im_wasser(zonen, x, z, m):
    return any(a + m <= x <= c - m and b + m <= z <= e - m for a, b, c, e in zonen)


def suche(cut, cams, lichter, zonen, V, np):
    p, t, sd = cams[cut]
    f = np.array(norm([t[i] - p[i] for i in range(3)])); r = np.array(norm([f[2], 0, -f[0]])); u = np.cross(f, r)
    best = None
    for ry in range(0, 4096, 128):
        R = M(0, ry, 0)
        W = V @ np.array(R).reshape(3, 3).T
        ext = [mv(R, v) for v in ((-444, 0, 0), (2626, 0, 0), (1090, 0, -1049), (1090, 0, 822))]
        kl = mv(R, KEY_LOKAL)
        for x in range(-8500, 7000, 100):
            for z in range(-26800, -5600, 100):
                if not all(im_wasser(zonen, x + e[0], z + e[2], 250) for e in ext):
                    continue
                mx = x + MITTE_X * C(ry); mz = z - MITTE_X * S(ry)
                sx_ = mx - KOPF13[0] - WEG4[0] - WEG6[0]; sz_ = mz - KOPF13[1] - WEG4[1] - WEG6[1]
                if not (im_wasser(zonen, sx_, sz_, 900) and zonen[2][0] + 900 <= mx <= zonen[2][2] - 900):
                    continue
                Q = W + np.array([x, LEICHE_Y, z]) - np.array(p, dtype=float)
                Z = Q @ f
                if Z.min() <= 300:
                    continue
                bx = 160 + sd * (Q @ r) / Z; by = 120 + sd * (Q @ u) / Z
                if bx.min() < 24 or bx.max() > 296 or by.min() < 40 or by.max() > 205:
                    continue
                kw = [x + kl[0], LEICHE_Y + kl[1], z + kl[2]]
                gr = 180 * sd / math.dist(kw, p)
                wert = gr * gr * licht(lichter[cut], kw, (0, -1, 0))
                if best is None or wert > best[0]:
                    best = (wert, x, z, ry, (bx.min(), by.min(), bx.max(), by.max()), (mx, mz), (sx_, sz_), gr, kw)
    return best


def main():
    import numpy as np
    cams, lichter, zonen, vs = lade()
    V = np.array(vs, dtype=float)
    alle = []
    for cut in range(8):
        b = suche(cut, cams, lichter, zonen, V, np)
        if b:
            print("Cut %d: Wert %6.0f  Leiche (%d,%d) Gier %d  Schluessel %.1f Bildpunkte, Licht flach %.1f" %
                  (cut, b[0], b[1], b[2], b[3], b[7], licht(lichter[cut], b[8], (0, -1, 0))))
            alle.append((b[0], cut, b))
        else:
            print("Cut %d: keine Lage" % cut)
    wert, cut, (w, x, z, ry, box, mitte, start, gr, kw) = max(alle)
    print("GEWAEHLT Cut %d: Leiche (%d,%d,%d) Gier %d, Bildkasten (%.0f,%.0f)-(%.0f,%.0f)" % (cut, x, LEICHE_Y, z, ry, *box))
    print("  Rumpfmitte (%.0f,%.0f) -> Gator-Start S (%.0f,%.0f) Gier 1024 (Nordbecken)" % (*mitte, *start))
    Rp = M(0, ry, 0)
    cam = cams[cut][0]
    dv = norm([cam[i] - kw[i] for i in range(3)])
    flach_cos = sum(-Rp[3 * i + 1] * dv[i] for i in range(3))
    flach_l = licht(lichter[cut], kw, [-Rp[1], -Rp[4], -Rp[7]])
    beste = None
    for rx in range(-1024, 1025, 32):
        for rz in range(-1024, 1025, 32):
            Rw = mul(Rp, M(rx, 0, rz))
            n = [-Rw[1], -Rw[4], -Rw[7]]
            kipp = math.degrees(math.acos(max(-1.0, min(1.0, -n[1]))))
            cv = sum(n[i] * dv[i] for i in range(3))
            if kipp > 60 or cv <= 0:
                continue
            l = licht(lichter[cut], kw, n)
            if beste is None or cv * l > beste[0]:
                beste = (cv * l, rx, rz, kipp, cv, l)
    _, rx, rz, kipp, cv, l = beste
    print("  Schluessel Welt (%.0f,%.0f,%.0f), Blick zur Kamera %.1f Grad ueber dem Wasser" %
          (*kw, math.degrees(math.asin(-dv[1]))))
    print("  flach: cos %.3f Licht %.1f  ->  lokal rx %d rz %d (Kippung %.1f Grad): cos %.3f Licht %.1f; ~%.1f x %.1f Bildpunkte" %
          (flach_cos, flach_l, rx, rz, kipp, cv, l, gr, gr * cv))
    return 0


if __name__ == "__main__":
    sys.exit(main())
