#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""schrank_bild.py - Runde 41 Spur N (AUFTRAG.md Nachtrag 30): der Schrank ROOM2010/2011 klappt auf und sieht danach aus wie der Sicherungsschrank in 2060.

Dossier analysis/befunde_runde41/N_nachtrag30.md §1. Erzeugt engine/src/gen/schrank2010_r41.inc: drei Port-Modelle (MD1 + TIM) der Form Aushang
(tools/r41_ventilkette/plakat.py aushang_md1, EIN Viereck im Format der Wasserflaeche ROOM2080 Prop 0):
  innen  obj 5  Schrank-Inneres an der Ostwand x 5400: ROOM2060 Cut 6 (BG06, das Bild aus fuse_box.bmp), Schrankwand entzerrt
  aussen obj 6  Tuer aussen: ROOM2010 Cut 3 (BG03) auf die Tuerebene zurueckprojiziert, deckt sich geschlossen mit dem Hintergrund
  tuer   obj 7  Tuer innen: ROOM2060 Cut 6, der offene Fluegel links der Schrankwand, entzerrt
Lage aus dem Hintergrund gemessen (Cut 3, Projektion H 208): Tuer x 5400, z -3300..-2000, y -3650..-50; Angel am fernen Rand z -2000 (der Fluegel
oeffnet zur Kamera hin nach links, wie in fuse_box.bmp).

Aufruf: R41N_BG=<dumps stage2> C:/Python310/python.exe re15_port/tools/r41_nachtrag30/schrank_bild.py [--beleg <png>]
"""
import hashlib
import math
import os
import struct
import sys
import numpy as np, cv2
from PIL import Image

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
GEN = os.path.join(REPO, "re15_port", "engine", "src", "gen")
RDT2010 = os.path.join(REPO, "re15_port", "shared_assets", "PSX", "STAGE2", "ROOM2010.RDT")
QUELLE = os.environ.get("R41N_BG", ".").split(";")
H_KAM = 208.0            # fov 26684 >> 7

X_WAND = 5400
Z_ANGEL, Z_FREI = -2000, -3300
Y_OBEN, Y_UNTEN = -3650, -50
# Ecken im Bild ROOM2060 Cut 6 (links oben, rechts oben, rechts unten, links unten), gemessen an fuse_box.bmp / BG06
Q_INNEN = [(48, 26), (122, 28), (126, 165), (62, 186)]
Q_TUER = [(0, 28), (48, 42), (60, 182), (22, 236)]
W_I, H_I = 128, 192
W_T, H_T = 64, 192


def bg(name):
    for d in QUELLE:
        p = os.path.join(d, name)
        if os.path.exists(p):
            return np.array(Image.open(p).convert("RGB"))
    raise SystemExit("Bild-Dump fehlt: %s" % name)


def entzerren(bild, quad, w, h):
    m = cv2.getPerspectiveTransform(np.float32(quad), np.float32([(0, 0), (w, 0), (w, h), (0, h)]))
    return cv2.warpPerspective(bild, m, (w, h), flags=cv2.INTER_CUBIC, borderMode=cv2.BORDER_REPLICATE)


def kamera(rdt, cut):
    d = open(rdt, "rb").read()
    rid = struct.unpack_from("<I", d, 0x24)[0]
    px, py, pz, tx, ty, tz = struct.unpack_from("<6i", d, rid + cut * 0x20 + 4)
    p = np.array([px, py, pz], float)
    z = np.array([tx, ty, tz], float) - p
    z /= np.linalg.norm(z)
    x = np.array([z[2], 0.0, -z[0]])
    x /= np.linalg.norm(x)
    y = np.cross(z, x)
    return p, x, y, z


def tuer_aussen(bild, w, h):
    """Zurueckprojektion: je Texel Weltpunkt der Tuerebene -> Bildpunkt in Cut 3."""
    p, x, y, z = kamera(RDT2010, 3)
    t, s = np.mgrid[0:h, 0:w].astype(np.float32)
    wz = Z_ANGEL + (Z_FREI - Z_ANGEL) * (s + .5) / w      # links im Texturbild = Angel (fern), rechts = freier Rand (nah)
    wy = Y_OBEN + (Y_UNTEN - Y_OBEN) * (t + .5) / h
    d0, d1, d2 = X_WAND - p[0], wy - p[1], wz - p[2]
    cx = d0 * x[0] + d1 * x[1] + d2 * x[2]
    cy = d0 * y[0] + d1 * y[1] + d2 * y[2]
    cz = d0 * z[0] + d1 * z[1] + d2 * z[2]
    mx = (160 + H_KAM * cx / cz).astype(np.float32) - .5
    my = (120 + H_KAM * cy / cz).astype(np.float32) - .5
    return cv2.remap(bild, mx, my, cv2.INTER_CUBIC, borderMode=cv2.BORDER_REPLICATE)


def rgb555(r, g, b):
    return (r >> 3) | ((g >> 3) << 5) | ((b >> 3) << 10)


def tim_8bpp(rgb):
    h, w = rgb.shape[:2]
    q = Image.fromarray(rgb.astype(np.uint8)).quantize(colors=255, method=Image.MEDIANCUT, dither=Image.NONE)
    pal = q.getpalette()[:255 * 3]
    farben = [rgb555(pal[i], pal[i + 1], pal[i + 2]) or 0x0421 for i in range(0, len(pal), 3)]
    clut = farben + [0x8000] * (256 - len(farben))
    pix = bytes(q.getdata())
    out = bytearray(struct.pack("<II", 0x10, 0x09))
    out += struct.pack("<IHHHH", 12 + 512, 0, 480, 256, 1) + struct.pack("<256H", *clut)
    out += struct.pack("<IHHHH", 12 + w * h, 0, 0, w // 2, h) + pix
    return bytes(out)


def md1_viereck(ecken, normale, w, h):
    """EIN Viereck, Form aushang_md1 (plakat.py). ecken = v0 unten links, v1 oben links, v2 unten rechts, v3 oben rechts (vom Betrachter der Seite)."""
    nv, nq = 4, 1
    tv = 56
    tn = tv + 8 * nv
    tf = tn + 8
    tu = tf + 16 * nq
    b = bytearray(struct.pack("<III", 12 + tu, 0, 2) + struct.pack("<14I", tv, nv, tn, 1, tf, 0, tu, tv, nv, tn, 1, tf, nq, tu))
    for (x, y, z) in ecken:
        b += struct.pack("<hhhh", x, y, z, 0)
    b += struct.pack("<hhhh", normale[0], normale[1], normale[2], 0)
    b += struct.pack("<8H", 0, 0, 0, 1, 0, 2, 0, 3)
    b += struct.pack("<BBHBBHBBHBBH", 0, h - 1, 0x7800, 0, 0, 0x0080, w - 1, h - 1, 0, w - 1, 0, 0)
    assert len(b) == 12 + tu + 16 * nq
    return bytes(b)


def inc_array(name, b, zeile=32):
    z = ["static const unsigned char %s[%d] = {" % (name, len(b))]
    for i in range(0, len(b), zeile):
        z.append(",".join(str(x) for x in b[i:i + zeile]) + ",")
    z.append("};")
    return "\n".join(z)


def main():
    b6 = bg("ROOM20606.ppm")
    b3 = bg("ROOM20103.ppm")
    innen = entzerren(b6, Q_INNEN, W_I, H_I)
    tuer = entzerren(b6, Q_TUER, W_T, H_T)
    aussen = tuer_aussen(b3, W_T, H_T)
    # innen: Prop im Ursprung, Ecken in Weltkoordinaten; aussen/tuer: Ursprung = Angel (Prop (X_WAND, 0, Z_ANGEL)), geschlossen laengs -z
    m_innen = md1_viereck([(X_WAND, Y_UNTEN, Z_ANGEL), (X_WAND, Y_OBEN, Z_ANGEL), (X_WAND, Y_UNTEN, Z_FREI), (X_WAND, Y_OBEN, Z_FREI)], (-4096, 0, 0), W_I, H_I)
    br = Z_FREI - Z_ANGEL
    m_aussen = md1_viereck([(0, Y_UNTEN, 0), (0, Y_OBEN, 0), (0, Y_UNTEN, br), (0, Y_OBEN, br)], (-4096, 0, 0), W_T, H_T)
    m_tuer = md1_viereck([(0, Y_UNTEN, br), (0, Y_OBEN, br), (0, Y_UNTEN, 0), (0, Y_OBEN, 0)], (4096, 0, 0), W_T, H_T)
    z = ["/* AUTOMATISCH ERZEUGT von re15_port/tools/r41_nachtrag30/schrank_bild.py - nicht von Hand aendern.",
         " * Schrank ROOM2010/2011 (Runde 41 Spur N, Nachtrag 30): Inneres + Tuer-Innenseite aus ROOM2060 Cut 6 (BG06, fuse_box.bmp), Tuer aussen aus ROOM2010",
         " * Cut 3 (BG03). Ecken in Cut 6: innen %s, Tuer %s. */" % (Q_INNEN, Q_TUER),
         inc_array("re15_r41n_schrank_innen_md1", m_innen), inc_array("re15_r41n_schrank_innen_tim", tim_8bpp(innen)),
         inc_array("re15_r41n_schrank_aussen_md1", m_aussen), inc_array("re15_r41n_schrank_aussen_tim", tim_8bpp(aussen)),
         inc_array("re15_r41n_schrank_tuer_md1", m_tuer), inc_array("re15_r41n_schrank_tuer_tim", tim_8bpp(tuer))]
    ziel = os.path.join(GEN, "schrank2010_r41.inc")
    open(ziel, "w", newline="\n").write("\n".join(z) + "\n")
    print("geschrieben:", ziel, os.path.getsize(ziel), "B")
    if "--beleg" in sys.argv:
        bogen = Image.new("RGB", (W_I * 3 + W_T * 6 + 40, H_I * 3), (30, 30, 30))
        bogen.paste(Image.fromarray(innen).resize((W_I * 3, H_I * 3), Image.NEAREST), (0, 0))
        bogen.paste(Image.fromarray(tuer).resize((W_T * 3, H_T * 3), Image.NEAREST), (W_I * 3 + 20, 0))
        bogen.paste(Image.fromarray(aussen).resize((W_T * 3, H_T * 3), Image.NEAREST), (W_I * 3 + W_T * 3 + 40, 0))
        bogen.save(sys.argv[sys.argv.index("--beleg") + 1])


if __name__ == "__main__":
    main()
