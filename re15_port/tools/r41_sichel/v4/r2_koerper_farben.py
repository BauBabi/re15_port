"""SF V4 Runde 2 (Bauer): Koerperfarben EXAKT - Texel und CLUT der Koerper-Meshes 0-8, 10-15 aus den Originalbildern.

    python r2_koerper_farben.py [--emd EM30_V4.EMD]   -> tools/r41_sichel/v4/farben_koerper_v4.npz (+ r2_koerper_protokoll.json)

Anlass (Sicht-Pruefung V4 r1 1.4, Zahlen-Pruefung V4 r1 F8): Runde 1 skalierte die Hautzeilen 480-482 der CLUT mit Faktoren; die
Koerpermeshes teilen dieselben Texel (r2_uv_belegung: z. B. Oberarme 10/13 teilen 5798 Texel, Rumpf 8 / Oberarm 13 5074), deshalb wurde
der ganze Rumpf rot (Anteil R-G >= 15: 0,83 gegen 0,31 im Original), die Oberarme doppelt so hell, die Unterarme rot. Die Texel sind
nach Riegel G1 frei (nur UVs/Primitive sind fest), also werden sie neu gesetzt.

Verfahren (Messblatt V4 Abschnitt 5 "Schirmfarbe = Texel x Licht", hier je Texel statt je Region):
  1. Render der aktuellen Form in beiden Vorlage-Kameras (b2-Dreiecke, eigener Rasterer = b2.rastern_ids) mit Texelkoordinate,
     CLUT-Zeile und Licht lambda = Gouraud-Farbe / 255 je Bildpunkt.
  2. Ziel je Bildpunkt: je Koerperregion (Ids) wird die Lab-Verteilung des Port-Renders (Vergleichsregel: sigma 1,5, V = a + b x P)
     kanalweise auf die Verteilung des Original-Fensters gebracht (Rangabbildung, Histogramm-Angleich) - Raumstruktur vom Port,
     Farbverteilung vom Original. Original-Fenster (2 px erodiert; Vorlage 1 entblendet, Vorlage 2 roh) siehe REGIONEN (Quellen:
     Messblatt V4 F6-F10, F14, F19 und die Boxen der Sicht-Pruefung V4 r1, pruefung_sicht/messen_sicht.py BOX1/BOX2).
     Ziel im Port-Schirm unweichgezeichnet: D = R + (T - V) / b.
  3. Je (Texel, CLUT-Zeile): kleinste Quadrate ueber alle Bildpunkte beider Ansichten, t = (Sum lambda D + mu t0) / (Sum lambda^2 + mu),
     t0 = Referenz-2-Texel x Medianverstaerkung des Meshes (Vorlauf ohne mu); ungesehene Texel = t0.
  4. Quantisierung: alle Koerpertexel gemeinsam auf 256 Indizes (k-Mittel ueber die Farben aller Zeilen, in denen der Texel benutzt
     wird), Eintraege der Zeilen 480/481/482/484 = Klassenmittel. 8 bpp, Texel 1:1, kein Weichzeichner (M3).
"""
import os, sys, json, argparse
import numpy as np, cv2
from skimage.color import lab2rgb
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v4_gemein as G
import b2
import birkin_buehne as BB
import r2_uv_belegung as UB

AUS = os.path.join(HIER, 'farben_koerper_v4.npz')
PROT = os.path.join(HIER, 'r2_koerper_protokoll.json')
ZEILEN = (480, 481, 482, 484, 488, 489, 490, 491)
# V4 Runde 3 (Entscheid O1): eigene Zeilen 488 (Mesh 14, Unterarm (W)-rechts) und 489 (Mesh 10, Oberarm (W)-links), birkin_formen.py
# KOERPER_ZEILEN_V4; Startwert (Referenz 2) = Zeile 482 bzw. 480
START = {488: 482, 489: 480, 490: 480, 491: 480}   # V4 Runde 4: Zeile 490 Mesh 13, 491 Beine (birkin_formen.KOERPER_ZEILEN_V4)
KOERPER = [b for b in range(16) if b != 9]
LAM_MIN = 0.35       # Bildpunkte mit Licht < 0,35 (streifend) zaehlen nicht - sie trieben einzelne Texel auf Weiss; PORT-WAHL
LUM_MAX = 190.0      # hellster erlaubter Koerpertexel (Lum); PORT-WAHL gegen Einzel-Ausreisser (200: zwei helle Punkte auf der Schulter
                     # noch sichtbar, 215 deutlich; F14 dL 190 / 200 / 215: -5,2 / -4,6 / -4,4, Laeufe r2 Koerper 11-13)
MU = float(os.environ.get("RE15_R4_MU", "0.25"))            # Vorwissen: wie ein Viertel Bildpunkt mit lambda 1 (haelt kaum gesehene Texel bei Referenz 2 x Verstaerkung); PORT-WAHL

# Regionen: (Name, Ansicht, Port-Ids, Original-Boxen (x0, y0, x1, y1), Quelle)
REGIONEN = [
    # Rumpf in zwei Teilen wie Messblatt F6 (Id 8 mit R-G >= 15 gegen das Fenster 372-424 x 197-232 mit R-G >= 15): die Port-Bildpunkte mit
    # dem hoechsten a* in der Menge des Original-Rotanteils (Sicht-Pruefung: 0,31) bekommen die F6-Verteilung, der Rest die nicht roten
    # Bildpunkte von Rumpf + Schultern (Lauf r2 Koerper 3 mit einer Region: F6 21,3/12,3/4,4 statt 16,6/17,3/6,2)
    ('rumpf_rot', '135', (7, 8), [(372, 197, 424, 232)], 'Messblatt F6 (372-424 x 197-232, R-G >= 15)', None, ('rot', 0.31, 15)),
    ('rumpf', '135', (7, 8), [(352, 196, 448, 292), (318, 160, 360, 190), (425, 165, 470, 195)],
     'Sicht-Pruefung V4 r1 1.4: Rumpf (352-448 x 196-292) + Schultern (B)-links (318-360 x 160-190) / (B)-rechts (425-470 x 165-195), '
     'ohne die roten Bildpunkte (R-G >= 15)', None, ('nicht_rot', 0.31, 15)),
    # Oberarme getrennt nach Hoehe (Lage der Fenster im Original-Oberarm): (B)-links Arm y 160-250 (Schulterbox Oberkante 160, Sicht-Box
    # Unterkante 250), F7-Fenster y 167-212 = obere 58 %; (B)-rechts Arm y 182-262, F8-Fenster y 182-227 = obere 56 %. Im Port dieselben
    # Anteile der Zeilen von Id 13 / Id 10 (Sicht-Pruefung V4 r1: "dunkel, hell nur die Schulterkuppe"; Messblatt F7 33,3 / F8 16,4)
    ('oberarm_bl_oben', '135', (13,), [(307, 167, 347, 212)], 'Messblatt F7 (307-347 x 167-212)', (0.0, 0.58)),
    ('oberarm_bl_unten', '135', (13,), [(300, 213, 345, 250)], 'Sicht OA_Bl (300-345 x 186-250) unterhalb des F7-Fensters', (0.58, 1.0)),
    # V4 Runde 3: Mesh 10 hat eine eigene CLUT-Zeile 489 (O1) und teilt keine Texel mehr mit dem hellen (B)-linken Arm -> EINE Region
    # fuer den ganzen Oberarm (B)-rechts (Messblatt F8 misst Id 10 ganz) mit der Verteilung von F8-Fenster + Sicht-Box darunter
    # (Runde 2 oben/unten getrennt: Id ganz 20,5 L*, F8 16,4 -> dL +4,1, Riegel 'F F8 Messblatt-Wortlaut' rot)
    # Verteilung = Messblatt-Fenster F8 allein (Wortlaut F8: Id 10 ganz gegen dieses Fenster); mit F8 + Sicht-Box unten (Mittel 18,5 L*)
    # blieb Id 10 ganz bei 20,8 L* (dL +4,4, Lauf r3 Koerper 2)
    # V4 Runde 4 (Pruefung V4 r3 Zahlen 4.3 / Sicht 1.5: Oberarm (B)-rechts unten Rotanteil 0,49 statt 0,04, weil Id 10 ganz die rote
    # F8-Verteilung bekam): wieder oben/unten getrennt wie Runde 2 - unten die Sicht-Box 455-500 x 228-262 (20,6 / 3,1 / -0,5, Rotanteil
    # 0,04); oben (im Oberkoerper-Polygon) setzen die Kacheln 'oberkoerper' die Verteilung des Originals an derselben Bildstelle (F8-Fenster)
    ('oberarm_br_unten', '135', (10,), [(455, 228, 500, 262)], 'Sicht OA_Br (455-500 x 228-262) unterhalb des F8-Fensters', (0.56, 1.0)),
    # V4 Runde 4 (Pruefung V4 r3 Sicht 1.5 Klasse A "Rot-Verteilung des Oberkoerpers vorn": im Video liegt das Rot in der Brustmitte,
    # Schultern rosa-grau - Rotanteil Mitte / aussen 2,75-2,91, Schultern a* > 12 0,00-0,05; V4 r3 1,06-1,18 und 0,10-0,13, weil die
    # Rangabbildung je Region die Raumverteilung vom Port nahm): Kacheln. Flaeche = Oberkoerper-Polygon der Sicht-Pruefung V4 r3
    # (pruefung_sicht_r3/oberkoerper_vorn.py POLY, Vorlage 1) vereinigt mit der Rumpfbox 352-448 x 196-292 (Sicht-Pruefung V4 r1 1.4);
    # je Kachel (KACHELN Spalten x Zeilen ueber die Bbox der Flaeche) bekommen die Port-Bildpunkte der Ids 7/8/10/13 die Verteilung der
    # Original-Bildpunkte DERSELBEN Kachel (Rangabbildung wie bisher) - die Raumverteilung kommt so aus dem Original.
    ('oberkoerper', '135', (7, 8, 10, 13), [], 'Kacheln im Oberkoerper-Polygon der Sicht-Pruefung V4 r3 + Rumpfbox 352-448 x 196-292 '
     '(Original-Verteilung je Kachel)', None, ('kacheln',)),
    ('unterarm', '135', (11,), [(468, 268, 500, 330)],
     'Sicht UA_Br (468-508 x 268-330) ohne den Wandrand x > 500 - Unterarm (B)-rechts = (W)-links (Id 11)'),
    # V4 Runde 3 (Pruefung V4 r2 Sicht 1.4, Klasse A; Entscheid O5): der (B)-linke = (W)-rechte Unterarm (Id 14) hat ein eigenes Ziel
    # aus der Box des Sicht-Pruefers 298-325 x 262-340 (8,8 / -3,4 / 3,2, L* p90 11,1, kein Pixel L* > 30). Runde 2 nahm fuer beide
    # Unterarme die Verteilung der (B)-rechten Box (29,3 L*) -> weiss gefleckt (35 % L* > 30). Eigene CLUT-Zeile 488 (O1).
    ('unterarm_wr', '135', (14,), [(298, 262, 325, 340)],
     'Sicht-Pruefung V4 r2 1.4: Unterarm (B)-links = (W)-rechts, Box 298-325 x 262-340 (deckt die Id-14-Box 289-334 x 264-334)'),
    ('klaue', '135', (12,), [(482, 330, 507, 356)], 'Messblatt F10 (482-507 x 330-356)'),
    ('jeans', '135', (0, 1, 4), [(374, 264, 437, 297), (362, 298, 428, 344)],
     'Messblatt F9 (374-437 x 264-297) + Sicht Jeans (362-428 x 298-344)'),
    ('beine', '135', (2, 3, 5, 6), [(356, 352, 392, 430)], 'Sicht US_Bl (356-392 x 352-430)'),
    # Hals: Rumpf-Bildpunkte im Riegelfenster K8 (KV4 = 0,21-0,62 x 0,88-1,05 der Kopfbox) - im Original dunkel und rotstichig (Messblatt
    # K8/F5, Fenster x 378-398, y 185-194); mit der allgemeinen Rumpfverteilung war K8 dE00 6,6 (Lauf r2 Koerper 5)
    ('hals', '135', (7, 8), [(378, 185, 398, 194)], 'Messblatt K8 / F5 (KV4, x 378-398, y 185-194)', None, ('kv4',)),
    # Buckel seitlich in zwei Teilen wie Messblatt F14 (Id 8, L* >= P70 gegen Fenster 372-414 x 195-227 mit Lum >= 80): der helle Teil
    # bekommt genau die F14-Verteilung, der Rest die uebrige Box (Lauf r2 Koerper 2 mit einer Region: F14 54,1 statt 60,0)
    ('buckel_s_hell', '85', (7, 8), [(372, 195, 414, 227)], 'Messblatt F14 (372-414 x 195-227, Lum >= 80)', None, ('hell', 70, 80)),
    ('buckel_s', '85', (7, 8), [(360, 188, 412, 245)],
     'Vorlage 2 Buckel/Rumpf seitlich (360-412 x 188-245, ohne Kopf x > 412 und Schwanzansatz x < 360), ohne den hellen F14-Teil',
     None, ('rest', 70, 80)),
    ('arm_s', '85', (10, 11, 12, 13, 14), [(395, 250, 440, 300)], 'Sicht OA_nah (395-440 x 250-300)'),
    ('jeans_s', '85', (0, 1, 4), [(397, 298, 432, 327)], 'Messblatt F19 (397-432 x 298-327)'),
]
# V4 Runde 4: Oberkoerper-Polygon (Sicht-Pruefung V4 r3, pruefung_sicht_r3/oberkoerper_vorn.py POLY = oberkoerper_spiel.py PV[135],
# von Hand auf Vorlage 1 gesetzt um Buckel/Schultern/Brust) und Rumpfbox (Sicht-Pruefung V4 r1 1.4); Kachelraster 6 x 4 ueber die Bbox
# (Kachel ~35 x 39 px: mindestens 2 Kacheln je Drittel der Sicht-Pruefung, je Kachel >= 30 Original-Bildpunkte). PORT-WAHL (Raster),
# Nutzer-Wortlaut "Ich will EXAKT diese Umsetzung und auch EXAKT seine Farben".
POLY_VORN = [(297, 175), (320, 150), (360, 140), (410, 138), (450, 150), (490, 165), (505, 195), (500, 235), (470, 250), (330, 250),
             (300, 230)]
RUMPF_BOX = (352, 196, 448, 292)
KACHELN = (6, 4)
# Lab-Versatz je Region/Gruppe (Nachstellung an den Riegeln F6/F7/F8/FS, r4_koerper_eichen.py schreibt die Datei; Protokoll dort)
KORR_DATEI = os.environ.get('RE15_R4_KORR', os.path.join(HIER, 'r4_koerper_korrektur.json'))
KORR = json.load(open(KORR_DATEI)).get('korrektur', {}) if os.path.exists(KORR_DATEI) else {}
# Anteil des Port-Feinmusters im Oberkoerper (ziel_bild): Lauf r4 Koerper 6 (1,0): Rot Mitte/aussen 1,60, Schultern a*>12 0,125, a*>18 739;
# Lauf r4 Koerper 7 (0,5): 2,01 / 0,016 / 291; Lauf r4 Koerper 8 (0,0): 2,18 / 0,009 / 281 - 0,5 behaelt die Hautstruktur (Original
# fleckig) bei fast gleicher Rot-Verteilung. PORT-WAHL, Nutzer-Wortlaut "Ich will ... auch EXAKT seine Farben".
HF_OBERKOERPER = float(os.environ.get('RE15_R4_HF', '0.5'))
KACHEL_MIN = 30      # Mindestzahl Bildpunkte je Kachel (Original und Port), sonst bleibt die Region davor; PORT-WAHL


def oberkoerper_flaeche(shape=(480, 640)):
    m = np.zeros(shape, np.uint8)
    cv2.fillPoly(m, [np.array(POLY_VORN, np.int32)], 1)
    x0, y0, x1, y1 = RUMPF_BOX
    m[y0:y1 + 1, x0:x1 + 1] = 1
    return m.astype(bool)


# Gewicht je Region in den kleinsten Quadraten (Standard 1): geteilte Texel (Oberarme 10/13, Rumpf vorn / Buckel seitlich) mitteln die
# Ziele; F8 (Oberarm (B)-rechts oben) und F14 (Buckel hell) lagen nach Lauf r2 Koerper 5 an bzw. ueber der Grenze (dL +4,0 / -5,4),
# F7 mit -3,0 innerhalb - die schwaecheren Regionen bekommen mehr Gewicht (Lauf r2 Koerper 6: F14 -4,4, Oberarm (B)-links unten
# +4,3 -> 4,0 / 1,5; nach LAM_MIN Lauf r2 Koerper 10: F14 -4,2, Oberarm +4,1 -> 5,0 / 2,0; nach LUM_MAX F14 -5,2 -> 8,0). PORT-WAHL (Ausgleich zwischen Messblatt-Riegeln und Sicht-Boxen).
GEWICHT = dict(oberarm_bl_unten=2.0, buckel_s_hell=8.0, hals=2.0)   # Runde 3: oberarm_br_oben entfaellt (eine Region 'oberarm_br')
# Id 15 (Sichel (B)-links, Zeile 484): Ziel = eigener Render (bleibt wie Referenz 2; nicht beschriftet, im Original nicht abgrenzbar)


def raster(mo, pose):
    """b2.zeichne-Nachbau: Bild, Ids, Texel-x/-y (mit Seitenversatz), CLUT-Zeile, lambda je Bildpunkt."""
    r, kam, M, li = b2.raum()
    kf = BB.kf_von_clip(mo, pose['clip'], pose['bild'])
    lage = (pose['x'], pose.get('y', 0), pose['z'])
    tris = b2.dreiecke_ids(mo, kf, lage, int(pose['gier']), kam, li)
    ziel = G.hintergrund().copy()
    Hh, W = 480, 640
    ids = np.full((Hh, W), -1, np.int16)
    U = np.full((Hh, W), -1, np.int32); Vt = np.full((Hh, W), -1, np.int32); Z = np.full((Hh, W), -1, np.int32)
    LAM = np.zeros((Hh, W, 3), np.float32)
    for _t, p, uv, zeile, farbe, b in sorted(tris, key=lambda t: -t[0]):
        x, y = BB.abbilden(np.array([q[0] + BB.PHASE for q in p], float), np.array([q[1] + BB.PHASE for q in p], float), M)
        fl = (x[1] - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (y[1] - y[0])
        if abs(fl) < 1e-9:
            continue
        x0, x1 = max(int(np.floor(x.min())), 0), min(int(np.ceil(x.max())) + 1, W)
        y0, y1 = max(int(np.floor(y.min())), 0), min(int(np.ceil(y.max())) + 1, Hh)
        if x0 >= x1 or y0 >= y1:
            continue
        PX, PY = np.meshgrid(np.arange(x0, x1) + 0.5, np.arange(y0, y1) + 0.5)
        w1 = ((PX - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (PY - y[0])) / fl
        w2 = ((x[1] - x[0]) * (PY - y[0]) - (PX - x[0]) * (y[1] - y[0])) / fl
        w0 = 1.0 - w1 - w2
        innen = (w0 >= -1e-9) & (w1 >= -1e-9) & (w2 >= -1e-9)
        if not innen.any():
            continue
        u = np.array([q[0] for q in uv], float); v = np.array([q[1] for q in uv], float)
        uu = np.clip((w0 * u[0] + w1 * u[1] + w2 * u[2]).astype(int), 0, mo.texw - 1)
        vv = np.clip((w0 * v[0] + w1 * v[1] + w2 * v[2]).astype(int), 0, mo.texh - 1)
        z = zeile - mo.clut_y
        rgba = mo.lut[z if 0 <= z < mo.lut.shape[0] else 0][mo.tex[vv, uu]]
        tref = innen & (rgba[..., 3] > 0)
        if not tref.any():
            continue
        c0, c1, c2 = (np.asarray(k, float) for k in farbe)
        farb = w0[..., None] * c0 + w1[..., None] * c1 + w2[..., None] * c2
        rgb = np.clip(rgba[..., :3].astype(float) * farb / 255.0, 0, 255).astype(np.uint8)
        s = ziel[y0:y1, x0:x1]; s[tref] = rgb[tref]
        ids[y0:y1, x0:x1][tref] = b
        U[y0:y1, x0:x1][tref] = uu[tref]; Vt[y0:y1, x0:x1][tref] = vv[tref]; Z[y0:y1, x0:x1][tref] = zeile
        LAM[y0:y1, x0:x1][tref] = (farb / 255.0)[tref]
    return ziel, ids, U, Vt, Z, LAM


def box_pixel(img, boxen):
    m = np.zeros(img.shape[:2], bool)
    for (x0, y0, x1, y1) in boxen:
        mm = np.zeros(img.shape[:2], np.uint8); mm[y0:y1 + 1, x0:x1 + 1] = 1
        m |= cv2.erode(mm, np.ones((5, 5), np.uint8)).astype(bool)       # 2 px erodiert
    return img[m]


def ziel_bild(img, ids, ansicht):
    """Ziel je Bildpunkt im Port-Schirm (unweichgezeichnet) fuer alle Koerper-Bildpunkte der Regionen dieser Ansicht."""
    V = G.videoraum(img)
    Lab = G.lab(V)
    T = V.copy()
    gesetzt = np.zeros(ids.shape, bool)
    gew = np.ones(ids.shape)
    kachel_px = np.zeros(ids.shape, bool)
    orig = G.vorlage(1 if ansicht == '135' else 2)
    info = {}
    for reg in REGIONEN:
        name, ans, idl, boxen = reg[:4]
        if ans != ansicht:
            continue
        m = np.isin(ids, idl)
        teil = reg[6] if len(reg) > 6 else None
        o_pix = box_pixel(orig, boxen) if boxen else np.zeros((0, 3))
        if teil is not None and teil[0] == 'kacheln':
            # die unteren Oberarme (unter den Fenstern F7/F8) behalten ihre Sicht-Boxen (oberarm_bl_unten / oberarm_br_unten)
            for b_, f_ in ((13, 0.58), (10, 0.56)):
                mb = ids == b_
                if mb.any():
                    ys_b = np.nonzero(mb.any(1))[0]; yc = int(round(ys_b.min() + f_ * (ys_b.max() + 1 - ys_b.min())))
                    m &= ~(mb & (np.arange(m.shape[0])[:, None] >= yc))
            A = oberkoerper_flaeche(m.shape)
            ys_, xs_ = np.nonzero(A)
            gx = np.linspace(xs_.min(), xs_.max() + 1, KACHELN[0] + 1); gy = np.linspace(ys_.min(), ys_.max() + 1, KACHELN[1] + 1)
            o_lab_all = G.lab(orig)
            yy, xx = np.mgrid[0:m.shape[0], 0:m.shape[1]]
            # Port -> Original nach Anteilslage (wie die Kopfstruktur, eichen_v4.struktur_original): die Port-Figur ist in Pose "135"
            # groesser und weiter links (Schulterbreite Ids 8/10/13 x 267-497 gegen Polygon 297-505). x: Schulterspanne der Ids 8/10/13 im
            # obersten 0,37 H -> Polygon-Spanne; y: (y - Oberkante) / H_port x H1 + Polygon-Oberkante (H1 302, Messblatt V4 Abschnitt 1)
            koerper = (ids >= 0) & ~np.isin(ids, G.SCHWANZ)
            top = int(np.nonzero(koerper.any(1))[0].min()); Hp = float(np.nonzero(koerper.any(1))[0].max() - top)
            sch = np.isin(ids, (8, 10, 13)) & (yy <= top + 0.37 * Hp)
            sx = np.nonzero(sch.any(0))[0]
            px0, px1 = float(sx.min()), float(sx.max())
            pnx0, pnx1 = float(np.array(POLY_VORN)[:, 0].min()), float(np.array(POLY_VORN)[:, 0].max())
            pny0 = float(np.array(POLY_VORN)[:, 1].min())
            xo = pnx0 + (xx - px0) * (pnx1 - pnx0) / max(px1 - px0, 1.0)
            yo = pny0 + (yy - top) * G.H_ORIG[1] / max(Hp, 1.0)
            Ao = np.zeros(m.shape, bool)
            ok_ = (xo >= 0) & (xo < m.shape[1]) & (yo >= 0) & (yo < m.shape[0])
            Ao[ok_] = A[np.clip(yo[ok_].astype(int), 0, m.shape[0] - 1), np.clip(xo[ok_].astype(int), 0, m.shape[1] - 1)]
            n_k = 0
            for i in range(KACHELN[0]):
                for j in range(KACHELN[1]):
                    k_ = A & (xx >= gx[i]) & (xx < gx[i + 1]) & (yy >= gy[j]) & (yy < gy[j + 1])
                    pm = m & Ao & (xo >= gx[i]) & (xo < gx[i + 1]) & (yo >= gy[j]) & (yo < gy[j + 1])
                    if pm.sum() < KACHEL_MIN or k_.sum() < KACHEL_MIN:
                        continue
                    ol = o_lab_all[k_]; pl = Lab[pm]
                    # Lab-Tripel des Originals nach L*-Rang (nicht kanalweise): kanalweise trennte die Abbildung Helligkeit und Rot - die
                    # roten Port-Bildpunkte bekamen helle L*-Werte (F6 dL +5,1, Lauf r4 Koerper 3); im Original sind die roten Stellen dunkel
                    q = (np.argsort(np.argsort(pl[:, 0])) + 0.5) / len(pl)
                    oi = np.argsort(ol[:, 0])
                    neu = ol[oi[np.clip((q * len(ol)).astype(int), 0, len(ol) - 1)]]
                    T[pm] = np.clip(lab2rgb(neu.reshape(-1, 1, 3)).reshape(-1, 3) * 255.0, 0, 255)
                    gesetzt |= pm; gew[pm] = GEWICHT.get(name, 1.0); n_k += 1; kachel_px |= pm
            info[name] = dict(kacheln=n_k, px=int((m & Ao).sum()), port_x=(px0, px1), port_top=top, port_H=Hp)
            continue
        if teil is not None and teil[0] == 'kv4':
            kb_ = G.kopfbox(ids)
            if kb_ is None:
                continue
            fx0, fy0, fx1, fy1 = G.fenster(kb_, (0.21, 0.88, 0.62, 1.05))
            bm = np.zeros(m.shape, bool); bm[fy0:fy1 + 1, fx0:fx1 + 1] = True
            m &= bm
            o_pix = orig[boxen[0][1]:boxen[0][3] + 1, boxen[0][0]:boxen[0][2] + 1].reshape(-1, 3)
        elif teil is not None and teil[0] in ('rot', 'nicht_rot') and m.any():   # Rotanteil: Port nach a*-Rang, Original nach R-G
            ar = Lab[..., 1]; grenze = np.percentile(ar[m], 100.0 * (1.0 - teil[1]))
            m &= (ar >= grenze) if teil[0] == 'rot' else (ar < grenze)
            rg = o_pix[:, 0] - o_pix[:, 1]
            o_pix = o_pix[rg >= teil[2]] if teil[0] == 'rot' else o_pix[rg < teil[2]]
        elif teil is not None and m.any():               # heller Teil (L* >= Perzentil) / Rest, Original nach Lum-Schwelle
            Lr = Lab[..., 0]; grenze = np.percentile(Lr[m], teil[1])
            m &= (Lr >= grenze) if teil[0] == 'hell' else (Lr < grenze)
            lum_o = G.lum(o_pix)
            o_pix = o_pix[lum_o >= teil[2]] if teil[0] == 'hell' else o_pix[lum_o < teil[2]]
        if len(reg) > 5 and reg[5] is not None and m.any():   # Zeilenanteil der Id-Box (Oberarm oben / unten)
            ys_ = np.nonzero(m.any(1))[0]; y0_, y1_ = ys_.min(), ys_.max() + 1
            f0, f1 = reg[5]
            band = np.zeros(m.shape, bool); band[int(round(y0_ + f0 * (y1_ - y0_))):int(round(y0_ + f1 * (y1_ - y0_)))] = True
            m &= band
        if m.sum() < 10:
            continue
        o_lab = G.lab(o_pix)
        p_lab = Lab[m]
        neu = np.empty_like(p_lab)
        for c in range(3):
            rang = np.argsort(np.argsort(p_lab[:, c]))
            q = (rang + 0.5) / len(rang)
            neu[:, c] = np.quantile(o_lab[:, c], q)
        if name in KORR:                     # nur L*: als RGB-Faktor (Farbton bleibt; Lab-Versatz trieb dunkle Farben aus dem Gamut ins Rot)
            dl_ = float(KORR[name][0])
            neu[:, 0] = np.clip(neu[:, 0] + dl_, 0, 100)
            neu[:, 1:] = neu[:, 1:] * (((neu[:, :1] + 16.0) / np.maximum(neu[:, :1] - dl_ + 16.0, 1.0)))
        T[m] = np.clip(lab2rgb(neu.reshape(-1, 1, 3)).reshape(-1, 3) * 255.0, 0, 255)
        gesetzt |= m
        kachel_px &= ~m                      # spaetere Regionen (Hals, Unterarme, ...) gehen vor den Kacheln
        gew[m] = GEWICHT.get(name, 1.0)
        info[name] = dict(px=int(m.sum()), orig_mittel=[round(float(x), 1) for x in o_lab.mean(0)],
                          port_vorher=[round(float(x), 1) for x in p_lab.mean(0)])
    if ansicht == '135' and kachel_px.any():
        # V4 Runde 4: Nachstellung der Kachelziele an den Messblatt-Fenstern (r4_koerper_eichen.py, Datei KORR_DATEI): Lab-Versatz je
        # Gruppe - F6 = rote Bildpunkte (R - G >= 15) von Id 8, F7 = Id 13, F8 = Id 10 in den Kacheln
        Tl = G.lab(T)
        rg_ = (T[..., 0] - T[..., 1]) >= 15
        for k_, mk_ in (('F6', kachel_px & (ids == 8) & rg_), ('F7', kachel_px & (ids == 13)), ('F8', kachel_px & (ids == 10)),
                        ('rumpf_k', kachel_px & (ids == 8) & ~rg_)):
            if k_ in KORR and mk_.any():
                # nur L*, als Faktor auf RGB (Y ~ ((L + 16) / 116)^3): Farbton und Saettigungsverhaeltnis bleiben (Lauf r4 Koerper-
                # Eichung 3: ein L*-Versatz bei festem a*/b* trieb dunkle Farben aus dem Gamut, sie wurden beim Zurueckrechnen rot)
                L_ = Tl[mk_][:, 0]; dl_ = float(KORR[k_][0])
                f_ = (np.clip(L_ + dl_, 0, 100) + 16.0) ** 3 / np.maximum(L_ + 16.0, 1.0) ** 3
                T[mk_] = np.clip(T[mk_] * f_[:, None] ** (1 / 2.2), 0, 255)
    D = img.astype(float) + (T - V) / G.EB_
    if ansicht == '135' and kachel_px.any() and HF_OBERKOERPER != 1.0:
        # V4 Runde 4 (Pruefung V4 r3 Sicht 1.5: auf dem Schirm fast schwarze Risslinien, L* p10 3,3 gegen Video 7,5, und 4,4-5,7 x so viele
        # kraeftig rote Bildpunkte a* > 18 wie das Video): im Oberkoerper den Feinanteil des Port-Bildes (Bild - Gauss sigma 1,5, das
        # Referenz-2-Wundmuster) nur zu HF_OBERKOERPER uebernehmen. D = HF x (Bild - Gauss) + (T - a) / b (HF 1 = Formel oben)
        glatt = cv2.GaussianBlur(img.astype(np.float32), (0, 0), G.SIGMA).astype(float)
        Dk = HF_OBERKOERPER * (img.astype(float) - glatt) + (T - G.EA_) / G.EB_
        D[kachel_px] = Dk[kachel_px]
    return D, gesetzt, info, gew


def main():
    ap = argparse.ArgumentParser(); ap.add_argument('--emd', default=os.path.join(G.SFV4, 'r2', 'emd', 'EM30_V4.EMD'))
    ap.add_argument('--k', type=int, default=256)
    a = ap.parse_args()
    mo = b2.modell(a.emd)
    ref = b2.modell(G.REF2)
    P = G.posen()
    bel, zl = UB.belegung(mo, KOERPER)
    fuss = np.zeros((mo.texh, mo.texw), bool)
    for b in KOERPER:
        fuss |= bel[b]
    num = {z: np.zeros((mo.texh, mo.texw, 3)) for z in ZEILEN}
    den = {z: np.zeros((mo.texh, mo.texw, 3)) for z in ZEILEN}
    prot = dict(regionen={})
    for ansicht in ('135', '85'):
        img, ids, U, Vt, Z, LAM = raster(mo, P[ansicht])
        D, gesetzt, info, gew = ziel_bild(img, ids, ansicht)
        prot['regionen'][ansicht] = info
        sel = gesetzt | (ids == 15)
        D[ids == 15] = img[ids == 15]
        for z in ZEILEN:
            q = sel & (Z == z)
            if not q.any():
                continue
            uu, vv = U[q], Vt[q]
            lam = LAM[q].astype(float); d = D[q]; w = gew[q][:, None]
            w = w * (lam.mean(1, keepdims=True) >= LAM_MIN)          # streifend beleuchtete Bildpunkte nicht (Ausreisser, s. LAM_MIN)
            np.add.at(num[z], (vv, uu), w * lam * d)
            np.add.at(den[z], (vv, uu), w * lam * lam)
    # Vorlauf: Verstaerkung je Mesh (Median t / Referenz-2-Texel ueber gesehene Texel), dann t0 fuer ungesehene Texel
    t0 = {}
    for z in ZEILEN:
        alt = ref.lut[START.get(z, z) - 480][ref.tex][..., :3].astype(float)
        frei = num[z] / np.maximum(den[z], 1e-6)
        ges = den[z][..., 0] > 0.05
        t0z = alt.copy()
        for b in KOERPER:
            if z not in zl[b]:
                continue
            m = bel[b] & ges
            if m.sum() < 20:
                continue
            g = np.median(frei[m] / np.maximum(alt[m], 4.0), 0)
            mm = bel[b] & ~ges
            t0z[mm] = alt[mm] * g
        t0[z] = np.clip(t0z, 0, 248)
    # Ausreisser begrenzen: je Texel hoechstens Faktor 2,0 ueber t0 (+16) und 0,2 darunter (mit 0,4 unten blieben die Oberarme zu hell,
    # F8 dL +4,6, Lauf r2 Koerper 8; mit 1,5 oben wurde der Buckel zu dunkel, F14 dL -6,3, Lauf 9). Ohne Grenze bekamen Buckel-Texel mit kleinem
    # lambda seitlich und hohem F14-Gewicht Weiss (248) und zeigten vorn auf der Schulter weisse Punkte (Leiste 03 Runde 2, Lauf r2 Koerper 7)
    # V4 Runde 4: die Grenze gilt fuer die Leuchtdichte, nicht je Kanal - je Kanal hielt sie den Farbton der roten Referenz-2-Texel fest
    # (G <= 2 x G0 + 16 laesst einen roten Texel nicht grau werden): Kachelziel Oberarm (B)-links Rotanteil 0,14, Fit 0,33 (Lauf r4
    # Koerper 3, Zeile 490 eigen). Faktoren 0,2 / 2,0 / +16 wie Runde 2 (Laeufe r2 Koerper 8/9).
    T_ = {}
    for z in ZEILEN:
        t_ = (num[z] + MU * t0[z]) / (den[z] + MU)
        lu, l0 = G.lum(t_), G.lum(t0[z])
        f_ = np.clip(lu, 0.2 * l0, 2.0 * l0 + 16.0) / np.maximum(lu, 1e-6)
        T_[z] = np.clip(t_ * f_[..., None], 0, 248)
    # Helligkeitsdecke: kein Texel heller als Lum LUM_MAX (Leiste 03 Runde 2: zwei weisse Texel, Lum 233, auf der (B)-linken Schulter
    # wirkten wie ein aufgemaltes Augenpaar neben dem Kopf)
    for z in ZEILEN:
        lu = G.lum(T_[z])
        f = np.where(lu > LUM_MAX, LUM_MAX / np.maximum(lu, 1e-6), 1.0)
        T_[z] = T_[z] * f[..., None]
    nutz = np.zeros((mo.texh, mo.texw, len(ZEILEN)), bool)
    for b in KOERPER:
        for z in zl[b]:
            if z in ZEILEN:
                nutz[..., ZEILEN.index(z)] |= bel[b]
    ys, xs = np.nonzero(fuss)
    X = np.stack([T_[z][ys, xs] for z in ZEILEN], 1)              # N x 4 x 3
    Wm = nutz[ys, xs].astype(float)                                # N x 4
    # V4 Runde 4: Gewicht je (Texel, Zeile) = Sichtbarkeit in den beiden Messansichten (Summe lambda^2 der Bildpunkte, die den Texel in
    # dieser Zeile zeigen) + 0,05 fuer benutzte, ungesehene Texel. Mit gleichem Gewicht fuer alle benutzten Zeilen zog die gemeinsame
    # Quantisierung die Zeile 490 (Oberarm (B)-links, teilt 5074 Texel mit der roten Brust in Zeile 480) zur Brustfarbe: Fit ohne
    # Quantisierung Rotanteil 0,33 (Schirm) / Render 0,42, Lauf r4 Koerper 5.
    sicht = np.stack([den[z][ys, xs].mean(1) for z in ZEILEN], 1)
    Wm = Wm * (0.05 + sicht)
    rng = np.random.default_rng(41)
    K = a.k
    C = X[rng.choice(len(X), K, replace=False)].copy()
    for _it in range(25):
        dist = np.zeros((len(X), K))
        for r in range(len(ZEILEN)):
            dd = ((X[:, None, r, :] - C[None, :, r, :]) ** 2).sum(2)
            dist += Wm[:, r:r + 1] * dd
        k_ = dist.argmin(1)
        for k in range(K):
            mk = k_ == k
            if not mk.any():
                continue
            for r in range(len(ZEILEN)):
                w = Wm[mk, r]
                if w.sum() > 0:
                    C[k, r] = (X[mk, r] * w[:, None]).sum(0) / w.sum()
    fehler = np.sqrt((Wm[..., None] * (X - C[k_]) ** 2).sum() / max(Wm.sum() * 3, 1))
    for r, z in enumerate(ZEILEN):
        print('Quantisierung Zeile %d: RMSE %.2f (gewichtet, %d Texel)' % (z, float(np.sqrt((Wm[:, r:r + 1] * (X[:, r] - C[k_][:, r]) ** 2).sum()
                                                                                          / max(Wm[:, r].sum() * 3, 1e-9))), int((nutz[ys, xs][:, r]).sum())))
    idx = np.full((mo.texh, mo.texw), -1, np.int16)
    idx[ys, xs] = k_
    zeilen = np.stack([C[:, r, :] for r in range(len(ZEILEN))], 0)  # 4 x 256 x 3
    np.savez(AUS, koerper_idx=idx, koerper_zeilen=zeilen, koerper_zeilen_nr=np.array(ZEILEN))
    if os.environ.get('RE15_R4_DEBUG'):
        np.savez(os.environ['RE15_R4_DEBUG'], **{'T%d' % z: T_[z] for z in ZEILEN}, **{'t0_%d' % z: t0[z] for z in ZEILEN},
                 **{'num%d' % z: num[z] for z in ZEILEN}, **{'den%d' % z: den[z] for z in ZEILEN})
    prot.update(texel=int(len(X)), k=K, quant_rmse=round(float(fehler), 2), mu=MU, quellen={r[0]: r[4] for r in REGIONEN})
    json.dump(prot, open(PROT, 'w'), indent=1)
    print('geschrieben', AUS, 'Texel', len(X), 'Quantisierung RMSE', round(float(fehler), 2))
    for ans, d in prot['regionen'].items():
        for k, v in d.items():
            print(ans, k, v)


if __name__ == '__main__':
    main()
