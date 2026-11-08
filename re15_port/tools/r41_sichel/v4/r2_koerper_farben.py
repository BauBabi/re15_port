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
# Vorwissen: wie ein Viertel Bildpunkt mit lambda 1 (haelt kaum gesehene Texel bei Referenz 2 x Verstaerkung). PORT-WAHL (Regularisierung
# der Texel-Loesung, keine Spielkonstante; Laeufe r4 Koerper 1-8), Nutzer-Wortlaut "Ich will EXAKT diese Umsetzung und auch EXAKT seine Farben" - der Nutzer hat den Koerper Referenz 2 gelobt
# (Dossier "Referenz 2"), ungesehene Texel bleiben deshalb nahe an ihm.
MU = float(os.environ.get("RE15_R4_MU", "0.25"))

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
KACHEL_ORT = os.environ.get('RE15_R5_KACHEL_RANG') != '1'
KACHEL_Y_BOX = os.environ.get('RE15_R5_Y_H') != '1'          # V4 Runde 5 (O10): y-Abbildung ueber die Oberkoerper-Spanne (s. ziel_bild)   # V4 Runde 5 (O10): Farbe nach Anteilslage statt Rangabbildung je Kachel
HF_NUR_HELL = os.environ.get('RE15_R5_HF_FARBE') != '1'     # V4 Runde 5 (O10): Feinanteil nur als Helligkeit (s. ziel_bild)
# Mindestzahl Bildpunkte je Kachel (Original und Port), sonst bleibt die Region davor. PORT-WAHL (Zaehlgrenze fuer eine Farbverteilung, Laeufe
# r4 Koerper 6-8), Nutzer-Wortlaut "Ich will EXAKT diese Umsetzung und auch EXAKT seine Farben". Seit Runde 5 (KACHEL_ORT) nur noch in der Rangabbildung (RE15_R5_KACHEL_RANG=1) wirksam.
KACHEL_MIN = 30


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


_gelb = {}
def gelb_grenze():
    """V4 Runde 5 (Pruefung V4 r4 Sicht 1.5 Klasse A "gelber Fleck (B)-linke Schulter", Entscheid O9): Hoechstes b* der Figur im Original =
    Maximum von b* in der Rumpfbox 352-448 x 196-292 (Sicht-Pruefung V4 r1 1.4) und der Kopfbox 368-416 x 139-191 (Messblatt Abschnitt 2)
    der Vorlage 1 (gemessen: 9,9 / 9,1). Bildpunkte mit hoeherem b* sind Hintergrund: die gelbe Lampe hinter der (B)-linken Schulter liegt in
    der Schulterbox 318-360 x 160-190 und im Polygon POLY_VORN (x 306-341, y 145-167, b* bis 28) - von dort kamen die 45 gelben Texel."""
    if not _gelb:
        L = G.lab(G.vorlage(1))
        _gelb['b'] = max(float(L[196:293, 352:449, 2].max()), float(L[139:192, 368:417, 2].max()))
    return _gelb['b']


def ohne_gelb(px):
    """Original-Bildpunkte ohne Hintergrund-Gelb (b* > gelb_grenze())."""
    return px[G.lab(px)[:, 2] <= gelb_grenze()] if len(px) else px


def box_pixel(img, boxen):
    m = np.zeros(img.shape[:2], bool)
    for (x0, y0, x1, y1) in boxen:
        mm = np.zeros(img.shape[:2], np.uint8); mm[y0:y1 + 1, x0:x1 + 1] = 1
        m |= cv2.erode(mm, np.ones((5, 5), np.uint8)).astype(bool)       # 2 px erodiert
    return ohne_gelb(img[m])


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
            if KACHEL_Y_BOX:
                # V4 Runde 5 (O10): y wie x ueber die Spanne - Port-Oberkante .. 0,37 H_port (Oberkoerper der Sicht-Pruefung, r4_mass.
                # oberkoerper_buehne) -> Polygon-Oberkante .. -Unterkante; mit dem Massstab H1/H_port lag die Port-Brust 0,03-0,05 H1 zu hoch im
                # Original (Pose "135" ist aufrechter als n1485, Messblatt Abschnitt 9 Punkt 5)
                pny1 = float(np.array(POLY_VORN)[:, 1].max())
                yo = pny0 + (yy - top) * (pny1 - pny0) / max(0.37 * Hp, 1.0)
            Ao = np.zeros(m.shape, bool)
            ok_ = (xo >= 0) & (xo < m.shape[1]) & (yo >= 0) & (yo < m.shape[0])
            Ao[ok_] = A[np.clip(yo[ok_].astype(int), 0, m.shape[0] - 1), np.clip(xo[ok_].astype(int), 0, m.shape[1] - 1)]
            n_k = 0
            if KACHEL_ORT:
                # V4 Runde 5 (Pruefung V4 r4 Sicht 1.5 Klasse A, Entscheid O10 "Rot nur in der Brustmitte wie im Original"): die
                # Rangabbildung je Kachel verteilte das Rot der Kachel auf die DUNKLEN Port-Bildpunkte (Spalten des Referenz-2-Musters) -
                # rote Sprenkel ueber den ganzen Oberkoerper (Mitte/aussen 1,5 im Spielbild, Video 2,8). Jetzt bekommt jeder Port-Bildpunkt
                # die Farbe des Originals an seiner Anteilslage (xo, yo) - die Raumverteilung des Rots kommt ganz aus dem Original. Quelle =
                # Bildpunkte der Flaeche A ohne Hintergrund-Gelb (gelb_grenze); ausserhalb der naechste Quellpunkt.
                from scipy import ndimage as nd_
                quelle = A & (o_lab_all[..., 2] <= gelb_grenze())
                _d, (qy, qx) = nd_.distance_transform_edt(~quelle, return_indices=True)
                pm = m & Ao
                yi = np.clip(np.round(yo[pm]).astype(int), 0, m.shape[0] - 1); xi = np.clip(np.round(xo[pm]).astype(int), 0, m.shape[1] - 1)
                T[pm] = orig[qy[yi, xi], qx[yi, xi]]
                gesetzt |= pm; gew[pm] = GEWICHT.get(name, 1.0); kachel_px |= pm
                info[name] = dict(ort=True, px=int(pm.sum()), port_x=(px0, px1), port_top=top, port_H=Hp)
                continue
            for i in range(KACHELN[0]):
                for j in range(KACHELN[1]):
                    k_ = A & (xx >= gx[i]) & (xx < gx[i + 1]) & (yy >= gy[j]) & (yy < gy[j + 1])
                    pm = m & Ao & (xo >= gx[i]) & (xo < gx[i + 1]) & (yo >= gy[j]) & (yo < gy[j + 1])
                    if pm.sum() < KACHEL_MIN or k_.sum() < KACHEL_MIN:
                        continue
                    ol = o_lab_all[k_]; ol = ol[ol[:, 2] <= gelb_grenze()]      # V4 Runde 5: ohne Hintergrund-Gelb (O9)
                    pl = Lab[pm]
                    if len(ol) < KACHEL_MIN:
                        continue
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
            o_pix = ohne_gelb(orig[boxen[0][1]:boxen[0][3] + 1, boxen[0][0]:boxen[0][2] + 1].reshape(-1, 3))
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
        if name in KORR:                     # L* (Farbton bleibt; ein Lab-Versatz trieb dunkle Farben aus dem Gamut ins Rot) und
            dl_ = float(KORR[name][0])       # als zweiter Wert ein Faktor auf a*/b* (Saettigung, 1 + k)
            neu[:, 0] = np.clip(neu[:, 0] + dl_, 0, 100)
            neu[:, 1:] = neu[:, 1:] * (((neu[:, :1] + 16.0) / np.maximum(neu[:, :1] - dl_ + 16.0, 1.0)))
            if len(KORR[name]) > 1 and KORR[name][1]:
                neu[:, 1:] = neu[:, 1:] * (1.0 + float(KORR[name][1]))
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
        hf = img.astype(float) - glatt
        if HF_NUR_HELL:
            # V4 Runde 5 (Pruefung V4 r4 Sicht 1.5 Klasse A: "mauve-braun mit roten Adern ueberall", Schultern a* > 12 0,07-0,08 im
            # Spielbild, Video 0,00-0,05; Entscheid O10 "Rot nur in der Brustmitte"): der Feinanteil des Referenz-2-Musters traegt die roten
            # Adern als Farbton - nur seine Helligkeit wird uebernommen (gleicher Wert auf R, G, B), der Farbton kommt allein aus dem Original
            hf = np.repeat(G.lum(hf)[..., None], 3, axis=2)
        Dk = HF_OBERKOERPER * hf + (T - G.EA_) / G.EB_
        D[kachel_px] = Dk[kachel_px]
    return D, gesetzt, info, gew


# V4 Runde 5 (Pruefung V4 r4 Sicht 1.5 Klasse A "Rot-Verteilung Oberkoerper vorn", Entscheid O10 "Rot nur in der Brustmitte wie im
# Original"): die Farbe der Texel kommt aus ZWEI Messansichten (Pose "135" und "85"); im Spielbild h118/h135 ist die Figur vorgebeugt und
# zeigt die (B)-linke Schulter/den Buckel breiter - dort lag das Rot (Drittel (B)-links 0,35-0,39, Video 0,18-0,23). Ausgleich je Drittel
# des Oberkoerpers: jeder Texel bekommt einen Farbtonfaktor k (a*, b* x k) = Mittel der Drittelfaktoren ueber die Bildpunkte, an denen er in
# den drei Vorderansichten (Buehne Pose "135", Laufposen h118 / h135 aus dem Lauf DRITTEL_LAUF) sichtbar ist. Die Faktoren werden so
# bestimmt (Halbierung), dass der Rotanteil (R - G >= 15, Schirm x Licht, Eichung a/b) je Drittel das Mittel der Originale trifft
# (Vorlage 1 Polygon POLY_135: 0,18 / 0,58 / 0,24; Video h118 Polygon PV_SPIEL: 0,23 / 0,55 / 0,14; Vorlage 1 h135 wie Buehne) - Ziel ist
# die gemessene Original-Verteilung, nicht die Riegelgrenze. Drittel und Polygone: r4_mass (Sicht-Pruefung V4 r3).
DRITTEL_AUSGLEICH = os.environ.get('RE15_R5_DRITTEL') != '0'
DRITTEL_NUR_SPIEL = os.environ.get('RE15_R5_DRITTEL_BUEHNE') != '1'   # Ziel/Messung nur an den Spielbildern h118/h135 (Buehne besteht FO)
DRITTEL_LAUF = os.path.join(G.SFV4, 'r', 'V4r4', 'debug.log')


def drittel_ausgleich(mo, T_, P):
    import r4_mass as R4
    sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'nb2neu'))
    import riegel_v3 as R3
    import cv2 as cv2_
    from skimage.color import rgb2lab, lab2rgb as l2r
    PO = R3.lauf_posen(DRITTEL_LAUF)
    os.environ['NB6_RUNS'] = os.path.dirname(os.path.dirname(DRITTEL_LAUF))
    import c6
    ansichten = []
    o1 = G.vorlage(1)
    vid118 = cv2_.cvtColor(c6.vid(118), cv2_.COLOR_BGR2RGB).astype(float)
    ziele = []
    for nm, pose, poly in (('135', P['135'], None), ('h118', PO[118], R4.PS_SPIEL[118]), ('h135', PO[135], R4.PS_SPIEL[135])):
        img, ids, U, Vt, Z, LAM = raster(mo, pose)
        if poly is None:
            top = np.nonzero((ids >= 0).any(1))[0].min(); Hp = float(np.nonzero((ids >= 0) & ~np.isin(ids, G.SCHWANZ))[0].max() - top)
            m = np.isin(ids, (8, 9, 10, 13)) & (np.arange(ids.shape[0])[:, None] <= int(round(top + 0.37 * Hp)))
            m = cv2_.erode(m.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
            vd = R4.drittel(o1, R4.poly_maske(R4.POLY_135))
        else:
            m = R4.poly_maske(poly)
            vd = R4.drittel(vid118 if nm == 'h118' else o1, R4.poly_maske(R4.PV_SPIEL[118 if nm == 'h118' else 135]))
        ys, xs = np.nonzero(m)
        x0, x1 = xs.min(), xs.max()
        dr = np.clip(((np.arange(640) - x0) * 3 // max(x1 - x0, 1)), 0, 2)
        q = m & np.isin(Z, ZEILEN) & (ids >= 0) & (ids != 9)
        ansichten.append(dict(nm=nm, q=q, m=m, img=img.astype(float), dr=np.broadcast_to(dr[None, :], q.shape)[q], z=Z[q], u=U[q], v=Vt[q],
                              lam=LAM[q].astype(float)))
        ziele.append([vd[k]['rot'] for k in ('B_links', 'Mitte', 'B_rechts')])
    ziel = np.mean(ziele, 0)
    # Gewicht je Texel und Drittel (Zahl der Bildpunkte in allen drei Ansichten)
    w = {z: np.zeros(T_[z].shape[:2] + (3,)) for z in ZEILEN}
    for a in ansichten:
        for t in range(3):
            s_ = a['dr'] == t
            for z in ZEILEN:
                zz = s_ & (a['z'] == z)
                np.add.at(w[z][..., t], (a['v'][zz], a['u'][zz]), 1.0)
    lab_ = {z: rgb2lab(np.clip(T_[z] / 255.0, 0, 1)) for z in ZEILEN}
    import birkin_formen as BF_
    A_MAX_ = BF_.FARB_A_MAX if BF_.FARB_A_MAX > 0 else 1e9

    def anwenden(k):
        out = {}
        for z in ZEILEN:
            n = w[z].sum(2)
            kk = np.where(n > 0, (w[z] * np.asarray(k)[None, None, :]).sum(2) / np.maximum(n, 1e-9), 1.0)
            L = lab_[z].copy(); L[..., 1:] *= kk[..., None]
            L[..., 1] = np.minimum(L[..., 1], A_MAX_)          # wie birkin_formen.farbklammer (a* <= FARB_A_MAX) im Generator
            out[z] = np.clip(l2r(L) * 255.0, 0, 248)
        return out

    def anteile(Tk):
        # Messung wie der Riegel (r4_mass.drittel nach der Vergleichsregel sigma 1,5 / a / b) am Bild mit den neuen Texelfarben
        r_ = np.zeros(3)
        for a in ansichten:
            px = np.zeros((len(a['z']), 3))
            for z in ZEILEN:
                zz = a['z'] == z
                px[zz] = Tk[z][a['v'][zz], a['u'][zz]] * a['lam'][zz]
            bild = a['img'].copy(); bild[a['q']] = px
            d_ = R4.drittel(G.videoraum(bild), a['m'])
            r_ += np.array([d_[k_]['rot'] for k_ in ('B_links', 'Mitte', 'B_rechts')])
        return r_ / len(ansichten)
    # Gemessen wird an den beiden Spielbildern (h118, h135); die Buehne (Pose "135") sieht die Texel mit, ihr Rot geht in die Gewichte w
    ansichten_alle = ansichten
    k = np.ones(3)
    ansichten = [a for a in ansichten_alle if a['nm'] != '135'] if DRITTEL_NUR_SPIEL else ansichten_alle
    ziel = np.mean([zz for zz, a in zip(ziele, ansichten_alle) if a in ansichten], 0)
    ziel_video = ziel.copy()
    for _it in range(3):                                  # je Drittel halbieren (die Drittel teilen Texel, daher 3 Durchgaenge)
        # Die Mitte erreicht im Port mit Faktor <= 1 nur ~0,48 (der Kopf liegt im Mitteldrittel, Video 0,55-0,58). Ziel fuer die
        # Aussendrittel ist deshalb die VERTEILUNG des Videos: Aussen-Ziel = Video-Aussenanteil x (Port-Mitte / Video-Mitte), d. h. das
        # Verhaeltnis Mitte / aussen des Videos (h118 2,91, h135 2,75) bei der erreichten Port-Mitte
        if _it > 0:
            cm = anteile(anwenden(k))[1]
            ziel = ziel_video * np.array([cm / ziel_video[1], 1.0, cm / ziel_video[1]])
        for t in (1, 0, 2):
            # Mitte hoechstens 1 (kein kraeftigeres Rot als der Fit: Lauf r5 Koerper g mit Mitte 1,125 -> Buehne a* > 18 ueber dem Original,
            # Spielbild-Mitte trotzdem nicht roeter, weil die a*-Grenze FARB_A_MAX die Mitte kappt)
            lo_, hi_ = 0.0, (1.0 if t == 1 else 2.0)
            for _ in range(12):
                k[t] = 0.5 * (lo_ + hi_)
                if anteile(anwenden(k))[t] > ziel[t]:
                    hi_ = k[t]
                else:
                    lo_ = k[t]
    DRITTEL_INFO.update(ziel_video=[round(float(x), 3) for x in ziel_video], ziel=[round(float(x), 3) for x in ziel], faktoren=[round(float(x), 3) for x in k],
                        anteile_vorher=[round(float(x), 3) for x in anteile(T_)], anteile_nachher=[round(float(x), 3) for x in anteile(anwenden(k))])
    print('Drittel-Ausgleich', DRITTEL_INFO, flush=True)
    return anwenden(k)


DRITTEL_INFO = {}

# V4 Runde 5 (Entscheid O9 "keine Farbe, die im Original-Bild an der Stelle nicht vorkommt"; Pruefung V4 r4 Sicht 1.5: gelber Fleck und
# 253 grelle Bildpunkte C* > 45 an Jeans/Beinen): jeder Koerper-Bildpunkt der beiden Messansichten, dessen Farbe (Schirm in der Eichung
# a + b x P) weiter als r5_mass.AF_DE von jeder Farbe der Original-Region seines Meshes liegt (r5_mass.regionen_mengen, dasselbe Mass wie der
# Riegel AF), bekommt als Ziel die naechste Farbe dieser Menge; die betroffenen Texel werden aus diesen Zielen neu geloest (wie Schritt 3).
FARBMENGE = os.environ.get('RE15_R5_FARBMENGE') != '0'
FARBMENGE_INFO = {}


def farbmenge(mo, T_, P):
    import r5_mass as M5
    from scipy.spatial import cKDTree
    from skimage.color import lab2rgb as l2r
    num = {z: np.zeros_like(T_[z]) for z in ZEILEN}; den = {z: np.zeros_like(T_[z]) for z in ZEILEN}
    for ans in ('135', '85'):
        img, ids, U, Vt, Z, LAM = raster(mo, P[ans])
        mengen = M5.regionen_mengen(ids, ans)
        n_aus = 0
        for b, S in mengen.items():
            q = (ids == b) & np.isin(Z, ZEILEN) & (LAM.mean(2) >= LAM_MIN)
            if not q.any():
                continue
            zz, uu, vv, lam = Z[q], U[q], Vt[q], LAM[q].astype(float)
            sch = np.zeros((len(zz), 3))
            for z in ZEILEN:
                k = zz == z
                sch[k] = T_[z][vv[k], uu[k]] * lam[k]
            pl = G.lab(np.clip(G.EA_ + G.EB_ * sch, 0, 255))
            d, j = cKDTree(S).query(pl)
            aus = d > M5.AF_DE
            n_aus += int(aus.sum())
            if not aus.any():
                continue
            ziel = (np.clip(l2r(S[j[aus]].reshape(-1, 1, 3)).reshape(-1, 3) * 255.0, 0, 255) - G.EA_) / G.EB_
            for z in ZEILEN:
                k = zz[aus] == z
                np.add.at(num[z], (vv[aus][k], uu[aus][k]), lam[aus][k] * ziel[k])
                np.add.at(den[z], (vv[aus][k], uu[aus][k]), lam[aus][k] ** 2)
        FARBMENGE_INFO[ans] = n_aus
    out = {}
    for z in ZEILEN:
        g = den[z][..., 0] > 0
        out[z] = T_[z].copy()
        out[z][g] = np.clip(num[z][g] / den[z][g], 0, 248)
        FARBMENGE_INFO['texel_%d' % z] = int(g.sum())
    print('Farbmenge', FARBMENGE_INFO, flush=True)
    return out


def main():
    ap = argparse.ArgumentParser(); ap.add_argument('--emd', default=os.path.join(G.SFV4, 'r2', 'emd', 'EM30_V4.EMD'))
    ap.add_argument('--k', type=int, default=256)
    ap.add_argument('--aus', default=AUS)                     # V4 Runde 5: Versuchslaeufe in den Scratchpad
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
    if DRITTEL_AUSGLEICH:
        T_ = drittel_ausgleich(mo, T_, P)
    if FARBMENGE:
        T_ = farbmenge(mo, T_, P)
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
    np.savez(a.aus, koerper_idx=idx, koerper_zeilen=zeilen, koerper_zeilen_nr=np.array(ZEILEN))
    if os.environ.get('RE15_R4_DEBUG'):
        np.savez(os.environ['RE15_R4_DEBUG'], **{'T%d' % z: T_[z] for z in ZEILEN}, **{'t0_%d' % z: t0[z] for z in ZEILEN},
                 **{'num%d' % z: num[z] for z in ZEILEN}, **{'den%d' % z: den[z] for z in ZEILEN})
    prot.update(drittel=DRITTEL_INFO, farbmenge=FARBMENGE_INFO, texel=int(len(X)), k=K, quant_rmse=round(float(fehler), 2), mu=MU, quellen={r[0]: r[4] for r in REGIONEN})
    json.dump(prot, open(PROT if a.aus == AUS else os.path.splitext(a.aus)[0] + '_protokoll.json', 'w'), indent=1)
    print('geschrieben', a.aus, 'Texel', len(X), 'Quantisierung RMSE', round(float(fehler), 2))
    for ans, d in prot['regionen'].items():
        for k, v in d.items():
            print(ans, k, v)


if __name__ == '__main__':
    main()
