# -*- coding: utf-8 -*-
"""Runde 34 Nacht, Spur E: KONTROLLABZUG der vier Dokument-Props in ihren Hintergruenden.

Vorbild analysis/befunde_runde30/werkzeuge/r30_idw_abzug.py (Irons Diary). Zeichnet das RE2-
Weltmodell (MD1/TIM unveraendert aus extracted_re2_dokumente/weltmodelle) mit der Sichtmatrix der
ENGINE (geom.py <- Sonde `kamera`) und derselben Prop-Drehmatrix wie platform/pc/main.c
pc_prop_rot_q12 (reines rot_y: Modell (x,y,z) -> Welt (c*x + s*z, y, -s*x + c*z)) in die
Hintergruende. Dazu die Original-Props des Raums aus der RDT (0x30-Tabelle), wo sie im Weg liegen.

NICHT byte-true und bewusst so: affine Texturabbildung, Tiefenpuffer statt OT, KEINE Masken, das
Licht als EIN Faktor je Raum (Vertexfarbe der Normalen -Y aus der Sonde `licht` / 128). Der Abzug
prueft LAGE, GROESSE und AUSRICHTUNG, nicht das Endbild - das misst die Abnahme am Spiel (Abschnitt 6).

Aufruf: python re15_port/tools/r34n_e/kontrollabzug.py [Ausgabeverzeichnis]
"""
import math
import os
import struct
import sys

import numpy as np
from PIL import Image, ImageDraw

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import geom  # noqa: E402

REPO = geom.REPO
WELT = os.path.join(REPO, "extracted_re2_dokumente", "weltmodelle")
CD = os.path.join(REPO, "re15_port", "shared_assets", "PSX")
SS = 4


# ------------------------------------------------------------------ Formate
def tim_laden(b, o=0):
    magic, flags = struct.unpack_from("<II", b, o)
    assert magic == 0x10
    p = o + 8
    clen, cx, cy, cw, ch = struct.unpack_from("<IHHHH", b, p)
    clut = [list(struct.unpack_from("<%dH" % cw, b, p + 12 + 2 * cw * r)) for r in range(ch)]
    p += clen
    ilen, ix, iy, iw, ih = struct.unpack_from("<IHHHH", b, p)
    bpp = 8 if (flags & 7) == 1 else 4
    W = iw * 2 if bpp == 8 else iw * 4
    raw = b[p + 12:p + 12 + iw * 2 * ih]
    if bpp == 8:
        pix = np.frombuffer(raw, np.uint8).reshape(ih, W)
    else:
        a = np.frombuffer(raw, np.uint8).reshape(ih, iw * 2)
        pix = np.zeros((ih, W), np.uint8)
        pix[:, 0::2] = a & 15
        pix[:, 1::2] = a >> 4
    return dict(pix=pix, clut=clut, cx=cx, cy=cy, bpp=bpp, ix=ix, iy=iy)


def md1_laden(b, o=0):
    objc = struct.unpack_from("<I", b, o + 8)[0]
    polys = []
    base = o + 12
    for m in range(objc // 2):
        f = struct.unpack_from("<14I", b, o + 12 + 56 * m)
        tvo, tvc, tno, tnc, tfo, tfc, tuo, qvo, qvc, qno, qnc, qfo, qfc, quo = f
        tv = [struct.unpack_from("<3h", b, base + tvo + 8 * k) for k in range(tvc)]
        qv = [struct.unpack_from("<3h", b, base + qvo + 8 * k) for k in range(qvc)]
        for k in range(tfc):
            n0, v0, n1, v1, n2, v2 = struct.unpack_from("<6H", b, base + tfo + 12 * k)
            u = struct.unpack_from("<BBHBBHBBH", b, base + tuo + 12 * k)
            polys.append(dict(v=[tv[v0], tv[v1], tv[v2]], uv=[(u[0], u[1]), (u[3], u[4]), (u[6], u[7])],
                              clut=u[2], page=u[5]))
        for k in range(qfc):
            idx = struct.unpack_from("<8H", b, base + qfo + 16 * k)
            u = struct.unpack_from("<BBHBBHBBHBBH", b, base + quo + 16 * k)
            polys.append(dict(v=[qv[idx[1]], qv[idx[3]], qv[idx[5]], qv[idx[7]]],
                              uv=[(u[0], u[1]), (u[3], u[4]), (u[6], u[7]), (u[9], u[10])],
                              clut=u[2], page=u[5]))
    return polys


def rdt_prop(raum, obj):
    d = open(os.path.join(CD, "STAGE%s" % raum[0], "ROOM%s.RDT" % raum), "rb").read()
    tbl = struct.unpack_from("<I", d, 0x30)[0]
    tim_off, md1_off = struct.unpack_from("<II", d, tbl + 8 * obj)
    return md1_laden(d, md1_off), tim_laden(d, tim_off)


# ------------------------------------------------------------------ Zeichnen
def tex_farbe(tim, u, v, clut, page, licht):
    pu = int(u) + (page & 0x0F) * (128 if tim["bpp"] == 8 else 256) - 0
    pv = int(v)
    pix = tim["pix"]
    pu = min(max(pu, 0), pix.shape[1] - 1)
    pv = min(max(pv, 0), pix.shape[0] - 1)
    row = ((clut >> 6) & 0x1FF) - tim["cy"]
    col = (clut & 0x3F) * 16 - tim["cx"]
    row = min(max(row, 0), len(tim["clut"]) - 1)
    c = tim["clut"][row][min(max(col + int(pix[pv, pu]), 0), len(tim["clut"][row]) - 1)]
    if c == 0:
        return None
    rgb = np.array([(c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3], float)
    return np.clip(rgb * licht, 0, 255)


def dreieck(bild, zbuf, P, UV, farbe_fn):
    (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) = P
    minx = max(int(math.floor(min(x0, x1, x2))), 0)
    maxx = min(int(math.ceil(max(x0, x1, x2))), bild.shape[1] - 1)
    miny = max(int(math.floor(min(y0, y1, y2))), 0)
    maxy = min(int(math.ceil(max(y0, y1, y2))), bild.shape[0] - 1)
    den = (y1 - y2) * (x0 - x2) + (x2 - x1) * (y0 - y2)
    if abs(den) < 1e-9 or minx > maxx or miny > maxy:
        return 0
    n = 0
    for y in range(miny, maxy + 1):
        for x in range(minx, maxx + 1):
            px, py = x + 0.5, y + 0.5
            w0 = ((y1 - y2) * (px - x2) + (x2 - x1) * (py - y2)) / den
            w1 = ((y2 - y0) * (px - x2) + (x0 - x2) * (py - y2)) / den
            w2 = 1 - w0 - w1
            if w0 < 0 or w1 < 0 or w2 < 0:
                continue
            z = w0 * z0 + w1 * z1 + w2 * z2
            if z >= zbuf[y, x]:
                continue
            c = farbe_fn(w0 * UV[0][0] + w1 * UV[1][0] + w2 * UV[2][0], w0 * UV[0][1] + w1 * UV[1][1] + w2 * UV[2][1])
            if c is None:
                continue
            zbuf[y, x] = z
            bild[y, x] = c
            n += 1
    return n


def modell_zeichnen(bild, zbuf, cam, pos, ry, polys, tim, licht):
    a = (ry & 4095) / 4096.0 * 2 * math.pi
    c, s = math.cos(a), math.sin(a)
    R = np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])
    xs, ys = [], []
    n = 0
    for p in polys:
        S = []
        for v in p["v"]:
            w = R @ np.array(v, float) + np.array(pos, float)
            q = np.floor((cam["Ri"] @ np.round(w)) / 4096.0) + cam["T"]
            if q[2] <= 64:
                S = None
                break
            S.append((SS * (160.0 + cam["H"] * q[0] / q[2]), SS * (120.0 + cam["H"] * q[1] / q[2]), q[2]))
        if S is None:
            continue
        xs += [t[0] / SS for t in S]
        ys += [t[1] / SS for t in S]
        f = (lambda cl, pg: (lambda u, v: tex_farbe(tim, u, v, cl, pg, licht)))(p["clut"], p["page"])
        tris = [(0, 1, 2)] if len(S) == 3 else [(0, 1, 3), (0, 3, 2)]
        for t in tris:
            n += dreieck(bild, zbuf, [S[i] for i in t], [p["uv"][i] for i in t], f)
    return (min(xs), max(xs), min(ys), max(ys)) if xs else None, n


# ------------------------------------------------------------------ die vier Faelle
def faelle():
    # ENDWERTE (E_dokumente.md 5.2, Konstanten-Tabelle). Herleitung je Wert dort:
    #   Dok 1 Schoss-Triangulation (16474,-347,-6592) - 13 (halbe Buchdicke, bbox y -13..13)
    #   Dok 2 Rueckprojektion der Markenmitte (193.0,172.5) Cut 0 auf die Bank y -385
    #         (Sonde `strahl`: (19226.4,-385,-11723.4)), Buchmitte -385 - 13
    #   Dok 3 Rueckprojektion der Markenmitte (156.5,105.5) Cut 6 auf den Tisch y -1410
    #         ((-9975.9,-16434.6)) minus Grundriss-Mitte des Blatts (-1,-7) -> Ursprung
    #   Dok 4 globales Optimum lage_1010.py (450,5600, rot_y 3840)
    #   Drehung Buecher/Dok 3: Grundstellung 0 (lange Achse z = lange Achse der Ablage)
    return [
        dict(name="dok1_1050", raum="1050", modell="mesh00_0541704e", cuts=(3, 9),
             pos=(16474, -360, -6592), ry=0, licht=(41 / 128, 39 / 128, 39 / 128), marke=None, extra=[]),
        dict(name="dok2_1000", raum="1000", modell="mesh03_cf9f316d", cuts=(0,),
             pos=(19226, -398, -11723), ry=0, licht=(41 / 128, 39 / 128, 39 / 128),
             marke=((182, 162, 203, 182), 0), extra=[]),
        dict(name="dok3_1020", raum="1020", modell="mesh01_ae2d0a30", cuts=(6, 3, 5, 8),
             pos=(-9975, -1410, -16428), ry=0, licht=(50 / 128, 52 / 128, 52 / 128), marke=((151, 101, 161, 109), 6), extra=[]),
        dict(name="dok4_1010", raum="1010", modell="mesh04_cab7b32d", cuts=(0, 1, 6),
             pos=(450, -1600, 5600), ry=3840, licht=(41 / 128, 39 / 128, 39 / 128),
             marke=((208, 166, 231, 187), 0), extra=[(0, (200, -1600, 5500), 3084)]),
    ]


def main():
    aus = sys.argv[1] if len(sys.argv) > 1 else os.path.join(geom.AUS, "abzug")
    os.makedirs(aus, exist_ok=True)
    nur = sys.argv[2] if len(sys.argv) > 2 else None
    for f in faelle():
        if nur and f["name"] != nur:
            continue
        cams = geom.lade_kameras(f["raum"])
        mb = open(os.path.join(WELT, f["modell"] + ".md1"), "rb").read()
        tb = open(os.path.join(WELT, f["modell"] + ".tim"), "rb").read()
        polys = md1_laden(mb)
        tim = tim_laden(tb)
        extra = [(rdt_prop(f["raum"], obj), pos, ry) for obj, pos, ry in f["extra"]]
        for cut in f["cuts"]:
            bg = Image.fromarray(geom.bg(f["raum"], cut).astype(np.uint8))
            gross = np.array(bg.resize((320 * SS, 240 * SS), Image.NEAREST)).astype(float)
            zbuf = np.full(gross.shape[:2], 1e18)
            licht = np.array(f["licht"] if f["licht"] else (0.5, 0.5, 0.5))
            for (ep, et), epos, ery in extra:
                modell_zeichnen(gross, zbuf, cams[cut], epos, ery, ep, et, licht)
            bb, n = modell_zeichnen(gross, zbuf, cams[cut], f["pos"], f["ry"], polys, tim, licht)
            img = Image.fromarray(gross.astype(np.uint8))
            dr = ImageDraw.Draw(img)
            if f["marke"] and f["marke"][1] == cut:
                x0, y0, x1, y1 = f["marke"][0]
                dr.rectangle([x0 * SS, y0 * SS, (x1 + 1) * SS - 1, (y1 + 1) * SS - 1], outline=(255, 0, 0))
            klein = img.resize((320, 240), Image.BOX)
            klein.save(os.path.join(aus, "%s_cut%d.png" % (f["name"], cut)))
            if bb:
                cx, cy = (bb[0] + bb[1]) / 2, (bb[2] + bb[3]) / 2
                box = [int(max(0, cx - 40) * SS), int(max(0, cy - 30) * SS), int(min(320, cx + 40) * SS), int(min(240, cy + 30) * SS)]
                img.crop(box).save(os.path.join(aus, "%s_cut%d_ausschnitt.png" % (f["name"], cut)))
                print("%s Cut %d: Huelle x %.1f..%.1f y %.1f..%.1f (Mitte %.1f,%.1f), %d Pixel (4x)"
                      % (f["name"], cut, bb[0], bb[1], bb[2], bb[3], cx, cy, n))
            else:
                print("%s Cut %d: nicht im Bild" % (f["name"], cut))


if __name__ == "__main__":
    main()
