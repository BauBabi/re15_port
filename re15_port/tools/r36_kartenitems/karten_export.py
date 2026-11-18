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
RE15_R36K_MAX_BLAETTER = 3          # = include/re15_kartenitems_r36.h (Revierplan 1F/2F/3F)          # MD1: 5 Ecken a 8 Byte (x,y,z,pad s16) ab Datei 0x44

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
    # Revierplan: Name und Rahmen woertlich RE2 room20B0 msg 0 @0x3B92 "A police station map. / Will you take it?"
    # und msg 17 @0x3DB8 "You've taken a / police station map."
    (0x1030, "police station map", "a", "room20B0", [(2, 89, "1F"), (3, 88, "2F"), (4, 87, "3F")], 8, 39),
    # Slot 8 = Muenztelefon-Satz (Aot_set @0x01D2E, 4096x4096) wird nach Slot 39 verschoben, die Karte nimmt
    # Slot 8 (Vorrang)
    # Kellerplan: Rahmen RE2 room2130 @0x1B32 "A police B1 map. / Will you take it?" und @0x1D57 "You've taken
    # the / police B1 map."; Name "police basement map" fuer zwei Kellerebenen = PORT-WAHL (RE2 kennt nur B1).
    (0x1200, "police basement map", "the", "room2130", [(1, 77, "B2"), (0, 78, "B1")], 40, 0xFF),
]


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
         " * EIN Revierplan (ROOM1030) + EIN Kellerplan (ROOM1200) wie RE2 room20B0 Set(33,2..4) @0x037DA-E2.",
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
        L.append("/* ROOM%04X: \"A %s. / Will you take it?\" (RE2 %s-Rahmen) */"
                 % (zeilen[i]["raum"], zeilen[i]["name"], zeilen[i]["rahmen"]))
        L.append(blob("k_r36k_frage_%d" % i, bytes(f)))
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
    L.append(" * n_blaetter, blaetter[], bits[] (Runde 42: alle Blaetter des Plans; [0] = blatt/bit der Fundstelle).")
    L.append(" * Licht: Flach-Attribut RE15_R36K_PROP_ATTR im Installer, keine Spalte (Nachbesserung 1). */")
    L.append("static const re15_kartenitem_r36_t k_r36k_lage[%d] = {" % len(zeilen))
    for z in zeilen:
        gb = [g[0] for g in z["gruppe"]] + [0] * (RE15_R36K_MAX_BLAETTER - len(z["gruppe"]))
        gt = [g[1] for g in z["gruppe"]] + [0] * (RE15_R36K_MAX_BLAETTER - len(z["gruppe"]))
        L.append("    { 0x%04X, %d, %d, %d, 0x%02X, %d, %d, %d, %d, %d, %d, %d, %d, %d, { %s }, { %s } },"
                 "   /* %s %s Mitte (%d,%d,%d) */"
                 % (z["raum"], z["blatt"], z["bit"], z["slot"], z["verdr"], z["x"], z["y"], z["z"],
                    z["rx"], z["ry"], z["rz"], z["rx0"], z["rz0"], len(z["gruppe"]),
                    ", ".join(str(v) for v in gb), ", ".join(str(v) for v in gt),
                    z["name"], "/".join(g[2] for g in z["gruppe"]),
                    round(z["mitte"][0]), round(z["mitte"][1]), round(z["mitte"][2])))
    L.append("};")
    open(ZIEL, "w", newline="\n").write("\n".join(L) + "\n")
    print("geschrieben:", ZIEL)


if __name__ == "__main__":
    main()
