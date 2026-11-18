#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Runde 36 Spur N / Runde 42 Strang lageplaene: die Polizei-Lageplaene als Weltmodell (RE2 karte01) + Lage je Raum.

RUNDE 42 (Strang lageplaene, Stufe 1) - RE2-FORM: EIN Revierplan + EIN Kellerplan statt fuenf Etagenplaene.
  NUTZERWUNSCH 18.11.2026 woertlich: "Dann will ich das nicht so. Ich will das es mehr funktioniert wie in Resident
  Evil 2." und "nicht benötigte maps kommen weg."
  RE2-Vorbild: room20B0 sub10 @0x037BA..0x0381A - EIN Plan ("police station map", Text RDT @0x3B92 / @0x3DB8) setzt
  Set(33,2..4) @0x037DA-E2 (Besitz 1F/2F/3F) und Set(35,2..4) @0x037E6-EE (Etagenwahl) in EINEM Ereignis.
  Revierplan = ROOM1030 (1F, Blatt 2/3/4, Bits 89/88/87), Kellerplan = ROOM1200 (B2, Blatt 1/0, Bits 77/78).
  Ort = die vorhandene Stelle, die beide Figuren auf dem Pflichtweg zuerst erreichen (Belege: Kopf
  include/re15_kartenitems_r36.h). Die Stellen ROOM1140 (3F), ROOM10D0 (2F), ROOM11D0 (B1) entfallen;
  ihre Bits setzt der zustaendige Plan mit (alte Spielstaende mit Einzelbits bleiben gueltig).

RUNDE 42 NACHARBEIT (Strang lageplaene_nach):
  - Revierplan schaltet zusaetzlich das Dach frei (Blatt 5 = ROOM1170/1171, re15_map_zones.h), Orchestrator-Entscheid
    18.11.2026 "das Dach gehoert zum Revier"; Bank-9-Bit 42 (frei: Mod-RDT-Zensus, Port-Zensus, 40-%-Disc, alle r42-Zweige).
  - Revierplan NEU an der Ostwand der Lobby neben der Ausgangstuer nach 1050 (Kamera 2 = Tuerkamera beider Figuren),
    ueber der Notfall-Tafel (msg 4), Groesse s = 2; Fabrikplan 30C1 an derselben Stelle wie Stufe 2, s = 2.
    NUTZERWUNSCH 18.11. "gut SICHTBAR"; PORT-WAHL s = 2 (RE2-Wandplan karte01 = 1080 x 720; RE2s Polizeiplaene room20B0
    obj 8 / room2130 obj 0 sind KEINE Wandplaene, sondern das gefaltete karte00, MD1 @0x004FDC md5 735097e1, 112 x 538).

Erzeugt re15_port/engine/src/gen/kartenitems_r36.inc (KEIN Asset-Patch, shared_assets bleibt
unangetastet). Dossier: analysis/befunde_runde36/N_kartenitems.md (Abschnitt 3).

QUELLE DES MODELLS (RE2 Retail, Leon-Disc):
  info/re2leon/PL0/RDT/ROOM3060.RDT  MD1 @0x0045C8 (236 B, md5 99dae133...)  TIM @0x02C6D4 (16928 B)
  = extracted_re2_items/modelle/karte01_99dae133.{md1,tim} (lageplaene.csv, Zeile room3060
  "sewage disposal map", Aufnahme sub22 @0x0283E). Geometrie: Flaeche 1080 (Modell-z) x 720
  (Modell-x) auf Modell-y = -85, 5 Ecken @MD1+0x44, Normale (0,-4096,0) @MD1+0x6C (Sichtseite -y).
  UV: u 0..126 laeuft mit +z, v 0..84 mit +x (MD1+0xBC) -> Bild-rechts = +z, Bild-unten = +x.

LAGE (PORT-WAHL aus den Nutzer-Marken, NUTZER-VORGABE = Ort der roten Marke):
  Je Raum ein Cut, in dem der Nutzer markiert hat (Cut aus dem Bildvergleich, Dossier 1.1), dessen
  Kameramatrix (re15_camera_build_view, ausgegeben von `probe_r34n_e_dokumente kamera RAUM`), die
  gemessene Flaechen-Ebene unter der Marke (Dossier 3), die Marken-Huelle (Pixel).
  Mitte = Sehstrahl der Markenmitte geschnitten mit der Ebene; Massstab s = groesster Wert, fuer den
  die vier projizierten Kartenecken im Nutzer-Cut INNERHALB der Marken-Huelle liegen (Bildraum, +-0,5 px;
  die Karte liegt in der Marke, Seitenverhaeltnis 3:2 bleibt; auf der schraegen Wand 11D0 waere ein
  Ebenen-Mass 28 % zu hoch geworden, gemessen im Framedump y 61..97 gegen Marke 65..93);
  das MD1 wird gleichmaessig um s skaliert (nur die 5 Ecken, UV/Normalen/Topologie unveraendert).
  Drehung: Modell +x -> Welt unten (0,1,0), Modell +y -> in die Wand (-n), Modell +z -> rechts
  (unten x vorwaerts), zerlegt in die Euler-Winkel von pc_prop_rot_q12 (Ry*Rx*Rz, platform/pc/main.c).
  Ursprung = Mitte - 85*s*n (die Flaeche liegt bei Modell-y = -85*s, also 85*s vor dem Ursprung).
  Aufhebe-Rechteck 1000 x 1000 (r35-Muster, haeufigste Item-Groesse) um Mitte + 300*n.
  LICHT: keine Spalte mehr (Nachbesserung 1). Die Karte traegt das Flach-Attribut 0x0010
  (RE15_R36K_PROP_ATTR, re15_kartenitems_r36.h) und wird vom Prop-Zeichner wie RE1.5 FUN_8002c18c
  @0x8002c2f4 -> FUN_80022f0c (0x808080) bzw. RE2 FUN_800363f0 ohne Raumlicht gezeichnet.

Aufruf: python re15_port/tools/r36_kartenitems/karten_export.py [--schreiben]
"""
import hashlib
import math
import os
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
RDT_RE2 = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM3060.RDT")
ZIEL = os.path.join(REPO, "re15_port", "engine", "src", "gen", "kartenitems_r36.inc")
MD1_OFF, MD1_LEN, MD1_MD5 = 0x0045C8, 236, "99dae1332d75c51a2621b73737f60a26"
TIM_OFF, TIM_LEN, TIM_MD5 = 0x02C6D4, 16928, "bf0fbcc75f644b18391b3cea57bd280d"
VERT_OFF, VERT_N = 0x44, 5
RE15_R36K_MAX_BLAETTER = 4          # = include/re15_kartenitems_r36.h (Revierplan 1F/2F/3F + Dach)   # MD1: 5 Ecken a 8 Byte (x,y,z,pad s16) ab Datei 0x44

# ---- Kameras: `probe_r34n_e_dokumente kamera <raum>` (VIEW-Zeile: R Q12 zeilenweise, T, H) ------
CAMS = {
    0x1030: (3, (-3946, 0, 1099, 89, 4096, 320, -1099, 333, -3946), (4697, 3905, -9501), 208),
    0x1200: (2, (3565, 0, 2020, 64, 4096, -115, -2020, 131, 3565), (26880, 1213, 9135), 208),
}
# ---- Marken-Huellen (Pixel, inklusiv), analysis/befunde_runde36/marken/map*.bmp ----------------
MARKEN = {0x1030: (155, 103, 164, 113), 0x1200: (58, 80, 74, 93)}
# ---- Ebenen (n zeigt in den Raum / zur Kamera; Ebene n.p = c), Herleitung Dossier 3 ------------
EBENEN = {
    0x1030: (np.array([0.0, 0, 1]), -23580.0),     # Rueckwand z = -23580 (Bodenkante Zeile 151,5)
    0x1200: (np.array([1.0, 0, 0]), -26330.0),     # Vitrinen-Front x = -26330 (Cut 1 + Cut 2)
}
# ---- Raum -> Blatt / Bit / Etage (Bank-9-Bits: VERTRAG.md 1.1 Spur N) ---------------------------
# Blatt aus re15_map_zones.h (Spalte 6), Etagenname = Kartenblatt-Titel (menu_common.c ORDER-Kommentar)
# BANK-9-BITS (Nachtrag kartenbits, Dossier 9): Bank 9 = 0x800b1078 (Flag-Tabelle 0x80074664[9] @0x80074688) ist im
# Original die Zone der Item-TAKEN-Bits (Item_aot_set @0x80040644: u16 pc[18] @0x80040680, Test FUN_8004efe4
# @0x800406dc). Ein Lageplan-Bit darf kein Original-Bit sein (Zensus N_belege/bank9_zensus.txt: 85 von 256 belegt).
# 87/88/89 sind im Original frei. B2/B1 lagen auf 90/91 = ROOM2090/2091 @0x00A12 Item 0x3F (Bit 0x5a) bzw.
# ROOM2040/2041 @0x01364 Item 0x32 (Bit 0x5b) -> jetzt 77/78: im Original frei (Block 53..84), in keiner
# Port-Version je benutzt (git log --all, Tags v0.8.21-23).
# Runde 42: EIN Plan je Bereich (RE2 room20B0 Set(33,2..4) @0x037DA-E2). Erstes Blatt = Etage der Fundstelle.
PLAENE = [  # (raum, name, artikel, rahmen, [(blatt, bit, etage)], aot_slot, verdraengt_nach)
    # Revierplan: seit der Nacharbeit NICHT mehr an der Nutzer-Marke map1F.bmp (Cut 3, ~11 x 8 Bildpunkte), sondern
    # direkt gesetzt - Zeile in PLAENE2 (dort steht Ort, Groesse und Begruendung). Die Zeilenfolge bleibt: 1030 zuerst.
    # Kellerplan: Rahmen RE2 room2130 @0x1B32 "A police B1 map. / Will you take it?" und @0x1D57 "You've taken
    # the / police B1 map."; Name "police basement map" fuer zwei Kellerebenen = PORT-WAHL (RE2 kennt nur B1).
    (0x1200, "police basement map", "the", "room2130", [(1, 77, "B2"), (0, 78, "B1")], 40, 0xFF),
]

# ---- Runde 42 Stufe 2: EIN Plan je Gebiet STAGE2..6 (ohne Forschungszentrum 4020 = r42/rest, ohne Labor 5030 = Original) --
# Lage NICHT aus Nutzer-Marken, sondern direkt: Wandebene aus der SCA-Zelle (Byte-Offset), Mitte auf der Wand, Groesse
# s = 1 (RE2-karte01 unskaliert = "wie ein Plakat", PORT-WAHL), Mitte 2000 ueber dem Boden des Standorts (PORT-WAHL).
# Kamera = die Ankunfts-/Pflichtkamera (Kopf include/re15_kartenitems_r36.h, RUNDE 42 STUFE 2); die VIEW-Zeilen stammen von
# `probe_r34n_e_dokumente kamera RAUM` und dienen nur der Kontrollausgabe (Plakat im Bild).
# Variante: 0 = beide Fassungen, 2 = nur Elza (re15_kartenitems_r36.h RE15_R36K_VARIANTE_ELZA).
VIEWS2 = {
    0x1030: (2, (-1520, 0, -3823, -718, 4027, 285, 3758, 769, -1495), (-13985, 2547, 3264), 208),
    0x2000: (11, (3258, 0, -2484, 531, 4007, 696, 2430, -876, 3187), (-13607, 6361, 18908), 208),
    0x30C0: (1, (3923, 0, 1210, 49, 4085, -160, -1207, 167, 3912), (9505, 2234, 7886), 208),
    0x6000: (0, (1581, 0, 3796, 872, 3988, -364, -3696, 941, 1539), (-2778, 3515, 11446), 208),
}
PLAENE2 = [
    # REVIERPLAN (Nacharbeit; Zeile 0 der Tabelle). Name/Rahmen woertlich RE2 room20B0 msg 0 @0x3B92 "A police station map. /
    # Will you take it?" und msg 17 @0x3DB8 "You've taken a / police station map."; Gruppe 1F/2F/3F wie RE2 Set(33,2..4)
    # @0x037DA-E2 plus DACH Blatt 5 (Bit 42, Orchestrator-Entscheid 18.11.2026).
    # ORT: Ostwand der Lobby x = 2398 (ROOM1030 SCA-Zelle 1 @0x01A1C x[2398..4198] z[-23528..-9632], n = -x) neben der
    # Ausgangstuer nach 1050 (main00 Door_aot_set Slot 0 @0x01C6A x 1500..3000 z -13800..-11600), ueber der Notfall-Tafel
    # (Slot 9 @0x01D42 msg 4 "Some sort of reference chart for / identifying emergency severity."). Beide Figuren verlassen
    # die Lobby durch diese Tuer (Leon L6 @0x01C6A, Elza E2->E3 1031 Slot 0 @0x01CD8) und stehen davor in Kamera 2: RVD
    # 1->2 @0x00294 und 3->2 @0x0030C sind die einzigen Wege in den Tuerbereich (0->2 gibt es nicht). Mitte z -14900,
    # y -4150 (unterkante -3430 ueber den Aushaengen, oberkante -4870 unter dem Deckenbalken) = PORT-WAHL, am Bild gemessen.
    # Aufhebe-Rechteck NICHT um Mitte+300n: vor der Bank/Tischreihe (SCA-Zelle 14 @0x01AB8 x[952..2752] z[-23204..-14744])
    # kommt man nicht an die Wand; das Rechteck liegt im freien Streifen z -14744..-13800 zwischen Bank und Tuer
    # (Mitte (2098, -14300)). Slot 9: die Tafel (Slot 9, 2384 x 1631 ueber dieser Stelle) zieht nach Slot 39 um, denn der
    # Aktions-Scan nimmt je Druck den KLEINSTEN treffenden Slot (FUN_80042bac); 39 ist in 1030/1031 frei (aots-Sonde).
    dict(raum=0x1030, name="police station map", artikel="a", rahmen="room20B0",
         gruppe=[(2, 89, "1F"), (3, 88, "2F"), (4, 87, "3F"), (5, 42, "Dach")], slot=9, verdr=39,
         variante=0, n=(-1.0, 0.0, 0.0), boden=0, mitte=(2398.0, -4150.0, -14900.0), s=2.0, rect=(2098.0, -14300.0)),
    # Kanalisation: Rahmen RE2 room4040 @0x1B9B "There's a sewer map. / Will you take it?", @0x1BCB "You've taken / the
    # sewer map."; Nordwand des Nordgangs z = -19800 (ROOM2000 SCA-Zelle 7 @0x012A4 x[-19850..7450] z[-19800..-17800],
    # floor 19 = Band 1, n = -z) links neben der Ostleiter (Tuer Slot 0 x 3900..4900), Mitte x 1500: frei von Elzas
    # John-Satz 2001 sub00 @0x017EA (Slot 7, x 2200..6100). Boden -1800 (1260 Slot 1/4 @0x0114A/@0x011AA next_y `f8 f8`);
    # Rohr an der Wand bei y -3250 (Cut 11 Bild (100,112)/(200,131) auf z -19800) -> Mitte -3800 liegt darueber.
    dict(raum=0x2000, name="sewer map", rahmen="room4040", gruppe=[(6, 34, "Kanalisation")], slot=20, verdr=0xFF,
         variante=0, n=(0.0, 0.0, -1.0), boden=-1800, mitte=(1500.0, -1800.0 - 2000.0, -19800.0)),
    # Fabrik (nur Elza, 30C1): Name RE2 room5040 @0x2341/@0x2376 "factory map"; Wand z = 1100 (ROOM30C1 SCA-Zelle 1
    # @0x007F8 x[-27200..-5300] z[1100..1600], n = -z), Mitte x -7600 zwischen Treppenfuss (Zelle 10..13 bis x -9400) und
    # Ostwand x -5800 (Zelle 0 @0x007EC), Boden 0 (3091 Slot 0 @0x01332 next_y 0).
    # Nacharbeit: s = 2 (NUTZERWUNSCH "gut SICHTBAR", Pruefer s2 gelb 2: s = 1 war in Kamera 1 nur ~20 x 14 Bildpunkte im
    # 320er Bild). Die Wand x -9400..-5800 traegt die 2160 Breite (Plakat x -8680..-6520).
    dict(raum=0x30C0, name="factory map", rahmen="room4040", gruppe=[(7, 35, "Fabrik")], slot=20, verdr=0xFF,
         variante=2, n=(0.0, 0.0, -1.0), boden=0, mitte=(-7600.0, 0.0 - 2000.0, 1100.0), s=2.0),
    # U-Bahn: Name "subway map" = PORT-WAHL im Rahmen room4040; Wand x = 2025 (ROOM6000 SCA-Zelle 17 @0x00828
    # x[25..2025] z[-2400..3725], n = +x), Mitte z -400 auf der linken Tafel der Rueckwand (Bild-x 80..115 = z -1000..272;
    # Hangar-Tuer Slot 0 erst ab z 2200), Boden -3600 (50B0 Slot 1 @0x004AE next_y `f0 f1`).
    dict(raum=0x6000, name="subway map", rahmen="room4040", gruppe=[(12, 37, "U-Bahn")], slot=20, verdr=0xFF,
         variante=0, n=(1.0, 0.0, 0.0), boden=-3600, mitte=(2025.0, -3600.0 - 2000.0, -400.0)),
]
S2 = 1.0   # RE2-Originalgroesse


def projiziere(raum, p):
    _, R, T, H = CAMS[raum]
    R = np.array(R, float).reshape(3, 3) / 4096.0
    v = R @ p + np.array(T, float)
    return 160.0 + H * v[0] / v[2], 120.0 + H * v[1] / v[2]


def massstab_in_marke(raum, mitte, rechts, unten):
    x0, y0, x1, y1 = MARKEN[raum]
    def passt(s):
        for a in (-540.0, 540.0):
            for b in (-360.0, 360.0):
                sx, sy = projiziere(raum, mitte + a * s * rechts + b * s * unten)
                if sx < x0 - 0.5 or sx > x1 + 1.5 or sy < y0 - 0.5 or sy > y1 + 1.5:
                    return False
        return True
    lo, hi = 0.01, 3.0
    for _ in range(60):
        mi = (lo + hi) / 2
        if passt(mi):
            lo = mi
        else:
            hi = mi
    return lo


def strahl(raum, sx, sy):
    _, R, T, H = CAMS[raum]
    R = np.array(R, float).reshape(3, 3) / 4096.0
    Ri = np.linalg.inv(R)
    o = Ri @ (-np.array(T, float))
    d = Ri @ np.array([(sx - 160.0) / H, (sy - 120.0) / H, 1.0])
    return o, d


def auf_ebene(raum, sx, sy):
    n, c = EBENEN[raum]
    o, d = strahl(raum, sx, sy)
    return o + (c - n @ o) / (n @ d) * d


def euler(m):
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


# ---- RE1.5-Textcodierung (Glyphen wie msg_common.c re15_msg_glyph / rdt_msgdump.py) -------------
def glyph(ch):
    if ch == " ":
        return 0x00
    if "0" <= ch <= "9":
        return 0x0C + ord(ch) - ord("0")
    if "A" <= ch <= "Z":
        return 0x1D + ord(ch) - ord("A")
    if "a" <= ch <= "z":
        return 0x3D + ord(ch) - ord("a")
    return {"'": 0x3A, "?": 0x1B, ".": 0x57}[ch]


def text(s):
    return [glyph(c) for c in s]


def frage(name):
    # RE2 room20B0 msg 0 @0x3B92 / room2130 @0x1B32: FA 02 "A " F9 01 NAME F9 00 "." FC "Will you take it" 1B FB .. FE 00
    # RE1.5-Rahmen ROOM2030 msg 2 @0x208B: 04 02 ... 05 01 NAME 05 00 1B 03 02 01 00; FC -> 08.
    return ([0x04, 0x02] + text("A ") + [0x05, 0x01] + text(name) + [0x05, 0x00]
            + [0x57, 0x08] + text("Will you take it") + [0x1B, 0x03, 0x02, 0x01, 0x00])


def frage_4040(name):
    # RE2 room4040 @0x1B9B: FA 02 "There's a " F9 01 "sewer map" F9 00 01(.) FC "Will you take it" 1B FB 40 FE 00
    # RE1.5-Rahmen wie frage(): F9 -> 05, "." -> 57, FC -> 08, Schluss 1B 03 02 01 00 (ROOM2030 msg 2).
    return ([0x04, 0x02] + text("There's a ") + [0x05, 0x01] + text(name) + [0x05, 0x00]
            + [0x57, 0x08] + text("Will you take it") + [0x1B, 0x03, 0x02, 0x01, 0x00])


def genommen_4040(name):
    # RE2 room4040 @0x1BCB: FA 02 "You've taken" FC "the " F9 01 "sewer map" F9 00 01 FE 00 (room5040 @0x2376 ebenso)
    return ([0x04, 0x02] + text("You've taken") + [0x08] + text("the ") + [0x05, 0x01] + text(name)
            + [0x05, 0x00, 0x57, 0x01, 0x00])


def genommen(name, artikel):
    # RE2 room20B0 msg 17 @0x3DB8: FA 02 "You've taken a" FC F9 01 NAME F9 00 "." FC FE 00
    # RE2 room2130 @0x1D57:        FA 02 "You've taken the " FC F9 01 NAME F9 00 "." FE 00
    # RE1.5-Rahmen ROOM2030 msg 3 @0x20AC: 04 02 ... 05 01 NAME 05 00 57 01 00 (das zweite FC vor FE in room20B0
    # entfaellt: der RE1.5-Rahmen endet mit 57 01 00).
    kopf = "You've taken a" if artikel == "a" else "You've taken the "
    return ([0x04, 0x02] + text(kopf) + [0x08, 0x05, 0x01] + text(name)
            + [0x05, 0x00, 0x57, 0x01, 0x00])


def main():
    schreiben = "--schreiben" in sys.argv
    rdt = open(RDT_RE2, "rb").read()
    md1 = rdt[MD1_OFF:MD1_OFF + MD1_LEN]
    tim = rdt[TIM_OFF:TIM_OFF + TIM_LEN]
    assert hashlib.md5(md1).hexdigest() == MD1_MD5, "MD1 md5"
    assert hashlib.md5(tim).hexdigest() == TIM_MD5, "TIM md5"
    ecken = [np.frombuffer(md1[VERT_OFF + 8 * i:VERT_OFF + 8 * i + 6], dtype="<i2").astype(float)
             for i in range(VERT_N)]
    zeilen, md1s, texte = [], [], []
    for (raum, name, artikel, rahmen, gruppe, slot, verdr) in PLAENE:
        blatt, bit, etage = gruppe[0]
        n, c = EBENEN[raum]
        x0, y0, x1, y1 = MARKEN[raum]
        P = {k: auf_ebene(raum, sx, sy) for k, (sx, sy) in
             {"tl": (x0, y0), "tr": (x1 + 1, y0), "bl": (x0, y1 + 1), "br": (x1 + 1, y1 + 1),
              "c": ((x0 + x1 + 1) / 2.0, (y0 + y1 + 1) / 2.0)}.items()}
        unten = np.array([0.0, 1.0, 0.0])
        vor = -n
        rechts = np.cross(unten, vor)
        M = np.column_stack([unten, vor, rechts])
        breite = abs(((P["tr"] - P["tl"]) @ rechts + (P["br"] - P["bl"]) @ rechts) / 2.0)
        hoehe = abs(((P["bl"] - P["tl"])[1] + (P["br"] - P["tr"])[1]) / 2.0)
        s = massstab_in_marke(raum, P["c"], rechts, unten)
        rx, ry, rz = euler(M)
        assert np.abs(wieder(rx, ry, rz) - M).max() < 1e-9
        urs = P["c"] - 85.0 * s * n
        rmitte = P["c"] + 300.0 * n
        sk = bytearray(md1)
        for i, e in enumerate(ecken):
            v = np.round(e * s).astype(int)
            sk[VERT_OFF + 8 * i:VERT_OFF + 8 * i + 6] = np.array(v, dtype="<i2").tobytes()
        md1s.append(bytes(sk))
        texte.append((frage(name), genommen(name, artikel)))
        zeilen.append(dict(raum=raum, blatt=blatt, bit=bit, etage=etage, slot=slot, verdr=verdr,
                           name=name, artikel=artikel, rahmen=rahmen, gruppe=gruppe,
                           x=int(round(urs[0])), y=int(round(urs[1])), z=int(round(urs[2])),
                           rx=q12(rx), ry=q12(ry), rz=q12(rz),
                           rx0=int(round(rmitte[0])) - 500, rz0=int(round(rmitte[2])) - 500,
                           s=s, breite=breite, hoehe=hoehe, mitte=P["c"]))
        print("ROOM%04X %s Blaetter/Bits %s | Mitte (%d,%d,%d) Marke %.0f x %.0f -> s %.4f (%.0f x %.0f) "
              "| Ursprung (%d,%d,%d) rot (%d,%d,%d) | Rechteck (%d,%d) 1000x1000 Slot %d"
              % (raum, name, gruppe, P["c"][0], P["c"][1], P["c"][2], breite, hoehe, s, 1080 * s, 720 * s,
                 zeilen[-1]["x"], zeilen[-1]["y"], zeilen[-1]["z"], zeilen[-1]["rx"], zeilen[-1]["ry"],
                 zeilen[-1]["rz"], zeilen[-1]["rx0"], zeilen[-1]["rz0"], slot))
        zeilen[-1]["variante"] = 0
        zeilen[-1]["band"] = 0   # Aot_set pc[4] = 0 wie ROOM5030 sub00 @0x0108A
    # ---- Runde 42 Stufe 2: Plaene STAGE2..6 mit direkter Lage (Wand aus der SCA-Zelle, s = 1) -----------------------
    for p in PLAENE2:
        raum = p["raum"]
        blatt, bit, etage = p["gruppe"][0]
        n = np.array(p["n"])
        mitte = np.array(p["mitte"])
        s = p.get("s", S2)   # Nacharbeit: Revier- und Fabrikplan s = 2 (PORT-WAHL "gut SICHTBAR"), sonst RE2-Groesse
        unten = np.array([0.0, 1.0, 0.0])
        vor = -n
        rechts = np.cross(unten, vor)
        M = np.column_stack([unten, vor, rechts])
        rx, ry, rz = euler(M)
        assert np.abs(wieder(rx, ry, rz) - M).max() < 1e-9
        urs = mitte - 85.0 * s * n
        rmitte = mitte + 300.0 * n
        if "rect" in p:   # Nacharbeit Revierplan: Rechteck im erreichbaren Streifen vor der Wand (Kommentar PLAENE2)
            rmitte = np.array([p["rect"][0], mitte[1], p["rect"][1]])
        sk = bytearray(md1)   # s = 1: das RE2-MD1 unveraendert; sonst nur die 5 Ecken @0x44 gleichmaessig skaliert
        for i, e in enumerate(ecken):
            v = np.round(e * s).astype(int)
            sk[VERT_OFF + 8 * i:VERT_OFF + 8 * i + 6] = np.array(v, dtype="<i2").tobytes()
        md1s.append(bytes(sk))
        if p["rahmen"] == "room20B0":
            texte.append((frage(p["name"]), genommen(p["name"], p["artikel"])))
        else:
            texte.append((frage_4040(p["name"]), genommen_4040(p["name"])))
        cut, R, T, H = VIEWS2[raum]
        R = np.array(R, float).reshape(3, 3) / 4096.0
        ecken_bild = []
        for a in (-540.0, 540.0):
            for b in (-360.0, 360.0):
                v = R @ (mitte + a * s * rechts + b * s * unten) + np.array(T, float)
                ecken_bild.append((160.0 + H * v[0] / v[2], 120.0 + H * v[1] / v[2]))
        zeilen.append(dict(raum=raum, blatt=blatt, bit=bit, etage=etage, slot=p["slot"], verdr=p["verdr"],
                           name=p["name"], artikel=p.get("artikel", "the"), rahmen=p["rahmen"], gruppe=p["gruppe"],
                           x=int(round(urs[0])), y=int(round(urs[1])), z=int(round(urs[2])),
                           rx=q12(rx), ry=q12(ry), rz=q12(rz),
                           rx0=int(round(rmitte[0])) - 500, rz0=int(round(rmitte[2])) - 500,
                           s=s, breite=1080.0 * s, hoehe=720.0 * s, mitte=mitte, variante=p["variante"],
                           band=int(-(int(p["boden"]) / 0x708))))   # re15_collision.c:186 -(y/0x708), C-Division
        print("ROOM%04X %s Blatt %d Bit %d Variante %d | Mitte (%d,%d,%d) s %.2f | Ursprung (%d,%d,%d) rot (%d,%d,%d) | "
              "Rechteck (%d,%d) Slot %d | Cut %d Bild x %.0f..%.0f y %.0f..%.0f"
              % (raum, p["name"], blatt, bit, p["variante"], mitte[0], mitte[1], mitte[2], s,
                 zeilen[-1]["x"], zeilen[-1]["y"], zeilen[-1]["z"], zeilen[-1]["rx"], zeilen[-1]["ry"], zeilen[-1]["rz"],
                 zeilen[-1]["rx0"], zeilen[-1]["rz0"], p["slot"], cut,
                 min(e[0] for e in ecken_bild), max(e[0] for e in ecken_bild),
                 min(e[1] for e in ecken_bild), max(e[1] for e in ecken_bild)))
    # Zeilenfolge: Revierplan ROOM1030 bleibt Zeile 0 (Riegel unit_r42_lageplaene_tabelle), dann Keller, dann STAGE2..6.
    folge = sorted(range(len(zeilen)), key=lambda i: (0 if zeilen[i]["raum"] == 0x1030 else 1, i))
    zeilen, md1s, texte = [zeilen[i] for i in folge], [md1s[i] for i in folge], [texte[i] for i in folge]
    if not schreiben:
        return

    def blob(name, b, typ="unsigned char"):
        out = ["static const %s %s[%d] = {" % (typ, name, len(b))]
        for i in range(0, len(b), 16):
            out.append("    " + ",".join("0x%02x" % v for v in b[i:i + 16]) + ",")
        out.append("};")
        return "\n".join(out)

    L = ["/* AUTOMATISCH ERZEUGT von re15_port/tools/r36_kartenitems/karten_export.py --schreiben - nicht von Hand.",
         " * Runde 36 Spur N (Dossier analysis/befunde_runde36/N_kartenitems.md Abschnitt 3); Runde 42 Strang lageplaene:",
         " * EIN Revierplan (ROOM1030) + EIN Kellerplan (ROOM1200) wie RE2 room20B0 Set(33,2..4) @0x037DA-E2; Stufe 2: je Gebiet",
         " * ein Plan wie RE2 room4040/5040 - Kanalisation ROOM2000, Fabrik ROOM30C1 (nur Elza), U-Bahn ROOM6000 (s = 1).",
         " * RE2-Lageplan karte01: info/re2leon/PL0/RDT/ROOM3060.RDT MD1 @0x%06X (%d B, md5 %s)," % (MD1_OFF, MD1_LEN, MD1_MD5),
         " *                       TIM @0x%06X (%d B, md5 %s) - TIM UNVERAENDERT." % (TIM_OFF, TIM_LEN, TIM_MD5),
         " * Je Raum EIN MD1: dieselben Bytes, nur die 5 Ecken @0x44 gleichmaessig um s skaliert (PORT-WAHL:",
         " * Karte liegt innerhalb der Nutzer-Marke, 3:2). Lage/Drehung/Rechteck: Kopf des Skripts. */",
         blob("k_r36k_tim", tim), ""]
    for i, b in enumerate(md1s):
        z = zeilen[i]
        L.append("/* ROOM%04X %s (%s): s = %.4f -> Flaeche %.0f x %.0f (Marke %.0f x %.0f auf der Ebene) */"
                 % (z["raum"], z["name"], z["etage"], z["s"], 1080 * z["s"], 720 * z["s"], z["breite"], z["hoehe"]))
        L.append(blob("k_r36k_md1_%d" % i, b))
    L.append("static const unsigned char *const k_r36k_md1[%d] = { %s };"
             % (len(md1s), ", ".join("k_r36k_md1_%d" % i for i in range(len(md1s)))))
    L.append("")
    for i, (f, g) in enumerate(texte):
        L.append("/* ROOM%04X: \"%s %s. / Will you take it?\" (RE2 %s-Rahmen) */"
                 % (zeilen[i]["raum"], "There's a" if zeilen[i]["rahmen"] == "room4040" else "A", zeilen[i]["name"],
                    zeilen[i]["rahmen"]))
        L.append(blob("k_r36k_frage_%d" % i, bytes(f)))
        if zeilen[i]["rahmen"] == "room4040":
            L.append("/* ROOM%04X: \"You've taken / the %s.\" (RE2 room4040-Rahmen) */" % (zeilen[i]["raum"], zeilen[i]["name"]))
        else:
            L.append("/* ROOM%04X: \"You've taken %s / %s.\" (RE2 %s-Rahmen) */"
                     % (zeilen[i]["raum"], zeilen[i]["artikel"], zeilen[i]["name"], zeilen[i]["rahmen"]))
        L.append(blob("k_r36k_genommen_%d" % i, bytes(g)))
    L.append("static const unsigned char *const k_r36k_frage[%d] = { %s };"
             % (len(texte), ", ".join("k_r36k_frage_%d" % i for i in range(len(texte)))))
    L.append("static const unsigned char k_r36k_frage_len[%d] = { %s };"
             % (len(texte), ", ".join(str(len(f)) for f, _ in texte)))
    L.append("static const unsigned char *const k_r36k_genommen[%d] = { %s };"
             % (len(texte), ", ".join("k_r36k_genommen_%d" % i for i in range(len(texte)))))
    L.append("static const unsigned char k_r36k_genommen_len[%d] = { %s };"
             % (len(texte), ", ".join(str(len(g)) for _, g in texte)))
    L.append("")
    L.append("/* raum, blatt, bit, aot_slot, verdraengt_nach, x, y, z, rot_x, rot_y, rot_z, rect_x, rect_z (1000x1000),")
    L.append(" * n_blaetter, blaetter[], bits[] (Runde 42: alle Blaetter des Plans; [0] = blatt/bit der Fundstelle),")
    L.append(" * variante (Runde 42 Stufe 2: 0 = beide Fassungen, 2 = nur Elza / raum|1), band (Aot_set pc[4] der Zone).")
    L.append(" * Licht: Flach-Attribut RE15_R36K_PROP_ATTR im Installer, keine Spalte (Nachbesserung 1). */")
    L.append("static const re15_kartenitem_r36_t k_r36k_lage[%d] = {" % len(zeilen))
    for z in zeilen:
        gb = [g[0] for g in z["gruppe"]] + [0] * (RE15_R36K_MAX_BLAETTER - len(z["gruppe"]))
        gt = [g[1] for g in z["gruppe"]] + [0] * (RE15_R36K_MAX_BLAETTER - len(z["gruppe"]))
        L.append("    { 0x%04X, %d, %d, %d, 0x%02X, %d, %d, %d, %d, %d, %d, %d, %d, %d, { %s }, { %s }, %d, %d },"
                 "   /* %s %s Mitte (%d,%d,%d) */"
                 % (z["raum"], z["blatt"], z["bit"], z["slot"], z["verdr"], z["x"], z["y"], z["z"],
                    z["rx"], z["ry"], z["rz"], z["rx0"], z["rz0"], len(z["gruppe"]),
                    ", ".join(str(v) for v in gb), ", ".join(str(v) for v in gt), z["variante"], z["band"],
                    z["name"], "/".join(g[2] for g in z["gruppe"]),
                    round(z["mitte"][0]), round(z["mitte"][1]), round(z["mitte"][2])))
    L.append("};")
    open(ZIEL, "w", newline="\n").write("\n".join(L) + "\n")
    print("geschrieben:", ZIEL)


if __name__ == "__main__":
    main()
