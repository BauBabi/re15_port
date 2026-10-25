#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""plakat.py - Runde 41 Spur C, Nachbesserung 4 (Nachtrag 13 Folgen 2/3): das Plakat mit der FALSCHEN Ventil-Anleitung
und die zwei Weltmodelle (Dokument - seit Nachbesserung 5 ein aufrechter Zettel an der Suedwand -, Plakat an der Wand) fuer ROOM2030/2031.

Erzeugt re15_port/engine/src/gen/ventilkette_r41_plakat.inc und die Vorschau analysis/befunde_runde41/C_belege/nb4_plakat.png.
Dossier: analysis/befunde_runde41/C_ventilkette.md §0e (B4-2..B4-5, §7 zur Freigabe).

1. NAHAUFNAHME (Port-Bild 320 x 240; seit Nachbesserung 5 / Nachtrag 18 Punkt 1 im Stil des Spiels, 8 bpp, 256 Farben, Herleitung
   bei nahaufnahme_spielstil; die Fassung der Nachbesserung 4 (4 bpp, Flaechenfarben, Blockziffern) bleibt nur als "vorher"-Bild
   C_belege/nb4_plakat.png): ein Blatt an der Wand mit der Reihenfolge Farbe + Nummer - genau so
   falsch, wie das Dokument es beschreibt (Nutzer-Text: "They mixed up black, green, and red, and swapped the numbers for
   black!"). Wahre Loesung (PORT-WAHL, Vorschlag zur Freigabe): 09 rot, 07 schwarz, 02 gruen, 01 schwarz. Auf dem Plakat:
   Farben zyklisch vertauscht (rot -> "GREEN", gruen -> "BLACK", schwarz -> "RED"), die zwei schwarzen Nummern getauscht
   (07 <-> 01) -> "1 GREEN 09 / 2 RED 01 / 3 BLACK 02 / 4 RED 07". Schrift = die RE2-Dokumentschrift (Atlas aus
   re2_doc_satz.atlas_bauen, dieselbe wie FILE31), Ziffernschilder weiss auf schwarz wie die Schilder im Raum.
2. AUSHANG im Raum (Weltmodell obj 7): EIN Viereck in der MD1-Form der Wasserflaeche (gen_r41_ventilkette.py
   reling_netz: Kopf 12 B + 14 Worte, Ecken, eine Normale, Viereck, UV-Satz), senkrecht in der x/y-Ebene, Normale
   (0,0,-4096) zur Kamera (Blick +z auf die Suedwand), 700 x 816 (Seitenverhaeltnis des Blatts 192 x 224); TIM 8 bpp 64 x 64
   (das verkleinerte Blatt), CLUT bei (0,480) wie das RE2-Blatt (UV-Satz clut 0x7800 / page 0x0080, ROOM2020 mesh01).
3. DOKUMENT-BLATT (Weltmodell obj 6). Bis Nachbesserung 4 das RE2-Blatt ROOM2020.RDT mesh01 flach am Boden - gemessen in der
   Abnahme 4 in 15 von 16 Kameras unsichtbar, in Cut 5 39 Pixel (Mangel 1). Seit Nachbesserung 5 ein ZETTEL an der Suedwand
   neben dem Schrank mit der Sicherung, aufrecht wie der Aushang (dieselbe Viereck-Form, Normale (0,0,-4096) zur Kamera):
   500 x 700, Papier und Schrift wie die Nahaufnahme (die ersten Zeilen des Nutzer-Texts in der RE2-Dokumentschrift, auf
   64 x 64 verkleinert = als beschriebener Zettel erkennbar, nicht lesbar - gelesen wird im Dokument-Leser).

Aufruf: C:/Python310/python.exe re15_port/tools/r41_ventilkette/plakat.py [--font <re2_doc_font.json>] [--atlas-dir <dir>]
        [--nah-dir <bg_dump STAGE1>]   (Vergleichsbogen mit ROOM11D0 Cut 12 / ROOM11F0 Cut 12: probe_bg_dump <cd> <dir> 1)
"""
import argparse
import hashlib
import json
import os
import random
import struct
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.abspath(os.path.join(HIER, "..")))
import re2_doc_satz as R  # noqa: E402

REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
GEN = os.path.join(REPO, "re15_port", "engine", "src", "gen")
BELEGE = os.path.join(REPO, "analysis", "befunde_runde41", "C_belege")
RE2 = os.path.join(REPO, "info", "re2leon", "PL0", "RDT")

# ---- Inhalt (PORT-WAHL, Vorschlag zur Freigabe; Ableitung im Dossier §7) ----------------------------------------------
LOESUNG = [("09", "red"), ("07", "black"), ("02", "green"), ("01", "black")]
FARBE_AUF_PLAKAT = {"red": "GREEN", "green": "BLACK", "black": "RED"}          # zyklisch vertauscht
SCHWARZ_TAUSCH = {"07": "01", "01": "07"}                                       # Nummern der Schwarzen getauscht
PLAKAT = [(FARBE_AUF_PLAKAT[f], SCHWARZ_TAUSCH.get(n, n) if f == "black" else n) for (n, f) in LOESUNG]
TITEL = ["VALVE OPERATION", "FLOOD GATES - POOL"]
FUSS = ["Turn the valves", "in this order."]

# ---- Farben (4 bpp, 16 Eintraege; Index 0 ist nicht durchsichtig, das Bild deckt alles) -----------------------------
PAL = [
    (14, 16, 15),     # 0 Wand dunkel
    (24, 27, 25),     # 1 Wand
    (34, 38, 35),     # 2 Wand hell (Rohr)
    (8, 8, 8),        # 3 Schatten
    (206, 198, 172),  # 4 Papier
    (186, 177, 150),  # 5 Papier dunkel (Flecken)
    (150, 139, 116),  # 6 Papier Rand
    (36, 32, 30),     # 7 Tinte
    (190, 36, 30),    # 8 rot
    (36, 140, 60),    # 9 gruen
    (22, 22, 22),     # 10 schwarz (Scheibe)
    (232, 232, 228),  # 11 weiss (Ziffern)
    (170, 160, 96),   # 12 Klebeband
    (120, 70, 40),    # 13 Rost
    (70, 66, 60),     # 14 Tinte hell (Glyphen-Rand)
    (48, 52, 49),     # 15 Rohr Glanz
]
W, H = 320, 240
BX0, BY0, BX1, BY1 = 64, 8, 256, 232            # Blatt 192 x 224


def atlas(args):
    if args.font:
        return json.load(open(args.font, encoding="utf-8"))
    adir = args.atlas_dir or os.path.join(REPO, "build", "r41_ventilkette", "atlas")
    pfad = os.path.join(adir, "re2_doc_font.json")
    if not os.path.exists(pfad):
        os.makedirs(adir, exist_ok=True)
        R.atlas_bauen(adir)
    return json.load(open(pfad, encoding="utf-8"))


def text(font, img, s, x, y, farbe, skala=1, fett=False):
    """Zeile s mit der RE2-Dokumentschrift ab Stift x, Oberkante y; Kernpixel (1..6) = farbe."""
    px = [[0] * 260 for _ in range(16)]
    R.zeile_setzen(font, px, s, 0, 0)
    for r in range(16):
        for c in range(260):
            if 1 <= px[r][c] <= 6:
                for dy in range(skala):
                    for dx in range(skala + (1 if fett else 0)):
                        X, Y = x + c * skala + dx, y + r * skala + dy
                        if 0 <= X < W and 0 <= Y < H:
                            img[Y][X] = farbe


def breite(font, s, skala=1):
    a, b = R.breite_von(font, s, 0)
    return (b + 1) * skala if b is not None else 0


def rechteck(img, x0, y0, x1, y1, f):
    for y in range(max(0, y0), min(H, y1)):
        for x in range(max(0, x0), min(W, x1)):
            img[y][x] = f


def scheibe(img, cx, cy, r, f, rand=None):
    for y in range(cy - r - 1, cy + r + 2):
        for x in range(cx - r - 1, cx + r + 2):
            d2 = (x - cx) ** 2 + (y - cy) ** 2
            if d2 <= r * r:
                img[y][x] = f
            elif rand is not None and d2 <= (r + 1) * (r + 1):
                img[y][x] = rand


# Ziffern der Nummernschilder: eigene 5 x 7-Blockschrift (x3), weiss auf schwarz wie die Schilder im Raum ("09", "08" ...
# im BSS-Bild, nb3_ventile_lage_sheet.png); die RE2-Dokumentschrift ist fuer Schilder zu fein (die "9" des Atlas hat nur
# wenige Belege).
ZIFFERN = {
    "0": ["01110", "10001", "10011", "10101", "11001", "10001", "01110"],
    "1": ["00100", "01100", "00100", "00100", "00100", "00100", "01110"],
    "2": ["01110", "10001", "00001", "00110", "01000", "10000", "11111"],
    "3": ["11110", "00001", "00001", "01110", "00001", "00001", "11110"],
    "4": ["00010", "00110", "01010", "10010", "11111", "00010", "00010"],
    "5": ["11111", "10000", "11110", "00001", "00001", "10001", "01110"],
    "6": ["00110", "01000", "10000", "11110", "10001", "10001", "01110"],
    "7": ["11111", "00001", "00010", "00100", "01000", "01000", "01000"],
    "8": ["01110", "10001", "10001", "01110", "10001", "10001", "01110"],
    "9": ["01110", "10001", "10001", "01111", "00001", "00010", "01100"],
}


def schild_ziffern(img, nummer, mitte_x, y, farbe, skala=3):
    breite_z = 5 * skala
    gesamt = len(nummer) * breite_z + (len(nummer) - 1) * skala
    x = mitte_x - gesamt // 2
    for ch in nummer:
        for r, zeile in enumerate(ZIFFERN[ch]):
            for c, bit in enumerate(zeile):
                if bit == "1":
                    for dy in range(skala):
                        for dx in range(skala):
                            img[y + r * skala + dy][x + c * skala + dx] = farbe
        x += breite_z + skala


def nahaufnahme(font):
    rnd = random.Random(2030)                   # fester Keim: reproduzierbar
    img = [[1] * W for _ in range(H)]
    for y in range(H):                          # Wand mit waagerechten Rohren (wie die Suedwand von 2030)
        for x in range(W):
            img[y][x] = 0 if rnd.random() < 0.35 else 1
    for (ry, rh) in ((30, 22), (96, 26), (170, 24)):
        for y in range(ry, ry + rh):
            for x in range(W):
                img[y][x] = 15 if y == ry + 4 else (2 if rnd.random() < 0.8 else 1)
        for y in (ry, ry + rh - 1):
            for x in range(W):
                img[y][x] = 3
    rechteck(img, BX0 + 4, BY0 + 4, BX1 + 4, BY1 + 4, 3)                   # Schatten
    rechteck(img, BX0, BY0, BX1, BY1, 4)                                    # Blatt
    for _ in range(900):                                                    # Flecken/Alterung
        x, y = rnd.randrange(BX0, BX1), rnd.randrange(BY0, BY1)
        img[y][x] = 5
    for (cx, cy, r) in ((BX1 - 24, BY1 - 14, 8), (BX1 - 22, BY0 + 60, 6)):
        for y in range(cy - r, cy + r):
            for x in range(cx - r, cx + r):
                if (x - cx) ** 2 + (y - cy) ** 2 < r * r and rnd.random() < 0.55:
                    img[y][x] = 13 if rnd.random() < 0.3 else 5
    for x in range(BX0, BX1):
        img[BY0][x] = 6; img[BY1 - 1][x] = 6
    for y in range(BY0, BY1):
        img[y][BX0] = 6; img[y][BX1 - 1] = 6
    rechteck(img, BX0 - 6, BY0 - 3, BX0 + 22, BY0 + 7, 12)                 # Klebeband
    rechteck(img, BX1 - 22, BY0 - 3, BX1 + 6, BY0 + 7, 12)
    # Titel
    y = BY0 + 12
    for i, t in enumerate(TITEL):
        b = breite(font, t)
        text(font, img, t, (BX0 + BX1 - b) // 2, y, 7, 1)
        y += 18
    for x in range(BX0 + 12, BX1 - 12):
        img[y + 2][x] = 7
    # vier Zeilen: Schritt, Farbscheibe, Farbwort, Nummernschild
    y0 = y + 12
    for k, (farbwort, nummer) in enumerate(PLAKAT):
        yy = y0 + k * 31
        text(font, img, "%d." % (k + 1), BX0 + 14, yy + 3, 7, 1, fett=True)
        f = {"RED": 8, "GREEN": 9, "BLACK": 10}[farbwort]
        scheibe(img, BX0 + 46, yy + 10, 9, f, rand=7)
        text(font, img, farbwort, BX0 + 62, yy + 3, 7, 1, fett=True)
        rechteck(img, BX0 + 130, yy - 3, BX0 + 176, yy + 24, 10)          # Schild schwarz
        schild_ziffern(img, nummer, BX0 + 153, yy, 11)
    y = y0 + 4 * 31 + 2
    for t in FUSS:
        b = breite(font, t)
        text(font, img, t, (BX0 + BX1 - b) // 2, y, 7, 1)
        y += 16
    return img


def png(img, pfad, skala=1):
    from PIL import Image
    im = Image.new("RGB", (W, H))
    im.putdata([PAL[v] for row in img for v in row])
    if skala != 1:
        im = im.resize((W * skala, H * skala), Image.NEAREST)
    im.save(pfad)


def rgb555(r, g, b):
    v = (r >> 3) | ((g >> 3) << 5) | ((b >> 3) << 10)
    return v if v else 0x8000                    # schwarz deckend (PSX: 0x0000 = durchsichtig)


def aushang_tim(img):
    """8bpp-TIM 64 x 64: das Blatt (BX0..BX1, BY0..BY1) auf 64 x 64 verkleinert (Mittelwert je Zelle), CLUT 256 bei (0,480)."""
    farben = []
    pix = []
    sx, sy = (BX1 - BX0) / 64.0, (BY1 - BY0) / 64.0
    for v in range(64):
        for u in range(64):
            acc = [0, 0, 0]; n = 0
            for y in range(int(BY0 + v * sy), int(BY0 + (v + 1) * sy)):
                for x in range(int(BX0 + u * sx), int(BX0 + (u + 1) * sx)):
                    c = PAL[img[y][x]]
                    acc[0] += c[0]; acc[1] += c[1]; acc[2] += c[2]; n += 1
            c = rgb555(acc[0] // n, acc[1] // n, acc[2] // n)
            if c not in farben:
                farben.append(c)
            pix.append(farben.index(c))
    assert len(farben) <= 256, len(farben)
    clut = farben + [0x8000] * (256 - len(farben))
    out = bytearray(struct.pack("<II", 0x10, 0x09))
    out += struct.pack("<IHHHH", 12 + 512, 0, 480, 256, 1) + struct.pack("<256H", *clut)
    out += struct.pack("<IHHHH", 12 + 64 * 64, 0, 0, 32, 64) + bytes(pix)
    return bytes(out), len(farben)


def aushang_md1(breite_w, hoehe_h):
    """EIN Viereck senkrecht in der x/y-Ebene (z 0), Mitte im Ursprung; Form der Wasserflaeche (reling_netz):
    v0 (x0,unten) v1 (x0,oben) v2 (x1,unten) v3 (x1,oben) = die Wasser-Ecken (x0,z0) (x0,z1) (x1,z0) (x1,z1) um +90 Grad
    um die x-Achse gedreht (y' = -z), Normale (0,-4096,0) -> (0,0,-4096)."""
    x0, x1 = -breite_w // 2, breite_w // 2
    yo, yu = -hoehe_h // 2, hoehe_h // 2
    ecken = [(x0, yu, 0), (x0, yo, 0), (x1, yu, 0), (x1, yo, 0)]
    nv, nq = 4, 1
    tv = 56
    tn = tv + 8 * nv
    tf = tn + 8
    tu = tf + 16 * nq
    b = bytearray(struct.pack("<III", 12 + tu, 0, 2) + struct.pack("<14I", tv, nv, tn, 1, tf, 0, tu, tv, nv, tn, 1, tf, nq, tu))
    for (x, y, z) in ecken:
        b += struct.pack("<hhhh", x, y, z, 0)
    b += struct.pack("<hhhh", 0, 0, -4096, 0)
    b += struct.pack("<8H", 0, 0, 0, 1, 0, 2, 0, 3)
    # UV: v0 unten links (0,63), v1 oben links (0,0), v2 unten rechts (63,63), v3 oben rechts (63,0); clut 0x7800, page 0x80
    b += struct.pack("<BBHBBHBBHBBH", 0, 63, 0x7800, 0, 0, 0x0080, 63, 63, 0, 63, 0, 0)
    assert len(b) == 12 + tu + 16 * nq
    return bytes(b)


NOTIZ_ZEILEN = ["TECHNICIAN'S NOTE", "", "These amateurs-not only", "did they completely mess",
                "up the water ventilation,", "so that the water level", "not only rises up to the", "railing but also goes",
                "beyond it until it has", "completely drained out-"]


def notiz_tim(font):
    """Zettel 160 x 224 (Papier wie die Nahaufnahme, Zeilen in der RE2-Dokumentschrift) -> 8bpp-TIM 64 x 64, CLUT (0,480)."""
    rng = np.random.default_rng(2031)
    w, h = 160, 224
    pap = np.zeros((h, w, 3), np.float32) + np.array([204, 198, 176], np.float32)
    pap += _rausch(rng, h, w, 0, 5)[..., None] + _rausch(rng, h, w, 10, 6)[..., None]
    yy, xx = np.mgrid[0:h, 0:w].astype(np.float32)
    pap *= (1.05 - 0.14 * (xx / w * 0.4 + yy / h * 0.6))[..., None]
    fl = np.exp(-(((xx - w * 0.7) ** 2 + (yy - h * 0.8) ** 2) / (2 * 24 * 24)))
    pap = pap * (1 - 0.15 * fl[..., None]) + np.array([120, 92, 52], np.float32) * (0.09 * fl[..., None])
    y = 10
    for k, t in enumerate(NOTIZ_ZEILEN):
        if t:
            m = _weich(_glyphen(font, t, 1), 0.5)
            m = m[:, :w - 16]
            _auftragen(pap, m, 8 if k else (w - m.shape[1]) // 2, y, (40, 38, 52), 0.9)
        y += 20 if k == 0 else 18
    rand = np.zeros((h, w), np.float32)
    rand[:2, :] = rand[-2:, :] = 1
    rand[:, :2] = rand[:, -2:] = 1
    pap *= (1 - 0.2 * _weich(rand, 1.0))[..., None]
    klein = Image.fromarray(np.clip(pap, 0, 255).astype(np.uint8)).resize((64, 64), Image.LANCZOS)
    q = klein.quantize(colors=255, method=Image.Quantize.MEDIANCUT, dither=Image.Dither.NONE)
    pal = q.getpalette()[:255 * 3]
    clut = [rgb555(pal[i], pal[i + 1], pal[i + 2]) for i in range(0, len(pal), 3)]
    clut += [0x8000] * (256 - len(clut))
    out = bytearray(struct.pack("<II", 0x10, 0x09))
    out += struct.pack("<IHHHH", 12 + 512, 0, 480, 256, 1) + struct.pack("<256H", *clut)
    out += struct.pack("<IHHHH", 12 + 64 * 64, 0, 0, 32, 64) + bytes(q.getdata())
    return bytes(out), klein


def re2_blatt():
    d = open(os.path.join(RE2, "ROOM2020.RDT"), "rb").read()
    m, t = d[0x003254:0x003254 + 404], d[0x01A160:0x01A160 + 34848]
    assert hashlib.md5(m).hexdigest() == "ae2d0a30a18ed9cb9f089c21839ca61e", "RE2 mesh01 MD1 md5"
    assert hashlib.md5(t).hexdigest() == "6b864a6f02ffd6c0720be1efa654c3cf", "RE2 mesh01 TIM md5"
    return m, t


def inc_array(name, b, pro_zeile=24):
    z = ["static const unsigned char %s[%d] = {" % (name, len(b))]
    for i in range(0, len(b), pro_zeile):
        z.append("    " + ",".join("0x%02x" % x for x in b[i:i + pro_zeile]) + ",")
    z.append("};")
    return "\n".join(z)


# ---- Nachbesserung 5 (Nachtrag 18 Punkt 1): die Nahaufnahme im Stil des Spiels ----------------------------------------------
# Nutzer: "entspricht nicht dem Stil des restlichen Spieles ... sieht aus wie aus Monkey Island". Vorbild (Dossier §0f B5-5,
# Vergleichsbogen C_belege/nb5_plakat_vergleich.png): die vorgerenderten Nahaufnahmen von RE1.5 (ROOM11D0 Cut 12 Zwinger-Pult:
# verwitterte blaugraue Stahlplatte, weisse Schildchen mit Schreibmaschinenschrift, Warnstreifen; ROOM11F0 Cut 12 Notstrom-
# Pult) und die RE2-Dokumente (Schreibmaschinenschrift = RE2-Dokumentschrift, Papier der FILE-Titelbilder, z. B. FILE17
# title_paper: gelbliches, fleckiges Papier, Licht von oben). Statt Flaechenfarben und Pixel-Blockziffern daher: Stahlwand mit
# Rauschen/Schlieren und Vignette, Papier mit Faserrauschen, Flecken, Knick, Klebeband und Schlagschatten, Schrift und
# Ziffern in der RE2-Dokumentschrift (Druckfarbe, weich gezeichnet), Farbpunkte als gedruckte Kreise mit Druckstruktur.
# Gerechnet in doppelter Aufloesung und auf 320 x 240 verkleinert (Kantenglaettung wie ein vorgerendertes Bild), danach
# 256 Farben (8 bpp) mit Fehlerstreuung. Inhalt unveraendert (Nachtrag 15: freigegeben).
import numpy as np
from PIL import Image, ImageFilter

S2 = 2                                            # Rechenaufloesung
PX0, PY0, PX1, PY1 = 62, 10, 258, 230             # Blatt (196 x 220, Seitenverhaeltnis des Aushangs 700 x 816 ~ 0,86)


def _rausch(rng, h, w, sigma, amp):
    n = rng.standard_normal((h, w)).astype(np.float32)
    if sigma > 0:
        n = np.array(Image.fromarray(((n - n.min()) / (np.ptp(n) + 1e-6) * 255).astype(np.uint8)).filter(
            ImageFilter.GaussianBlur(sigma))).astype(np.float32)
        n = (n - n.mean()) / (n.std() + 1e-6)
    return n * amp


def _glyphen(font, s, skala):
    """Zeile s in der RE2-Dokumentschrift als Deckungsmaske (0..1), Kernindizes 1..6 = Grauverlauf."""
    px = [[0] * 300 for _ in range(16)]
    R.zeile_setzen(font, px, s, 0, 0)
    a = np.array(px, dtype=np.float32)
    m = np.where((a >= 1) & (a <= 6), 1.0 - (a - 1) * 0.12, 0.0)
    b = 0
    for c in range(299, -1, -1):
        if m[:, c].any():
            b = c + 1
            break
    m = m[:, :max(b, 1)]
    return np.kron(m, np.ones((skala, skala), dtype=np.float32))


def _auftragen(bild, maske, x, y, farbe, deck=1.0):
    h, w = maske.shape
    H2, W2 = bild.shape[:2]
    x0, y0 = max(0, x), max(0, y)
    x1, y1 = min(W2, x + w), min(H2, y + h)
    if x1 <= x0 or y1 <= y0:
        return
    m = maske[y0 - y:y1 - y, x0 - x:x1 - x, None] * deck
    bild[y0:y1, x0:x1] = bild[y0:y1, x0:x1] * (1 - m) + np.array(farbe, np.float32) * m


def _weich(maske, sigma):
    im = Image.fromarray(np.clip(maske * 255, 0, 255).astype(np.uint8)).filter(ImageFilter.GaussianBlur(sigma))
    return np.array(im).astype(np.float32) / 255.0


def nahaufnahme_spielstil(font):
    rng = np.random.default_rng(2030)               # fester Keim: reproduzierbar
    W2, H2 = W * S2, H * S2
    yy, xx = np.mgrid[0:H2, 0:W2].astype(np.float32)
    # Stahlwand (blaugrau wie ROOM11D0 Cut 12), grobe Flecken + Korn + senkrechte Schlieren + Plattenfugen + Vignette
    wand = np.zeros((H2, W2, 3), np.float32) + np.array([62, 68, 72], np.float32)
    wand += _rausch(rng, H2, W2, 24, 9)[..., None]
    wand += _rausch(rng, H2, W2, 0, 5)[..., None]
    schlieren = _rausch(rng, 1, W2, 3, 1.0)[0]
    wand += (np.clip(schlieren, 0, None) * 10)[None, :, None] * np.array([0.9, 0.7, 0.5], np.float32)  # Rostspuren
    for fy in (88, 400):                            # waagerechte Plattenfugen (2x)
        wand[fy:fy + 3] *= 0.55
        wand[fy + 3:fy + 5] *= 1.15
    for fx in (60, 580):
        wand[:, fx:fx + 3] *= 0.6
    r = np.sqrt(((xx - W2 / 2) / (W2 / 2)) ** 2 + ((yy - H2 / 2) / (H2 / 2)) ** 2)
    wand *= (1.05 - 0.45 * np.clip(r, 0, 1.4) ** 2)[..., None]
    bild = wand
    x0, y0, x1, y1 = PX0 * S2, PY0 * S2, PX1 * S2, PY1 * S2
    # Schlagschatten des Blatts
    sm = np.zeros((H2, W2), np.float32)
    sm[y0 + 8:y1 + 10, x0 + 7:x1 + 9] = 1
    sm = _weich(sm, 7)
    bild *= (1 - 0.55 * sm)[..., None]
    # Papier: vergilbt, Faserrauschen, Lichtverlauf von oben links, Flecken, ein waagerechter Knick
    pap = np.zeros((y1 - y0, x1 - x0, 3), np.float32) + np.array([200, 193, 170], np.float32)
    ph, pw = pap.shape[:2]
    pap += _rausch(rng, ph, pw, 0, 5)[..., None]
    pap += _rausch(rng, ph, pw, 14, 6)[..., None]
    py, pxg = np.mgrid[0:ph, 0:pw].astype(np.float32)
    pap *= (1.06 - 0.16 * (pxg / pw * 0.4 + py / ph * 0.6))[..., None]
    for (cx, cy, rad, st) in ((pw * 0.78, ph * 0.83, 34, 0.18), (pw * 0.18, ph * 0.12, 22, 0.10), (pw * 0.62, ph * 0.40, 18, 0.07)):
        fl = np.exp(-(((pxg - cx) ** 2 + (py - cy) ** 2) / (2 * rad * rad)))
        fl *= np.clip(1 + _rausch(rng, ph, pw, 6, 0.6), 0, 2)
        pap = pap * (1 - st * fl[..., None]) + np.array([120, 92, 52], np.float32) * (st * 0.6 * fl[..., None])
    kn = int(ph * 0.36)
    pap[kn:kn + 2] *= 0.86
    pap[kn + 2:kn + 4] *= 1.05
    rand = np.zeros((ph, pw), np.float32)
    rand[:3, :] = rand[-3:, :] = 1
    rand[:, :3] = rand[:, -3:] = 1
    pap *= (1 - 0.18 * _weich(rand, 1.5))[..., None]
    bild[y0:y1, x0:x1] = pap
    # Schrift (Druckfarbe, weich): Titel fett (zweifach versetzt), Linie, vier Zeilen, Fuss
    tinte = (44, 40, 36)

    def zeile(s, y, skala, fett=False, x=None, farbe=tinte):
        m = _glyphen(font, s, skala)
        if fett:
            m = np.maximum(m, np.pad(m, ((0, 0), (1, 0)))[:, :-1])
        m = _weich(m, 0.7)
        xs = (x0 + x1 - m.shape[1]) // 2 if x is None else x
        _auftragen(bild, m, xs, y, farbe, 0.92)
        return m.shape[1]
    y = y0 + 22
    for t in TITEL:
        zeile(t, y, 2, fett=True)
        y += 34
    linie = np.zeros((3, x1 - x0 - 56), np.float32) + 1
    _auftragen(bild, _weich(np.pad(linie, 2), 0.8), x0 + 26, y + 2, tinte, 0.8)
    y0z = y + 24
    for k, (farbwort, nummer) in enumerate(PLAKAT):
        yy0 = y0z + k * 62
        zeile("%d." % (k + 1), yy0 + 6, 2, fett=True, x=x0 + 22)
        # gedruckter Farbpunkt mit Druckstruktur
        cx, cy, rr = x0 + 96, yy0 + 22, 17
        d = np.sqrt((xx - cx) ** 2 + (yy - cy) ** 2)
        punkt = np.clip(rr - d + 0.5, 0, 1) * np.clip(1 + _rausch(rng, H2, W2, 2.0, 0.05), 0, 1.1)
        farbe = {"RED": (168, 34, 30), "GREEN": (38, 118, 56), "BLACK": (34, 32, 30)}[farbwort]
        _auftragen(bild, _weich(np.clip(punkt, 0, 1), 0.6), 0, 0, farbe, 0.95)
        ring = np.clip(1 - np.abs(d - rr - 1.5), 0, 1)
        _auftragen(bild, ring, 0, 0, tinte, 0.5)
        zeile(farbwort, yy0 + 6, 2, fett=True, x=x0 + 126)
        # Nummernschild: schwarzes Feld mit weisser Schrift wie die Schilder im Raum
        bx0, by0, bx1, by1 = x0 + 262, yy0 - 2, x0 + 352, yy0 + 48
        feld = np.zeros((by1 - by0, bx1 - bx0), np.float32) + 1
        _auftragen(bild, _weich(feld, 0.8), bx0, by0, (28, 27, 26), 0.95)
        m = _weich(_glyphen(font, nummer, 4), 0.9)
        _auftragen(bild, m, (bx0 + bx1 - m.shape[1]) // 2, by0 + (by1 - by0 - m.shape[0]) // 2 + 2, (226, 224, 214), 0.95)
    y = y0z + 4 * 62 + 4
    for t in FUSS:
        zeile(t, y, 2)
        y += 32
    # Klebeband oben (halbdurchsichtig, Korn)
    for (tx, ty) in ((x0 - 14, y0 - 8), (x1 - 58, y0 - 8)):
        band = np.zeros((26, 72), np.float32) + 1
        b = _weich(band, 1.2) * np.clip(0.75 + _rausch(rng, 26, 72, 1, 0.08), 0, 1)
        _auftragen(bild, b, tx, ty, (214, 204, 158), 0.62)
    # Korn ueber alles, verkleinern (Kantenglaettung), 256 Farben
    bild += _rausch(rng, H2, W2, 0, 3)[..., None]
    bild = np.clip(bild, 0, 255).astype(np.uint8)
    klein = Image.fromarray(bild).resize((W, H), Image.LANCZOS).filter(ImageFilter.GaussianBlur(0.35))
    q = klein.quantize(colors=256, method=Image.Quantize.MEDIANCUT, dither=Image.Dither.FLOYDSTEINBERG)
    pal = q.getpalette()[:256 * 3]
    pal += [0] * (256 * 3 - len(pal))
    return q, bytes(pal), klein


def aushang_tim_rgb(klein):
    """8bpp-TIM 64 x 64: das Blatt (PX0..PX1, PY0..PY1) auf 64 x 64 verkleinert, 256 Farben, CLUT bei (0,480)."""
    blatt = klein.crop((PX0, PY0, PX1, PY1)).resize((64, 64), Image.LANCZOS)
    q = blatt.quantize(colors=255, method=Image.Quantize.MEDIANCUT, dither=Image.Dither.NONE)
    pal = q.getpalette()[:255 * 3]
    farben = []
    for i in range(0, len(pal), 3):
        farben.append(rgb555(pal[i], pal[i + 1], pal[i + 2]))
    clut = farben + [0x8000] * (256 - len(farben))
    pix = bytes(q.getdata())
    out = bytearray(struct.pack("<II", 0x10, 0x09))
    out += struct.pack("<IHHHH", 12 + 512, 0, 480, 256, 1) + struct.pack("<256H", *clut)
    out += struct.pack("<IHHHH", 12 + 64 * 64, 0, 0, 32, 64) + pix
    return bytes(out), len(set(pix))


def vergleichsbogen(vorher, nachher, nah_dir, ziel):
    from PIL import ImageDraw
    bilder = [(vorher, "vorher (Nachbesserung 4)"), (nachher, "nachher (Nachbesserung 5)")]
    if nah_dir:
        for name, was in (("ROOM11D12.ppm", "RE1.5 ROOM11D0 Cut 12 (Zwinger-Pult)"), ("ROOM11F12.ppm", "RE1.5 ROOM11F0 Cut 12 (Notstrom)")):
            p = os.path.join(nah_dir, name)
            if os.path.exists(p):
                bilder.append((Image.open(p).convert("RGB"), was))
    bogen = Image.new("RGB", (640 * 2, (480 + 18) * ((len(bilder) + 1) // 2)), (0, 0, 0))
    d = ImageDraw.Draw(bogen)
    for k, (b, was) in enumerate(bilder):
        x, y = (k % 2) * 640, (k // 2) * 498
        bogen.paste(b.resize((640, 480), Image.NEAREST), (x, y + 18))
        d.text((x + 4, y + 3), was, fill=(255, 255, 0))
    bogen.save(ziel)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--font", default=None)
    ap.add_argument("--atlas-dir", default=None)
    ap.add_argument("--nah-dir", default=None)
    args = ap.parse_args()
    font = atlas(args)
    alt = nahaufnahme(font)                         # Fassung der Nachbesserung 4 (nur fuer den Vergleich)
    vorher = Image.new("RGB", (W, H))
    vorher.putdata([PAL[v] for row in alt for v in row])
    q, pal, klein = nahaufnahme_spielstil(font)
    pix8 = bytes(q.getdata())
    tim, nfarben = aushang_tim_rgb(klein)
    md1 = aushang_md1(700, 816)
    bt, notiz = notiz_tim(font)                     # Nachbesserung 5 (Mangel 1): Zettel an der Wand statt RE2-Blatt am Boden
    bm = aushang_md1(500, 700)
    kopf = ("/* AUTOMATISCH ERZEUGT von re15_port/tools/r41_ventilkette/plakat.py - nicht von Hand aendern.\n"
            " * Runde 41 Spur C, Nachbesserung 4 (Nachtrag 13) + 5 (Nachtrag 18 Punkt 1, Stil des Spiels): Plakat mit der FALSCHEN\n"
            " * Ventil-Anleitung + Weltmodelle. Plakat (Reihenfolge wie gedruckt): %s; wahre Loesung %s.\n"
            " * Nahaufnahme 320 x 240, 8 bpp (1 Byte je Pixel), Palette 256 x RGB; md5 %s.\n"
            " * Aushang: MD1 1 Viereck 700 x 816 (md5 %s), TIM 8 bpp 64 x 64, %d Farben (md5 %s).\n"
            " * Dokument-Zettel (Nachbesserung 5): MD1 1 Viereck 500 x 700 (Aushang-Form), TIM 8 bpp 64 x 64 (md5 %s). */\n"
            % (PLAKAT, LOESUNG, hashlib.md5(pix8).hexdigest(), hashlib.md5(md1).hexdigest(), nfarben,
               hashlib.md5(tim).hexdigest(), hashlib.md5(bt).hexdigest()))
    teile = [kopf,
             "#define RE15_R41C_PLAKAT_W %d\n#define RE15_R41C_PLAKAT_H %d\n#define RE15_R41C_PLAKAT_BPP 8" % (W, H),
             inc_array("re15_r41c_plakat_pal", pal),
             inc_array("re15_r41c_plakat_8bpp", pix8),
             inc_array("re15_r41c_aushang_md1", md1),
             inc_array("re15_r41c_aushang_tim", tim),
             inc_array("re15_r41c_blatt_md1", bm),
             inc_array("re15_r41c_blatt_tim", bt)]
    ziel = os.path.join(GEN, "ventilkette_r41_plakat.inc")
    open(ziel, "w", newline="\n").write("\n".join(teile) + "\n")
    os.makedirs(BELEGE, exist_ok=True)
    q.convert("RGB").resize((W * 2, H * 2), Image.NEAREST).save(os.path.join(BELEGE, "nb5_plakat.png"))
    vergleichsbogen(vorher, q.convert("RGB"), args.nah_dir, os.path.join(BELEGE, "nb5_plakat_vergleich.png"))
    notiz.resize((256, 256), Image.NEAREST).save(os.path.join(BELEGE, "nb5_dokument_zettel_tex.png"))
    print("geschrieben:", ziel, "Plakat %s" % (PLAKAT,), "Aushang-TIM %d Farben" % nfarben)


if __name__ == "__main__":
    main()
