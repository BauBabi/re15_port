#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""panel_bild.py - Runde 41 Spur N (AUFTRAG.md Nachtrag 37, Nutzer-Freigabe "A Druckspeicher"): Bild des Panel-Raetsels ROOM2030.

Dossier analysis/befunde_runde41/N_nachtrag30.md §2 (Kamera, Lage, Regeln). Erzeugt engine/src/gen/panel2030_r41.inc:
  re15_r41n_panel_grund  8 bpp 320 x 240: Panel mit LEEREN Tanks, ohne Zeiger, Lampen aus, ohne Cursor
  re15_r41n_panel_voll   8 bpp je Fenster: dieselbe Stelle mit VOLLEM Tank (engine/src/panel2030_r41.c setzt beide je Fuellstand zusammen)
  re15_r41n_panel_pal    gemeinsame Palette 256 x RGB
  re15_r41n_panel_cursor Cursor-Stempel aus dem Spielbild 2050 (C_belege/pult2050_loesung_sheet.png, Kachel F540)
Rahmen pixelgleich aus ROOM2050 Cut 15, EXIT-Feld pixelgleich aus ROOM11F0 Cut 10 (x 210..238, y 175..204).

Aufruf: R41N_BG=<dumps stage2>;<dumps stage1> C:/Python310/python.exe re15_port/tools/r41_nachtrag30/panel_bild.py [--beleg <png>]
"""
import hashlib
import os
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
D0 = (124.0, 87.0)                                                        # Hauptanzeige
DR = [(125, 160), (163, 160), (200.5, 160), (237.5, 160)]                 # Scheibenreihe
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
    """Echtes Schild oben rechts (rotes Feld, helles Feld) neu aufgebaut, Ziel in Klartext."""
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
    text(img, 'BRIDGE', (x0 + x1) / 2, y0 + 11.9, 8.2, col(232, 228, 214), .94, F_BD, blur=.1)
    text(img, 'PRESSURE 80', (x0 + x1) / 2, y0 + 29.6, 6.7, col(34, 30, 28), .94, F_BD, blur=.1)


def schild_rot(img, z1, z2, g1=6.0, g2=5.6):
    """Rotes Schild unter der Scheibenreihe."""
    x0, y0, x1, y1 = 129, 182.5, 184.5, 205
    schatten(img, sd_box(x0, y0, x1, y1, .8), .55, 1., .8, 1.1)
    a = aa(sd_box(x0, y0, x1, y1, .8), .5)
    f = col(146, 34, 30)[None, None, :] * (1.04 - .010 * (Y - y0))[..., None] + (rausch(1.2, 76) * 6 + rausch(.3, 77) * 2.5)[..., None]
    img[:] = img * (1 - a[..., None]) + f * a[..., None]
    blend(img, col(236, 226, 214), aa(np.abs(sd_box(x0 + 1.6, y0 + 1.6, x1 - 1.6, y1 - 1.6, .5)) - .18, .4) * .55)
    text(img, z1, (x0 + x1) / 2, y0 + 7.8, g1, col(238, 232, 218), .95, F_BD, blur=.1)
    text(img, z2, (x0 + x1) / 2, y0 + 15.2, g2, col(238, 232, 218), .9, F_BD, blur=.1)


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
    rot = col(172, 22, 18)
    for v, s, c, gr in ((0, '0', tinte, 4.6), (50, '50', tinte, 4.6), (80, '80', rot, 6.0)):
        ph = skala_winkel(v)
        rr = R * (.45 if v == 80 else .50)
        text(img, s, cx + rr * np.sin(ph), cy - rr * np.cos(ph) + (0 if v != 50 else .4), gr, c, .96, F_BD, blur=.12)
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
    schildchen(img, 92, yw(80) - 4.2, 105.5, yw(80) + 4.2, '80', 6.6, tinte=col(226, 70, 54))
    instrument_ohne_zeiger(img, D0[0], D0[1], R)
    for (cx, cy) in DR:
        drehscheibe(img, cx, cy, R, 60)
    for x0, x1, s, lx in ((55.5, 82, 'A  100', 84.5), (135, 158.5, 'B  50', 161), (177, 202.5, 'C  30', 205)):
        schildchen(img, x0, 133.2, x1, 141.4, s, 6.0)
        lampe_unten(img, lx, 135.8, 7, 3, an=False)
    schild_oben(img)
    schild_rot(img, 'CAUTION', 'HIGH PRESSURE')
    return fertig(img, None, None)


def inc_array(name, b, zeile=32):
    z = ["static const unsigned char %s[%d] = {" % (name, len(b))]
    for i in range(0, len(b), zeile):
        z.append(",".join(str(x) for x in b[i:i + zeile]) + ",")
    z.append("};")
    return "\n".join(z)


def main():
    leer = bild(0).convert("RGB")
    voll = bild(1).convert("RGB")
    bogen = Image.new("RGB", (W, H * 2))
    bogen.paste(leer, (0, 0)); bogen.paste(voll, (0, H))
    q = bogen.quantize(colors=256, method=Image.MEDIANCUT, dither=Image.NONE)
    pal = bytes(q.getpalette()[:768]).ljust(768, b"\0")
    pix = np.asarray(q)
    p_leer, p_voll = pix[:H], pix[H:]
    z = ["/* AUTOMATISCH ERZEUGT von re15_port/tools/r41_nachtrag30/panel_bild.py - nicht von Hand aendern.",
         " * Panel-Raetsel ROOM2030 (Runde 41 Spur N, Nachtrag 37, Entwurf A). Herleitung: Dossier N_nachtrag30.md Abschnitt 2. */",
         "#define RE15_R41N_PANEL_W %d" % W, "#define RE15_R41N_PANEL_H %d" % H, inc_array("re15_r41n_panel_pal", pal),
         inc_array("re15_r41n_panel_grund", p_leer.tobytes())]
    fenster = []
    teile = bytearray()
    for i, k in TANK:
        x0, x1 = T[i]
        fx0, fx1 = int(x0 + 3.3) - 1, int(x1 - 3.3) + 2
        fy0, fy1 = int(yw(KAP[k])) - 1, int(GB) + 2
        fenster.append((fx0, fy0, fx1 - fx0, fy1 - fy0, "ABC".index(k), len(teile)))
        teile += p_voll[fy0:fy1, fx0:fx1].tobytes()
    z.append(inc_array("re15_r41n_panel_voll", bytes(teile)))
    z.append("typedef struct { short x, y, w, h, tank; int off; } re15_r41n_fenster_t;")
    z.append("static const re15_r41n_fenster_t re15_r41n_panel_fenster[%d] = {" % len(fenster))
    for f in fenster:
        z.append("    { %d, %d, %d, %d, %d, %d }," % f)
    z.append("};")
    c = cursor_stempel()
    z.append("typedef struct { signed char dx, dy; unsigned char r, g, b; } re15_r41n_cursorpx_t;")
    z.append("static const re15_r41n_cursorpx_t re15_r41n_panel_cursor[%d] = {" % len(c))
    for dx, dy, r_, g_, b_ in c:
        z.append("    { %d, %d, %d, %d, %d }," % (dx, dy, r_, g_, b_))
    z.append("};")
    ziel = os.path.join(GEN, "panel2030_r41.inc")
    open(ziel, "w", newline="\n").write("\n".join(z) + "\n")
    print("geschrieben:", ziel, "Grund md5", hashlib.md5(p_leer.tobytes()).hexdigest(), "Cursor-Pixel", len(c), "Fenster", len(fenster))
    if "--beleg" in sys.argv:
        bogen.resize((W * 2, H * 4), Image.NEAREST).save(sys.argv[sys.argv.index("--beleg") + 1])


if __name__ == "__main__":
    main()
