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

Runde 41 Spur PF (Nutzer: "der Kasten ist bei der Kamerasicht irgendwie komisch gestaucht"; Dossier analysis/befunde_runde41/PF_panelfix.md §1):
  * Seitenverhaeltnis: die Vierecke Q_INNEN / Q_TUER wurden frueher stumpf auf die Tuerflaeche 1300 x 3600 (0,361) gezogen. In der Welt hat die
    Rueckwand in ROOM2060 B/H 0,460, der sichtbare Fluegel 0,511 (Homographie Q -> Rechteck, Lochkamera H 208 wie Cut 6 @0x120; gegengeprueft:
    Rueckwand-Ebene z 4500, Unterkante auf dem Boden y 0 -> 1581 x 3487 = 0,453). Jetzt wird erst auf das wahre Seitenverhaeltnis entzerrt und dann
    mittig auf 1300:3600 beschnitten: gleicher Massstab in beiden Achsen, links und rechts faellt ein Rand weg (innen bleiben 78,5 % der Breite).
  * Affine Abbildung: SDL_RenderGeometry (wie die PSX-GPU) bildet je Dreieck affin ab. EIN Viereck unter dem streifenden Blick von Cut 3 verschob
    Texel um bis zu 9,5 Bildpunkte (Knick entlang der Diagonale). Die Flaechen sind jetzt ein Gitter 4 x 8 (GITTER), Fehler <= 0,40 Bildpunkte
    (Rechnung affin_fehler(), auch fuer den Fluegel bei 0, 45 und 90 Grad) - dieselbe Abhilfe wie die Unterteilung der Original-Netze (Wasser ROOM2080
    Prop 0: Vierecke <= 1800 x 1800).

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
GITTER = (4, 8)          # Spalten x Zeilen je Flaeche (Spur PF): affiner Fehler unter Cut 3 <= 0,40 Bildpunkte, siehe affin_fehler()


def bg(name):
    for d in QUELLE:
        p = os.path.join(d, name)
        if os.path.exists(p):
            return np.array(Image.open(p).convert("RGB"))
    raise SystemExit("Bild-Dump fehlt: %s" % name)


def seitenverhaeltnis(quad):
    """Welt-Seitenverhaeltnis B/H des Rechtecks, dessen Bild das Viereck quad ist (Lochkamera H_KAM, Bildmitte 160/120 wie tuer_aussen):
    Homographie Einheitsquadrat -> quad, |K^-1 * Spalte 1| / |K^-1 * Spalte 2| = Breite / Hoehe (Lage und Richtung der Kamera kuerzen sich heraus)."""
    k = np.array([[H_KAM, 0, 160], [0, H_KAM, 120], [0, 0, 1.]])
    m = cv2.getPerspectiveTransform(np.float32([(0, 0), (1, 0), (1, 1), (0, 1)]), np.float32(quad))
    ki = np.linalg.inv(k)
    return float(np.linalg.norm(ki @ m[:, 0]) / np.linalg.norm(ki @ m[:, 1]))


def entzerren(bild, quad, w, h, ziel):
    """quad perspektivisch richtig auf sein wahres Seitenverhaeltnis entzerren, dann mittig auf B/H = ziel beschneiden und auf w x h abtasten
    (EINE Abtastung: Homographie quad -> Rechteck, danach Verschiebung + Massstab). Liefert Bild, wahres B/H, verbleibenden Breitenanteil."""
    a = seitenverhaeltnis(quad)
    hb = float(h)
    wb = a * hb                                                  # wahres Rechteck, gemessen in Hoehen-Texeln
    wk = min(ziel * hb, wb)                                      # Breite des Ausschnitts bei gleichem Massstab
    x0 = (wb - wk) / 2
    m = cv2.getPerspectiveTransform(np.float32(quad), np.float32([(0, 0), (wb, 0), (wb, hb), (0, hb)]))
    s = np.array([[w / wk, 0, -x0 * w / wk], [0, h / hb, 0], [0, 0, 1.]])
    return cv2.warpPerspective(bild, s @ m, (w, h), flags=cv2.INTER_CUBIC, borderMode=cv2.BORDER_REPLICATE), a, wk / wb


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


def md1_gitter(ecken, normale, w, h, gitter=GITTER):
    """Gitter aus Vierecken in der Form aushang_md1 (plakat.py) / Wasserflaeche ROOM2080 Prop 0 (je Viereck 8 Halbworte Normale/Ecke, 16 B UV).
    ecken = v0 unten links, v1 oben links, v2 unten rechts, v3 oben rechts (vom Betrachter der Seite) der ganzen Flaeche; die Gitterpunkte liegen
    bilinear dazwischen, die UV-Werte 0..w-1 / 0..h-1 gleichmaessig (gerundet, Nachbarn teilen ihre Kanten exakt). gitter (1, 1) = das alte EINE Viereck."""
    nx, ny = gitter
    e0, e1, e2, e3 = (np.array(e, float) for e in ecken)

    def punkt(i, j):        # i Spalte 0..nx (links -> rechts), j Zeile 0..ny (oben -> unten)
        oben = e1 + (e3 - e1) * (i / nx)
        unten = e0 + (e2 - e0) * (i / nx)
        return tuple(int(round(c)) for c in oben + (unten - oben) * (j / ny))
    nv, nq = (nx + 1) * (ny + 1), nx * ny
    tv = 56
    tn = tv + 8 * nv
    tf = tn + 8
    tu = tf + 16 * nq
    b = bytearray(struct.pack("<III", 12 + tu, 0, 2) + struct.pack("<14I", tv, nv, tn, 1, tf, 0, tu, tv, nv, tn, 1, tf, nq, tu))
    for j in range(ny + 1):
        for i in range(nx + 1):
            b += struct.pack("<hhhh", *punkt(i, j), 0)
    b += struct.pack("<hhhh", normale[0], normale[1], normale[2], 0)
    idx = lambda i, j: j * (nx + 1) + i
    zellen = [(i, j) for j in range(ny) for i in range(nx)]
    for (i, j) in zellen:   # v0 unten links, v1 oben links, v2 unten rechts, v3 oben rechts
        b += struct.pack("<8H", 0, idx(i, j + 1), 0, idx(i, j), 0, idx(i + 1, j + 1), 0, idx(i + 1, j))
    uu = lambda i: int(round(i * (w - 1) / nx))
    vv = lambda j: int(round(j * (h - 1) / ny))
    for (i, j) in zellen:
        b += struct.pack("<BBHBBHBBHBBH", uu(i), vv(j + 1), 0x7800, uu(i), vv(j), 0x0080, uu(i + 1), vv(j + 1), 0, uu(i + 1), vv(j), 0)
    assert len(b) == 12 + tu + 16 * nq
    return bytes(b)


def affin_fehler(gitter):
    """Groesster Bildabstand unter Cut 3 zwischen dem perspektivisch richtigen Ort eines Texels und dem affin je Teildreieck (v0 v1 v2 / v1 v3 v2)
    gezeichneten: Rueckwand und Fluegel bei rot_y 0, 512 und 1024."""
    p, x, y, z = kamera(RDT2010, 3)

    def bild(w):
        d = np.array(w, float) - p
        return np.array([160 + H_KAM * d.dot(x) / d.dot(z), 120 + H_KAM * d.dot(y) / d.dot(z)])

    def tuer(rot):
        w_ = math.radians(rot * 90 / 1024)
        return lambda u, v: (X_WAND + (Z_FREI - Z_ANGEL) * u * math.sin(w_), Y_OBEN + (Y_UNTEN - Y_OBEN) * v, Z_ANGEL + (Z_FREI - Z_ANGEL) * u * math.cos(w_))
    flaechen = {"innen": lambda u, v: (X_WAND, Y_OBEN + (Y_UNTEN - Y_OBEN) * v, Z_ANGEL + (Z_FREI - Z_ANGEL) * u)}
    for rot in (0, 512, 1024):
        flaechen["tuer %d" % rot] = tuer(rot)
    nx, ny = gitter
    aus = {}
    for name, welt in flaechen.items():
        schlimm = 0.0
        for i in range(nx):
            for j in range(ny):
                u0, u1, v0, v1 = i / nx, (i + 1) / nx, j / ny, (j + 1) / ny
                ecke = {"tl": (u0, v0), "tr": (u1, v0), "bl": (u0, v1), "br": (u1, v1)}
                pb = {k: bild(welt(*uv)) for k, uv in ecke.items()}
                for a in np.linspace(.02, .98, 6):
                    for c in np.linspace(.02, .98, 6):
                        tri = ("bl", "tl", "br") if c >= a else ("tl", "tr", "br")
                        m = np.array([[ecke[t][0], ecke[t][1], 1.] for t in tri]).T
                        lam = np.linalg.solve(m, np.array([u0 + (u1 - u0) * a, v0 + (v1 - v0) * c, 1.]))
                        aff = sum(l * pb[t] for l, t in zip(lam, tri))
                        schlimm = max(schlimm, float(np.linalg.norm(aff - bild(welt(u0 + (u1 - u0) * a, v0 + (v1 - v0) * c)))))
        aus[name] = schlimm
    return aus


def inc_array(name, b, zeile=32):
    z = ["static const unsigned char %s[%d] = {" % (name, len(b))]
    for i in range(0, len(b), zeile):
        z.append(",".join(str(x) for x in b[i:i + zeile]) + ",")
    z.append("};")
    return "\n".join(z)


def main():
    b6 = bg("ROOM20606.ppm")
    b3 = bg("ROOM20103.ppm")
    ziel = (Z_ANGEL - Z_FREI) / float(Y_UNTEN - Y_OBEN)          # Tuerflaeche 1300 x 3600 in ROOM2010
    innen, a_i, k_i = entzerren(b6, Q_INNEN, W_I, H_I, ziel)
    tuer, a_t, k_t = entzerren(b6, Q_TUER, W_T, H_T, ziel)
    aussen = tuer_aussen(b3, W_T, H_T)
    fehler = affin_fehler(GITTER)
    print("Seitenverhaeltnis 2060: innen %.3f (bleibt %.1f %% der Breite), Fluegel %.3f (%.1f %%), Ziel %.3f; affiner Fehler Gitter %dx%d: %s; EIN Viereck: %.2f"
          % (a_i, 100 * k_i, a_t, 100 * k_t, ziel, GITTER[0], GITTER[1], ", ".join("%s %.2f" % kv for kv in fehler.items()), affin_fehler((1, 1))["innen"]))
    assert max(fehler.values()) < 0.5, fehler
    # innen: Prop im Ursprung, Ecken in Weltkoordinaten; aussen/tuer: Ursprung = Angel (Prop (X_WAND, 0, Z_ANGEL)), geschlossen laengs -z
    m_innen = md1_gitter([(X_WAND, Y_UNTEN, Z_ANGEL), (X_WAND, Y_OBEN, Z_ANGEL), (X_WAND, Y_UNTEN, Z_FREI), (X_WAND, Y_OBEN, Z_FREI)], (-4096, 0, 0), W_I, H_I)
    br = Z_FREI - Z_ANGEL
    m_aussen = md1_gitter([(0, Y_UNTEN, 0), (0, Y_OBEN, 0), (0, Y_UNTEN, br), (0, Y_OBEN, br)], (-4096, 0, 0), W_T, H_T)
    m_tuer = md1_gitter([(0, Y_UNTEN, br), (0, Y_OBEN, br), (0, Y_UNTEN, 0), (0, Y_OBEN, 0)], (4096, 0, 0), W_T, H_T)
    z = ["/* AUTOMATISCH ERZEUGT von re15_port/tools/r41_nachtrag30/schrank_bild.py - nicht von Hand aendern.",
         " * Schrank ROOM2010/2011 (Runde 41 Spur N, Nachtrag 30): Inneres + Tuer-Innenseite aus ROOM2060 Cut 6 (BG06, fuse_box.bmp), Tuer aussen aus ROOM2010",
         " * Cut 3 (BG03). Ecken in Cut 6: innen %s, Tuer %s." % (Q_INNEN, Q_TUER),
         " * Spur PF: auf das wahre Seitenverhaeltnis entzerrt (innen %.3f, Fluegel %.3f) und mittig auf %.3f beschnitten; Gitter %d x %d je Flaeche. */"
         % (a_i, a_t, ziel, GITTER[0], GITTER[1]),
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
