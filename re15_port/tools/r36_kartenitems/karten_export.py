#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Runde 36 Spur N: die fuenf Etagen-Lageplaene als Weltmodell (RE2 karte01) + Lage je Raum.

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
  Mitte = Sehstrahl der Markenmitte geschnitten mit der Ebene; Massstab s = min(Markenbreite/1080,
  Markenhoehe/720) auf der Ebene (die Karte liegt INNERHALB der Marke, Seitenverhaeltnis 3:2 bleibt);
  das MD1 wird gleichmaessig um s skaliert (nur die 5 Ecken, UV/Normalen/Topologie unveraendert).
  Drehung: Modell +x -> Welt unten (0,1,0), Modell +y -> in die Wand (-n), Modell +z -> rechts
  (unten x vorwaerts), zerlegt in die Euler-Winkel von pc_prop_rot_q12 (Ry*Rx*Rz, platform/pc/main.c).
  Ursprung = Mitte - 85*s*n (die Flaeche liegt bei Modell-y = -85*s, also 85*s vor dem Ursprung).
  Aufhebe-Rechteck 1000 x 1000 (r35-Muster, haeufigste Item-Groesse) um Mitte + 300*n.
  LICHT (PORT-WAHL aus Messung): das Figurenlicht der RDT liefert an den Waenden 31..70 von 128
  (probe_r34n_e_dokumente licht; 1140 Cut 5 Tafel: (41,39,39) -> Karte 2,8x dunkler als die Tafel im
  Hintergrund). Die Karte ist PAPIER: ihr Papiergrund bekommt die Helligkeit der hellsten Flaechen
  (Zettel, Schilder, Tafel, Rahmen) in ihrer unmittelbaren Umgebung unter demselben Licht:
  Tint = 128 * Ref / Median(Kartentextur), Ref = Mittel der hellsten 15 % (Summe R+G+B) im Fenster
  "Marken-Huelle um je eine Huellenbreite/-hoehe erweitert" des Nutzer-Cuts, je Kanal.
  (Verworfen, gemessen: Mittel UNTER der Marke -> auf dunklen Waenden 11D0/1200 so dunkel wie die
  Wand, Karte im Framedump unsichtbar.)

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
VERT_OFF, VERT_N = 0x44, 5          # MD1: 5 Ecken a 8 Byte (x,y,z,pad s16) ab Datei 0x44

# ---- Kameras: `probe_r34n_e_dokumente kamera <raum>` (VIEW-Zeile: R Q12 zeilenweise, T, H) ------
CAMS = {
    0x1140: (5, (4031, 0, -816, -246, 3901, -1214, 777, 1233, 3839), (3708, -1933, 18717), 208),
    0x10D0: (6, (-2580, 0, -3204, -607, 4016, 488, 3141, 775, -2530), (20915, -192, 16689), 208),
    0x1030: (3, (-3946, 0, 1099, 89, 4096, 320, -1099, 333, -3946), (4697, 3905, -9501), 208),
    0x11D0: (0, (3903, 0, -1276, -595, 3637, -1818, 1133, 1907, 3465), (-5734, -7218, 21469), 208),
    0x1200: (2, (3565, 0, 2020, 64, 4096, -115, -2020, 131, 3565), (26880, 1213, 9135), 208),
}
# ---- Marken-Huellen (Pixel, inklusiv), analysis/befunde_runde36/marken/map*.bmp ----------------
MARKEN = {0x1140: (231, 72, 241, 81), 0x10D0: (270, 120, 289, 135), 0x1030: (155, 103, 164, 113),
          0x11D0: (227, 65, 250, 93), 0x1200: (58, 80, 74, 93)}
# ---- Ebenen (n zeigt in den Raum / zur Kamera; Ebene n.p = c), Herleitung Dossier 3 ------------
_J = np.array([(2390, -17045), (1847, -18566), (1475, -19609), (1186, -20393)], float)  # 11D0 Bodenkante y=0


def _ebene_11d0():
    m = _J.mean(axis=0)
    _, _, vt = np.linalg.svd(_J - m)
    d = vt[0] if vt[0][1] > 0 else -vt[0]
    n = np.array([-d[1], 0.0, d[0]])
    if n @ (np.array([-1479.0, 0, -22928.0]) - np.array([m[0], 0, m[1]])) < 0:   # Kamera Cut 0
        n = -n
    return n, float(n @ np.array([m[0], 0, m[1]]))


EBENEN = {
    0x1140: (np.array([0.0, 0, -1]), 8710.0),      # Tafel z = -8710 (Cut 4 + Cut 5 trianguliert)
    0x10D0: (np.array([0.0, 0, 1]), 21624.0),      # Wand z = 21624 (Bodenkante)
    0x1030: (np.array([0.0, 0, 1]), -23580.0),     # Rueckwand z = -23580 (Bodenkante Zeile 151,5)
    0x11D0: _ebene_11d0(),                         # schraege rechte Wand (19,8 Grad)
    0x1200: (np.array([1.0, 0, 0]), -26330.0),     # Vitrinen-Front x = -26330 (Cut 1 + Cut 2)
}
# ---- Licht-Referenz (RGB 0..255) im Nutzer-Cut: Engine-BSS-Dekoder probe_bg_dump (alle 428 STAGE1-
# Bilder); Mittel der hellsten 15 % im Fenster um die Marke (Dossier 3.3):
#   1140 Cut 5 Fenster (220,62)-(252,91)   10D0 Cut 6 (250,104)-(309,151)   1030 Cut 3 (145,92)-(174,124)
#   1200 Cut 2 (41,66)-(91,107)            11D0 Cut 0 (203,36)-(274,122)
BG_REF = {0x1140: (127, 126, 115), 0x10D0: (147, 151, 135), 0x1030: (38, 57, 78),
          0x1200: (69, 69, 56), 0x11D0: (60, 61, 58)}
# ---- Raum -> Blatt / Bit / Etage (Bank-9-Bits: VERTRAG.md 1.1 Spur N) ---------------------------
# Blatt aus re15_map_zones.h (Spalte 6), Etagenname = Kartenblatt-Titel (menu_common.c ORDER-Kommentar)
RAEUME = [  # (raum, blatt, bit, etage, aot_slot, verdraengt_nach)
    (0x1140, 4, 87, "3F", 40, 0xFF),
    (0x10D0, 3, 88, "2F", 40, 0xFF),
    (0x1030, 2, 89, "1F", 8, 39),    # Slot 8 = Muenztelefon-Satz (Aot_set @0x01D2E, 4096x4096) wird
                                     # nach Slot 39 verschoben, die Karte nimmt Slot 8 (Vorrang)
    (0x1200, 1, 90, "B2", 40, 0xFF),
    (0x11D0, 0, 91, "B1", 40, 0xFF),
]


def textur_median(tim):
    """Median-Farbe (= Papiergrund) der Kartenkunst (UV u 0..126, v 0..84, CLUT 0, deckende Texel)."""
    import struct
    flags = struct.unpack_from("<I", tim, 4)[0]
    assert flags & 3 == 1 and flags & 8, "8bpp mit CLUT erwartet"
    blen, _, _, cw, _ = struct.unpack_from("<IHHHH", tim, 8)
    clut = [struct.unpack_from("<H", tim, 20 + 2 * i)[0] for i in range(cw)]
    off = 8 + blen
    _, _, _, iw, _ = struct.unpack_from("<IHHHH", tim, off)
    px = tim[off + 12:]
    farben = []
    for v in range(85):
        for u in range(127):
            c = clut[px[v * iw * 2 + u]]
            if c == 0:
                continue
            farben.append(((c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3))
    return np.median(np.array(farben, float), axis=0)


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


def frage(etage):
    # RE2 room2130 sub00: FA 02 "A " F9 01 "police B1 map" F9 00 "." FC "Will you take it" 1B FB 40 FE 00
    # RE1.5-Rahmen ROOM2030 msg 2 @0x208B: 04 02 ... 05 01 NAME 05 00 1B 03 02 01 00; FC -> 08.
    return ([0x04, 0x02] + text("A ") + [0x05, 0x01] + text("police %s map" % etage) + [0x05, 0x00]
            + [0x57, 0x08] + text("Will you take it") + [0x1B, 0x03, 0x02, 0x01, 0x00])


def genommen(etage):
    # RE2 room2130 sub12: FA 02 "You've taken the " FC F9 01 "police B1 map" F9 00 "." FE 00
    # RE1.5-Rahmen ROOM2030 msg 3 @0x20AC: 04 02 ... 05 01 NAME 05 00 57 01 00
    return ([0x04, 0x02] + text("You've taken the ") + [0x08, 0x05, 0x01] + text("police %s map" % etage)
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
    tex_m = textur_median(tim)
    print("Kartentextur Median (CLUT 0, u0..126 v0..84): %.1f %.1f %.1f" % tuple(tex_m))
    for (raum, blatt, bit, etage, slot, verdr) in RAEUME:
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
        s = min(breite / 1080.0, hoehe / 720.0)
        rx, ry, rz = euler(M)
        assert np.abs(wieder(rx, ry, rz) - M).max() < 1e-9
        urs = P["c"] - 85.0 * s * n
        rmitte = P["c"] + 300.0 * n
        sk = bytearray(md1)
        for i, e in enumerate(ecken):
            v = np.round(e * s).astype(int)
            sk[VERT_OFF + 8 * i:VERT_OFF + 8 * i + 6] = np.array(v, dtype="<i2").tobytes()
        md1s.append(bytes(sk))
        texte.append((frage(etage), genommen(etage)))
        tint = [max(0, min(255, int(round(128.0 * BG_REF[raum][i] / tex_m[i])))) for i in range(3)]
        zeilen.append(dict(raum=raum, blatt=blatt, bit=bit, etage=etage, slot=slot, verdr=verdr,
                           x=int(round(urs[0])), y=int(round(urs[1])), z=int(round(urs[2])),
                           rx=q12(rx), ry=q12(ry), rz=q12(rz),
                           rx0=int(round(rmitte[0])) - 500, rz0=int(round(rmitte[2])) - 500,
                           s=s, breite=breite, hoehe=hoehe, mitte=P["c"], tint=tint))
        print("ROOM%04X %s Blatt %d Bit %d | Mitte (%d,%d,%d) Marke %.0f x %.0f -> s %.4f (%.0f x %.0f) "
              "| Ursprung (%d,%d,%d) rot (%d,%d,%d) | Rechteck (%d,%d) 1000x1000 Slot %d | Licht %s"
              % (raum, etage, blatt, bit, P["c"][0], P["c"][1], P["c"][2], breite, hoehe, s, 1080 * s, 720 * s,
                 zeilen[-1]["x"], zeilen[-1]["y"], zeilen[-1]["z"], zeilen[-1]["rx"], zeilen[-1]["ry"],
                 zeilen[-1]["rz"], zeilen[-1]["rx0"], zeilen[-1]["rz0"], slot, tint))
    if not schreiben:
        return

    def blob(name, b, typ="unsigned char"):
        out = ["static const %s %s[%d] = {" % (typ, name, len(b))]
        for i in range(0, len(b), 16):
            out.append("    " + ",".join("0x%02x" % v for v in b[i:i + 16]) + ",")
        out.append("};")
        return "\n".join(out)

    L = ["/* AUTOMATISCH ERZEUGT von re15_port/tools/r36_kartenitems/karten_export.py --schreiben - nicht von Hand.",
         " * Runde 36 Spur N (Dossier analysis/befunde_runde36/N_kartenitems.md Abschnitt 3).",
         " * RE2-Lageplan karte01: info/re2leon/PL0/RDT/ROOM3060.RDT MD1 @0x%06X (%d B, md5 %s)," % (MD1_OFF, MD1_LEN, MD1_MD5),
         " *                       TIM @0x%06X (%d B, md5 %s) - TIM UNVERAENDERT." % (TIM_OFF, TIM_LEN, TIM_MD5),
         " * Je Raum EIN MD1: dieselben Bytes, nur die 5 Ecken @0x44 gleichmaessig um s skaliert (PORT-WAHL:",
         " * Karte liegt innerhalb der Nutzer-Marke, 3:2). Lage/Drehung/Rechteck: Kopf des Skripts. */",
         blob("k_r36k_tim", tim), ""]
    for i, b in enumerate(md1s):
        z = zeilen[i]
        L.append("/* ROOM%04X %s: s = %.4f -> Flaeche %.0f x %.0f (Marke %.0f x %.0f auf der Ebene) */"
                 % (z["raum"], z["etage"], z["s"], 1080 * z["s"], 720 * z["s"], z["breite"], z["hoehe"]))
        L.append(blob("k_r36k_md1_%d" % i, b))
    L.append("static const unsigned char *const k_r36k_md1[%d] = { %s };"
             % (len(md1s), ", ".join("k_r36k_md1_%d" % i for i in range(len(md1s)))))
    L.append("")
    for i, (f, g) in enumerate(texte):
        L.append("/* ROOM%04X: \"A police %s map. / Will you take it?\" (RE2 room2130 sub00-Rahmen) */"
                 % (zeilen[i]["raum"], zeilen[i]["etage"]))
        L.append(blob("k_r36k_frage_%d" % i, bytes(f)))
        L.append("/* ROOM%04X: \"You've taken the / police %s map.\" (RE2 room2130 sub12-Rahmen) */"
                 % (zeilen[i]["raum"], zeilen[i]["etage"]))
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
    L.append(" * licht r,g,b (Tint = 128 * Licht-Referenz der Umgebung / Textur-Median %.0f,%.0f,%.0f) */" % tuple(tex_m))
    L.append("static const re15_kartenitem_r36_t k_r36k_lage[%d] = {" % len(zeilen))
    for z in zeilen:
        L.append("    { 0x%04X, %d, %d, %d, 0x%02X, %d, %d, %d, %d, %d, %d, %d, %d, { %d, %d, %d } },   /* %s Mitte (%d,%d,%d) */"
                 % (z["raum"], z["blatt"], z["bit"], z["slot"], z["verdr"], z["x"], z["y"], z["z"],
                    z["rx"], z["ry"], z["rz"], z["rx0"], z["rz0"], z["tint"][0], z["tint"][1], z["tint"][2], z["etage"],
                    round(z["mitte"][0]), round(z["mitte"][1]), round(z["mitte"][2])))
    L.append("};")
    open(ZIEL, "w", newline="\n").write("\n".join(L) + "\n")
    print("geschrieben:", ZIEL)


if __name__ == "__main__":
    main()
