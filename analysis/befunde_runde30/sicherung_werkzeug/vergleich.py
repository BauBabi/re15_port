#!/usr/bin/env python3
"""Legt die VIER Darstellungen der Sicherung nebeneinander und misst ihre Unterschiede.

  (a) Welt-Modell   build/sicherung/probe.png + eingebackene Textur (gen/sicherung_prop.inc)
  (b) Item-Bild     ITEM/ITPS.ITP @0xC0000 (Item 0x40), 112x72
  (c) Inventar-Icon DATA/ITEMALL.PIX Tile 0x40 @0x12C00, 40x30, CLUT = ST_00.TIM Zeile 0
  (d) ROOM1050      Cut 7 / Cut 8 (STAGE1/ROOM105.BSS Scheibe 7/8), Ausschnitt Sicherungskasten

Gemessen wird je Darstellung die SILHOUETTE des Gegenstands (Hauptachsen ueber die
Kovarianz der Objektpixel): Laenge, Dicke, Schlankheit, mittlere Farbe, Anteil heller
Pixel (Keramik/Metall hell), Anteil MESSING-farbener Pixel (R > G > B, R-B >= 60).

    python analysis/befunde_runde30/sicherung_werkzeug/vergleich.py
Voraussetzung: item_bilder.py gelaufen, build/r30_sicherung/bg/ROOM1050{7,8}.ppm vorhanden.
"""
import os
import sys

import numpy as np
from PIL import Image, ImageDraw

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from r30_lib import REPO, inc_bytes, tim_lesen  # noqa: E402
from raster import tim_als_rgba  # noqa: E402

ZIEL = os.path.join(REPO, "build", "r30_sicherung")


def hauptachsen(maske):
    ys, xs = np.nonzero(maske)
    p = np.stack([xs, ys], 1).astype(float)
    m = p.mean(0)
    c = np.cov((p - m).T)
    w, v = np.linalg.eigh(c)
    lang = v[:, 1]
    quer = v[:, 0]
    a = (p - m) @ lang
    b = (p - m) @ quer
    # robuste Ausdehnung: 1.-99. Perzentil (einzelne Ausreisserpixel zaehlen nicht)
    L = np.percentile(a, 99) - np.percentile(a, 1) + 1
    D = np.percentile(b, 99) - np.percentile(b, 1) + 1
    winkel = np.degrees(np.arctan2(lang[1], lang[0]))
    # Dickenprofil entlang der Laengsachse in 10 Abschnitten
    prof = []
    kanten = np.linspace(np.percentile(a, 1), np.percentile(a, 99), 11)
    for i in range(10):
        s = b[(a >= kanten[i]) & (a < kanten[i + 1])]
        prof.append(0.0 if len(s) == 0 else float(np.percentile(s, 98) - np.percentile(s, 2) + 1))
    return L, D, winkel, prof


def farben(rgb, maske):
    px = rgb[maske].astype(float)
    lum = 0.299 * px[:, 0] + 0.587 * px[:, 1] + 0.114 * px[:, 2]
    messing = (px[:, 0] > px[:, 1]) & (px[:, 1] > px[:, 2]) & (px[:, 0] - px[:, 2] >= 60)
    return px.mean(0), lum.mean(), float((lum >= 150).mean()), float(messing.mean()), \
        float((lum < 60).mean())


def bericht(name, rgb, maske):
    L, D, w, prof = hauptachsen(maske)
    m, lum, hell, mess, dunkel = farben(rgb, maske)
    print("%-28s Objektpixel %5d  Laenge %5.1f px  Dicke %4.1f px  Schlankheit %4.1f : 1  "
          "Achse %+5.0f Grad" % (name, int(maske.sum()), L, D, L / D, w))
    print("%-28s Farbe Mittel RGB (%3.0f,%3.0f,%3.0f) Lum %5.1f | hell(>=150) %4.1f %% | "
          "dunkel(<60) %4.1f %% | messingfarben %4.1f %%"
          % ("", m[0], m[1], m[2], lum, hell * 100, dunkel * 100, mess * 100))
    print("%-28s Dickenprofil entlang der Achse (10 Abschnitte, px): %s"
          % ("", " ".join("%.0f" % x for x in prof)))
    return dict(L=L, D=D, lum=lum, hell=hell, mess=mess, rgb=m)


def objekt_auf_blau(img):
    """ITPS/Icon: Hintergrund ist das Item-Blau (B dominiert, R und G klein), der Rand
    hellgrau. Objekt = alles, was weder Blau noch der 2-px-Rahmen ist."""
    a = np.asarray(img.convert("RGB")).astype(int)
    h, w, _ = a.shape
    blau = (a[..., 2] > a[..., 0] + 25) & (a[..., 2] > a[..., 1] + 25)
    schwarzblau = (a[..., 0] < 20) & (a[..., 1] < 20)   # tiefes Hintergrundblau/-schwarz
    m = ~(blau | schwarzblau)
    rand = max(2, w // 40)
    m[:rand] = m[-rand:] = False
    m[:, :rand] = m[:, -rand:] = False
    return a.astype(np.uint8), m


def gross(img, f):
    return img.resize((img.size[0] * f, img.size[1] * f), Image.NEAREST)


def main():
    print("== Silhouetten-Messung ==")
    itps = Image.open(os.path.join(ZIEL, "itps_40.png"))
    icon = Image.open(os.path.join(ZIEL, "icon_40.png"))
    a, m = objekt_auf_blau(itps)
    r_itps = bericht("(b) Item-Bild ITPS 0x40", a, m)
    Image.fromarray((m * 255).astype(np.uint8)).save(os.path.join(ZIEL, "maske_itps_40.png"))
    a, m = objekt_auf_blau(icon)
    r_icon = bericht("(c) Icon ITEMALL 0x40", a, m)

    bg7 = np.asarray(Image.open(os.path.join(ZIEL, "bg", "ROOM10507.ppm")).convert("RGB"))
    bg8 = np.asarray(Image.open(os.path.join(ZIEL, "bg", "ROOM10508.ppm")).convert("RGB"))
    d = np.abs(bg8.astype(int) - bg7.astype(int)).max(2)
    print("(d) ROOM1050 Cut 8 gegen Cut 7: %d Pixel mit Kanaldifferenz > 4, bbox x%d..%d y%d..%d"
          % ((d > 4).sum(), np.nonzero(d > 4)[1].min(), np.nonzero(d > 4)[1].max(),
             np.nonzero(d > 4)[0].min(), np.nonzero(d > 4)[0].max()))
    # die NEUE (rechte) Sicherung: Summendifferenz > 30 im Fenster x185..215, y80..180
    s = bg8.astype(int).sum(2) - bg7.astype(int).sum(2)
    m8 = np.zeros(d.shape, bool)
    m8[80:180, 185:216] = s[80:180, 185:216] > 30
    r_bg = bericht("(d) Sicherung in Cut 8", bg8, m8)
    # die LINKE Sicherung, die in BEIDEN Cuts steckt. Der Hintergrund ist eine NACHT-Szene:
    # die Schwelle wird aus dem Bild genommen (Mitte zwischen Wand und Sicherung), nicht gesetzt.
    lum7 = 0.299 * bg7[..., 0] + 0.587 * bg7[..., 1] + 0.114 * bg7[..., 2]
    fen = lum7[100:156, 136:157]
    schw = (np.percentile(fen, 20) + np.percentile(fen, 90)) / 2.0
    ml = np.zeros(d.shape, bool)
    ml[100:156, 136:157] = fen > schw
    print("(d) linke Sicherung: Fenster x136..156 y100..155, Lum 20.%%=%.0f 90.%%=%.0f -> Schwelle %.0f"
          % (np.percentile(fen, 20), np.percentile(fen, 90), schw))
    bericht("(d) linke Sicherung Cut 7", bg7, ml)

    tim = tim_als_rgba(tim_lesen(inc_bytes("re15_sicherung_tim")))
    tex = tim[0:128, 0:32]          # Mantelflaeche oben links (sicherung_engine_export.py)
    mt = tex[..., 3] > 0
    mm, lum, hell, mess, dunkel = farben(tex[..., :3], mt)
    print("%-28s Textur 32x128 (eingebacken, x2.87): Mittel RGB (%3.0f,%3.0f,%3.0f) Lum %5.1f | "
          "hell %4.1f %% | dunkel %4.1f %% | messingfarben %4.1f %%"
          % ("(a) Welt-Modell", mm[0], mm[1], mm[2], lum, hell * 100, dunkel * 100, mess * 100))
    print("%-28s Geometrie: Laenge 406, Durchmesser 52..56 -> Schlankheit %.1f : 1 "
          "(MD1 bbox x[-203..203] y/z[-26..26])" % ("", 406 / 54.0))

    # ---- Blatt --------------------------------------------------------------
    probe = Image.open(os.path.join(REPO, "build", "sicherung", "probe.png")).convert("RGB")
    kasten7 = Image.fromarray(bg7[10:190, 120:280]).resize((480, 540), Image.NEAREST)
    kasten8 = Image.fromarray(bg8[10:190, 120:280]).resize((480, 540), Image.NEAREST)
    itps_g = Image.new("RGB", itps.size, (40, 40, 48))
    itps_g.paste(itps, (0, 0), itps)
    icon_g = Image.new("RGB", icon.size, (40, 40, 48))
    icon_g.paste(icon, (0, 0), icon)
    texb = Image.fromarray(tex[..., :3]).resize((32 * 4, 128 * 4), Image.NEAREST)

    B = 1700
    blatt = Image.new("RGB", (B, 1500), (24, 24, 28))
    dr = ImageDraw.Draw(blatt)
    y = 10
    dr.text((10, y), "(a) WELT-MODELL (Port, gen/sicherung_prop.inc) - Abnahmebild build/sicherung/probe.png: "
                     "Cut 7 | Cut 8 | Cut 7 + Modell", fill=(255, 255, 0))
    p2 = probe.resize((probe.size[0] * 1500 // probe.size[0], probe.size[1] * 1500 // probe.size[0]))
    blatt.paste(p2, (10, y + 16))
    dr.text((1530, y + 16), "Textur 32x128", fill=(255, 255, 0))
    blatt.paste(texb, (1540, y + 32))
    y += 16 + p2.size[1] + 14
    dr.text((10, y), "(b) ITEM-BILD ITPS.ITP @0xC0000 (112x72, 5x) = Aufnahme-Modal + CHECK-Foto",
            fill=(255, 255, 0))
    blatt.paste(gross(itps_g, 5), (10, y + 16))
    dr.text((600, y), "(c) INVENTAR-ICON ITEMALL.PIX Tile 0x40 @0x12C00 (40x30, 10x)",
            fill=(255, 255, 0))
    blatt.paste(gross(icon_g, 10), (600, y + 16))
    dr.text((1030, y), "Nachbarn zur Gegenprobe der Indexregel: 0x3F Pocket Watch | 0x41 Spark Plug",
            fill=(255, 255, 0))
    for i, it in enumerate((0x3F, 0x41)):
        n = Image.open(os.path.join(ZIEL, "itps_%02X.png" % it))
        g = Image.new("RGB", n.size, (40, 40, 48))
        g.paste(n, (0, 0), n)
        blatt.paste(gross(g, 3), (1030 + i * 340, y + 16))
    y += 16 + 360 + 14
    dr.text((10, y), "(d) ROOM1050 Sicherungskasten, Ausschnitt x120..279 y10..189 (3x): "
                     "Cut 7 (rechter Sockel leer) | Cut 8 (eingesetzt)", fill=(255, 255, 0))
    blatt.paste(kasten7, (10, y + 16))
    blatt.paste(kasten8, (500, y + 16))
    y += 16 + 540 + 10
    blatt = blatt.crop((0, 0, B, y))
    blatt.save(os.path.join(ZIEL, "nebeneinander.png"))
    print("\ngeschrieben: build/r30_sicherung/nebeneinander.png  %dx%d" % blatt.size)


if __name__ == "__main__":
    main()
