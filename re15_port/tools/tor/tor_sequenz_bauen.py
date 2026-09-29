#!/usr/bin/env python3
"""tor_sequenz_bauen.py - baut die Tuersequenz des Gelaendertors ROOM1170 fuer den Port.

Aufruf
    python re15_port/tools/tor/tor_sequenz_bauen.py            # alles schreiben
    python re15_port/tools/tor/tor_sequenz_bauen.py --liste    # nur die Skripte zeigen

Ausgabe
    re15_port/engine/src/gen/tor_1170_door.inc   Modellteil im RE2-Aufbau (Skripte + MD1 + TIM)
    re15_port/engine/src/gen/re2_rcossin.inc     RE2-libgte-Tabelle rcossin_tbl (RotMatrix)
    re15_port/shared_assets/RE2/TORSE.VBS        Tonteil von RE2 DOOR2E, unveraendert

WARUM RE2 (Beta -> Retail)
    RE1.5 hat die Tuermaschine, aber keine Sequenz: das einzige Skript ist Evt_end
    (DOOR00.DO2 @0x9A6 `01 00`, analysis/tor_1170/03_tuersequenz.md 2.2). Wo RE1.5 unfertig
    ist, ist RE2 Retail das Vorbild. Vorbild fuer das Tor ist das RE2-Gittertor DOOR2E
    (Rahmen mit runden Ecken, einfluegelig, analysis/tor_1170/04_tuerkatalog.md Abschnitt 4).

MODELLTEIL (RE2-Aufbau, 03 Abschnitt 2.1 / 02 "re2-modellteil")
    +0 u32 MD1-Versatz, +4 u32 TIM-Versatz (beide ab Teilanfang), ab +8 SCD-Tabelle (u16,
    ab +8), dann Skripte, MD1, TIM zuletzt (RE2 legt den Primitivpuffer auf die TIM,
    02 "re2-prim-auf-tim"). Der Port laedt ihn aus dem eingebackenen Feld.

SKRIPTE - Byte fuer Byte aus DOOR2E (info/re2leon/COMMON/DOOR/DOOR2E.DO2), nur drei Aenderungen:
    (a) der Riegel (DOOR2E Mesh 1, Objekt 1/2, Skripte 5/6) entfaellt - das Tor hat keinen;
        sein Evt_exec (`04 0b 18 05` @0x05070) faellt weg. Er kostet kein Bild (Evt_exec
        schiebt nur den PC, @0x800538bc addiu v0,v0,4), die Zeitachse bleibt gleich.
    (b) der Pfosten an der Angel ist ein eigenes Wurzelobjekt (Objekt 1, Mesh 1) - so fuehrt
        RE2 feste Teile neben dem Fluegel (DOOR15 Variante 2, 04 K30). Er bekommt die
        Kamerafahrt als eigenes Skript auf Platz 11 (DOOR2E: Platz des Riegels, jetzt frei),
        gestartet im selben Bild wie die Fahrt des Fluegels.
    (c) die Lage des Pfostens: Angelachse + 364 in z (Tuerszene) = 191,51 Raumeinheiten
        (Cut 12, M9) * k 1,90 (06_massstab.skeptiker.md).
    (d) Runde 32 (Tor zu dunkel, analysis/befunde_runde32/tor_helligkeit.md): Fluegel und Pfosten
        tragen Objekt-Flag 0x1000 = Hintergrundfarbe 136 statt 68 (RE2 @0x800142ac lhu 304(s1),
        @0x800142b4 andi 0x1000, @0x800142bc addiu a3,zero,136). RE2-MECHANISMUS, belegt in DOOR2B
        (@Datei 0x5022 `4d 00 00 00 01 00 80 1a`, Blatt flags 0x1a80 - das Torblatt traegt genau
        dieses Flagwort).
    Alles andere - Aufstellung des Fluegels (2500,3512,+-1714/-1374), Schwenk 570 in 80
    Bildern, Klang in Bild 100, Fahrt 90 Bilder, Blenden - ist DOOR2E.

TEXTUR FUERS TUERLICHT (Runde 32, PORT-WAHL mit gemessener Herleitung)
    Die Tortextur ist ein Abzug der beleuchteten Cut-12-Pixel (tor_modell.textur_bauen). Die
    Tuermaschine moduliert jeden Texel mit der NCCT-Eckfarbe c / 128 (psx-spx GPU:1438-1446).
    RE2 malt seine Tuertexturen fuer dieses Licht (DOOR2E: gezeigt/Texel 0,567, Texel-Mittel im
    Blatt 69 gegen 37 im Torschild). Damit das Tor so hell erscheint wie das gemalte Tor an
    DERSELBEN Stelle, wird die Textur um 128 / c_schild angehoben, c_schild = NCCT der Schild-
    Normale mit den RE2-Konstanten (L @0x8009a470, LCM 1600 @0x8009a490, BK 136 @0x800142bc,
    RGBC 0x808080 @0x80014b58, Kamera @0x80076cb0 / 08_re_zeichnen 1.2) = 107 -> Faktor 1,196.
    Und die 5-Bit-Stufe wird GERUNDET statt abgeschnitten (tim_aus_bild schneidet `a >> 3` ab,
    der Port liest `<< 3`, include/re15_tim.h:114 - das kostete im Schild 8,6 %).
"""
import argparse
import os
import struct
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import do2_format as fmt       # noqa: E402

PORT = os.path.abspath(os.path.join(HIER, "..", ".."))
REPO = os.path.abspath(os.path.join(PORT, ".."))
DOOR2E = os.path.join(REPO, "info", "re2leon", "COMMON", "DOOR", "DOOR2E.DO2")
RE2_EXE = os.path.join(REPO, "info", "re2leon", "PSX.EXE")
AUS_TUER = os.path.join(PORT, "engine", "src", "gen", "tor_1170_door.inc")
AUS_TRIG = os.path.join(PORT, "engine", "src", "gen", "re2_rcossin.inc")
AUS_TON = os.path.join(PORT, "shared_assets", "RE2", "TORSE.VBS")

RCOSSIN = 0x800ADEAC            # RotMatrix @0x8008e21c `lw t9,-8532(t9)` (lui 0x800b)
PFOSTEN_DZ = 364                # round(191,51 * 1,90), s.o. (c)
DOOR2B = os.path.join(REPO, "info", "re2leon", "COMMON", "DOOR", "DOOR2B.DO2")

# Runde 32 (d): Objekt-Flag 0x1000 -> BK 136 (RE2 @0x800142b4 andi v0,v0,0x1000;
# @0x800142bc..c4 addiu a3/t0/t1,zero,136; sonst @0x800142e8..f0 addiu ..,zero,68).
FLAG_HELL = 0x1000
BK_NORMAL = 68                  # @0x800142e8 addiu a3,zero,68
BK_HELL = 136                   # @0x800142bc addiu a3,zero,136
DOOR2B_BLATT = (0x5022, bytes.fromhex("4d0000000100801a"))   # DOOR2B Skript 1: Blatt flags 0x1a80
# Tuerlicht RE2 (analysis/tor_1170/08_re_zeichnen.md 3), Bytes selbst gelesen:
L_TUER = ((400, 800, -500), (-1800, -1000, -2700), (3500, 6700, 1200))   # @0x8009a470
LCM_TUER = 1600                 # @0x8009a490 (neunmal 0x0640)
RGBC_TUER = 128                 # @0x80014b4c lui v0,0x80 / @0x80014b58 ori v0,v0,0x8080
KAMERA = ((0, 0, 4096), (0, 4096, 0), (-4096, 0, 0))   # Blickmatrix 0x80076cb0, Auge (10000,0,0) - 08 1.2
SCHILD_NORMALE = (4096, 0, 0)   # Schild zur Kamera bei Drehung 0 (V0); V1 (Drehung 2048) dieselbe


# =============================================================================
# DOOR2E lesen
# =============================================================================
def door2e():
    d = open(DOOR2E, "rb").read()
    do2 = fmt.Do2.lesen(d)
    if do2.variante != "re2":
        raise SystemExit("DOOR2E.DO2 nicht als RE2-Archiv erkannt")
    return d, do2


def skript_bytes(do2, k):
    return fmt.scd_lesen(fmt.scd_schreiben(do2.skripte))[k] if False else do2.skripte[k]


# =============================================================================
# Skripte des Tors
# =============================================================================
def s16(v):
    return struct.pack("<h", v)


def u16(v):
    return struct.pack("<H", v)


def door_model_set(obj, mesh, flags, x, y, z, rx=0, ry=0, rz=0, b2=0, frame=0, on=1, w8=16):
    """0x4D Door_model_set, 22 B (RE2-Handler 0x80014ba4, Vorschub @0x80014cac addiu v0,a1,22).
    Felder 03 Abschnitt 5.1 / 04 K04: +1 Objekt, +2 -> obj+8, +3 Bildnummer, +4 an, +5 Mesh,
    +6 u16 Flags, +8 s16 (in DOOR2E immer 16), +10/12/14 Lage, +16/18/20 Drehung."""
    return (bytes([0x4D, obj, b2, frame, on, mesh]) + u16(flags) + s16(w8)
            + s16(x) + s16(y) + s16(z) + u16(rx & 0xFFFF) + u16(ry & 0xFFFF) + u16(rz & 0xFFFF))


def skripte_bauen(do2):
    s = do2.skripte
    # DOOR2E-Skripte (Datei-Offsets in 04 Abschnitt 4.2):
    s0, s1, s2, s3, s4, s7, s8 = s[0], s[1], s[2], s[3], s[4], s[7], s[8]

    # --- Pruefanker: die Bytes, auf die sich die Aenderungen stuetzen, muessen stimmen ---
    def pruef(b, off, soll, was):
        if b[off:off + len(soll)] != soll:
            raise SystemExit("DOOR2E %s: erwartet %s, gefunden %s" % (was, soll.hex(" "), b[off:off + len(soll)].hex(" ")))
    pruef(s1, 0x00, bytes.fromhex("4d0000000100800a1000c409b80db206000000000000"), "Skript 1 Fluegel V0 @0x05036")
    pruef(s1, 0x16, bytes.fromhex("4d0100000101d0001000"), "Skript 1 Riegel @0x0504c")
    pruef(s1, 0x2C, bytes.fromhex("5300020700fe7400001c090a4600040b1805"), "Skript 1 Blende/Sleep/Riegel @0x05062")
    pruef(s1, 0x3E, bytes.fromhex("090a1e001804"), "Skript 1 Sleep 30 + Gosub 4 @0x05074")
    pruef(s2, 0x00, bytes.fromhex("4d0000000100800a1000c409b80da2fa000000080000"), "Skript 2 Fluegel V1 @0x05096")
    pruef(s2, 0x42, bytes.fromhex("5300020700fe7400001c090a4600040b1805"), "Skript 2 Blende/Sleep/Riegel @0x050d8")
    pruef(s7, 0x00, bytes.fromhex("2e0500"), "Skript 7 Work_set 5,0 @0x051ba")
    pruef(s7, 0x40, bytes.fromhex("530002070004"), "Skript 7 Ausblenden @0x051fa")
    pruef(s8, 0x00, bytes.fromhex("2e0500"), "Skript 8 Work_set 5,0 @0x0520c")
    pruef(s8, 0x40, bytes.fromhex("530002070004"), "Skript 8 Ausblenden @0x0524c")

    def hell(rec):
        """(d) Flag 0x1000 im Door_model_set (+6 u16 Flags, 03 5.1): 0x0a80 -> 0x1a80 wie DOOR2B."""
        b = bytearray(rec)
        fl = struct.unpack_from("<H", b, 6)[0] | FLAG_HELL
        struct.pack_into("<H", b, 6, fl)
        return bytes(b)

    d2b = open(DOOR2B, "rb").read()
    off, soll = DOOR2B_BLATT
    if d2b[off:off + len(soll)] != soll:
        raise SystemExit("DOOR2B @0x%x: erwartet %s, gefunden %s" % (off, soll.hex(" "), d2b[off:off + len(soll)].hex(" ")))

    fluegel_v0 = hell(s1[0x00:0x16])
    fluegel_v1 = hell(s2[0x00:0x16])
    if fluegel_v0[6:8] != d2b[off + 6:off + 8]:
        raise SystemExit("Torblatt-Flags weichen von DOOR2B ab")
    # Pfosten: Wurzel (Eltern = Kamera, Flag 0x10 aus), Flags wie der Fluegel ohne den
    # Schliesston-Merker 0x800 (den setzt der Fluegel, @0x80014c90..a8 genuegt einmal).
    pf_flags = (0x0A80 | FLAG_HELL) & ~0x0800
    pfosten_v0 = door_model_set(1, 1, pf_flags, 2500, 3512, 1714 + PFOSTEN_DZ)
    pfosten_v1 = door_model_set(1, 1, pf_flags, 2500, 3512, -1374 - PFOSTEN_DZ, ry=2048)

    def aufbau(fluegel, pfosten, fahrt_pfosten, fahrt_fluegel):
        return (fluegel + pfosten
                + bytes.fromhex("5300020700fe")        # Sce_fade_set 0,2,7,-512   (DOOR2E @0x05062)
                + bytes.fromhex("7400001c")            # Sce_fade_adjust 0,7168    (@0x05068)
                + bytes.fromhex("090a4600")            # Sleep 70                  (@0x0506c)
                + bytes.fromhex("090a1e00")            # Sleep 30                  (@0x05074)
                + bytes.fromhex("1804")                # Gosub 4 (Klang geladen?)  (@0x05078)
                + bytes.fromhex("360000000100000000000000")  # Se_on 0,0,1        (@0x0507a)
                + bytes.fromhex("090a1e00")            # Sleep 30                  (@0x05086)
                + bytes.fromhex("040c1803")            # Evt_exec 12, Skript 3     (@0x0508a)
                + bytes.fromhex("090a4600")            # Sleep 70                  (@0x0508e)
                + bytes([0x04, 0x0B, 0x18, fahrt_pfosten])   # Evt_exec 11: Fahrt Pfosten (neu)
                + bytes([0x18, fahrt_fluegel])         # Gosub 7/8                 (@0x05092)
                + bytes.fromhex("0100"))               # Evt_end                   (@0x05094)

    def ohne_blende(fahrt):
        """Fahrt des Pfostens = DOOR2E-Fahrt fuer Objekt 1 ohne das Ausblenden (das macht
        schon der Fluegel). Sce_fade_set @+0x40 (6 B) entfaellt; es kostet kein Bild
        (@0x80057fa8 addiu v0,s2,6, Rueckgabe 1)."""
        b = bytearray(fahrt)
        b[2] = 1                                    # Work_set typ 5, id 1
        return bytes(b[:0x40]) + bytes(b[0x46:])

    neu = [
        s0,                                          # 0 Verteiler (DOOR2E verbatim)
        aufbau(fluegel_v0, pfosten_v0, 5, 7),        # 1 Variante 0: Landeplatz-Seite, aufdruecken
        aufbau(fluegel_v1, pfosten_v1, 6, 8),        # 2 Variante 1: Laufsteg-Seite, aufziehen
        s3,                                          # 3 Fluegel schwenkt (verbatim)
        s4,                                          # 4 Warten auf den Klang (verbatim)
        ohne_blende(s7),                             # 5 Fahrt Pfosten V0
        ohne_blende(s8),                             # 6 Fahrt Pfosten V1
        s7,                                          # 7 Fahrt Fluegel V0 + Ausblenden (verbatim)
        s8,                                          # 8 Fahrt Fluegel V1 + Ausblenden (verbatim)
    ]
    return neu


# =============================================================================
# Modellteil
# =============================================================================
def modellteil(skripte, md1_b, tim_b):
    scd = fmt.scd_schreiben(skripte)
    kopf = 8
    md1_rel = kopf + len(scd)
    tim_rel = md1_rel + len(md1_b)
    return struct.pack("<II", md1_rel, tim_rel) + scd + md1_b + tim_b, md1_rel, tim_rel


def ncct_eckfarbe(normale, bk, rot_y=0):
    """NCCT (sf=1, lm=1) einer Tuerecke, Rechenweg 08_re_zeichnen.md 4.3 (bitgleich gegen RE2 ueber
    1 084 116 Ecken): LLM = L * (C * R), IR = lm1(LLM n >> 12), IR_c = lm1((BK<<4<<12 + LCM*sum IR) >> 12),
    Farbe = ((RGBC<<4) * IR_c >> 12) >> 4. Nur Drehung um y 0 oder 2048 (sin 0) wird gebraucht."""
    cy = {0: 4096, 2048: -4096}[rot_y]
    R = ((cy, 0, 0), (0, 4096, 0), (0, 0, cy))
    W = [[sum(KAMERA[i][k] * R[k][j] for k in range(3)) >> 12 for j in range(3)] for i in range(3)]
    LLM = [[sum(L_TUER[i][k] * W[k][j] for k in range(3)) >> 12 for j in range(3)] for i in range(3)]
    ir = [min(max(sum(LLM[i][k] * normale[k] for k in range(3)) >> 12, 0), 32767) for i in range(3)]
    irc = min(max(((bk << 4) * 4096 + LCM_TUER * sum(ir)) >> 12, 0), 32767)
    return min(max((((RGBC_TUER << 4) * irc) >> 12) >> 4, 0), 255)


def textur_fuers_tuerlicht(tex):
    """Runde 32 (PORT-WAHL, Kopf "TEXTUR FUERS TUERLICHT"), zwei Schritte je Kanal:
      1. gemalt -> 5 Bit GERUNDET: v5 = (p + 4) >> 3 (statt des Abschneidens p >> 3),
      2. Farbwert fuers Tuerlicht angehoben: v5' = round(v5 * 128 / c_schild), hoechstens 31.
    Schritt 2 wirkt auf die 5-Bit-Farbe, nicht auf den 8-Bit-Bildpunkt: so entstehen keine neuen
    Farben (228 statt 297 bei direkter Skalierung - die passen nicht in die 255 CLUT-Plaetze, und
    der Median-Cut von tim_aus_bild waere verlustbehaftet). Gemessen (tor_helligkeit.py):
    gezeigt/gemalt im Schild 1,008 zweistufig, 0,997 direkt+Median-Cut, 0,5125 vorher.
    Ausgabe auf dem 5-Bit-Raster (v5' << 3), damit tim_aus_bild (`>> 3`) nichts mehr abschneidet.
    Rueckgabe (Bild, Protokoll)."""
    import numpy as np
    from PIL import Image
    c_v0 = ncct_eckfarbe(SCHILD_NORMALE, BK_HELL, 0)
    c_v1 = ncct_eckfarbe((-SCHILD_NORMALE[0], 0, 0), BK_HELL, 2048)
    if c_v0 != c_v1:
        raise SystemExit("Schild-Eckfarbe V0 %d != V1 %d" % (c_v0, c_v1))
    a = np.asarray(tex.convert("RGBA")).astype(np.int64)
    rgb = a[..., :3]
    v5 = np.minimum(31, (rgb + 4) >> 3)                          # 1. runden statt abschneiden
    roh = (v5 * 256 + c_v0) // (2 * c_v0)                        # 2. round(v5 * 128 / c)
    aus = a.copy()
    aus[..., :3] = np.minimum(31, roh) << 3
    gekappt = int((roh > 31).any(-1).sum())
    proto = dict(c_schild=c_v0, c_schild_bk68=ncct_eckfarbe(SCHILD_NORMALE, BK_NORMAL, 0),
                 faktor=128.0 / c_v0, gekappt=gekappt)
    return Image.fromarray(aus.astype(np.uint8)), proto


def modell_bauen():
    import tor_modell as tm
    tex, _ = tm.textur_bauen()
    tex, proto = textur_fuers_tuerlicht(tex)
    if proto["c_schild"] != 107 or proto["c_schild_bk68"] != 73:
        raise SystemExit("Eckfarbe Schild %d/%d statt 107/73 (08_re_zeichnen 4.3)" % (
            proto["c_schild"], proto["c_schild_bk68"]))
    tim, info = fmt.tim_aus_bild(tex)
    if info.get("quantisiert"):
        raise SystemExit("Tortextur: %d 15-Bit-Farben > 255 Plaetze" % info["farben_15bit"])
    Sf, Sp = tm.Sammler(), tm.Sammler()
    tm.fluegel_bauen(Sf)
    tm.pfosten_bauen(Sp)
    md1 = fmt.md1_bauen([fmt.mesh_aus_dreiecken(Sf.dreiecke), fmt.mesh_aus_dreiecken(Sp.dreiecke)])
    return md1.schreiben(), tim.schreiben(), len(Sf.dreiecke), len(Sp.dreiecke)


def exe_lesen(addr, n):
    d = open(RE2_EXE, "rb").read()
    t_addr = struct.unpack_from("<I", d, 0x18)[0]
    off = addr - t_addr + 0x800
    return d[off:off + n], off


def carr(name, b, typ="unsigned char"):
    z = ["static const %s %s[%d] = {" % (typ, name, len(b))]
    for i in range(0, len(b), 16):
        z.append("    " + ",".join("0x%02x" % x for x in b[i:i + 16]) + ",")
    z.append("};")
    return "\n".join(z)


def liste(skripte):
    import tuerskript_dump as td   # noqa: F401  (nur fuer die Anzeige, falls vorhanden)


def main(argv=None):
    ap = argparse.ArgumentParser()
    ap.add_argument("--liste", action="store_true")
    a = ap.parse_args(argv)

    d2e, do2 = door2e()
    skripte = skripte_bauen(do2)
    md1_b, tim_b, n_f, n_p = modell_bauen()
    teil, md1_rel, tim_rel = modellteil(skripte, md1_b, tim_b)
    # Selbstpruefung: dasselbe Teil muss sich wie ein RE2-Modellteil lesen lassen
    rueck = fmt.scd_lesen(teil[8:md1_rel])
    # das letzte Skript traegt die Auffuellung auf 4 (scd_schreiben; 27 von 55 RE2-Bloecken)
    rest = rueck[-1][len(skripte[-1]):]
    if (rueck[:-1] != [bytes(x) for x in skripte[:-1]] or not rueck[-1].startswith(skripte[-1])
            or rest.strip(b"\0")):
        raise SystemExit("SCD-Rundlauf stimmt nicht")
    for k, sk in enumerate(skripte):          # 16-Bit-Felder nur an geraden Adressen (lhu)
        if (8 + fmt.u16(teil, 8 + 2 * k)) % 2:
            raise SystemExit("Skript %d beginnt ungerade" % k)
    fmt.Md1.lesen(teil[md1_rel:tim_rel])
    fmt.Tim.lesen(teil[tim_rel:])

    if a.liste:
        for k, sk in enumerate(skripte):
            print("Skript %d (%d B): %s" % (k, len(sk), sk.hex(" ")))
        return 0

    ton = do2.tonteil()
    tab, tab_off = exe_lesen(RCOSSIN, 4096 * 4)

    os.makedirs(os.path.dirname(AUS_TUER), exist_ok=True)
    kopf = [
        "/* GENERIERT von re15_port/tools/tor/tor_sequenz_bauen.py - NICHT HAND-EDITIEREN.",
        " *",
        " * Tuerarchiv (Modellteil, RE2-Aufbau) des Gelaendertors ROOM1170 fuer die RE2-Tuersequenz.",
        " * +0 u32 MD1-Versatz = 0x%x, +4 u32 TIM-Versatz = 0x%x, ab +8 SCD-Tabelle; TIM zuletzt." % (md1_rel, tim_rel),
        " * %d Skripte, abgeleitet aus RE2 DOOR2E (Aenderungen im Kopf des Werkzeugs)." % len(skripte),
        " * Mesh 0 Fluegel %d Dreiecke, Mesh 1 Pfosten %d Dreiecke (tools/tor/tor_modell.py," % (n_f, n_p),
        " * analysis/tor_1170/07_modell.md). Den Ton liefert shared_assets/RE2/TORSE.VBS.",
        " * Runde 32 (analysis/befunde_runde32/tor_helligkeit.md): Fluegel/Pfosten mit Flag 0x1000",
        " * (BK 136, RE2 @0x800142b4, Vorbild DOOR2B @Datei 0x5022); Textur fuers Tuerlicht angehoben",
        " * (128 / c_schild = 128 / 107) und auf 5 Bit gerundet.",
        " */",
    ]
    for k, sk in enumerate(skripte):
        kopf.append("/* Skript %d: %s */" % (k, sk.hex(" ")))
    open(AUS_TUER, "w").write("\n".join(kopf) + "\n" + carr("re15_tor1170_door", teil) + "\n")

    werte = struct.unpack("<4096I", tab)
    z = ["/* GENERIERT von re15_port/tools/tor/tor_sequenz_bauen.py - NICHT HAND-EDITIEREN.",
         " *",
         " * RE2 libgte rcossin_tbl @0x%08X (info/re2leon/PSX.EXE Datei-Offset 0x%x), 4096 Worte:" % (RCOSSIN, tab_off),
         " * unteres Halbwort = sin, oberes = cos (Q12). Leser: RotMatrix @0x8008e1f4",
         " * (@0x8008e21c lw t9,-8532(t9); @0x8008e224/28 sll/sra 16 = sin, @0x8008e234 sra 16 = cos).",
         " */",
         "static const uint32_t re2_rcossin_tbl[4096] = {"]
    for i in range(0, 4096, 8):
        z.append("    " + ",".join("0x%08x" % v for v in werte[i:i + 8]) + ",")
    z.append("};")
    open(AUS_TRIG, "w").write("\n".join(z) + "\n")

    os.makedirs(os.path.dirname(AUS_TON), exist_ok=True)
    open(AUS_TON, "wb").write(ton)

    import tor_modell as tm
    print("Textur fuers Tuerlicht:", textur_fuers_tuerlicht(tm.textur_bauen()[0])[1])
    print("Modellteil %d B (MD1 @0x%x %d B, TIM @0x%x %d B), %d Skripte" % (
        len(teil), md1_rel, len(md1_b), tim_rel, len(tim_b), len(skripte)))
    print("TORSE.VBS %d B = DOOR2E-Tonteil (Vorspann %d, VH %d, Nachspann %d, VB %d)" % (
        len(ton), len(do2.ton_vorspann), len(do2.vh), len(do2.ton_nachspann), len(do2.vb)))
    print("Tonkopf:", ton[:16].hex(" "))
    print("rcossin: sin(0)=%d cos(0)=%d sin(1024)=%d cos(1024)=%d" % (
        struct.unpack("<hh", tab[0:4]) + struct.unpack("<hh", tab[1024 * 4:1024 * 4 + 4])))
    return 0


if __name__ == "__main__":
    sys.exit(main())
