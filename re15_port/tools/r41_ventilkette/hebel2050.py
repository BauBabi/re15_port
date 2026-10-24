#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""hebel2050.py - Runde 41 Spur C, Nachbesserung 5 (Nachtrag 18 Punkt 2): die vier Hebel des 2050-Schaltpults gehen beim
Umlegen sichtbar nach unten.

Die Hebel sind im Hintergrund ROOM2050 Cut 15 GEMALT (BSS/ROOM2050/BG15.BSS; keine Modelle, Dossier §6). Gemessen im
dekodierten Bild (probe_bg_dump, dieselben Engine-Dekoder wie im Spiel; Dossier §0f B5-2): je Hebel ein Griff (weisser
T-Umriss mit schwarzem Fuss) ueber einem gruenen Gehaeuse, von dem zwei schwarze Klingen senkrecht bis zur Unterkante des
Gehaeuses laufen (Spaltenprofil: Helligkeit < 16 in den Klingenspalten, 30..70 im Gehaeuse daneben). Der bewegliche Teil
= Griff + Klingen, Drehpunkt = Gehaeuse-Unterkante.

FORM der Bewegung = die Original-Hebel ROOM11F0 (Notstrom, dieselbe Pult-Art): sub06 @0x01322.. `2f 05 40 00` Speed_set(5,
+0x40) + For 16 (`0d 00 04 00 10 00` @0x01336) { Add_speed `30`, Evt_next } -> rot_z +1024 (90 Grad) in 16 Bildern, Grundstellung
rot_z 512 (Obj_model_set obj 2..11 @0x00E76.. `.. 00 00 00 00 00 02 ..`). In der Bildebene: Hebellaenge L um den Drehpunkt,
Lage des Griffs = L * cos(45 + 90 * t/16 Grad) / cos(45 Grad) -> von oben (t 0, wie gemalt) bis gespiegelt unter den
Drehpunkt (t 16). Gezeichnet wird das als senkrechte Skalierung des beweglichen Teils um den Drehpunkt (Faktor +1 .. -1).

Je Hebel erzeugt: Maske (Griff + Klingen), "Grund" = das Bild unter der Maske mit cv2.inpaint (Telea, Radius 3) aus der
Umgebung aufgefuellt (das Gehaeuse ohne Klingen, die Wand hinter dem Griff), "Teil" = die Originalpixel unter der Maske.

Ausgabe: re15_port/engine/src/gen/ventilkette_r41_hebel.inc, Vorschau analysis/befunde_runde41/C_belege/nb5_hebel2050.png
Aufruf: python hebel2050.py --ppm <ROOM20515.ppm>   (bg_dump: probe_bg_dump <cd> <dir> 2 -> <dir>/ROOM20515.ppm)
"""
import argparse
import hashlib
import math
import os

import cv2
import numpy as np
from PIL import Image, ImageDraw

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
GEN = os.path.join(REPO, "re15_port", "engine", "src", "gen")
BELEGE = os.path.join(REPO, "analysis", "befunde_runde41", "C_belege")

# Gemessen im Bild ROOM20515 (320 x 240), Dossier §0f B5-2: Griff-Rechteck (x0, x1, y0, y1 einschliesslich), Klingenspalten
# (x0, x1), Gehaeuse-Oberkante (Klingenbeginn), Drehpunkt = Gehaeuse-Unterkante.
HEBEL = [
    # Griff                Klingen                Gehaeuse-Oberkante  Drehpunkt
    ((69, 97, 79, 92),    ((74, 76), (92, 95)),  93,                121),
    ((117, 146, 79, 91),  ((122, 124), (140, 143)), 91,             120),
    ((165, 194, 79, 91),  ((169, 172), (188, 190)), 91,             120),
    ((231, 253, 98, 105), ((234, 235), (246, 248)), 106,            127),
]


def masken(rgb):
    h, w = rgb.shape[:2]
    out = []
    for (gx0, gx1, gy0, gy1), klingen, hy0, py in HEBEL:
        m = np.zeros((h, w), np.uint8)
        m[gy0:gy1 + 1, gx0:gx1 + 1] = 1
        for kx0, kx1 in klingen:
            m[hy0:py + 1, kx0:kx1 + 1] = 1
        out.append(m)
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--ppm", required=True)
    a = ap.parse_args()
    roh = open(a.ppm, "rb").read()
    rgb = np.array(Image.open(a.ppm).convert("RGB"))
    bgr = cv2.cvtColor(rgb, cv2.COLOR_RGB2BGR)
    ms = masken(rgb)
    alle = np.zeros(rgb.shape[:2], np.uint8)
    for m in ms:
        alle |= m
    grund = cv2.cvtColor(cv2.inpaint(bgr, alle * 255, 3, cv2.INPAINT_TELEA), cv2.COLOR_BGR2RGB)
    zeilen = ["/* AUTOMATISCH ERZEUGT von re15_port/tools/r41_ventilkette/hebel2050.py - nicht von Hand aendern.",
              " * Hebel des 2050-Schaltpults (Runde 41 Spur C, Nachbesserung 5, Nachtrag 18 Punkt 2): je Hebel Rechteck, Maske",
              " * (Griff + Klingen), Grund (Bild unter der Maske, aus der Umgebung aufgefuellt) und Teil (Originalpixel), RGB.",
              " * Quelle: ROOM2050 Cut 15 (BSS/ROOM2050/BG15.BSS), Bild md5 %s. Messung im Kopf des Werkzeugs. */"
              % hashlib.md5(roh).hexdigest(), ""]
    eintraege = []
    for i, ((gx0, gx1, gy0, gy1), klingen, hy0, py) in enumerate(HEBEL):
        m = ms[i]
        ys, xs = np.where(m)
        x0, x1, y0, y1 = xs.min(), xs.max(), ys.min(), ys.max()
        w, h = x1 - x0 + 1, y1 - y0 + 1
        mk = m[y0:y1 + 1, x0:x1 + 1].astype(np.uint8)
        g = grund[y0:y1 + 1, x0:x1 + 1]
        t = rgb[y0:y1 + 1, x0:x1 + 1]
        name = "re15_r41c_hebel%d" % i
        zeilen.append("/* Hebel %d: Rechteck x %d..%d y %d..%d, Drehpunkt y %d, %d Maskenpixel */" % (i + 1, x0, x1, y0, y1, py, int(mk.sum())))
        for teil, arr in (("maske", mk.reshape(-1)), ("grund", g.reshape(-1)), ("teil", t.reshape(-1))):
            b = bytes(int(v) for v in arr)
            zeilen.append("static const unsigned char %s_%s[%d] = {" % (name, teil, len(b)))
            for k in range(0, len(b), 24):
                zeilen.append("    " + ",".join("0x%02x" % v for v in b[k:k + 24]) + ",")
            zeilen.append("};")
        eintraege.append((name, x0, y0, w, h, py))
        zeilen.append("")
    zeilen.append("typedef struct { int x0, y0, w, h, dreh_y; const unsigned char *maske, *grund, *teil; } re15_r41c_hebel_t;")
    zeilen.append("static const re15_r41c_hebel_t re15_r41c_hebel[4] = {")
    for name, x0, y0, w, h, py in eintraege:
        zeilen.append("    { %d, %d, %d, %d, %d, %s_maske, %s_grund, %s_teil }," % (x0, y0, w, h, py, name, name, name))
    zeilen.append("};")
    ziel = os.path.join(GEN, "ventilkette_r41_hebel.inc")
    open(ziel, "w", newline="\n").write("\n".join(zeilen) + "\n")
    print("geschrieben:", ziel)

    # Vorschau: Spalten t = 0 / 4 / 8 / 12 / 16 (alle vier Hebel umgelegt), Ausschnitt x 60..260 y 70..180, 2-fach
    bilder = []
    for t16 in (0, 4, 8, 12, 16):
        img = rgb.copy()
        if t16:
            s = math.cos(math.radians(45 + 90 * t16 / 16)) / math.cos(math.radians(45))
            for i, ((gx0, gx1, gy0, gy1), klingen, hy0, py) in enumerate(HEBEL):
                m = ms[i]
                img[m == 1] = grund[m == 1]
                ys, xs = np.where(m)
                for y, x in zip(ys, xs):
                    yn = int(round(py + (y - py) * s))
                    if 0 <= yn < img.shape[0]:
                        img[yn, x] = rgb[y, x]
        bilder.append(Image.fromarray(img[70:180, 60:260]).resize((400, 220), Image.NEAREST))
    sheet = Image.new("RGB", (400 * 5, 236), (0, 0, 0))
    d = ImageDraw.Draw(sheet)
    for k, b in enumerate(bilder):
        sheet.paste(b, (400 * k, 16))
        d.text((400 * k + 4, 2), "t = %d/16 (11F0 sub06 For 16)" % (0, 4, 8, 12, 16)[k], fill=(255, 255, 0))
    os.makedirs(BELEGE, exist_ok=True)
    sheet.save(os.path.join(BELEGE, "nb5_hebel2050.png"))
    print("Vorschau:", os.path.join(BELEGE, "nb5_hebel2050.png"))


if __name__ == "__main__":
    main()
