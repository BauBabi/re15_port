#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""panel_bild.py - Runde 41 Spur N (AUFTRAG.md Nachtrag 37, Nutzer-Freigabe "A Druckspeicher"): Bild des Panel-Raetsels ROOM2030.

Dossier analysis/befunde_runde41/N_nachtrag30.md §2 (Kamera, Lage, Regeln). Erzeugt engine/src/gen/panel2030_r41.inc:
  re15_r41n_panel_grund  RGB555 320 x 240 wie die Hintergruende (15 Bit je Bildpunkt): Panel mit LEEREN Tanks, ohne Zeiger, Lampen aus, ohne Cursor
  re15_r41n_panel_voll   RGB555 je Fenster: dieselbe Stelle mit VOLLEM Tank (engine/src/panel2030_r41.c setzt beide je Fuellstand zusammen)
  re15_r41n_panel_cursor Cursor-Stempel aus dem Spielbild 2050 (C_belege/pult2050_loesung_sheet.png, Kachel F540)
Rahmen pixelgleich aus ROOM2050 Cut 15, EXIT-Feld pixelgleich aus ROOM11F0 Cut 10 (x 210..238, y 175..204).
  re15_r41n_panel_glyphen / re15_r41n_panel_clut  die Beschriftung (Spur PF, unten)

Runde 41 Spur PF (Nutzer: "Zahlen und Texte auf den Panel in Nahansicht sind kaum lesbar/verschwommen"; Dossier PF_panelfix.md §2):
Ursache: die Schrift war Arial Bold mit 4,6..8,2 Bildpunkten, 4-fach ueberabgetastet, weichgezeichnet, mit INTER_AREA verkleinert, verrauscht und
durch die JPEG-Kette (4:2:0, Farbe auf halber Aufloesung) gezogen. Jetzt: Bild ohne Schrift durch dieselbe Kette, DANACH die Beschriftung Pixel fuer
Pixel aus der 8x8-Schrift des Spiels gestempelt (kein Weichzeichnen, ganzzahlige Lage, 15 Bit): DATA/TEX.TIM, Glyphe c bei
u = ((c-0x20) & 0x1f) * 8, v = ((c-0x20) & 0xe0) >> 5 * 8 auf der Seite ab Texel x 256 (FUN_80029214 @0x80029214, tpage VRAM 768 = TIM-Texel 256,
render_pc.c re15_render_pc_debug_text), Farben je Schild als 16er-CLUT wie die PSX (helle Schrift = CLUT-Zeile 0 von TEX.TIM @(256,480) unveraendert).
Laufweite, Lage und Wortlaut der Schilder sind PORT-WAHL; "PRESSURE 80" passt mit 8 Bildpunkten je Zeichen nicht auf das Schild (88 > 54) -> "SET 80".

Runde 41 Spur PF, Nachbesserung 1 (Nutzer zur Schrift: "ja, ist super."; Zusage: Drehscheiben und Teilstriche auf ganze Bildpunkte): die vier
Drehscheiben und das Zeiger-Instrument liefen bisher wie das uebrige Bild durch die Kette (4-fach ueberabgetastet, weiche Kanten, INTER_AREA, JPEG 4:2:0):
Teilstriche 0,44..0,72 Texel breit und Querbalken 2,3 Texel wurden zu grauen Verlaeufen. Jetzt stempelt scheiben_stempeln() sie NACH der Kette Texel fuer
Texel (wie die Schrift): jeder Texel gehoert ganz oder gar nicht zu Lippe, Ring, Blatt, Teilstrich, Band oder Balken (Entscheidung an der Texelmitte),
Farben wie kit.scheibe/kit.instrument/kit.drehscheibe, danach das Gamma von fertig() (1,16). Mitten auf Texelmitten (x,5): Instrument 124,5/87,5,
Scheiben 125,5 / 163,5 / 200,5 / 237,5 bei 160,5. Striche: je Schritt entlang der Hauptachse genau ein Texel (linie_px), der Querbalken zwei Texel je
Spalte in der Steigung 1:2 (63,4 Grad statt 60: die einzige Lage nahe 60 Grad mit gleichmaessiger Treppe). In der Kette bleibt nur der Schlagschatten.
Der Zeiger (Laufzeit, engine/src/panel2030_r41.c) folgt derselben Regel. Alles PORT-WAHL wie das Panel selbst.

Aufruf: R41N_BG=<dumps stage2>;<dumps stage1> C:/Python310/python.exe re15_port/tools/r41_nachtrag30/panel_bild.py [--beleg <png>]
"""
import hashlib
import os
import struct
import sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import numpy as np, cv2
from PIL import Image
import panel_kit as kit
from panel_kit import *

kit.QUELLE[:] = os.environ.get("R41N_BG", ".").split(";")
GEN = os.path.join(kit.TREE, "re15_port", "engine", "src", "gen")
ECHT = kit.echtbild()

# Lage der Elemente im entzerrten Bild (Schirmpixel, gemessen auf view/grund_grid.png; S 0.14, Y0 -9040)
T = [(53, 69), (73, 89), (139.5, 155.5), (173.5, 189.5), (193.5, 209.5)]   # fuenf Roehrenelemente, x
TK0, TK1 = 41, 131                                                        # Kopf oben, Fuss unten
GT, GB = 66, 116                                                          # Schauglas: Wert 100 / Wert 0
D0 = (124.5, 87.5)                                                        # Hauptanzeige (Nachbesserung 1: Texelmitte, frueher 124/87)
DR = [(125.5, 160.5), (163.5, 160.5), (200.5, 160.5), (237.5, 160.5)]     # Scheibenreihe (Texelmitten, frueher 125/163/200,5/237,5 bei 160)
R = 13.9
PL = (50, 33, 272.5, 228)                                                 # Tafel unter der Schiene
yw = lambda v: GB - (GB - GT) * v / 100                                    # Wert -> Bildzeile im Schauglas
LOCH_Y = 122.5


def schraube(img, cx, cy, r=3.0):
    schatten(img, sd_kreis(cx, cy, r), .55, .8, .7, .9)
    phi = np.arctan2(Y - cy, X - cx)
    li = .5 + .5 * np.cos(phi + np.radians(125))
    f = (36 + 104 * li ** 1.6)[..., None] * col(1., 1., .96) + (rausch(.5, 67) * 3)[..., None]
    a = aa(sd_kreis(cx, cy, r), .5)
    img[:] = img * (1 - a[..., None]) + f * a[..., None]
    blend(img, col(70, 70, 68), aa(sd_kreis(cx, cy, r - 1.), .5))
    w = np.radians(35)
    s, c = np.sin(w), np.cos(w)
    blend(img, col(10, 10, 10), aa(sd_seg(cx - 1.8 * c, cy - 1.8 * s, cx + 1.8 * c, cy + 1.8 * s, .36), .4))


def dreck(img, maske):
    """Laufspuren und Rost: von der Schiene und den Schrauben abwaerts, an den Raendern dichter."""
    r = np.random.default_rng(91)
    n = r.standard_normal((H * SS, W * SS)).astype(np.float32)
    lauf = cv2.GaussianBlur(n, (0, 0), sigmaX=1.2 * SS, sigmaY=16 * SS)
    lauf /= lauf.std()
    dichte = np.exp(-(Y - 34) / 42) * 1.05 + .28
    d = np.clip(lauf * .5 + .1, 0, 1.5) * dichte * maske
    img *= (1 - .36 * np.clip(d, 0, 1))[..., None]
    fl = rausch(5, 92) + rausch(1.4, 93) * .55
    rand = np.clip(1 - (np.minimum.reduce([X - PL[0], PL[2] - X]) / 26), 0, 1) ** 1.5 + np.clip(1 - (Y - 34) / 20, 0, 1) * .6
    rost = np.clip(fl - 1.25 + rand * 1.35, 0, 1) * maske
    blend(img, col(92, 58, 34), rost * .38)
    blend(img, col(50, 30, 18), np.clip(rost - .5, 0, 1) * .5)


def grund():
    e = ECHT.copy()
    lit = 255 * np.power(e / 255, .80) * 1.12
    # Aufbauten, die neu gezeichnet werden, aus dem Tiefpass nehmen (normierte Faltung)
    m = np.ones((H * SS, W * SS), np.float32)
    for (x0, x1) in T:
        m = np.minimum(m, 1 - aa(sd_box(x0 - 4, TK0 - 4, x1 + 4, TK1 + 4, 1), 1.))
    for (cx, cy) in [D0] + DR:
        m = np.minimum(m, 1 - aa(sd_kreis(cx, cy, 21), 1.))
    m = np.minimum(m, 1 - aa(sd_box(208, 42, 276, 92), 1.))
    m = np.minimum(m, 1 - aa(sd_box(125, 178, 188, 209), 1.))
    m = np.minimum(m, 1 - aa(sd_box(66, 208, 276, 240), 1.))
    z = cv2.GaussianBlur(lit * m[..., None], (0, 0), 7 * SS)
    n = cv2.GaussianBlur(m, (0, 0), 7 * SS)[..., None] + 1e-3
    tief = z / n
    # Wand: Echtbild, leicht nachgeschaerft, dunkler; etwas Korn gegen die Unschaerfe der Vergroesserung
    gross = cv2.GaussianBlur(lit, (0, 0), 1.6 * SS)
    img = (lit + (lit - gross) * .7).clip(0, 255) * .74
    img += (rausch(.8, 51) * 3.4 + rausch(.3, 52) * 3.2)[..., None]
    # Tafel: tiefpass-gefiltertes Echtbild + Rest der Echtstruktur + feine Blechstruktur
    fein = (lit - cv2.GaussianBlur(lit, (0, 0), 3 * SS))
    tafel = tief * 1.05 + .22 * fein * m[..., None]
    tafel += (rausch(5, 61) * 1.6 + rausch(1.1, 62) * 1.8 + rausch(.35, 63) * 2.4 + rausch(.15, 64) * 1.4)[..., None]
    b = rausch(0, 65)
    b = cv2.GaussianBlur(b, (1, 0), sigmaX=.1, sigmaY=10 * SS)
    tafel += (b / b.std() * 1.5)[..., None]
    tafel *= (1.10 - .0016 * (X - 60) - .0016 * (Y - 40))[..., None]        # Licht von oben links
    sk = np.zeros((H * SS, W * SS), np.uint8)
    r = np.random.default_rng(78)
    for _ in range(46):
        x0, y0 = r.integers(50 * SS, 272 * SS), r.integers(34 * SS, 226 * SS)
        l, w = r.uniform(6 * SS, 34 * SS), r.uniform(-.3, .3) + (np.pi / 2 if r.random() < .5 else 0)
        cv2.line(sk, (int(x0), int(y0)), (int(x0 + l * np.cos(w)), int(y0 + l * np.sin(w))), int(r.integers(50, 170)), 1, cv2.LINE_AA)
    sk = cv2.GaussianBlur(sk.astype(np.float32) / 255, (0, 0), 1.1)
    tafel += (sk * 16)[..., None]
    dreck(tafel, np.ones((H * SS, W * SS), np.float32))
    # Tafel auf die Wand legen: Schatten, Grat
    sd = sd_box(*PL, 1.)
    schatten(img, sd, .75, 3., 1.5, 2.)
    a = aa(sd, .6)
    img[:] = img * (1 - a[..., None]) + tafel * a[..., None]
    licht = aa(sd_box(PL[0], PL[1], PL[2] - 1, PL[3] - 1, 1.) + .4, .7) * (1 - aa(sd_box(PL[0] + 1, PL[1] + 1, PL[2], PL[3], 1.) + .4, .7))
    blend(img, col(190, 192, 186), licht * .34)
    dunkel = aa(sd_box(PL[0] + 1, PL[1] + 1, PL[2], PL[3], 1.) + .4, .7) * (1 - aa(sd_box(PL[0], PL[1], PL[2] - 1, PL[3] - 1, 1.) + .4, .7))
    blend(img, col(0, 0, 0), dunkel * .55)
    # Schiene oben (Lage aus dem Echtbild): Vierkantleiste
    rx0, ry0, rx1, ry1 = 45.5, 22.5, 277, 33
    schatten(img, sd_box(rx0, ry0, rx1, ry1, 1.), .7, 1.6, .6, 1.8)
    u = np.clip((Y - ry0) / (ry1 - ry0), 0, 1)
    p = np.interp(u, [0, .12, .3, .62, .85, 1], [150, 118, 92, 64, 40, 30]).astype(np.float32)
    leiste = p[..., None] * col(1., 1.02, 1.03) * tief.mean(2, keepdims=True).clip(30, 90) / 62 + (rausch(.4, 66) * 2.6)[..., None]
    dreck(leiste, np.ones((H * SS, W * SS), np.float32) * .6)
    a = aa(sd_box(rx0, ry0, rx1, ry1, 1.), .6)
    img[:] = img * (1 - a[..., None]) + leiste * a[..., None]
    for (sx, sy) in ((57.5, 39.5), (265.5, 39.5)):
        schraube(img, sx, sy)
    unten(img)
    return img


def unten(img):
    """Untere Kante des Bildes: Oberkanten des weissen Schilds und des blauen Kastens (Lage aus dem Echtbild)."""
    # weisses Schild im Metallrahmen
    x0, y0, x1, y1 = 72, 214, 131, 240
    schatten(img, sd_box(x0, y0, x1, y1, .8), .55, 1., .8, 1.1)
    a = aa(sd_box(x0, y0, x1, y1, .8), .5)
    f = col(142, 144, 138)[None, None, :] + (rausch(.8, 81) * 4)[..., None]
    img[:] = img * (1 - a[..., None]) + f * a[..., None]
    blend(img, col(16, 16, 16), aa(sd_box(x0 + 3, y0 + 2.5, x1 - 3, y0 + 8, .3), .4))
    blend(img, col(204, 202, 188), aa(sd_box(x0 + 3, y0 + 9.5, x1 - 3, y0 + 20, .3), .4))
    # blauer Kasten (Klappe)
    x0, y0, x1, y1 = 173, 211, 272.5, 240
    schatten(img, sd_box(x0, y0, x1, y1, 1.), .7, 1.4, 1., 1.4)
    u = np.clip((Y - y0) / 8, 0, 1)
    f = (col(22, 42, 112)[None, None, :] * (1.25 - .35 * u)[..., None]) + (rausch(1.5, 82) * 3.5 + rausch(.4, 83) * 2)[..., None] * col(.5, .7, 1.2)
    a = aa(sd_box(x0, y0, x1, y1, 1.), .5)
    img[:] = img * (1 - a[..., None]) + f * a[..., None]
    blend(img, col(120, 140, 200), aa(sd_box(x0, y0, x1, y0 + .9), .5) * .5)


def alle_koerper(img):
    for (x0, x1) in T:
        koerper(img, x0, x1, TK0, TK1)
        loch(img, (x0 + x1) / 2, LOCH_Y)
    return img


def schild_oben(img):
    """Echtes Schild oben rechts (rotes Feld, helles Feld) neu aufgebaut; die Schrift stempelt beschriften() (Spur PF)."""
    x0, y0, x1, y1 = 212, 46.5, 270, 87
    schatten(img, sd_box(x0, y0, x1, y1, .8), .5, 1., .8, 1.1)
    a = aa(sd_box(x0, y0, x1, y1, .8), .5)
    f = col(142, 144, 138)[None, None, :] * (1.02 - .004 * (Y - y0))[..., None] + (rausch(.8, 71) * 4)[..., None]
    img[:] = img * (1 - a[..., None]) + f * a[..., None]
    blend(img, col(30, 28, 28), aa(sd_box(x0, y0, x1, y0 + 2.6, .4), .4) * .8)
    a = aa(sd_box(x0 + 2, y0 + 5, x1 - 2, y0 + 18.5, .5), .5)
    f = col(136, 26, 24)[None, None, :] + (rausch(.9, 72) * 5 + rausch(.3, 73) * 2)[..., None]
    img[:] = img * (1 - a[..., None]) + f * a[..., None]
    a = aa(sd_box(x0 + 2, y0 + 21.5, x1 - 2, y0 + 37.5, .5), .5)
    f = col(208, 206, 190)[None, None, :] + (rausch(.9, 74) * 5 + rausch(.3, 75) * 2)[..., None]
    img[:] = img * (1 - a[..., None]) + f * a[..., None]


ROT = (122, 182.5, 230, 205)   # Spur PF: breiter (frueher 129..184,5), "HIGH PRESSURE" = 98 Bildpunkte in der Spielschrift


def schild_rot(img):
    """Rotes Schild unter der Scheibenreihe; die Schrift stempelt beschriften() (Spur PF)."""
    x0, y0, x1, y1 = ROT
    schatten(img, sd_box(x0, y0, x1, y1, .8), .55, 1., .8, 1.1)
    a = aa(sd_box(x0, y0, x1, y1, .8), .5)
    f = col(146, 34, 30)[None, None, :] * (1.04 - .010 * (Y - y0))[..., None] + (rausch(1.2, 76) * 6 + rausch(.3, 77) * 2.5)[..., None]
    img[:] = img * (1 - a[..., None]) + f * a[..., None]
    blend(img, col(236, 226, 214), aa(np.abs(sd_box(x0 + 1.6, y0 + 1.6, x1 - 1.6, y1 - 1.6, .5)) - .18, .4) * .55)


def linie(img, stuecke, st=.8):
    """Aufgedruckte Leitung (Blindschaltbild): mattes Elfenbein mit Abrieb."""
    m = np.zeros((H * SS, W * SS), np.float32)
    for sd in stuecke:
        m = np.maximum(m, aa(sd, .5))
    abrieb = np.clip(.86 + rausch(.8, 41) * .2, 0, 1)
    blend(img, col(204, 200, 174), m * abrieb * st)



KAP = {'A': 100, 'B': 50, 'C': 30}
TANK = ((0, 'A'), (1, 'A'), (2, 'B'), (3, 'C'), (4, 'C'))


def instrument_ohne_zeiger(img, cx, cy, R):
    """Hauptanzeige wie kit.instrument, der Zeiger kommt zur Laufzeit dazu (engine/src/panel2030_r41.c)."""
    scheibe(img, cx, cy, R)
    tinte = col(30, 28, 26)
    for v in range(0, 101, 10):
        ph = skala_winkel(v)
        lang = v % 20 == 0
        strich(img, cx, cy, ph, R * (.70 if lang else .78), R * .92, .36 if lang else .24, tinte, .9)
    for v in np.arange(80, 100.01, 1.):
        ph = skala_winkel(v)
        strich(img, cx, cy, ph, R * .76, R * .92, .42, col(178, 28, 22), .95)
    # Spur PF: keine Ziffern im Zifferblatt (4,6 Bildpunkte waren nicht lesbar, 6x8-Ziffern passen nicht in die 18 Bildpunkte innen);
    # das Ziel 80 steht scharf am Schild neben Tank A und oben rechts, auf der Skala bleibt das rote Band 80..100.
    glas(img, cx, cy, R)


def bild(voll):
    """Entwurf A (Freigabe Nachtrag 37) ohne die Laufzeit-Teile: voll = 1 alle Tanks voll, voll = 0 alle leer."""
    img = grund()
    linie(img, [sd_box(89, 86, 108, 88)])          # Leitung Tank A -> Hauptanzeige
    alle_koerper(img)
    for i, k in TANK:
        x0, x1 = T[i]
        gx0, gx1 = x0 + 3.3, x1 - 3.3
        ytop = yw(KAP[k])                          # gleiche Skala je Tank: kleiner Tank = kurzes Fenster
        roehre(img, gx0, gx1, ytop, GB, 100 if voll else 0)
        for v in range(0, KAP[k] + 1, 10):
            y = yw(v)
            blend(img, col(24, 22, 20), aa(sd_box(gx0, y - .22, gx0 + (3 if v % 50 == 0 else 1.8), y + .22), .35) * .7)
    for i in (0, 1):                               # Zielmarke 80 am Tank A
        x0, x1 = T[i]
        blend(img, col(182, 30, 24), aa(sd_box(x0 + 2.2, yw(80) - .65, x1 - 2.2, yw(80) + .65, .3), .4))
    schildchen(img, *SCHILD_80, '')
    for (cx, cy) in [D0] + DR:                     # Nachbesserung 1: in der Kette nur der Schlagschatten, die Scheiben stempelt scheiben_stempeln()
        schatten(img, sd_kreis(cx, cy, R + RING), .6, 1.5, 1.1, 1.5)
    for (x0, x1), lx in zip(TANKSCHILD, (84.5, 161, 205)):
        schildchen(img, x0, TANKSCHILD_Y[0], x1, TANKSCHILD_Y[1], '')
        lampe_unten(img, lx, 135.8, 7, 3, an=False)
    schild_oben(img)
    schild_rot(img)
    return beschriften(scheiben_stempeln(fertig(img, None, None)))


# ---- Scheiben auf ganze Texel (Spur PF, Nachbesserung 1) ----------------------------------------------------------------
RING = 3.9                                              # kit.scheibe ring
GAMMA_FERTIG = 1.16                                     # fertig(): arr = 255 * (arr / 255) ** 1.16
TINTE = (30, 28, 26)
STEMPEL = []                                            # (x, y, r, g, b) aller gestempelten Strich-/Balken-Texel nach dem Gamma, fuer die Riegel


def linie_px(x0, y0, x1, y1, dick=False):
    """Texel einer Strecke: je Schritt entlang der Hauptachse genau EIN Texel, dessen Mitte der Geraden am naechsten liegt (bei Gleichstand der untere bzw.
    rechte); dick: dazu der Nachbar in der Nebenachse, der naeher an der Geraden liegt (zwei Texel je Schritt, gleichmaessige Treppe). Dieselbe Regel hat
    linie_px() in engine/src/panel2030_r41.c fuer den Zeiger."""
    dx, dy = x1 - x0, y1 - y0
    pts = []
    if abs(dx) >= abs(dy):
        a, b = int(np.ceil(min(x0, x1) - .5)), int(np.floor(max(x0, x1) - .5))
        if a > b:
            a = b = int(np.floor((x0 + x1) / 2))
        for i in range(a, b + 1):
            y = y0 + (i + .5 - x0) * dy / dx if dx else y0
            j = int(np.floor(y))
            pts.append((i, j))
            if dick:
                pts.append((i, j + 1 if y - j >= .5 else j - 1))
    else:
        a, b = int(np.ceil(min(y0, y1) - .5)), int(np.floor(max(y0, y1) - .5))
        if a > b:
            a = b = int(np.floor((y0 + y1) / 2))
        for j in range(a, b + 1):
            x = x0 + (j + .5 - y0) * dx / dy
            i = int(np.floor(x))
            pts.append((i, j))
            if dick:
                pts.append((i + 1 if x - i >= .5 else i - 1, j))
    return pts


def scheibe_px(a, cx, cy, R, seed):
    """Lippe, Ring, innere Lippe und Blatt wie kit.scheibe, je Texel nach dem Abstand der Texelmitte (ohne Teilbedeckung); a = Bild vor dem Gamma (float).
    Liefert die Maske des Blatts (fuer Glas und Striche)."""
    y0, y1, x0, x1 = int(cy - R - RING - 2), int(cy + R + RING + 3), int(cx - R - RING - 2), int(cx + R + RING + 3)
    yy, xx = np.mgrid[y0:y1, x0:x1]
    X, Y = xx + .5, yy + .5
    d = np.hypot(X - cx, Y - cy)
    phi = np.arctan2(Y - cy, X - cx)
    r = np.random.default_rng(seed)
    licht = .5 + .5 * np.cos(phi + np.radians(125))
    ringf = (46 + 150 * licht ** 2.4)[..., None] * np.array([1., 1., .96]) + (r.standard_normal(d.shape) * 3)[..., None]
    u = d / R
    n = cv2.GaussianBlur(r.standard_normal(d.shape).astype(np.float32), (0, 0), 1.2)
    schmutz = (n / (n.std() + 1e-6) * 6 + r.standard_normal(d.shape) * 2)[..., None]
    blatt = np.array([219., 214., 190.]) * (1.02 - .22 * u ** 3 + .06 * ((cx - X) + (cy - Y)) / R)[..., None] + schmutz
    blk = a[y0:y1, x0:x1]
    blk[d <= R + RING + .5] = (6, 6, 6)
    m = d <= R + RING - .4
    blk[m] = ringf[m]
    blk[d <= R + .7] = (10, 10, 9)
    mb = d <= R
    blk[mb] = blatt[mb]
    # Glas wie kit.glas, nur auf dem Blatt und VOR den Strichen (die Striche bleiben in voller Deckung)
    d1 = np.hypot(X - (cx - R * .28), Y - (cy - R * .34)) - R * .82
    d2 = np.hypot(X - (cx - R * .02), Y - (cy - R * .02)) - R * .90
    ga = (np.clip(.5 - d1 / 1., 0, 1) * (1 - np.clip(.5 - d2 / 2.2, 0, 1)) * .34)[..., None]
    blk[mb] = (blk * (1 - ga) + np.array([255., 255., 248.]) * ga)[mb]
    maske = np.zeros(a.shape[:2], bool)
    maske[y0:y1, x0:x1] = d <= R + RING + .5
    blatt_m = np.zeros(a.shape[:2], bool)
    blatt_m[y0:y1, x0:x1] = mb
    return maske, blatt_m


def setze(a, pts, farbe, deckung=1.):
    for (i, j) in pts:
        a[j, i] = a[j, i] * (1 - deckung) + np.array(farbe, float) * deckung


def scheiben_stempeln(im):
    """Instrument (ohne Zeiger) und vier Drehscheiben Texel fuer Texel in das fertige Bild (nach fertig(), vor beschriften())."""
    out = np.array(im).astype(np.float64)
    lin = 255 * (out / 255) ** (1 / GAMMA_FERTIG)           # zurueck vor das Gamma, damit die Farben wie in der Kette gerechnet werden
    neu = np.zeros(out.shape[:2], bool)
    striche = []
    # Instrument: Teilstriche 0..100 je 10 (lang je 20), rotes Band 80..100 (kit.instrument)
    cx, cy = D0
    m, blatt = scheibe_px(lin, cx, cy, R, 2031)
    neu |= m
    for v in range(0, 101, 10):
        ph = skala_winkel(v)
        lang = v % 20 == 0
        r0, r1 = R * (.70 if lang else .78), R * .92
        pts = linie_px(cx + r0 * np.sin(ph), cy - r0 * np.cos(ph), cx + r1 * np.sin(ph), cy - r1 * np.cos(ph))
        setze(lin, pts, TINTE, .9)
        striche += pts
    yy, xx = np.mgrid[0:H, 0:W]
    dd = np.hypot(xx + .5 - cx, yy + .5 - cy)
    wk = np.arctan2(xx + .5 - cx, -(yy + .5 - cy))         # Winkel wie skala_winkel: 0 oben, im Uhrzeigersinn
    band = (dd >= R * .76) & (dd <= R * .92) & (wk >= skala_winkel(80)) & (wk <= skala_winkel(100))
    pts = [(int(i), int(j)) for j, i in zip(*np.nonzero(band))]
    setze(lin, pts, (178, 28, 22), .95)
    striche += pts
    # Drehscheiben: 12 Teilstriche (Deckung .6 wie kit.drehscheibe), Querbalken 2 Texel je Spalte, Steigung 1:2, Schatten (+1,+1) Deckung .32
    for k, (cx, cy) in enumerate(DR):
        m, blatt = scheibe_px(lin, cx, cy, R, 2040 + k)
        neu |= m
        for t in range(12):
            ph = np.radians(30 * t)
            pts = linie_px(cx + R * .84 * np.sin(ph), cy - R * .84 * np.cos(ph), cx + R * .95 * np.sin(ph), cy - R * .95 * np.cos(ph))
            setze(lin, pts, TINTE, .6)
            striche += pts
        ex, ey = 2 / np.sqrt(5), -1 / np.sqrt(5)
        L = R * .93
        balken = linie_px(cx - L * ex, cy - L * ey, cx + L * ex, cy + L * ey, dick=True)
        setze(lin, [(i + 1, j + 1) for (i, j) in balken if (i + 1, j + 1) not in balken], (0, 0, 0), .32)
        setze(lin, balken, (14, 14, 13), 1.)
        striche += balken
    out[neu] = 255 * (np.clip(lin[neu], 0, 255) / 255) ** GAMMA_FERTIG
    o8 = out.clip(0, 255).astype(np.uint8)
    if not STEMPEL:
        q = lambda c: (int(c) >> 3) << 3                    # 15 Bit wie rgb555()
        STEMPEL.extend(sorted({(i, j, q(o8[j, i, 0]), q(o8[j, i, 1]), q(o8[j, i, 2])) for (i, j) in striche}))
    return Image.fromarray(o8)


# ---- Beschriftung (Spur PF) --------------------------------------------------------------------------------------------
TEX_TIM = os.path.join(kit.TREE, "re15_port", "shared_assets", "PSX", "DATA", "TEX.TIM")
TANKSCHILD = ((50.5, 82.5), (136, 160), (180, 204))     # Plaettchen unter den Tanks (frueher 55,5..82 / 135..158,5 / 177..202,5)
TANKSCHILD_Y = (132, 142)                               # frueher 133,2..141,4: 8 Zeilen Glyphe + je 1 Rand
SCHILD_80 = (91, 71, 106, 81)                           # Zielmarke an Tank A, Mitte yw(80) = 76


def rgb15(r, g, b):
    return (r >> 3) | ((g >> 3) << 5) | ((b >> 3) << 10)


def tex_schrift():
    """Die 8x8-Schrift: TEX.TIM 4 bpp, Seite ab Texel x 256, Zeilen 0..31 (der 16er-Spielfont beginnt bei v 32), CLUT-Zeile 0."""
    d = open(TEX_TIM, "rb").read()
    assert struct.unpack_from("<I", d, 4)[0] & 8, "TEX.TIM ohne CLUT"
    clen, cx, cy, cw, ch = struct.unpack_from("<IHHHH", d, 8)
    assert (cx, cy) == (256, 480), (cx, cy)                 # CLUT-Rechteck (256,480) = FUN_80029214 CLUT 0x1e0 * 0x40 | 0x10
    clut0 = list(struct.unpack_from("<16H", d, 20))         # Zeile 0
    o = 8 + clen
    ilen, ix, iy, iw, ih = struct.unpack_from("<IHHHH", d, o)
    roh = np.frombuffer(d, np.uint8, iw * 2 * ih, o + 12).reshape(ih, iw * 2)
    idx = np.zeros((ih, iw * 4), np.uint8)
    idx[:, 0::2] = roh & 15
    idx[:, 1::2] = roh >> 4
    return idx[0:32, 256:512], clut0


SCHRIFT, CLUT0 = None, None


def glyphe(c):
    g = ord(c) - 0x20                                       # FUN_80029214 @0x80029214: u = (g & 0x1f) * 8, v = ((g & 0xe0) >> 5) * 8
    u, v = (g & 0x1F) * 8, ((g & 0xE0) >> 5) * 8
    return SCHRIFT[v:v + 8, u:u + 8]


def clut_aus(tinte, schatten):
    """16er-CLUT wie CLUT-Zeile 0, die Schriftindizes 1..4 (Schattierung der Glyphe) in der Tinte, 8 = Schlagschatten (0 = durchsichtig)."""
    c = list(CLUT0)
    if tinte is not None:
        hell = [CLUT0[k] for k in (1, 2, 3, 4)]
        for k, h in zip((1, 2, 3, 4), hell):
            f = ((h & 31) + ((h >> 5) & 31) + ((h >> 10) & 31)) / (3.0 * 27)    # Anteil an Index 1 (0x677b = 27,27,25)
            c[k] = rgb15(*(min(255, int(t * f)) for t in tinte)) or 0x0421
    if schatten is not None:
        c[8] = rgb15(*schatten) if schatten else 0
    return c


# CLUTs der Schilder: 0 = helle Spielschrift (CLUT-Zeile 0 unveraendert), 1 = dunkle Schrift ohne Schatten auf hellem Feld, 2 = rote Ziffern
CLUTS = None
ZEICHEN_VORSCHUB = {"ziffer": 6, "leer": 3}             # Laufweite PORT-WAHL: Buchstaben 8 (FUN_80029214), Ziffern 6 (Tinte 5 breit), Leerzeichen 3


def lauf(text):
    """Lage der Glyphen relativ zum Anfang: Buchstaben 8, Ziffern 6, Leerzeichen 3."""
    aus, x = [], 0
    for c in text:
        if c == " ":
            x += ZEICHEN_VORSCHUB["leer"]
            continue
        aus.append((c, x))
        x += ZEICHEN_VORSCHUB["ziffer"] if c.isdigit() else 8
    return aus


def ausdehnung(text):
    """Linke und rechte Spalte der gesetzten Texel (Tinte und Schatten) relativ zum Anfang."""
    spalten = [dx + int(s) for c, dx in lauf(text) for s in np.where((glyphe(c) != 0).any(0))[0]]
    return min(spalten), max(spalten)


def mittig(text, x0, x1):
    """Anfang so, dass die gesetzten Texel mittig zwischen x0 und x1 stehen (ganzzahlig)."""
    links, rechts = ausdehnung(text)
    return int(np.floor((x0 + x1) / 2.0 - (rechts - links + 1) / 2.0)) - links


ETIKETTEN = None


def etiketten():
    """(Text, x, y, CLUT) - y = obere Glyphenzeile."""
    t0, t1 = TANKSCHILD_Y
    sx0, sy0, sx1, sy1 = 212, 46.5, 270, 87                 # schild_oben
    rx0, ry0, rx1, ry1 = ROT
    e = []
    for (x0, x1), txt in zip(TANKSCHILD, ("A 100", "B 50", "C 30")):
        e.append((txt, mittig(txt, x0, x1), t0 + 1, 0))
    e.append(("80", mittig("80", SCHILD_80[0], SCHILD_80[2]), SCHILD_80[1] + 1, 2))
    e.append(("BRIDGE", mittig("BRIDGE", sx0, sx1), int(sy0 + 7.5), 0))          # rotes Feld y0+5 .. y0+18,5
    e.append(("SET 80", mittig("SET 80", sx0, sx1), int(sy0 + 25.5), 1))         # helles Feld y0+21,5 .. y0+37,5
    e.append(("CAUTION", mittig("CAUTION", rx0, rx1), int(ry0 + 2.5), 0))
    e.append(("HIGH PRESSURE", mittig("HIGH PRESSURE", rx0, rx1), int(ry0 + 12.5), 0))
    return e


def beschriften(im):
    """Stempelt die Beschriftung Texel fuer Texel (Index 0 und CLUT-Wert 0 = durchsichtig, PSX-Regel) in das fertige 320x240-Bild."""
    global SCHRIFT, CLUT0, CLUTS, ETIKETTEN
    if SCHRIFT is None:
        SCHRIFT, CLUT0 = tex_schrift()
        CLUTS = [clut_aus(None, None), clut_aus((34, 30, 28), 0), clut_aus((236, 76, 56), None)]
        ETIKETTEN = etiketten()
    a = np.array(im).copy()
    for txt, x, y, k in ETIKETTEN:
        for c, dx in lauf(txt):
            g = glyphe(c)
            for gy in range(8):
                for gx in range(8):
                    v = CLUTS[k][g[gy, gx]] if g[gy, gx] else 0
                    if v:
                        a[y + gy, x + dx + gx] = ((v & 31) << 3, ((v >> 5) & 31) << 3, ((v >> 10) & 31) << 3)
    return Image.fromarray(a)


def rgb555(bild):
    a = np.asarray(bild).astype(np.uint16)
    return (a[..., 0] >> 3) | ((a[..., 1] >> 3) << 5) | ((a[..., 2] >> 3) << 10)


def inc_array(name, b, zeile=20):
    z = ["static const unsigned short %s[%d] = {" % (name, len(b))]
    for i in range(0, len(b), zeile):
        z.append(",".join(str(int(x)) for x in b[i:i + zeile]) + ",")
    z.append("};")
    return "\n".join(z)


def main():
    leer = bild(0).convert("RGB")
    voll = bild(1).convert("RGB")
    # Abnahme 0, Mangel 5: 256 gemeinsame Farben (Median-Schnitt) zogen die kleinen gruenen Flaechen ins Grau (EXIT-Pfeil 82,107,88 -> 91,98,96). Direkte 15 Bit wie die BSS-Hintergruende.
    p_leer, p_voll = rgb555(leer), rgb555(voll)
    z = ["/* AUTOMATISCH ERZEUGT von re15_port/tools/r41_nachtrag30/panel_bild.py - nicht von Hand aendern.",
         " * Panel-Raetsel ROOM2030 (Runde 41 Spur N, Nachtrag 37, Entwurf A). Herleitung: Dossier N_nachtrag30.md Abschnitt 2. */",
         "#define RE15_R41N_PANEL_W %d" % W, "#define RE15_R41N_PANEL_H %d" % H,
         inc_array("re15_r41n_panel_grund", p_leer.ravel())]
    fenster = []
    teile = []
    for i, k in TANK:
        x0, x1 = T[i]
        fx0, fx1 = int(x0 + 3.3) - 1, int(x1 - 3.3) + 2
        fy0, fy1 = int(yw(KAP[k])) - 1, int(GB) + 2
        fenster.append((fx0, fy0, fx1 - fx0, fy1 - fy0, "ABC".index(k), len(teile)))
        teile += [int(v) for v in p_voll[fy0:fy1, fx0:fx1].ravel()]
    z.append(inc_array("re15_r41n_panel_voll", teile))
    z.append("typedef struct { short x, y, w, h, tank; int off; } re15_r41n_fenster_t;")
    z.append("static const re15_r41n_fenster_t re15_r41n_panel_fenster[%d] = {" % len(fenster))
    for f in fenster:
        z.append("    { %d, %d, %d, %d, %d, %d }," % f)
    z.append("};")
    # Beschriftung (Spur PF): Glyphen + CLUTs, damit unit_r41_nachtrag30 und das Werkzeug "etikett" sie gegen TEX.TIM nachpruefen
    z.append("typedef struct { unsigned char code; short x, y; unsigned char clut; } re15_r41n_glyphe_t;")
    gl = [(ord(c), x + dx, y, k) for txt, x, y, k in ETIKETTEN for c, dx in lauf(txt)]
    z.append("static const re15_r41n_glyphe_t re15_r41n_panel_glyphen[%d] = {" % len(gl))
    for g in gl:
        z.append("    { 0x%02x, %d, %d, %d }," % g)
    z.append("};")
    z.append("static const unsigned short re15_r41n_panel_clut[%d][16] = {" % len(CLUTS))
    for c in CLUTS:
        z.append("    { " + ", ".join("0x%04x" % v for v in c) + " },")
    z.append("};")
    # Nachbesserung 1: Strich-/Balken-Texel der Scheiben (nach dem Gamma, 15 Bit), damit unit_r41_nachtrag30 sie Texel fuer Texel im Panel-Bild nachprueft
    z.append("typedef struct { short x, y; unsigned short farbe; } re15_r41n_stempel_t;")
    z.append("static const re15_r41n_stempel_t re15_r41n_panel_stempel[%d] = {" % len(STEMPEL))
    for (x, y, r_, g_, b_) in STEMPEL:
        z.append("    { %d, %d, 0x%04x }," % (x, y, rgb15(r_, g_, b_)))
    z.append("};")
    c = cursor_stempel()
    z.append("typedef struct { signed char dx, dy; unsigned char r, g, b; } re15_r41n_cursorpx_t;")
    z.append("static const re15_r41n_cursorpx_t re15_r41n_panel_cursor[%d] = {" % len(c))
    for dx, dy, r_, g_, b_ in c:
        z.append("    { %d, %d, %d, %d, %d }," % (dx, dy, r_, g_, b_))
    z.append("};")
    ziel = os.path.join(GEN, "panel2030_r41.inc")
    open(ziel, "w", newline="\n").write("\n".join(z) + "\n")
    print("geschrieben:", ziel, "Grund md5", hashlib.md5(p_leer.tobytes()).hexdigest(), "Cursor-Pixel", len(c), "Fenster", len(fenster),
          "Etiketten", ", ".join("%s @%d,%d" % (t, x, y) for t, x, y, k in ETIKETTEN), "Stempel-Texel", len(STEMPEL))
    if "--beleg" in sys.argv:
        Image.fromarray(np.asarray(leer)).resize((W * 3, H * 3), Image.NEAREST).save(sys.argv[sys.argv.index("--beleg") + 1])


if __name__ == "__main__":
    main()
