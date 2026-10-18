#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Runde 38 Spur K "itemorte": Feuerloescher nach ROOM1030, Parking Lot Key / Prison Key an die Pinnwand ROOM10F0/10F1.

Erzeugt re15_port/engine/src/gen/itemorte_r38_lage.inc (Lage-Konstanten, #define) und gen/itemorte_r38_md1.inc
(die zwei skalierten RE2-Schluessel-MD1) — KEIN Asset-Patch, shared_assets bleibt unangetastet.
Dossier: analysis/befunde_runde38/K_itemorte.md.

NUTZER-VORGABE (AUFTRAG.md Runde 38, woertlich):
  "Ich haette den fire extinguisher gerne in room 1030 haengen an der stelle von fire_extinguisher.bmp, anstatt in
   room 1050."
  "Ich moechte das du den Parking Lot key, und den Prison Key - je nach Szenario - gut sichtbar an die Location
   platzierst, die ich im Bild key_location.bmp markiert habe."

WAS GEMESSEN IST (Dossier Abschnitt 1/2, alle Zahlen dort mit Herkunft):
  * Cut der Bilder: Bildvergleich gegen ALLE 428 STAGE1-Hintergruende (Engine-Dekoder probe_bg_dump), mittlere
    Abweichung je Pixel ausserhalb der Marke: fire_extinguisher.bmp = ROOM1030 Cut 0 (15,94; Cut 9 = dieselbe
    Kamera 22,29; Rest >= 62), key_location.bmp = ROOM10F0 Cut 5 (4,34; Rest >= 90,9). Marken-Huelle hier aus den
    BMP gelesen (exakte Markenfarbe (237,28,36), wie re15_port/tools/r34n_e/marken.py).
  * Kameras: `probe_r34n_e_dokumente kamera 1030|10F0` (VIEW-Zeilen = re15_camera_build_view, FUN_80053ca4).
  * ROOM1030 Pfeiler: Kanten trianguliert (Cut 0/1/3, Strahlen der Engine-Matrix, Senkrechte als Gerade in der
    Draufsicht): (-x,+z)-Ecke (-2668,-17564), (-x,-z)-Ecke (-2709,-18459) (5 Beobachtungen, Rest <= 8),
    (+x,+z)-Ecke (-1790,-17606). Die Marke liegt auf der -x-Flaeche (Bildspalten 35,3..49,4 in Cut 0; die +z-Flaeche
    mit dem Aushang liegt bei 24..35), unter dem Notruf-Monitor (ROOM1030 msg 14 "...emergency broadcast...",
    Monitor-Anzeige trianguliert (-3225,-3940,-18026) aus Cut 0/1/2).
  * ROOM10F0 Pinnwand: Rueckwand z = -3400 (SCA-Zelle 0 x -600..11300 z -5400..-3400; Zettel der Pinnwand aus
    Cut 5 + Cut 6 trianguliert z -3481 / -3352).

PORT-WAHL (die FORM ist jeweils belegt, die Zahlen kommen aus der Messung):
  FEUERLOESCHER: dasselbe stehende Netz wie Runde 37 (gen/feuerloescher_r37.inc, Masse aus dem Netz gelesen:
    Hoehe 792, Radius 110), dieselbe Wandregel wie ROOM1050 (Achse R + 15 vor der Wand, Runde 37 Dossier 3.2),
    Etikett zum Raum (rot_y so, dass Modell -z in die Wand zeigt). MITTE = Sehstrahl der Markenmitte geschnitten
    mit der Achsenebene (Runde 36 N: "Mitte = Sehstrahl der Markenmitte x Ebene"); Groesse NICHT an die Marke
    angepasst (es ist derselbe Loescher, den Leon spaeter in ROOM1090 ablegt).
  SCHLUESSEL: flach an der Pinnwand (Modellebene parallel zur Wand). Sichtseite = Wicklungs-Vorderseite = die Seite,
    auf der die Textur NICHT gespiegelt erscheint (P. Room Key +y; Rechnung im Dossier 2.6, der Small Key des Prison Key ebenso aus seinen UV), Textur-u
    waagerecht nach rechts (wie im TIM gezeichnet; P. Room Key Kopf rechts wie im RE2-Itembild ITPS 95).
    "gut sichtbar" = NUTZER-VORGABE; MASSSTAB s = groesster Wert, bei dem alle SICHTBAREN Texel (CLUT-Farbe != 0)
    im Nutzer-Cut innerhalb der Marken-Huelle liegen (+-0,5 / +1,5 px wie karten_export.py, Runde 36 N 3.3),
    Mitte des sichtbaren Rechtecks auf dem Sehstrahl der Markenmitte. Das MD1 wird gleichmaessig skaliert und in der
    Modellebene verschoben (nur die Ecken @0x44, Normalen/UV/Topologie unveraendert — wie karten_export.py).
    KONTROLLE (Nachbesserung 1, Dossier 9.5): die GEZEICHNETE Huelle (gerundete Ecken, Dreiecke wie der Port, Texel =
    (int) des baryzentrisch interpolierten UV wie unit_r38_itemorte_lage) muss in der Marke liegen; sonst wird s in
    Schritten von 0,05 % kleiner (Parking Lot Key: 0 Schritte, Small Key des Prison Key: 10 Schritte, s 5,3297 -> 5,3032).

RUNDE 39 SPUR L "optik" (Dossier analysis/befunde_runde39/L_optik.md, Punkt 2) — NUTZER-VORGABE (AUFTRAG.md Runde 39):
  "Der Schluessel im Raum 10F0 und 10F1 sollte schon kleiner/subtiler sein, wie er dran haengt, nicht so riesig."
  Der Runde-38-Massstab (groesster Wert, bei dem der Schluessel die 11x9-Marke fuellt: 735 x 468 bzw. 736 x 345 Einheiten,
  12 Bildspalten) war 5..6 x die RE2-Modellgroesse. FORM der neuen Groesse (Beta -> Retail, RE2 = Vorbild): jeder
  Schluessel erscheint im Nutzer-Cut so gross, wie RE2 Retail DASSELBE Modell an seinen eigenen Platzierungen in den
  NAEHEREN Kameras zeigt — oberes Quartil (75. Perzentil, linear interpoliert) der Bildlaenge L_nat * H / vz ueber alle
  RE2-Kameras, die den Schluessel im Bild haben (L_nat = sichtbare Modell-Laenge bei s = 1, H = fov >> 7 wie
  camera_common.c, vz = Kamera-Tiefe des Objektursprungs; Kamera-Saetze der RE2-RDT @0x64 + 32*cut, Aufbau wie RE1.5:
  +2 fov, +4 from, +0x10 to). Platzierungen = die Obj_model_set (0x2D) der beiden Modelle in RE2 Retail (Runde 38 K
  2.7/9.3), Ursprung pc[14..19]. WARUM das obere Quartil und nicht der Median (Dossier Punkt 2, gemessen 2026-10-18 an der
  exe bei Fensterskala 3): die Pinnwand wird aus EINER fernen Kamera gesehen (vz ~13000, ferner als 11 der 13 RE2-Kameras);
  RE2 laesst den Spieler den Schluessel in den naeheren Kameras finden. Beim Median (Small Key 4,26 px) war der Prison Key
  ein 23-Pixel-Fleck ohne Ring (Sum|dL| 890), beim Quartil (5,78 px) ein lesbarer Schluessel (63 Pixel, 16 breit, Sum|dL|
  3960); das RE2-Maximum (9,00 px, 120 Pixel, 26 breit) waere noch 72 % der Runde-38-Breite - nicht "nicht so riesig".
  Lage, Drehung, Mitte auf dem Sehstrahl der Marke, Zone: unveraendert (Runde 38). Der Massstab wird so gesetzt, dass die
  GEZEICHNETE Huelle (wie Port/Riegel) im Nutzer-Cut diese Bildlaenge hat (Nachiteration, da die Projektion nicht exakt
  linear in s ist; Toleranz 0,06 px wegen des Abtastrasters).

RUNDE 39 SPUR M "schluessel2" (Dossier analysis/befunde_runde39/M_schluessel2.md) — NUTZER-VORGABE (AUFTRAG.md Runde 39,
Nachtrag 5, 2026-10-18 19:29, woertlich): "Und die Schlüssel im Communicationsroom sind immer noch zu groß, die sollten
maximal halb so groß sein."
  ZAHL = NUTZER-VORGABE, KEIN RE-Wert: NUTZER_FAKTOR_M = 0,5 (linear) auf den Stand Spur L (RE2-Quartil-Groesse, oben).
  FORM: die GEZEICHNETE Huelle (Raster wie Port/Riegel) im Nutzer-Cut ist in Breite UND Hoehe hoechstens 0,5 x die
  gezeichnete Huelle des Spur-L-Massstabs (Flaeche ~1/4); gesucht wird das GROESSTE Bild, das die Grenzen einhaelt
  (Bisektion; zweite Grenze und Wahl im Intervall unter GTE-RASTER) — "maximal halb so gross", nicht kleiner als noetig:
  der Schluessel bleibt an der Pinnwand sichtbar und aufnehmbar. Die Bildgroesse ist linear im Massstab: s steckt nur in den Ecken des MD1 (@0x44), die GTE projiziert
  SX2 = OFX + IR1 * H/SZ3 (psx-spx geometrytransformationenginegte.md:418, RTPS), H = fov >> 7 (RE1.5 @0x80021e68
  `lhu a0,2(v0)` Kamera-Satz +2, @0x80021e70 `srl a0,a0,7` im Verzoegerungsplatz von `jal 0x80066c30` = SetGeomScreen
  `gte_ldH a0` @0x80066c30; ebenso @0x80046120..28), und die Pinnwand liegt im Nutzer-Cut fast auf einer Tiefe (vz
  ~13000) — den Rest rechnet die Bisektion am gezeichneten Netz genau. Lage (Mitte auf dem Sehstrahl der Nutzer-Marke;
  skaliert wird um die Mitte des sichtbaren Rechtecks), Drehung, Licht und Aufhebe-Zone bleiben unveraendert.
  GTE-RASTER (gemessen 2026-10-18, Dossier "Zwischenstand 1"): so kleine Objekte zeichnet das Original mit GANZZAHLIGEN
  Bildschirm-Ecken — der Objekt-Zeichner FUN_80022f0c rechnet RTPT (@0x80022fb0 `4a280030`) und schreibt SXY0..2
  unmittelbar ins GPU-Polygon (@0x80022fd8..e0 `swc2` nach +8/+0x14/+0x20), der Port ebenso (objpoly_r38.c RTPS mit
  UNR-Division; Messschiene RE15_TRILOG zeigt die Ecken). Mit nur der stetigen Regel wurde das Viereck des Parking Lot
  Key 2 x 2 statt 6 x 3 Pixel (Hoehe 2/3 statt 1/2; bei 960x720 4 x 6 statt <= 7 x 4). Deshalb gilt ZUSAETZLICH: das
  GTE-Viereck (Ausdehnung der ganzzahligen SXY aller MD1-Ecken, hier Ganzzahl fuer Ganzzahl wie der Port nachgerechnet)
  ist in Breite UND Hoehe hoechstens NUTZER_FAKTOR_M x das GTE-Viereck des Spur-L-Massstabs. Jeder Massstab, der
  dieselben ganzzahligen Ecken liefert, zeichnet dasselbe Bild; genommen wird die MITTE dieses Intervalls (groesster
  Abstand zu den Sprungstellen des Rasters), nicht sein Rand.

Aufruf: python re15_port/tools/r38_itemorte/itemorte_export.py [--schreiben]
"""
import hashlib
import math
import os
import re
import struct
import sys

import numpy as np
from PIL import Image

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
RE2 = os.path.join(REPO, "info", "re2leon")
NUTZER = os.path.join(REPO, "analysis", "befunde_runde38", "nutzer")
FL37_INC = os.path.join(REPO, "re15_port", "engine", "src", "gen", "feuerloescher_r37.inc")
ZIEL = os.path.join(REPO, "re15_port", "engine", "src", "gen", "itemorte_r38_lage.inc")      # nur #define
ZIEL_MD1 = os.path.join(REPO, "re15_port", "engine", "src", "gen", "itemorte_r38_md1.inc")  # die zwei MD1

# ---- Kameras: `probe_r34n_e_dokumente kamera <raum>` VIEW-Zeilen (R Q12 zeilenweise, T, H) -----------------------
CAMS = {
    (0x1030, 0): ((-3810, 0, -1515, -102, 4084, 255, 1510, 275, -3799), (-15554, 3393, -4125), 208),
    (0x1030, 1): ((3508, 0, -2147, -180, 4078, -294, 2137, 343, 3492), (-334, 863, 26824), 208),
    (0x1030, 2): ((-1520, 0, -3823, -718, 4027, 285, 3758, 769, -1495), (-13985, 2547, 3264), 208),
    (0x1030, 3): ((-3946, 0, 1099, 89, 4096, 320, -1099, 333, -3946), (4697, 3905, -9501), 208),
    (0x10F0, 5): ((-4082, 0, 353, 88, 3992, 1025, -345, 1029, -3979), (303, 847, 10437), 208),
    (0x10F0, 6): ((-3805, 0, 1553, 536, 3833, 1314, -1454, 1415, -3561), (-539, 1151, 7613), 208),
    (0x10F0, 7): ((-3738, 0, -1681, -538, 3879, 1195, 1591, 1310, -3540), (-3804, 945, 9768), 208),
}
MARKENFARBE = (237, 28, 36)

# ---- ROOM1030: Pfeiler (Dossier 2.2) -----------------------------------------------------------------------------
PFEILER_XA_ZB = (-2668, -17564)      # (-x,+z)-Ecke: Cut 3 Spalte 228,6 + Cut 1 Spalte 296,6
PFEILER_XA_ZA = (-2709, -18459)      # (-x,-z)-Ecke: Cut 0 Spalte 49,4 + Cut 1 Spalten 318,3/317,5/314,6/313,8
FLAECHE_X = int(round((PFEILER_XA_ZB[0] + PFEILER_XA_ZA[0]) / 2.0))   # -x-Flaeche, achsparallel (Mittel der Ecken)
WAND_ABSTAND_ZUSATZ = 15             # Runde 37 Dossier 3.2: "Prop z -23775 (R + 15 vor der Wand)"
FL_ROT_Y = 3072                      # Modell -z (Wandseite) -> Welt +x (in den Pfeiler), Etikett +z -> Welt -x (Raum)
# Aufhebe-Rechteck: Klemmgrenze des Spielers vor den Tischen x = -4100 (probe_r34n_e_dokumente stand 1030
# -6000 -1000 -19000 -16800 100: begehbar x[..-4100] fuer z -19000..-16800), Pruefpunkt 620 voraus (@0x80042bd0
# `ori v0,zero,0x26c`) -> Blick +x landet bei x -3480; Rechteck 1000 x 1000 (r35-Muster, 69 von 162 Item_aot_set)
# mit dieser Mitte. z: um die Loescher-Mitte, aber nicht in Slot 13 "Desks..." (z bis -18372, ROOM1030 @0x01D92;
# der kleinere Slot gewaenne dort, `j 0x80043028` @0x80042f94).
KLEMME_X = -4100
VORAUS = 620
SLOT13_Z1 = -18372

# ---- ROOM10F0: Pinnwand (Dossier 2.3) ---------------------------------------------------------------------------
WAND_Z = -3400                       # SCA-Zelle 0 (+z-Flaeche), Zettel trianguliert -3481/-3352
ZONE_VOR_WAND = 300                  # Runde 36 N (karten_export.py): "Aufhebe-Rechteck 1000 x 1000 um Mitte + 300*n"
SLOT12_X0 = 1800                     # ROOM10F0 Slot 12 (30 Patronen, Item_aot_set @0x01028) beginnt bei x 1800: das
                                     # Rechteck endet davor (sonst naehme der kleinere Slot 8 dort die Patronen-Zone)

# ---- RE2-Schluessel (wie re15_port/tools/r36_schluessel/schluessel_export.py, md5-geprueft) -----------------------
SCHLUESSEL = {
    "park":   dict(item=0x45, rdt="PL0/RDT/ROOM6150.RDT", md1=(0x004FBC, 236, "3c9176ee9bbb7e91b7c9433d6fe5c371"),
                   tim=(0x030B74, 34848, "0d43d30f076929dbc396075df3e051f6")),
    # Prison Key = RE2 0x1F "Small Key" (Nachbesserung 1, Dossier 9.3; vorher 0x58 Cabin Key ROOM1050 @0x001558)
    "prison": dict(item=0x42, rdt="PL0/RDT/ROOM10F0.RDT", md1=(0x001650, 236, "c5d49ef60b6b111ecac53a19652f3278"),
                   tim=(0x0215BC, 34848, "326b822bb81b07ecb6d3544ce6e094a6")),
}
VERT_OFF = 0x44                      # MD1: Ecken ab Datei 12 + t_vertex_offset 56 (beide Schluessel)

# ---- RUNDE 39 SPUR L: RE2-Platzierungen der beiden Modelle (Obj_model_set 0x2D, Attribut 0x0010 = flach) -----------
# Belege (Bytes selbst gelesen, info/re2leon): ROOM6150 @0x2C98 `2d 08 ... 10 00 ba f1 b0 f8 b6 bc` (P. Room Key,
# Ursprung (-3654,-1872,-17226)); Small Key: ROOM10F0 @0x0706 `2d 04 ... 10 00 25 b3 59 fd 23 a0` ((-19675,-679,-24541)),
# ROOM20E0 @0x18F8 ((-22150,-1600,-22050)), ROOM20E0 @0x1A96 ((-21100,-1300,-13600)), ROOM2020 @0x1C1E
# ((-9350,-1400,-19050)).
RE2_PLATZIERUNGEN = {
    "park":   [("PL0/RDT/ROOM6150.RDT", 0x2C98)],
    "prison": [("PL0/RDT/ROOM10F0.RDT", 0x0706), ("PL0/RDT/ROOM20E0.RDT", 0x18F8),
               ("PL0/RDT/ROOM20E0.RDT", 0x1A96), ("PL0/RDT/ROOM2020.RDT", 0x1C1E)],
}


def re2_kamera_view(fx, fy, fz, tx, ty, tz, fov):
    """re15_camera_build_view (camera_common.c, FUN_80053ca4 @0x80053ca4): ganzzahlig; SquareRoot0 hier int(sqrt)."""
    dx, dy, dz = tx - fx, ty - fy, tz - fz
    dist = int(math.sqrt(dx * dx + dy * dy + dz * dz)); horiz = int(math.sqrt(dx * dx + dz * dz))
    sp = int((-dy) * 4096 / dist); cp = int(horiz * 4096 / dist)
    sy = int(dx * 4096 / horiz); cy = int(dz * 4096 / horiz)
    r = [cy, 0, -sy, (sp * sy) >> 12, cp, (sp * cy) >> 12, (cp * sy) >> 12, -sp, (cp * cy) >> 12]
    tr = [(r[0] * -fx + r[1] * -fy + r[2] * -fz) >> 12, (r[3] * -fx + r[4] * -fy + r[5] * -fz) >> 12,
          (r[6] * -fx + r[7] * -fy + r[8] * -fz) >> 12]
    return r, tr, fov >> 7


def re2_bildgroessen(name, l_nat):
    """[(rdt, obj_off, cut, vz, px)] fuer jede RE2-Kamera, die den Schluessel im Bild hat; px = l_nat * H / vz."""
    out = []
    for rdt, off in RE2_PLATZIERUNGEN[name]:
        b = open(os.path.join(RE2, rdt), "rb").read()
        assert b[off] == 0x2D, "kein Obj_model_set @0x%X" % off
        x, y, z = struct.unpack_from("<3h", b, off + 14)
        n = b[1]; cam = struct.unpack_from("<I", b, 0x24)[0]
        for ci in range(n):
            fl, fov, fx, fy, fz, tx, ty, tz, pri = struct.unpack_from("<HHiiiiiiI", b, cam + 32 * ci)
            r, tr, H = re2_kamera_view(fx, fy, fz, tx, ty, tz, fov)
            vx = (r[0] * x + r[1] * y + r[2] * z) / 4096.0 + tr[0]
            vy = (r[3] * x + r[4] * y + r[5] * z) / 4096.0 + tr[1]
            vz = (r[6] * x + r[7] * y + r[8] * z) / 4096.0 + tr[2]
            if vz <= 0:
                continue
            sx = 160.0 + H * vx / vz; sy_ = 120.0 + H * vy / vz
            if not (0 <= sx < 320 and 0 <= sy_ < 240):
                continue
            out.append((rdt, off, ci, vz, l_nat * H / vz))
    return out


def median(w):
    w = sorted(w); n = len(w)
    return w[n // 2] if n % 2 else (w[n // 2 - 1] + w[n // 2]) / 2.0


RE2_QUANTIL = 0.75                   # oberes Quartil (Kopf des Skripts; Riegel unit_r39_optik_schluessel rechnet es nach)
# RUNDE 39 SPUR M: NUTZER-VORGABE Nachtrag 5 ("maximal halb so groß"), 0,5 x RE2-Quartil-Groesse (Stand Spur L), linear in
# Breite und Hoehe der gezeichneten Huelle. Ausdrueckliche Nutzer-Zahl, KEIN RE-Wert (Kopf des Skripts; Riegel
# unit_r39_schluessel2_halb misst es am gelieferten Netz mit der Engine-Kamera nach).
NUTZER_FAKTOR_M = 0.5


def quantil(w, q):
    """Perzentil mit linearer Interpolation (wie numpy.percentile Standard): pos = (n-1)*q."""
    w = sorted(w); n = len(w)
    pos = (n - 1) * q; lo = int(math.floor(pos)); hi = min(lo + 1, n - 1)
    return w[lo] + (w[hi] - w[lo]) * (pos - lo)


def marke(datei):
    a = np.array(Image.open(os.path.join(NUTZER, datei)).convert("RGB")).astype(int)
    m = (a[:, :, 0] == MARKENFARBE[0]) & (a[:, :, 1] == MARKENFARBE[1]) & (a[:, :, 2] == MARKENFARBE[2])
    ys, xs = np.nonzero(m)
    x0, x1, y0, y1 = int(xs.min()), int(xs.max()), int(ys.min()), int(ys.max())
    assert len(xs) == (x1 - x0 + 1) * (y1 - y0 + 1), "Marke ist kein volles Rechteck"
    return x0, y0, x1, y1


def kamera(raum, cut):
    R, T, H = CAMS[(raum, cut)]
    return np.array(R, float).reshape(3, 3) / 4096.0, np.array(T, float), float(H)


def projiziere(raum, cut, p):
    R, T, H = kamera(raum, cut)
    v = R @ np.asarray(p, float) + T
    return 160.0 + H * v[0] / v[2], 120.0 + H * v[1] / v[2], v[2]


def strahl(raum, cut, sx, sy):
    R, T, H = kamera(raum, cut)
    Ri = np.linalg.inv(R)
    return Ri @ (-T), Ri @ np.array([(sx - 160.0) / H, (sy - 120.0) / H, 1.0])


def auf_ebene(raum, cut, sx, sy, n, c):
    o, d = strahl(raum, cut, sx, sy)
    return o + (c - n @ o) / (n @ d) * d


def euler(m):
    """Zerlegung nach pc_prop_rot_q12 (platform/pc/main.c, Ry*Rx*Rz) — wie karten_export.py."""
    sy = max(-1.0, min(1.0, m[0, 2]))
    ry = math.asin(sy)
    if abs(math.cos(ry)) > 1e-9:
        rx = math.atan2(-m[1, 2], m[2, 2])
        rz = math.atan2(-m[0, 1], m[0, 0])
    else:
        rx = 0.0
        rz = math.atan2(m[1, 0], m[1, 1])
    return rx, ry, rz


def wieder(rx, ry, rz):
    sx, cx, sy, cy, sz, cz = math.sin(rx), math.cos(rx), math.sin(ry), math.cos(ry), math.sin(rz), math.cos(rz)
    return np.array([[cz * cy, -sz * cy, sy],
                     [sz * cx + cz * sy * sx, cz * cx - sz * sy * sx, -cy * sx],
                     [sz * sx - cz * sy * cx, cz * sx + sz * sy * cx, cy * cx]])


def q12(a):
    return int(round(a / (2 * math.pi) * 4096)) & 0xFFF


def q12_matrix(rx, ry, rz):
    f = lambda a: a / 4096.0 * 2 * math.pi
    return wieder(f(rx), f(ry), f(rz))


# ---- MD1 / TIM lesen ----------------------------------------------------------------------------------------------
def md1_lesen(b):
    n_obj = struct.unpack_from("<I", b, 8)[0]
    assert n_obj == 2, "ein Netz erwartet"
    h = struct.unpack_from("<14I", b, 12)
    tvo, tvc, tno, tnc, tfo, tfc, tuo, qvo, qvc, qno, qnc, qfo, qfc, quo = h
    assert tvo == qvo == VERT_OFF - 12, "Ecken nicht bei @0x44"
    V = [struct.unpack_from("<4h", b, 12 + tvo + 8 * i)[:3] for i in range(tvc)]
    faces = []
    for i in range(tfc):
        f = struct.unpack_from("<6H", b, 12 + tfo + 12 * i)
        uv = struct.unpack_from("<BBHBBHBB", b, 12 + tuo + 12 * i)
        faces.append(([f[1], f[3], f[5]], [(uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7])], uv[2]))
    for i in range(qfc):
        f = struct.unpack_from("<8H", b, 12 + qfo + 16 * i)
        uv = struct.unpack_from("<BBHBBHBBHBB", b, 12 + quo + 16 * i)
        faces.append(([f[1], f[3], f[5], f[7]], [(uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7]), (uv[9], uv[10])], uv[2]))
    return V, faces, tvc


def tim_texel_sichtbar(tb, clut_feld):
    """Sichtbarkeit je Texel (8 bpp): CLUT-Farbe != 0x0000 (PSX: 0x0000 = durchsichtig)."""
    magic, flags = struct.unpack_from("<II", tb, 0)
    assert flags & 8 and (flags & 3) == 1, "8bpp mit CLUT erwartet"
    ln, cx, cy, cw, ch = struct.unpack_from("<IHHHH", tb, 8)
    clut = np.frombuffer(tb[8 + 12:8 + 12 + cw * ch * 2], dtype="<u2").reshape(ch, cw)
    o = 8 + ln
    _, px, py, pw, ph = struct.unpack_from("<IHHHH", tb, o)
    idx = np.frombuffer(tb[o + 12:o + 12 + pw * ph * 2], dtype=np.uint8).reshape(ph, pw * 2)
    zeile = (clut_feld >> 6) - cy
    assert 0 <= zeile < ch and (clut_feld & 0x3F) * 16 == cx, "CLUT-Feld passt nicht zum TIM"
    return clut[zeile][idx] != 0


def affin_uv_nach_modell(faces, V):
    """(u,v) -> Modell (x,z) als affine Abbildung aus allen Ecken (kleinste Quadrate, Rest geprueft)."""
    A, B = [], []
    for vi, uvs, _c in faces:
        for i, (u, v) in zip(vi, uvs):
            A.append([u, v, 1.0]); B.append([V[i][0], V[i][2]])
    A = np.array(A); B = np.array(B)
    X, res, *_ = np.linalg.lstsq(A, B, rcond=None)
    rest = np.abs(A @ X - B).max()
    assert rest < 1.0, "UV-Abbildung nicht affin (Rest %.2f)" % rest
    return X       # [u v 1] @ X = [x z]


# ---- RUNDE 39 SPUR M: GTE-Raster wie der Port-Prop-Zeichner (ganzzahlig, Ecke fuer Ecke) -------------------------------
# UNR-Tabelle wie re15_math.c re15_unr_table (psx-spx GTE Division: unr[i] = max(0,(0x40000/(i+0x100)+1)/2-0x101)).
UNR_TAB = [max(0, (0x40000 // (i + 0x100) + 1) // 2 - 0x101) for i in range(257)]
# re15_trig_lut bei Vielfachen von 1024 (selbst ausgelesen: 0 -> (0,4096), 1024 -> (4096,0), 2048 -> (0,-4096),
# 3072 -> (-4096,0)); andere Winkel braeuchten die ganze Tabelle.
LUT_SIN = {0: 0, 1024: 4096, 2048: 0, 3072: -4096}
LUT_COS = {0: 4096, 1024: 0, 2048: -4096, 3072: 0}


def gte_divide(h, sz3):
    """re15_gte_divide (re15_math.c): GTE-UNR-Division (psx-spx), gesaettigt auf 0x1FFFF."""
    if sz3 == 0 or h >= sz3 * 2:
        return 0x1FFFF
    z = 0; d = sz3
    while d < 0x8000:
        d <<= 1; z += 1
    n = h << z
    u = UNR_TAB[(d - 0x7FC0) >> 7] + 0x101
    d = ((0x2000080 - d * u) & 0xFFFFFFFF) >> 8
    d = ((0x0000080 + d * u) & 0xFFFFFFFF) >> 8
    return min((n * d + 0x8000) >> 16, 0x1FFFF)


def rot_q12_prop(rx, ry, rz):
    """pc_prop_rot_q12 (platform/pc/main.c, Ry*Rx*Rz, Ganzzahl) fuer Winkel aus Vielfachen von 1024."""
    for a in (rx, ry, rz):
        assert a in LUT_SIN, "GTE-Nachrechnung nur fuer Vielfache von 1024 (Winkel %d)" % a
    sx, cx, sy, cy, sz, cz = LUT_SIN[rx], LUT_COS[rx], LUT_SIN[ry], LUT_COS[ry], LUT_SIN[rz], LUT_COS[rz]
    q = lambda a, b: (a * b) >> 12
    return [q(cz, cy), -q(sz, cy), sy,
            ((sz * cx << 12) + cz * sy * sx) >> 24, ((cz * cx << 12) - sz * sy * sx) >> 24, -q(cy, sx),
            ((sz * sx << 12) - cz * sy * cx) >> 24, ((cz * sx << 12) + sz * sy * cx) >> 24, q(cy, cx)]


def gte_ecken(raum, cut, pos, rot, ecken):
    """Ganzzahlige Bildschirm-Ecken (SX, SY) der MD1-Ecken wie der Port-Prop-Zeichner: re15_camera_compose_view_bone
    (Q12-Produkt, ein >> 12) -> objpoly_r38.c sicht() -> re15_objpoly_r38_rtps (IR gesaettigt, SZ3 0..FFFFh,
    n = UNR-Division, SX = 160 + IR1*n >> 16). Original: FUN_80022f0c RTPT @0x80022fb0, SXY ins Polygon @0x80022fd8..e0."""
    R, T, H = CAMS[(raum, cut)]
    br = rot_q12_prop(*rot)
    bm = [sum(R[r * 3 + k] * br[k * 3 + c] for k in range(3)) >> 12 for r in range(3) for c in range(3)]
    bt = [(sum(R[r * 3 + k] * pos[k] for k in range(3)) >> 12) + T[r] for r in range(3)]
    sat = lambda v, lo, hi: lo if v < lo else (hi if v > hi else v)
    out = []
    for x, y, z in ecken:
        mac = [((x * bm[r * 3] + y * bm[r * 3 + 1] + z * bm[r * 3 + 2]) >> 12) + bt[r] for r in range(3)]
        ir1, ir2 = sat(mac[0], -0x8000, 0x7FFF), sat(mac[1], -0x8000, 0x7FFF)
        n = gte_divide(H, sat(mac[2], 0, 0xFFFF))
        out.append((sat(160 + ((ir1 * n) >> 16), -0x400, 0x3FF), sat(120 + ((ir2 * n) >> 16), -0x400, 0x3FF)))
    return out


def viereck(ge):
    xs = [p[0] for p in ge]; ys = [p[1] for p in ge]
    return min(xs), max(xs), min(ys), max(ys)


def schluessel_rechnen(name, raum, cut, mk):
    q = SCHLUESSEL[name]
    rdt = open(os.path.join(RE2, q["rdt"]), "rb").read()
    off, n, md5 = q["md1"]; md1 = rdt[off:off + n]
    assert hashlib.md5(md1).hexdigest() == md5, "MD1 md5 " + name
    off, n, md5 = q["tim"]; tim = rdt[off:off + n]
    assert hashlib.md5(tim).hexdigest() == md5, "TIM md5 " + name
    V, faces, nv = md1_lesen(md1)
    assert all(abs(v[1]) == 0 for v in V), "Schluessel-Netz ist nicht flach (y != 0)"
    clut_feld = faces[0][2]
    sicht = tim_texel_sichtbar(tim, clut_feld)
    X = affin_uv_nach_modell(faces, V)
    # u-Richtung im Modell (x,z): d(x,z)/du = X[0]; v-Richtung = X[1]
    du = np.array([X[0][0], 0.0, X[0][1]]); dv = np.array([X[1][0], 0.0, X[1][1]])
    # Sichtseite: die Seite (+y / -y), von der aus die Textur NICHT gespiegelt erscheint. Blick von -y (oben):
    # Bild-rechts = +x, Bild-unten = -z; von +y (unten): Bild-rechts = +x, Bild-unten = +z. Nicht gespiegelt heisst
    # det [[du/dsx, du/dsy],[dv/dsx, dv/dsy]] > 0 (Textur-v waechst nach Bild-unten).
    J = np.array([[X[0][0], X[0][1]], [X[1][0], X[1][1]]])        # (x,z) je (u,v)
    Jinv = np.linalg.inv(J.T)                                      # (u,v) je (x,z)
    det_oben = np.linalg.det(Jinv @ np.array([[1.0, 0.0], [0.0, -1.0]]))
    seite_y = -1.0 if det_oben > 0 else 1.0                        # Modell-Achse, die zum Raum zeigt
    # Wicklung der Flaechen von dieser Seite (Kreuzprodukt wie IS_BACKFACE, platform/pc/main.c)
    wick = []
    for vi, _uv, _c in faces:
        p = [(V[i][0], -V[i][2] if seite_y < 0 else V[i][2]) for i in vi[:3]]
        wick.append((p[1][0] - p[0][0]) * (p[2][1] - p[0][1]) - (p[1][1] - p[0][1]) * (p[2][0] - p[0][0]))
    # Drehung: u -> Bild-rechts (Welt -x vor der Wand z = -3400, Blick -z), Sichtseite -> Welt +z (zum Raum)
    eu = du / np.linalg.norm(du)
    ey = np.array([0.0, seite_y, 0.0])
    ew = np.cross(eu, ey)
    Bm = np.column_stack([eu, ey, ew])
    A_w = np.column_stack([np.array([-1.0, 0, 0]), np.array([0.0, 0, 1.0]), np.cross([-1.0, 0, 0], [0.0, 0, 1.0])])
    M = A_w @ Bm.T
    assert abs(np.linalg.det(M) - 1.0) < 1e-9
    rx, ry, rz = euler(M)
    RX, RY, RZ = q12(rx), q12(ry), q12(rz)
    Mq = q12_matrix(RX, RY, RZ)
    # sichtbare Texel -> Modell (x,z), Ecken je Texel; Huelle in Modellkoordinaten
    us, vs = [], []
    for vi, uvs, _c in faces:
        u0 = min(u for u, v in uvs); u1 = max(u for u, v in uvs); v0 = min(v for u, v in uvs); v1 = max(v for u, v in uvs)
        for vv in range(v0, v1 + 1):
            for uu in range(u0, u1 + 1):
                if vv < sicht.shape[0] and uu < sicht.shape[1] and sicht[vv, uu]:
                    us.append(uu); vs.append(vv)
    pts = set(zip(us, vs))
    ecken = []
    for (uu, vv) in pts:
        for du_, dv_ in ((0, 0), (1, 0), (0, 1), (1, 1)):
            x, z = np.array([uu + du_, vv + dv_, 1.0]) @ X
            ecken.append((x, z))
    ecken = np.array(ecken)
    # nur Texel INNERHALB der Flaechen zaehlen (der UV-Bereich der Flaechen ist das Rechteck/Parallelogramm)
    xs, zs = ecken[:, 0], ecken[:, 1]
    vx = [v[0] for v in V]; vz = [v[2] for v in V]
    innen = (xs >= min(vx) - 1e-6) & (xs <= max(vx) + 1e-6) & (zs >= min(vz) - 1e-6) & (zs <= max(vz) + 1e-6)
    ecken = ecken[innen]
    mx = (ecken[:, 0].min() + ecken[:, 0].max()) / 2.0
    mz = (ecken[:, 1].min() + ecken[:, 1].max()) / 2.0
    x0, y0, x1, y1 = mk
    n = np.array([0.0, 0.0, 1.0]); c = float(WAND_Z)
    mitte = auf_ebene(raum, cut, (x0 + x1 + 1) / 2.0, (y0 + y1 + 1) / 2.0, n, c)

    def welt(s, ex, ez):
        return mitte + Mq @ np.array([(ex - mx) * s, 0.0, (ez - mz) * s])

    def passt(s):
        for ex, ez in ecken:
            sx, sy, _ = projiziere(raum, cut, welt(s, ex, ez))
            if sx < x0 - 0.5 or sx > x1 + 1.5 or sy < y0 - 0.5 or sy > y1 + 1.5:
                return False
        return True
    lo, hi = 0.01, 20.0
    for _ in range(60):
        mi = (lo + hi) / 2
        if passt(mi):
            lo = mi
        else:
            hi = mi
    s = lo
    pos = np.round(mitte).astype(int)

    def md1_skaliert(s):
        # skaliertes + verschobenes MD1 (nur die Ecken @0x44)
        sk = bytearray(md1)
        neu = []
        for i, v in enumerate(V):
            nx = int(round((v[0] - mx) * s)); nz = int(round((v[2] - mz) * s))
            assert -32768 <= nx <= 32767 and -32768 <= nz <= 32767
            struct.pack_into("<3h", sk, VERT_OFF + 8 * i, nx, 0, nz)
            neu.append((nx, 0, nz))
        return sk, neu
    # Nachbesserung 1 (Runde 38 K, Dossier 9.5): die affine UV->Modell-Abbildung X ist ein Ausgleich ueber ALLE Flaechen
    # (Rest bis 1 Modell-Einheit = bis ~0,09 px); beim Small Key lag die so gerechnete Huelle 0,07 px innerhalb, die
    # GEZEICHNETE ausserhalb der Toleranz (unit_r38_itemorte_lage: x 144,52 > 144,5). Kontrolle deshalb wie Port und Riegel:
    # GERUNDETE Ecken, Dreiecke direkt / Vierecke (0,1,3)+(0,3,2), Texel = (int) des baryzentrisch interpolierten UV, Raster
    # N = 48 (das des Riegels) und N = 480; liegt ein sichtbarer Punkt ausserhalb, wird s in Schritten von 0,05 % kleiner.
    def huelle_gezeichnet(sk):
        Vn, fn, _nv = md1_lesen(bytes(sk))
        R_, T_, H_ = kamera(raum, cut)
        sxs_, sys__ = [], []
        for vi, uvs, _c in fn:
            tris = [(0, 1, 2)] if len(vi) == 3 else [(0, 1, 3), (0, 3, 2)]
            for t in tris:
                P = np.array([Vn[vi[j]] for j in t], float)
                UVt = np.array([uvs[j] for j in t], float)
                for N in (48, 480):
                    a, b = np.meshgrid(np.arange(N + 1), np.arange(N + 1), indexing="ij")
                    m = (a + b) <= N
                    l1 = a[m] / float(N); l2 = b[m] / float(N); l0 = 1.0 - l1 - l2
                    fu = l0 * UVt[0, 0] + l1 * UVt[1, 0] + l2 * UVt[2, 0]
                    fv = l0 * UVt[0, 1] + l1 * UVt[1, 1] + l2 * UVt[2, 1]
                    if N == 48:   # bitgleich mit dem Riegel (C: (int) des double-Ausdrucks in derselben Reihenfolge)
                        tu = fu.astype(int); tv = fv.astype(int)
                    else:         # feines Raster mit exakter Arithmetik: 191,9999999 an der Flaechenkante ist Zeile 192,
                                  # nicht der Nachbar-Ausschnitt des geteilten TIM (sonst Streupunkte ausserhalb)
                        tu = np.floor(fu + 1e-7).astype(int); tv = np.floor(fv + 1e-7).astype(int)
                    ok = (tu >= 0) & (tv >= 0) & (tu < sicht.shape[1]) & (tv < sicht.shape[0])
                    ok[ok] = sicht[tv[ok], tu[ok]]
                    Pm = l0[ok, None] * P[0] + l1[ok, None] * P[1] + l2[ok, None] * P[2]
                    W_ = pos + Pm @ Mq.T
                    v_ = W_ @ R_.T + T_
                    sxs_.append(160.0 + H_ * v_[:, 0] / v_[:, 2]); sys__.append(120.0 + H_ * v_[:, 1] / v_[:, 2])
        sxs_ = np.concatenate(sxs_); sys__ = np.concatenate(sys__)
        return float(sxs_.min()), float(sxs_.max()), float(sys__.min()), float(sys__.max())

    schritte = 0
    while True:
        sk, neu = md1_skaliert(s)
        hg = huelle_gezeichnet(sk)
        if hg[0] >= x0 - 0.5 and hg[1] <= x1 + 1.5 and hg[2] >= y0 - 0.5 and hg[3] <= y1 + 1.5:
            break
        s *= 0.9995
        schritte += 1
        assert schritte < 200, "Huelle passt nicht in die Marke"
    # RUNDE 39 SPUR L: Bildlaenge der gezeichneten Huelle = RE2-Median (Kopf des Skripts). s38/hg38 = Runde-38-Stand.
    s38, hg38 = s, hg
    breite_nat = float(ecken[:, 0].max() - ecken[:, 0].min())       # sichtbare Laenge bei s = 1
    re2 = re2_bildgroessen(name, breite_nat)
    assert len(re2) >= 1, "keine RE2-Kamera zeigt den Schluessel"
    ziel = quantil([p for *_r, p in re2], RE2_QUANTIL)
    ziel_median = median([p for *_r, p in re2])
    # Messhilfe (nur fuer Vergleichslaeufe, nie fuer die erzeugten Dateien): --ziel-<name>=<px> ersetzt den RE2-Median.
    for a in sys.argv[1:]:
        if a.startswith("--ziel-%s=" % name):
            ziel = float(a.split("=", 1)[1]); print("           ACHTUNG Messhilfe: Ziel %s = %.2f px (statt RE2-Median)" % (name, ziel))
    s = s38 * ziel / (hg38[1] - hg38[0])
    best = None
    for _ in range(12):
        sk, neu = md1_skaliert(s)
        hg = huelle_gezeichnet(sk)
        abw = abs((hg[1] - hg[0]) - ziel)
        if best is None or abw < best[0]:
            best = (abw, s, sk, neu, hg)
        if abw < 0.01:
            break
        s *= ziel / (hg[1] - hg[0])
    _abw, s, sk, neu, hg = best
    assert _abw < 0.06, "Huelle nicht am Ziel (%.3f px daneben)" % _abw
    assert hg[0] >= x0 - 0.5 and hg[1] <= x1 + 1.5 and hg[2] >= y0 - 0.5 and hg[3] <= y1 + 1.5, "Huelle nicht in der Marke"
    # RUNDE 39 SPUR M (NUTZER-VORGABE Nachtrag 5 "maximal halb so gross", Kopf des Skripts): Basis = Spur-L-Massstab
    # (s_l, RE2-Quartil-Groesse). Zwei Grenzen, beide in Breite UND Hoehe, beide hoechstens NUTZER_FAKTOR_M x Spur L:
    #   (1) stetig: die gezeichnete Huelle (Raster 48/480 wie Port/Riegel);
    #   (2) GTE-Raster: das Viereck der ganzzahligen Bildschirm-Ecken (gte_ecken, wie Original und Port zeichnen).
    # s_oben = groesster Massstab, der beide haelt (Bisektion, 40 Schritte, zwischen 0,1 x s_l und (Faktor + 0,1) x s_l);
    # s_unten = kleinster Massstab mit DENSELBEN ganzzahligen Ecken wie bei s_oben (Bisektion); geliefert wird die Mitte
    # (dasselbe Bild wie s_oben, groesster Abstand zu den Sprungstellen des Rasters).
    s_l, hg_l, neu_l = s, hg, neu
    pos_i = [int(v) for v in pos]
    rot_i = (RX, RY, RZ)
    grenze_b = NUTZER_FAKTOR_M * (hg_l[1] - hg_l[0])
    grenze_h = NUTZER_FAKTOR_M * (hg_l[3] - hg_l[2])
    ge_l = gte_ecken(raum, cut, pos_i, rot_i, neu_l)
    gv_l = viereck(ge_l)
    grenze_gb = NUTZER_FAKTOR_M * (gv_l[1] - gv_l[0])
    grenze_gh = NUTZER_FAKTOR_M * (gv_l[3] - gv_l[2])

    def stand(s_):
        sk_, neu_ = md1_skaliert(s_)
        hg_ = huelle_gezeichnet(sk_)
        ge_ = gte_ecken(raum, cut, pos_i, rot_i, neu_)
        gv_ = viereck(ge_)
        stetig = hg_[1] - hg_[0] <= grenze_b and hg_[3] - hg_[2] <= grenze_h
        raster = gv_[1] - gv_[0] <= grenze_gb and gv_[3] - gv_[2] <= grenze_gh
        return dict(ok=stetig and raster, stetig=stetig, raster=raster, sk=sk_, neu=neu_, hg=hg_, ge=ge_, gv=gv_)
    lo, hi = s_l * 0.1, s_l * (NUTZER_FAKTOR_M + 0.1)
    assert stand(lo)["ok"] and not stand(hi)["ok"], "Spur M: Bisektionsgrenzen falsch"
    for _ in range(40):
        mi = (lo + hi) / 2
        if stand(mi)["ok"]:
            lo = mi
        else:
            hi = mi
    s_oben, st_oben, st_drueber = lo, stand(lo), stand(hi)
    bindet = "+".join(n_ for n_, f_ in (("stetig", st_drueber["stetig"]), ("GTE-Raster", st_drueber["raster"])) if not f_)
    lo2, hi2 = s_l * 0.1, s_oben
    assert stand(lo2)["ge"] != st_oben["ge"], "Spur M: untere Intervallgrenze nicht eingeschlossen"
    for _ in range(40):
        mi = (lo2 + hi2) / 2
        if stand(mi)["ge"] == st_oben["ge"]:
            hi2 = mi
        else:
            lo2 = mi
    s_unten = hi2
    s = (s_unten + s_oben) / 2
    st = stand(s)
    assert st["ok"] and st["ge"] == st_oben["ge"], "Spur M: Intervallmitte zeichnet anders"
    sk, neu, hg = st["sk"], st["neu"], st["hg"]
    assert hg[0] >= x0 - 0.5 and hg[1] <= x1 + 1.5 and hg[2] >= y0 - 0.5 and hg[3] <= y1 + 1.5, "Huelle nicht in der Marke"
    # Kontrolle mit den GERUNDETEN Werten: sichtbare Huelle im Bild
    sxs, sys_ = [], []
    for ex, ez in ecken:
        w = pos + Mq @ np.array([(ex - mx) * s, 0.0, (ez - mz) * s])
        sx, sy, _ = projiziere(raum, cut, w)
        sxs.append(sx); sys_.append(sy)
    breite = (ecken[:, 0].max() - ecken[:, 0].min()) * s
    tiefe = (ecken[:, 1].max() - ecken[:, 1].min()) * s
    return dict(name=name, item=q["item"], md1=bytes(sk), md1_md5=hashlib.md5(md1).hexdigest(), s=s, pos=pos,
                rot=(RX, RY, RZ), seite_y=seite_y, wick=wick, mitte=mitte, sichtbar=len(pts),
                huelle=(min(sxs), max(sxs), min(sys_), max(sys_)), ecken_neu=neu, breite=breite, tiefe=tiefe,
                det_oben=det_oben, huelle_gezeichnet=hg, schritte=schritte,
                s38=s38, hg38=hg38, breite_nat=breite_nat, re2=re2, ziel=ziel, ziel_median=ziel_median,
                s_l=s_l, hg_l=hg_l, grenze_b=grenze_b, grenze_h=grenze_h, mx=mx, mz=mz,
                ge_l=ge_l, gv_l=gv_l, ge=st["ge"], gv=st["gv"], s_oben=s_oben, s_unten=s_unten, bindet=bindet,
                gv_drueber=st_drueber["gv"], hg_drueber=st_drueber["hg"])


def loescher_rechnen(mk):
    src = open(FL37_INC).read()
    m = re.search(r"k_fl_md1_stehend\[\d*\]\s*=\s*\{(.*?)\};", src, re.S)
    b = bytes(int(x, 16) for x in re.findall(r"0x[0-9a-fA-F]+", m.group(1)))
    h = struct.unpack_from("<14I", b, 12)
    V = [struct.unpack_from("<4h", b, 12 + h[0] + 8 * i)[:3] for i in range(h[1])]
    hoehe = -min(v[1] for v in V)
    radius = max(abs(v[2]) for v in V)
    assert max(v[1] for v in V) == 0, "Ursprung nicht Bodenmitte"
    achse_x = FLAECHE_X - (radius + WAND_ABSTAND_ZUSATZ)
    x0, y0, x1, y1 = mk
    c = auf_ebene(0x1030, 0, (x0 + x1 + 1) / 2.0, (y0 + y1 + 1) / 2.0, np.array([1.0, 0, 0]), float(achse_x))
    urs = np.array([achse_x, c[1] + hoehe / 2.0, c[2]])
    pos = np.round(urs).astype(int)
    fp = KLEMME_X + VORAUS
    rx0 = fp - 500
    rz0 = max(int(round(c[2])) - 500, SLOT13_Z1 + 2)
    # Kontrolle: Netz mit rot_y projizieren -> Huelle im Nutzer-Cut
    M = q12_matrix(0, FL_ROT_Y, 0)
    sxs, sys_ = [], []
    for v in V:
        sx, sy, _ = projiziere(0x1030, 0, pos + M @ np.array(v, float))
        sxs.append(sx); sys_.append(sy)
    return dict(hoehe=hoehe, radius=radius, achse_x=achse_x, mitte=c, pos=pos, rot_y=FL_ROT_Y,
                rect=(rx0, rz0, 1000, 1000), huelle=(min(sxs), max(sxs), min(sys_), max(sys_)), n_ecken=len(V))


def main():
    schreiben = "--schreiben" in sys.argv
    mk_fl = marke("fire_extinguisher.bmp")
    mk_ks = marke("key_location.bmp")
    print("Marke fire_extinguisher.bmp x%d..%d y%d..%d (ROOM1030 Cut 0)" % (mk_fl[0], mk_fl[2], mk_fl[1], mk_fl[3]))
    print("Marke key_location.bmp      x%d..%d y%d..%d (ROOM10F0 Cut 5)" % (mk_ks[0], mk_ks[2], mk_ks[1], mk_ks[3]))
    fl = loescher_rechnen(mk_fl)
    print("LOESCHER: Netz Hoehe %d Radius %d (%d Ecken) | Pfeiler -x-Flaeche x %d -> Achse x %d | Mitte (%.0f,%.0f,%.0f)"
          % (fl["hoehe"], fl["radius"], fl["n_ecken"], FLAECHE_X, fl["achse_x"], *fl["mitte"]))
    print("          Ursprung (Bodenmitte) (%d,%d,%d) rot_y %d | Huelle Cut 0 x %.1f..%.1f y %.1f..%.1f | Rechteck (%d,%d) %dx%d"
          % (*fl["pos"], fl["rot_y"], *fl["huelle"], *fl["rect"]))
    ks = []
    for name in ("park", "prison"):
        k = schluessel_rechnen(name, 0x10F0, 5, mk_ks)
        ks.append(k)
        print("SCHLUESSEL %-6s Item 0x%02X: Sichtseite Modell %+dy (Wicklung %s, det_oben %.3f) | %d sichtbare Texel | s %.4f"
              " -> sichtbar %.0f x %.0f | Ursprung (%d,%d,%d) rot (%d,%d,%d) | Huelle Cut 5 x %.2f..%.2f y %.2f..%.2f"
              % (name, k["item"], int(k["seite_y"]), ["%.0f" % w for w in k["wick"]], k["det_oben"], k["sichtbar"],
                 k["s"], k["breite"], k["tiefe"], *k["pos"], *k["rot"], *k["huelle"]))
        print("           Ecken neu", k["ecken_neu"], "| gezeichnete Huelle (gerundete Ecken, Raster 48/480) x %.2f..%.2f y %.2f..%.2f,"
              " %d Verkleinerungsschritte" % (*k["huelle_gezeichnet"], k["schritte"]))
        print("           Runde 39 L: Runde-38-Massstab %.4f (Huelle %.2f px breit) -> RE2 oberes Quartil %.2f px (Median %.2f) aus"
              " %d Kameras -> s %.4f (sichtbar %.0f x %.0f Einheiten, L_nat %.1f, gezeichnete Huelle %.2f px)"
              % (k["s38"], k["hg38"][1] - k["hg38"][0], k["ziel"], k["ziel_median"], len(k["re2"]),
                 k["s_l"], k["breite"] * k["s_l"] / k["s"], k["tiefe"] * k["s_l"] / k["s"], k["breite_nat"],
                 k["hg_l"][1] - k["hg_l"][0]))
        hl, hm = k["hg_l"], k["huelle_gezeichnet"]
        gl, gm, gd = k["gv_l"], k["gv"], k["gv_drueber"]
        print("           Runde 39 M: NUTZER-VORGABE Faktor %.2f; stetige Grenze %.3f x %.3f px (Spur L %.3f x %.3f), GTE-Viereck"
              " Spur L x %d..%d y %d..%d = %d x %d -> Grenze %.1f x %.1f"
              % (NUTZER_FAKTOR_M, k["grenze_b"], k["grenze_h"], hl[1] - hl[0], hl[3] - hl[2], gl[0], gl[1], gl[2], gl[3],
                 gl[1] - gl[0], gl[3] - gl[2], NUTZER_FAKTOR_M * (gl[1] - gl[0]), NUTZER_FAKTOR_M * (gl[3] - gl[2])))
        print("           -> s_oben %.4f (darueber bindet %s: GTE %d x %d, Huelle %.3f x %.3f), s_unten %.4f -> s %.4f (%.1f %% von"
              " Spur L): GTE-Ecken %s = x %d..%d y %d..%d (%d x %d), gezeichnete Huelle %.3f x %.3f px (%.1f %% / %.1f %%),"
              " sichtbar %.0f x %.0f Einheiten"
              % (k["s_oben"], k["bindet"], gd[1] - gd[0], gd[3] - gd[2], k["hg_drueber"][1] - k["hg_drueber"][0],
                 k["hg_drueber"][3] - k["hg_drueber"][2], k["s_unten"], k["s"], 100.0 * k["s"] / k["s_l"], k["ge"], gm[0], gm[1],
                 gm[2], gm[3], gm[1] - gm[0], gm[3] - gm[2], hm[1] - hm[0], hm[3] - hm[2], 100.0 * (hm[1] - hm[0]) / (hl[1] - hl[0]),
                 100.0 * (hm[3] - hm[2]) / (hl[3] - hl[2]), k["breite"], k["tiefe"]))
        for rdt, off, ci, vz, px in k["re2"]:
            print("             RE2 %s @0x%04X Cut %d: vz %.0f -> %.2f px" % (rdt[8:16], off, ci, vz, px))
    rx1 = min(int(round(ks[0]["pos"][0])) + 500, SLOT12_X0 - 2)
    rect_ks = (rx1 - 1000, WAND_Z + ZONE_VOR_WAND - 500)
    print("SCHLUESSEL-Zone Rechteck (%d,%d) 1000x1000 (Mitte (x_Marke, %d + %d), rechts vor Slot 12 x %d)"
          % (rect_ks[0], rect_ks[1], WAND_Z, ZONE_VOR_WAND, SLOT12_X0))
    if not schreiben:
        return

    def blob(name, b):
        out = ["static const unsigned char %s[%d] = {" % (name, len(b))]
        for i in range(0, len(b), 16):
            out.append("    " + ",".join("0x%02x" % v for v in b[i:i + 16]) + ",")
        out.append("};")
        return "\n".join(out)

    L = ["/* AUTOMATISCH ERZEUGT von re15_port/tools/r38_itemorte/itemorte_export.py --schreiben - nicht von Hand.",
         " * Runde 38 Spur K (Dossier analysis/befunde_runde38/K_itemorte.md). Herleitung jeder Zahl: Kopf des Skripts.",
         " * Nutzer-Marken: fire_extinguisher.bmp x%d..%d y%d..%d (ROOM1030 Cut 0), key_location.bmp x%d..%d y%d..%d"
         % (mk_fl[0], mk_fl[2], mk_fl[1], mk_fl[3], mk_ks[0], mk_ks[2], mk_ks[1], mk_ks[3]),
         " * (ROOM10F0 Cut 5). */",
         "",
         "/* FEUERLOESCHER ROOM1030 (stehendes Netz Runde 37: Hoehe %d, Radius %d): Pfeiler -x-Flaeche x %d, Achse x %d"
         % (fl["hoehe"], fl["radius"], FLAECHE_X, fl["achse_x"]),
         " * (R + %d), Mitte auf dem Sehstrahl der Markenmitte (%.0f,%.0f,%.0f); Huelle im Nutzer-Cut x %.1f..%.1f y %.1f..%.1f."
         % (WAND_ABSTAND_ZUSATZ, fl["mitte"][0], fl["mitte"][1], fl["mitte"][2], *fl["huelle"]),
         " * Aufhebe-Rechteck: Mitte x %d = Klemmgrenze %d + %d voraus, z ab %d (Slot 13 endet bei %d). */"
         % (KLEMME_X + VORAUS, KLEMME_X, VORAUS, fl["rect"][1], SLOT13_Z1),
         "#define RE15_R38K_FL_X        (%d)" % fl["pos"][0],
         "#define RE15_R38K_FL_Y        (%d)" % fl["pos"][1],
         "#define RE15_R38K_FL_Z        (%d)" % fl["pos"][2],
         "#define RE15_R38K_FL_ROT_Y    %d" % fl["rot_y"],
         "#define RE15_R38K_FL_RECT_X   (%d)" % fl["rect"][0],
         "#define RE15_R38K_FL_RECT_Z   (%d)" % fl["rect"][1],
         "#define RE15_R38K_FL_RECT_W   %d" % fl["rect"][2],
         "#define RE15_R38K_FL_RECT_D   %d" % fl["rect"][3],
         ""]
    M1 = ["/* AUTOMATISCH ERZEUGT von re15_port/tools/r38_itemorte/itemorte_export.py --schreiben - nicht von Hand.",
          " * Runde 38 Spur K: die zwei RE2-Schluessel-MD1 fuer die Pinnwand ROOM10F0/10F1 (Lage: itemorte_r38_lage.inc). */",
          ""]
    for k in ks:
        N = k["name"].upper()
        L += ["/* SCHLUESSEL %s (Item 0x%02X, RE2-MD1 md5 %s): s = %.4f -> sichtbar %.0f x %.0f Einheiten;"
              % (N, k["item"], k["md1_md5"], k["s"], k["breite"], k["tiefe"]),
              " * Sichtseite Modell %+dy, Textur-u nach Bild-rechts; Huelle im Nutzer-Cut x %.2f..%.2f y %.2f..%.2f. */"
              % (int(k["seite_y"]), *k["huelle"]),
              "#define RE15_R38K_%s_X        (%d)" % (N, k["pos"][0]),
              "#define RE15_R38K_%s_Y        (%d)" % (N, k["pos"][1]),
              "#define RE15_R38K_%s_Z        (%d)" % (N, k["pos"][2]),
              "#define RE15_R38K_%s_ROT_X    %d" % (N, k["rot"][0]),
              "#define RE15_R38K_%s_ROT_Y    %d" % (N, k["rot"][1]),
              "#define RE15_R38K_%s_ROT_Z    %d" % (N, k["rot"][2]),
              "#define RE15_R38K_%s_S10000   %d   /* Massstab x 10000 (Sonden/Riegel); Runde 39 Spur M: hoechstens 0,5 x Spur L */"
              % (N, int(round(k["s"] * 10000))),
              "/* Runde 39 Spur L (kleiner/subtiler): Bildlaenge der gezeichneten Huelle = oberes Quartil (75. Perzentil, linear"
              " interpoliert) der RE2-Bildlaenge",
              " * L_nat*H/vz desselben Modells an seinen %d RE2-Kameras (%s; Median %.2f);"
              % (len(k["re2"]), ", ".join("%s C%d %.2f" % (r[8:16], c, p) for r, _o, c, _vz, p in k["re2"]), k["ziel_median"]),
              " * Runde-38-Massstab %.4f fuellte die Marke (%.2f px breit). */" % (k["s38"], k["hg38"][1] - k["hg38"][0]),
              "#define RE15_R39L_%s_S38_10000    %d   /* Runde-38-Massstab x 10000 (Marke gefuellt) */" % (N, int(round(k["s38"] * 10000))),
              "#define RE15_R39L_%s_ZIEL_PX100   %d   /* RE2 oberes Quartil der Bildlaenge, Pixel x 100 */" % (N, int(round(k["ziel"] * 100))),
              "#define RE15_R39L_%s_MEDIAN_PX100 %d   /* RE2-Median der Bildlaenge, Pixel x 100 (nur zur Einordnung) */" % (N, int(round(k["ziel_median"] * 100))),
              "#define RE15_R39L_%s_HUELLE_PX100 %d   /* gezeichnete Huelle im Nutzer-Cut beim Spur-L-Massstab, Pixel x 100 */"
              % (N, int(round((k["hg_l"][1] - k["hg_l"][0]) * 100))),
              "#define RE15_R39L_%s_RE2_KAMERAS  %d" % (N, len(k["re2"])),
              "#define RE15_R39L_%s_LNAT100      %d   /* sichtbare Modell-Laenge bei s = 1, Einheiten x 100 */"
              % (N, int(round(k["breite_nat"] * 100))),
              "/* Runde 39 Spur M (NUTZER-VORGABE Nachtrag 5 \"maximal halb so gross\", 0,5 x RE2-Quartil-Groesse = Spur L): das groesste",
              " * Bild, dessen gezeichnete Huelle UND GTE-Viereck in Breite und Hoehe hoechstens RE15_R39M_FAKTOR100/100 x Spur L sind;",
              " * geliefert die Mitte des Massstab-Intervalls gleicher ganzzahliger Ecken (S_UNTEN..S_OBEN). */",
              "#define RE15_R39M_%s_SL_10000          %d   /* Spur-L-Massstab x 10000 (Basis) */" % (N, int(round(k["s_l"] * 10000))),
              "#define RE15_R39M_%s_L_BREITE_PX1000   %d   /* gezeichnete Huelle Spur L, Breite, Pixel x 1000 */"
              % (N, int(round((k["hg_l"][1] - k["hg_l"][0]) * 1000))),
              "#define RE15_R39M_%s_L_HOEHE_PX1000    %d   /* gezeichnete Huelle Spur L, Hoehe, Pixel x 1000 */"
              % (N, int(round((k["hg_l"][3] - k["hg_l"][2]) * 1000))),
              "#define RE15_R39M_%s_BREITE_PX1000     %d   /* gezeichnete Huelle (geliefert), Breite, Pixel x 1000 */"
              % (N, int(round((k["huelle_gezeichnet"][1] - k["huelle_gezeichnet"][0]) * 1000))),
              "#define RE15_R39M_%s_HOEHE_PX1000      %d   /* gezeichnete Huelle (geliefert), Hoehe, Pixel x 1000 */"
              % (N, int(round((k["huelle_gezeichnet"][3] - k["huelle_gezeichnet"][2]) * 1000))),
              "/* GTE-Raster (FUN_80022f0c RTPT @0x80022fb0, ganzzahlige SXY ins Polygon @0x80022fd8): Viereck der Bildschirm-Ecken",
              " * im Nutzer-Cut, Spur L %s, geliefert %s; bindende Grenze darueber: %s. */"
              % (" ".join("(%d,%d)" % p for p in k["ge_l"]), " ".join("(%d,%d)" % p for p in k["ge"]), k["bindet"]),
              "#define RE15_R39M_%s_L_GTE_X0  %d" % (N, k["gv_l"][0]),
              "#define RE15_R39M_%s_L_GTE_Y0  %d" % (N, k["gv_l"][2]),
              "#define RE15_R39M_%s_L_GTE_B   %d   /* Spur L: Breite des GTE-Vierecks, Pixel (320er Raster) */" % (N, k["gv_l"][1] - k["gv_l"][0]),
              "#define RE15_R39M_%s_L_GTE_H   %d   /* Spur L: Hoehe */" % (N, k["gv_l"][3] - k["gv_l"][2]),
              "#define RE15_R39M_%s_GTE_X0    %d" % (N, k["gv"][0]),
              "#define RE15_R39M_%s_GTE_Y0    %d" % (N, k["gv"][2]),
              "#define RE15_R39M_%s_GTE_B     %d   /* geliefert: Breite des GTE-Vierecks */" % (N, k["gv"][1] - k["gv"][0]),
              "#define RE15_R39M_%s_GTE_H     %d   /* geliefert: Hoehe */" % (N, k["gv"][3] - k["gv"][2]),
              "#define RE15_R39M_%s_S_UNTEN_10000  %d   /* Massstab-Intervall gleicher ganzzahliger Ecken, unten (x 10000) */"
              % (N, int(math.floor(k["s_unten"] * 10000))),
              "#define RE15_R39M_%s_S_OBEN_10000   %d   /* ... oben (groesster Massstab, der beide Grenzen haelt) */"
              % (N, int(math.ceil(k["s_oben"] * 10000))),
              "#define RE15_R39M_%s_MX100000   %d   /* Mitte des sichtbaren Rechtecks im RE2-Modell (x), Einheiten x 100000 */"
              % (N, int(round(k["mx"] * 100000))),
              "#define RE15_R39M_%s_MZ100000   %d   /* ... (z) */" % (N, int(round(k["mz"] * 100000))), ""]
        M1 += ["/* SCHLUESSEL %s: RE2-MD1 (md5 %s), nur die Ecken @0x44 skaliert (s = %.4f, Runde 39 Spur M: NUTZER-VORGABE"
               " hoechstens 0,5 x Spur L, s_L %.4f = RE2-Bildgroesse; Runde 38: %.4f) und verschoben. */" % (N, k["md1_md5"], k["s"], k["s_l"], k["s38"]),
               blob("k_r38k_%s_md1" % k["name"], k["md1"]), ""]
    L += ["#define RE15_R39L_RE2_QUANTIL100  %d   /* Perzentil der RE2-Bildlaengen (oberes Quartil), x 100 */" % int(round(RE2_QUANTIL * 100)),
          "#define RE15_R39M_FAKTOR100       %d   /* NUTZER-VORGABE Nachtrag 5 (\"maximal halb so gross\"), 0,5 x Spur L, x 100 - kein RE-Wert */"
          % int(round(NUTZER_FAKTOR_M * 100)), ""]
    L += ["/* Aufhebe-Rechteck der Schluessel (beide Varianten derselbe Ort): 1000 x 1000 um (x_Marke, Wand %d + %d),"
          % (WAND_Z, ZONE_VOR_WAND),
          " * rechts vor Slot 12 (x ab %d) beendet. */" % SLOT12_X0,
          "#define RE15_R38K_KS_RECT_X   (%d)" % rect_ks[0],
          "#define RE15_R38K_KS_RECT_Z   (%d)" % rect_ks[1],
          "#define RE15_R38K_KS_RECT_W   1000",
          "#define RE15_R38K_KS_RECT_D   1000", ""]
    open(ZIEL, "w", newline="\n").write("\n".join(L) + "\n")
    open(ZIEL_MD1, "w", newline="\n").write("\n".join(M1) + "\n")
    print("geschrieben:", ZIEL, ZIEL_MD1)


if __name__ == "__main__":
    main()
