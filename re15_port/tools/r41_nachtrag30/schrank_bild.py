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

Runde 41 Spur PF, Nachbesserung 1 (Abnahme 1 M1: Inhalt geschert, Rueckwand +4,2 Grad, Fluegel +6,0 Grad; Dossier PF_panelfix.md "Nachbesserung 1"):
  * Die Handvierecke Q_INNEN / Q_TUER sind weg. Beide Flaechen werden ueber die RDT-Kamera ROOM2060 Cut 6 entzerrt (ROOM2060.RDT Kameratabelle
    @0x60, Satz 6 @0x120: fov 26684 -> H 208, Auge (-5146,-2700,-442), Ziel (6281,2882,18728); Projektion wie camera_common.c / FUN_80053ca4):
    je Texel ein Weltpunkt auf der Flaeche -> Bildpunkt in BG06. Senkrechte der Welt bleiben senkrecht, Waagrechte waagrecht, gleicher Massstab in
    beiden Achsen (Seitenverhaeltnis 1,00 statt 1,02/1,01).
  * Rueckwand: Ebene z = WAND_Z (achsparallel; z aendert nur den Massstab). Rechteck RUECKWAND im entzerrten BG06 gemessen.
  * Fluegel: senkrechte Ebene, Richtung aus dem Fluchtpunkt der Waagrechten auf dem Fluegel. Gemessen an den vier Kanten der zwei Schilder in BG06
    (fluchtpunkt(): Schwellenkreuzung je Spalte, gemeinsamer Fluchtpunkt auf dem Horizont der Kamera); frueher aus den Handkanten des Vierecks
    (139,7 statt 130,9 -> Waagrechte um 2 Grad schief), und die linke Kante von Q_TUER war der Bildrand (-> 6 Grad Scherung).
  * Ausschnitt wie bisher mittig auf 1300:3600 (PORT-WAHL der Spur PF), jetzt in Weltmass auf der Flaeche.

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
RDT2060 = os.path.join(REPO, "re15_port", "shared_assets", "PSX", "STAGE2", "ROOM2060.RDT")
QUELLE = os.environ.get("R41N_BG", ".").split(";")
H_KAM = 208.0            # fov 26684 >> 7

X_WAND = 5400
Z_ANGEL, Z_FREI = -2000, -3300
Y_OBEN, Y_UNTEN = -3650, -50
# Spur PF, Nachbesserung 1: Flaechen in ROOM2060 ueber die Kamera Cut 6 (@0x120) statt Handvierecken
CUT_2060 = 6
WAND_Z = 4500.0              # Rueckwand-Ebene z = const; die Unterkante (62,186) liegt damit auf dem Boden y 0 (z 4504)
# Rueckwand-Rechteck auf z 4500 (x links, x rechts, y oben, y unten), im entzerrten BG06 (5 Einheiten je Bildpunkt) gemessen: Fuge links x -4830
# (Helligkeit 5,8), Fuge rechts vor der grauen Nachbartuer x -3250 (1,1), Oberkante des Deckels y -3545 (steilster Anstieg -3560 -> -3520),
# Boden y 0 (Unterkante, wie Gegenrechnung Spur PF). Mittel der alten Handecken: -4831 / -3255 / -3554 / -68.
RUECKWAND = (-4830.0, -3250.0, -3545.0, 0.0)
ANGEL_2060 = np.array([-4830.0, 0.0, WAND_Z])        # Punkt der Fluegelebene (linke Fuge der Rueckwand; aendert nur den Massstab)
# Fluegel auf seiner Ebene (Abstand s vom Punkt ANGEL_2060 zum freien Rand, y): Scharnierfuge s 290 (Spaltenprofil 7,6 vor der Tuerflaeche ab
# s 360), Oberkante y -3198 (steilster Anstieg -3215 -> -3190), Unterkante y 15 (Uebergang Tuer -> Boden). Der freie Rand liegt ausserhalb des Bilds.
FLUEGEL = (290.0, -3198.0, 15.0)
# Schildkanten auf dem Fluegel in BG06 (weisses und gelbes Schild): Spalten, Startzeile, Richtung der Kreuzung (Schwelle 40 auf dem Grauwert)
SCHILDKANTEN = (("weiss oben", range(26, 40), 104, "runter", True), ("weiss unten", range(30, 45), 130, "runter", False),
                ("gelb oben", range(32, 45), None, "runter", True), ("gelb unten", range(36, 50), 196, "hoch", True))
W_I, H_I = 128, 192
W_T, H_T = 64, 192
GITTER = (4, 8)          # Spalten x Zeilen je Flaeche (Spur PF): affiner Fehler unter Cut 3 <= 0,40 Bildpunkte, siehe affin_fehler()


def bg(name):
    for d in QUELLE:
        p = os.path.join(d, name)
        if os.path.exists(p):
            return np.array(Image.open(p).convert("RGB"))
    raise SystemExit("Bild-Dump fehlt: %s" % name)


def gitter_uv(n, w):
    """UV der Gitterpunkte 0..n wie md1_gitter: gleichmaessig 0..w-1, gerundet (Nachbarn teilen ihre Kanten exakt)."""
    return [int(round(i * (w - 1) / n)) for i in range(n + 1)]


def flaeche_aus_texel(n, w):
    """Anteil 0..1 ueber die Flaeche fuer jede Texelmitte 0..w-1. Die Textur wird ueber UV 0..w-1 gelegt (SDL tex_coord = u / Texturbreite,
    render_pc.c), je Gitterzelle linear zwischen den gerundeten UV von gitter_uv(): Texel t (Mitte t + 0,5) liegt in Zelle i bei
    (i + (t + 0,5 - uv_i) / (uv_i+1 - uv_i)) / n. Die letzte Spalte (Mitte w - 0,5 > w - 1) liegt knapp hinter dem Rand."""
    uv = gitter_uv(n, w)
    aus = np.empty(w)
    for t in range(w):
        m = t + .5
        i = min(max(j for j in range(n) if uv[j] <= m), n - 1)
        aus[t] = (i + (m - uv[i]) / float(uv[i + 1] - uv[i])) / n
    return aus


def kamera_2060():
    """ROOM2060 Cut 6 (Kameratabelle @0x60, Satz 6 @0x120): Lage, Achsen (wie camera_common.c / FUN_80053ca4) und H = fov >> 7."""
    d = open(RDT2060, "rb").read()
    o = struct.unpack_from("<I", d, 0x24)[0] + CUT_2060 * 0x20
    assert o == 0x120, hex(o)
    assert struct.unpack_from("<H", d, o + 2)[0] >> 7 == H_KAM
    return kamera(RDT2060, CUT_2060)


def projektion(kam, w):
    """Weltpunkte (..., 3) -> Bildpunkte u, v (Bildpunkt-Mitte bei +0,5 wie tuer_aussen) in einem 320 x 240-Bild."""
    p, x, y, z = kam
    d = np.asarray(w, float) - p
    cz = d @ z
    return 160 + H_KAM * (d @ x) / cz, 120 + H_KAM * (d @ y) / cz


def horizont(kam):
    """Bildzeile, auf der sich alle waagrechten Linien der Welt treffen (Fluchtpunkt der x-Achse)."""
    p, x, y, z = kam
    e = np.array([1.0, 0, 0])
    return 120 + H_KAM * (e @ y) / (e @ z)


def fluchtpunkt(b6, kam):
    """Fluchtpunkt der Waagrechten auf dem offenen Fluegel in BG06, gemessen an den vier Kanten der zwei Schilder (SCHILDKANTEN): je Spalte die
    Kreuzung der Schwelle 40 (Grauwert), linear interpoliert; dann der gemeinsame Punkt auf dem Horizont, durch den alle vier Kanten am besten gehen
    (Summe der senkrechten Abstandsquadrate). Liefert x des Fluchtpunkts, Horizont, Rest und die Einzelschnitte der Kanten mit dem Horizont."""
    g = b6.astype(float) @ np.array([.299, .587, .114])          # Grauwert wie PIL "L" (ITU-R 601)
    hor = horizont(kam)
    schw = 40.0

    def kreuz(x, y, abwaerts, hell):
        while 1 <= y < 239:
            a, b = g[y, x], g[y + (1 if abwaerts else -1), x]
            if (a <= schw < b) if hell else (a > schw >= b):
                f = (schw - a) / (b - a)
                return (y + f if abwaerts else y - f) + .5
            y += 1 if abwaerts else -1
        return None
    kanten, einzeln, weiss_unten = [], [], {}
    for name, spalten, y0, richtung, hell in SCHILDKANTEN:
        pkt = []
        for x in spalten:
            start = y0 if y0 is not None else int(weiss_unten[x]) + 1      # gelb oben: unter der Unterkante des weissen Schilds weitersuchen
            y = kreuz(x, start, richtung == "runter", hell)
            if y is not None:
                pkt.append((x + .5, y))
                if name == "weiss unten":
                    weiss_unten[x] = y
        q = np.array(pkt)
        m, c = np.polyfit(q[:, 0], q[:, 1], 1)
        einzeln.append((name, (hor - c) / m, float(np.abs(q[:, 1] - m * q[:, 0] - c).max())))
        kanten.append(q)

    def rest(vx):
        return sum(float(np.linalg.eigvalsh((q - [vx, hor]).T @ (q - [vx, hor]))[0]) for q in kanten)
    vs = np.arange(100.0, 180.0, 0.05)
    vx = float(vs[int(np.argmin([rest(v) for v in vs]))])
    return vx, hor, rest(vx), einzeln


def fluegel_richtung(kam, vx):
    """Waagrechte Richtung d der senkrechten Fluegelebene, deren Waagrechte im Bild durch (vx, Horizont) laufen; d zeigt vom freien Rand zum
    Scharnier (von der Kamera weg)."""
    p, x, y, z = kam
    c = (vx - 160) / H_KAM
    t = math.atan2(x[0] - c * z[0], -(x[2] - c * z[2]))
    d = np.array([math.cos(t), 0.0, math.sin(t)])
    return (d if d @ z > 0 else -d), math.degrees(t) % 180


def ausschnitt(b6, kam, welt, w, h, n=GITTER):
    """Textur w x h: je Texelmitte der Weltpunkt welt(fu, fv) (Anteile ueber die Port-Flaeche, siehe flaeche_aus_texel) -> Bildpunkt in BG06."""
    fu, fv = np.meshgrid(flaeche_aus_texel(n[0], w), flaeche_aus_texel(n[1], h))
    u, v = projektion(kam, welt(fu, fv))
    assert u.min() >= .5 and u.max() <= 319.5 and v.min() >= .5 and v.max() <= 239.5, (u.min(), u.max(), v.min(), v.max())
    return cv2.remap(b6, (u - .5).astype(np.float32), (v - .5).astype(np.float32), cv2.INTER_CUBIC, borderMode=cv2.BORDER_REPLICATE)


def innen_2060(b6, kam, w, h, ziel):
    """Rueckwand: Rechteck RUECKWAND auf z = WAND_Z, mittig auf B/H = ziel beschnitten (gleicher Massstab in beiden Achsen)."""
    xl, xr, yo, yu = RUECKWAND
    br = (yu - yo) * ziel
    xm = (xl + xr) / 2
    x0, x1 = xm - br / 2, xm + br / 2                           # links im Texturbild = Angel (links in 2060), rechts = freier Rand
    welt = lambda fu, fv: np.stack([x0 + fu * (x1 - x0), yo + fv * (yu - yo), np.full_like(fu, WAND_Z)], -1)
    return ausschnitt(b6, kam, welt, w, h), (xr - xl) / (yu - yo), br / (xr - xl), (x0, x1, yo, yu)


def sichtbar_bis(kam, d, yo, yu):
    """Groesstes s (Abstand vom Punkt ANGEL_2060 zum freien Rand), bis zu dem der Fluegel zwischen yo und yu ganz im Bild BG06 liegt."""
    s_max = 1e9
    for y in np.linspace(yo, yu, 200):
        s = np.arange(0.0, 4000.0, 1.0)
        u, v = projektion(kam, ANGEL_2060[None, :] - s[:, None] * d[None, :] + np.array([0, y, 0]))
        drin = (u >= .5) & (v <= 239.5) & (v >= .5)
        s_max = min(s_max, float(s[np.argmin(drin)] - 1) if not drin.all() else 4000.0)
    return s_max


def tuer_2060(b6, kam, d, w, h, ziel):
    """Fluegel: senkrechte Ebene durch ANGEL_2060 mit Richtung d; Hoehe FLUEGEL, Breite mittig zwischen Scharnierfuge und Bildrand auf B/H = ziel."""
    sa, yo, yu = FLUEGEL
    sb = sichtbar_bis(kam, d, yo, yu)
    br = (yu - yo) * ziel
    sm = (sa + sb) / 2
    s0, s1 = sm + br / 2, sm - br / 2                           # links im Texturbild = freier Rand (grosses s), rechts = Scharnier
    welt = lambda fu, fv: np.stack([ANGEL_2060[0] - (s0 + fu * (s1 - s0)) * d[0], yo + fv * (yu - yo),
                                    ANGEL_2060[2] - (s0 + fu * (s1 - s0)) * d[2]], -1)
    return ausschnitt(b6, kam, welt, w, h), (sb - sa) / (yu - yo), br / (sb - sa), (s0, s1, yo, yu, sb)


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
    uu, vv = gitter_uv(nx, w), gitter_uv(ny, h)          # dieselben UV liest flaeche_aus_texel() beim Abtasten
    for (i, j) in zellen:
        b += struct.pack("<BBHBBHBBHBBH", uu[i], vv[j + 1], 0x7800, uu[i], vv[j], 0x0080, uu[i + 1], vv[j + 1], 0, uu[i + 1], vv[j], 0)
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
    kam = kamera_2060()
    vx, hor, rest, einzeln = fluchtpunkt(b6, kam)
    assert 125 < vx < 137 and rest < 4 and all(abs(e[1] - vx) < 3 for e in einzeln), (vx, rest, einzeln)
    d, winkel = fluegel_richtung(kam, vx)
    innen, a_i, k_i, r_i = innen_2060(b6, kam, W_I, H_I, ziel)
    tuer, a_t, k_t, r_t = tuer_2060(b6, kam, d, W_T, H_T, ziel)
    aussen = tuer_aussen(b3, W_T, H_T)
    fehler = affin_fehler(GITTER)
    p2 = lambda w: tuple(round(float(c), 1) for c in projektion(kam, np.array(w, float)))
    ecken_i = [p2((r_i[0], r_i[2], WAND_Z)), p2((r_i[1], r_i[2], WAND_Z)), p2((r_i[1], r_i[3], WAND_Z)), p2((r_i[0], r_i[3], WAND_Z))]
    fw = lambda s_, y_: ANGEL_2060 - s_ * d + np.array([0, y_, 0])
    ecken_t = [p2(fw(r_t[0], r_t[2])), p2(fw(r_t[1], r_t[2])), p2(fw(r_t[1], r_t[3])), p2(fw(r_t[0], r_t[3]))]
    print("ROOM2060 Cut 6: Horizont y %.2f; Fluchtpunkt Fluegel x %.2f (Rest %.2f; Kanten %s) -> Richtung %.2f Grad"
          % (hor, vx, rest, ", ".join("%s %.1f/%.2f" % e for e in einzeln), winkel))
    print("Seitenverhaeltnis 2060: innen %.3f (bleibt %.1f %% der Breite), Fluegel sichtbar %.3f (bis s %.0f, bleibt %.1f %%), Ziel %.3f"
          % (a_i, 100 * k_i, a_t, r_t[4], 100 * k_t, ziel))
    print("Ausschnitt in BG06 (Projektion, oben links / oben rechts / unten rechts / unten links): innen %s, Fluegel %s" % (ecken_i, ecken_t))
    print("affiner Fehler Gitter %dx%d: %s; EIN Viereck: %.2f" % (GITTER[0], GITTER[1], ", ".join("%s %.2f" % kv for kv in fehler.items()),
                                                                 affin_fehler((1, 1))["innen"]))
    assert max(fehler.values()) < 0.5, fehler
    # innen: Prop im Ursprung, Ecken in Weltkoordinaten; aussen/tuer: Ursprung = Angel (Prop (X_WAND, 0, Z_ANGEL)), geschlossen laengs -z
    m_innen = md1_gitter([(X_WAND, Y_UNTEN, Z_ANGEL), (X_WAND, Y_OBEN, Z_ANGEL), (X_WAND, Y_UNTEN, Z_FREI), (X_WAND, Y_OBEN, Z_FREI)], (-4096, 0, 0), W_I, H_I)
    br = Z_FREI - Z_ANGEL
    m_aussen = md1_gitter([(0, Y_UNTEN, 0), (0, Y_OBEN, 0), (0, Y_UNTEN, br), (0, Y_OBEN, br)], (-4096, 0, 0), W_T, H_T)
    m_tuer = md1_gitter([(0, Y_UNTEN, br), (0, Y_OBEN, br), (0, Y_UNTEN, 0), (0, Y_OBEN, 0)], (4096, 0, 0), W_T, H_T)
    z = ["/* AUTOMATISCH ERZEUGT von re15_port/tools/r41_nachtrag30/schrank_bild.py - nicht von Hand aendern.",
         " * Schrank ROOM2010/2011 (Runde 41 Spur N, Nachtrag 30): Inneres + Tuer-Innenseite aus ROOM2060 Cut 6 (BG06, fuse_box.bmp), Tuer aussen aus ROOM2010",
         " * Cut 3 (BG03). Spur PF Nachbesserung 1: ueber die Kamera ROOM2060 Cut 6 (@0x120, H %d) entzerrt, keine Handvierecke. Rueckwand z %.0f," % (H_KAM, WAND_Z),
         " * x %.0f..%.0f, y %.0f..%.0f (B/H %.3f, mittig auf %.3f); Fluegel: Fluchtpunkt x %.2f -> Richtung %.2f Grad, s %.0f..%.0f, y %.0f..%.0f." % (
             r_i[0], r_i[1], r_i[2], r_i[3], a_i, ziel, vx, winkel, r_t[0], r_t[1], r_t[2], r_t[3]),
         " * Ausschnitt in BG06: innen %s, Fluegel %s; Gitter %d x %d je Flaeche. */" % (ecken_i, ecken_t, GITTER[0], GITTER[1]),
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
