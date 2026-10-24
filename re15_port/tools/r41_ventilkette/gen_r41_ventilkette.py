#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Runde 41 Spur C "ventilkette": erzeugt die eingebackenen Daten des Moduls ventilkette_r41.c.

  1. gen/ventilkette_r41_wasser.inc   Wasserflaeche = ROOM2080.RDT Prop 0 (MD1 + TIM) UNVERAENDERT
                                      (RDT+0x30-Tabelle, Scheibe bis zur naechsten Grenze wie rdt_common.c
                                      parse_props / r35_inhalt/schrot_export.py); dazu (Nachbesserung 3,
                                      Nachtrag 8) das Netz der Relinghoehe: dieselbe MD1-Form (1 Netz, 1 Normale
                                      (0,-4096,0), EIN UV-Satz = eine Kachel je Viereck), Vierecke <= 1800 x 1800
                                      nur ueber der Schachtoeffnung von ROOM2030 (RE15_R41C_RELING_RECHTECKE).
  2. gen/ventilkette_r41_texte.inc    Port-Nachrichten (Ids laut VERTRAG), kodiert mit der Glyphentabelle des
                                      Ports (engine/src/msg_common.c re15_msg_glyph) in der Satzform der
                                      Original-Nachrichten (Kopf `04 02`, Satzende 0x57 + `01 00`, Frage
                                      `1b 03 02 01 00` wie ROOM2050 msg 0 @0x2446, Zeilenumbruch 0x08,
                                      Seitenwechsel `02 00` wie ROOM2080 msg 0 @0x0CF8, gruen `05 01 .. 05 00`
                                      wie ROOM11F0 msg 2 @0x18F1). Jeder Text wird nach dem Kodieren wieder
                                      dekodiert und muss den Ausgangstext ergeben.
  3. gen/ventilkette_r41_original.inc Original-Byte-Scheiben, die das Modul Byte fuer Byte wiederverwendet
                                      (ROOM2080 sub02, ROOM2050 sub09, ROOM11F0 msg 1/2, Tuersatz 2000 Slot 4,
                                      Obj_model_set 2050 obj 8, Aot_set 2050 Slot 8).

Aufruf: C:/Python310/python.exe re15_port/tools/r41_ventilkette/gen_r41_ventilkette.py
"""
import hashlib
import os
import struct

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
CD = os.path.join(REPO, "re15_port", "shared_assets", "PSX")
GEN = os.path.join(REPO, "re15_port", "engine", "src", "gen")


def u32(d, o):
    return struct.unpack_from("<I", d, o)[0]


def grenze(d, start, tbl, n):
    best = len(d)
    for h in range(0x08, 0x5C + 1, 4):
        p = u32(d, h)
        if start < p < best:
            best = p
    for i in range(n):
        p = u32(d, tbl + 4 * i)
        if start < p < best:
            best = p
    return best


def prop(d, k):
    nom = d[2]
    tbl = u32(d, 0x30)
    n = nom * 2
    tim = u32(d, tbl + 8 * k)
    md1 = u32(d, tbl + 8 * k + 4)
    return (md1, d[md1:grenze(d, md1, tbl, n)]), (tim, d[tim:grenze(d, tim, tbl, n)])


def inc_array(name, b, pro_zeile=16):
    z = ["static const unsigned char %s[%d] = {" % (name, len(b))]
    for i in range(0, len(b), pro_zeile):
        z.append("    " + ",".join("0x%02x" % x for x in b[i:i + pro_zeile]) + ",")
    z.append("};")
    return "\n".join(z)


# ---- 1. Wasserflaeche ----------------------------------------------------------------------------------------
def wasser():
    pfad = os.path.join(CD, "STAGE2", "ROOM2080.RDT")
    d = open(pfad, "rb").read()
    (mo, mb), (to, tb) = prop(d, 0)
    assert len(mb) == 4572 and len(tb) == 16928, (len(mb), len(tb))
    kopf = ("/* AUTOMATISCH ERZEUGT von re15_port/tools/r41_ventilkette/gen_r41_ventilkette.py - nicht von Hand aendern.\n"
            " * Wasserflaeche fuer ROOM2030/2031 (Runde 41 Spur C) = ROOM2080.RDT Prop 0 (das Wasser, das der Rechner\n"
            " * senkt: Obj_model_set sub00 @0x00C2E `2d 00 03 02 09 00 0a 00 1a 00 ...`), UNVERAENDERT:\n"
            " *   MD1 ROOM2080.RDT @0x%06X %d B md5 %s (108 Vierecke, x 0..21600 y 0 z 0..16200)\n"
            " *   TIM ROOM2080.RDT @0x%06X %d B md5 %s\n"
            " * Prop 0 der RDT+0x30-Tabelle. */\n"
            % (mo, len(mb), hashlib.md5(mb).hexdigest(), to, len(tb), hashlib.md5(tb).hexdigest()))
    rb, rk = reling_netz(mb)
    ziel = os.path.join(GEN, "ventilkette_r41_wasser.inc")
    open(ziel, "w", newline="\n").write(kopf + inc_array("re15_r41c_wasser_md1", mb) + "\n" +
                                        inc_array("re15_r41c_wasser_tim", tb) + "\n" + rk +
                                        inc_array("re15_r41c_wasser_reling_md1", rb) + "\n")
    print("geschrieben:", ziel, "MD1 @0x%06X %d B, TIM @0x%06X %d B, Reling-Netz %d B" % (mo, len(mb), to, len(tb), len(rb)))


# Schachtoeffnung ROOM2030 (x0, x1, z0, z1) = include/re15_ventilkette_r41.h RE15_R41C_RELING_RECHTECKE (Belege dort und
# im Dossier §0e B4-1: Innenkanten der Absperrung = Ring-Reling-Zellen 19/20/21/22/31/32 fl 0x33, Aussenwand 26/27).
# Nachbesserung 4 (Nachtrag 12 (k)): der Pegel steht knapp unter dem Ringboden (y -5350), der Treppenschacht
# x -5400..-4200 z -8500..-6900 liegt damit unter Wasser (frueher bei -3600 ausgespart).
RELING_RECHTECKE = [(-5400, 5400, -8500, -6400), (-7700, 5400, -6400, 2450), (-5850, 5400, 2450, 8150)]
ZELLE = 1800   # Gitterschritt des Originals (ROOM2080 MD1: x 0..21600, z 0..16200 in 1800er Schritten)


def reling_netz(org):
    """Netz der Relinghoehe in der MD1-Form von ROOM2080 Prop 0 (org = die 4572 Original-Bytes)."""
    laenge, unbek, oc = struct.unpack_from("<III", org, 0)
    h = struct.unpack_from("<14I", org, 12)
    assert oc == 2 and h[5] == 0 and h[10] == 1 and h[12] == 108, (oc, h)
    normale = org[12 + h[2]:12 + h[2] + 8]
    assert normale == bytes([0, 0, 0, 0xf0, 0, 0, 0, 0]), normale.hex()
    uv = org[12 + h[13]:12 + h[13] + 16]
    # alle 108 Vierecke tragen denselben UV-Satz (eine Kachel je Viereck)
    assert all(org[12 + h[13] + 16 * k:12 + h[13] + 16 * k + 16] == uv for k in range(108))
    q0 = struct.unpack_from("<8H", org, 12 + h[11])
    assert q0 == (0, 0, 0, 13, 0, 1, 0, 14), q0   # v0 (x0,z0), v1 (x0,z1), v2 (x1,z0), v3 (x1,z1)
    # EIN gemeinsames Gitter fuer alle Rechtecke (Kanten = alle Rechteckgrenzen, jedes Intervall in gleiche Teile
    # <= 1800 geteilt): benachbarte Vierecke teilen ihre Kanten exakt -> keine T-Stoesse/Risse an den Rechteckgrenzen
    # (gemessen 2026-10-24 10:30, Cut 4: Rissreihe entlang z -6400 bei getrennten Teilgittern).
    def teile(grenzen):
        g = sorted(set(grenzen))
        aus = []
        for a, e in zip(g, g[1:]):
            n = -(-(e - a) // ZELLE)
            aus += [a + ((e - a) * i) // n for i in range(n)]
        return aus + [g[-1]]
    xs = teile([v for r in RELING_RECHTECKE for v in r[0:2]])
    zs = teile([v for r in RELING_RECHTECKE for v in r[2:4]])
    def drin(x, z):
        return any(r[0] <= x <= r[1] and r[2] <= z <= r[3] for r in RELING_RECHTECKE)
    ecken, vier, index = [], [], {}
    def ecke(x, z):
        if (x, z) not in index:
            index[(x, z)] = len(ecken)
            ecken.append((x, 0, z))
        return index[(x, z)]
    for j in range(len(zs) - 1):
        for i in range(len(xs) - 1):
            if not drin((xs[i] + xs[i + 1]) / 2.0, (zs[j] + zs[j + 1]) / 2.0):
                continue
            vier.append((ecke(xs[i], zs[j]), ecke(xs[i], zs[j + 1]), ecke(xs[i + 1], zs[j]), ecke(xs[i + 1], zs[j + 1])))
    nv, nq = len(ecken), len(vier)
    tv = 56
    tn = tv + 8 * nv
    tf = tn + 8
    tu = tf + 16 * nq
    kopf = struct.pack("<III", 12 + tu, 0, 2) + struct.pack("<14I", tv, nv, tn, 1, tf, 0, tu, tv, nv, tn, 1, tf, nq, tu)
    b = bytearray(kopf)
    for (x, y, z) in ecken:
        b += struct.pack("<hhhh", x, y, z, 0)
    b += normale
    for (a, c, d, e) in vier:
        b += struct.pack("<8H", 0, a, 0, c, 0, d, 0, e)
    for _ in vier:
        b += uv
    assert len(b) == 12 + tu + 16 * nq
    k = ("/* Netz der Relinghoehe fuer ROOM2030/2031 (Nachbesserung 3, Nachtrag 8): MD1-Form von ROOM2080 Prop 0 (oben),\n"
         " * %d Ecken, %d Vierecke (<= %d x %d), Normale und UV-Satz (%s) Byte fuer Byte aus dem Original, Ursprung = Weltursprung\n"
         " * von ROOM2030; Rechtecke (x0,x1,z0,z1) %s. md5 %s */\n"
         % (nv, nq, ZELLE, ZELLE, uv.hex(), RELING_RECHTECKE, hashlib.md5(bytes(b)).hexdigest()))
    return bytes(b), k


# ---- 2. Texte --------------------------------------------------------------------------------------------------
# Glyphen = engine/src/msg_common.c re15_msg_glyph (Dekodierer des Ports) umgekehrt.
GLYPH = {" ": 0x00, ":": 0x16, ",": 0x18, '"': 0x19, "!": 0x1A, "?": 0x1B, "[": 0x37, "/": 0x38, "]": 0x39,
         "'": 0x3A, "-": 0x3B, ".": 0x57}
for i in range(10):
    GLYPH[str(i)] = 0x0C + i
for i in range(26):
    GLYPH[chr(ord("A") + i)] = 0x1D + i
    GLYPH[chr(ord("a") + i)] = 0x3D + i
RUECK = {v: k for k, v in GLYPH.items()}
RUECK[0x3C] = "."


def kodieren(zeilen, frage=False, gruen=None):
    """zeilen: Liste von Seiten; jede Seite ist eine Liste von Zeilen (0x08 dazwischen); Seiten durch `02 00`.
    frage=True: Schluss `1b 03 02 01 00` (das '?' gehoert zum Schluss). gruen: Teilstring in Gruen (05 01 .. 05 00)."""
    out = bytearray([0x04, 0x02])
    for si, seite in enumerate(zeilen):
        if si:
            out += bytes([0x02, 0x00])
        for zi, zeile in enumerate(seite):
            if zi:
                out.append(0x08)
            text = zeile
            if gruen and gruen in text:
                a = text.index(gruen)
                for ch in text[:a]:
                    out.append(GLYPH[ch])
                out += bytes([0x05, 0x01])
                for ch in gruen:
                    out.append(GLYPH[ch])
                out += bytes([0x05, 0x00])
                text = text[a + len(gruen):]
            for ch in text:
                out.append(GLYPH[ch])
    if frage:
        out += bytes([0x1B, 0x03, 0x02, 0x01, 0x00])
    else:
        out += bytes([0x01, 0x00])
    return bytes(out)


def dekodieren(b):
    """Wie re15_msg_decode_text: Steuercodes 02/04/05/06/09-0B = Code + 1 Arg, 03/07 = 1 Byte, 08 = Zeile,
    01 = Ende. Liefert (Text, Frage?)."""
    i = 0
    out = []
    frage = False
    while i < len(b):
        c = b[i]
        if c == 0x01:
            break
        if c in (0x02, 0x04, 0x05, 0x06, 0x09, 0x0A, 0x0B):
            if c == 0x02:
                out.append(" | ")
            i += 2
            continue
        if c == 0x03:               # Ja/Nein-Wahl: danach folgt nur der Original-Schluss `02 01 00`
            frage = True
            break
        if c == 0x07:
            i += 1
            continue
        if c == 0x08:
            out.append(" / ")
            i += 1
            continue
        out.append(RUECK.get(c, "<%02x>" % c))
        i += 1
    return "".join(out), frage


# Wortlaute: NUTZER-VORGABE (woertlich aus AUFTRAG.md) bzw. PORT-WAHL (englisch wie das Spiel) - Freigabe im Dossier.
TEXTE = [
    # (Raumgruppe, Id, Name, Seiten, frage, gruen, Herkunft)
    ("2000", 40, "leiter", [["I cannot climb up the ladder", "because of the water pressure."]], False, None,
     "NUTZER-VORGABE (AUFTRAG.md: \"I cannot climb up the ladder because of the water pressure\")"),
    ("2030", 40, "pult_ohne_druck", [["A control panel for the bridge.", "The pressure gauge reads zero."],
                                      ["Intake valve 13 is closed."]], False, None,
     "PORT-WAHL (Nutzer: \"Pult meldet zu wenig Druck / Zulaufventil 13 geschlossen\")"),
    ("2030", 41, "ventil13_frage", [["Intake valve 13.", "Will you open it"]], True, None,
     "PORT-WAHL, Frageform ROOM2050 msg 0 @0x2446"),
    ("2030", 42, "ventil13_offen", [["You've opened valve 13.", "The pressure is rising."]], False, None,
     "PORT-WAHL, Form ROOM2030 msg 3 @0x20AC"),
    ("2030", 43, "bruecke_frage", [["The pressure gauge reads 80.", "Lower the bridge"]], True, None,
     "NUTZER-VORGABE \"Lower the bridge?\" (AUFTRAG.md)"),
    ("2030", 44, "bruecke_unten", [["The bridge is down."]], False, None,
     "PORT-WAHL, Zustandssatz wie ROOM2060 msg 6 @0x1892 \"The fuse is in place.\""),
    ("2030", 45, "ventile_zurueck", [["The valves turned back", "to their original position."]], False, None,
     "PORT-WAHL (Nachbesserung 4, Nachtrag 13 Folge 4: falsche Reihenfolge -> zurueckgesetzt; Form ROOM11F0 sub17 "
     "@0x016C0..@0x016E4 Schalterbits := 0, RE2 ROOM2130 sub04 @0x017D6/@0x01800 Meldung + Fehlerton)"),
    ("2030", 46, "warnung", [["WARNING: The water will rise", "in one minute. Leave the room!"]], False, None,
     "NUTZER-VORGABE (sinngemaess: \"Warnung, dass das Wasser in einer Minute steigt und man den Raum verlassen soll\")"),
    ("2030", 47, "ventil13_zustand", [["Intake valve 13 is open."]], False, None, "PORT-WAHL, Zustandssatz"),
    ("2030", 48, "treppe_flut", [["I cannot go down the stairs", "because of the water."]], False, None,
     "PORT-WAHL (Nachbesserung 3, Nachtrag 8: unterer Bereich nach dem Strom geflutet und gesperrt), Form der Leiter-Vorgabe Id 40"),
    ("2030", 49, "ventile_vorher", [["I should look around here first."]], False, None,
     "Nachtrag 13 Folge 1 (Vorschlag des Orchestrators woertlich: kein Wissen, das der Spieler nicht haben kann; Gate der "
     "Nachtraege 9/10 bleibt: Bruecke unten UND Sicherung aufgehoben)"),
    ("2030", 50, "ventil_01", [["Valve 01, black wheel.", "Will you turn it"]], True, None,
     "PORT-WAHL (Nachtrag 13 Folge 4; Nummer + Farbe aus dem Hintergrund, Dossier §0e B4-2), Frageform ROOM2050 msg 0 @0x2446"),
    ("2030", 51, "ventil_02", [["Valve 02, green wheel.", "Will you turn it"]], True, None, "PORT-WAHL wie Id 50"),
    ("2030", 52, "ventil_03", [["Valve 03, red wheel.", "Will you turn it"]], True, None, "PORT-WAHL wie Id 50"),
    ("2030", 53, "ventil_07", [["Valve 07, black wheel.", "Will you turn it"]], True, None, "PORT-WAHL wie Id 50"),
    ("2030", 54, "ventil_08", [["Valve 08, green wheel.", "Will you turn it"]], True, None, "PORT-WAHL wie Id 50"),
    ("2030", 55, "ventil_09", [["Valve 09, red wheel.", "Will you turn it"]], True, None, "PORT-WAHL wie Id 50"),
    ("2050", 40, "hebel_ohne_strom", [["The pump has no power.", "The switchboard must be set first."]], False, None,
     "PORT-WAHL (Nutzer: Wasserlevel erst nach dem Pult-Raetsel), Form ROOM2060 msg 1 @0x15D0 Sperrsatz"),
    ("2050", 43, "pult_beschreibung", [["A high voltage switchboard.", "The pump needs 80 volts."]], False, None,
     "PORT-WAHL (Nutzer: \"Ziel 80 V wie 11F0 Notstrom\")"),
    ("2050", 44, "pult_zustand", [["The switchboard is set to 80 volts.", "The pump is powered."]], False, None,
     "PORT-WAHL, Zustandssatz"),
    ("20B0", 41, "ring_tuer", [["I cannot open the door", "because of the water pressure."]], False, None,
     "PORT-WAHL (Orchestrator-Entscheid C-E2: Zugaenge ins geflutete 2030 gesperrt), Form der Leiter-Vorgabe Id 40"),
    ("2070", 40, "ring_tuer", [["I cannot open the door", "because of the water pressure."]], False, None,
     "PORT-WAHL (Orchestrator-Entscheid C-E2), wie 20B0 Id 41"),
]


def texte():
    zeilen = ["/* AUTOMATISCH ERZEUGT von re15_port/tools/r41_ventilkette/gen_r41_ventilkette.py - nicht von Hand aendern.",
              " * Port-Nachrichten der Ventilkette (Runde 41 Spur C). Glyphen = msg_common.c re15_msg_glyph (umgekehrt),",
              " * Satzformen: Kopf `04 02`, Satzende 0x57 + `01 00` (ROOM2050 msg 1 @0x247E), Frage `1b 03 02 01 00`",
              " * (ROOM2050 msg 0 @0x2446), Zeile 0x08, Seite `02 00` (ROOM2080 msg 0 @0x0CF8). Jeder Text ist nach dem",
              " * Kodieren wieder dekodiert worden (Pruefung im Generator). Wortlaute und Herkunft je Eintrag. */", ""]
    tabelle = []
    for raum, mid, name, seiten, frage, gruen, herkunft in TEXTE:
        b = kodieren(seiten, frage, gruen)
        txt, f = dekodieren(b)
        soll = " | ".join(" / ".join(s) for s in seiten) + ("?" if frage else "")
        assert txt == soll and f == frage, (name, txt, soll, f, frage)
        sym = "re15_r41c_msg_%s_%d_%s" % (raum, mid, name)
        zeilen.append("/* ROOM%s Id %d \"%s\" - %s */" % (raum, mid, soll, herkunft))
        zeilen.append(inc_array(sym, b, 24))
        zeilen.append("")
        tabelle.append((raum, mid, sym, len(b)))
    zeilen.append("/* Tabelle (Raumgruppe = Leon-Raum, gilt auch fuer die Elza-Variante |1). */")
    zeilen.append("typedef struct { uint16_t raum; uint8_t id; const unsigned char *b; int n; } re15_r41c_text_t;")
    zeilen.append("static const re15_r41c_text_t re15_r41c_texte[] = {")
    for raum, mid, sym, n in tabelle:
        zeilen.append("    { 0x%s, %d, %s, %d }," % (raum, mid, sym, n))
    zeilen.append("};")
    zeilen.append("#define RE15_R41C_TEXTE_N ((int)(sizeof re15_r41c_texte / sizeof re15_r41c_texte[0]))")
    ziel = os.path.join(GEN, "ventilkette_r41_texte.inc")
    open(ziel, "w", newline="\n").write("\n".join(zeilen) + "\n")
    print("geschrieben:", ziel, "%d Texte" % len(tabelle))


# ---- 3. Original-Scheiben --------------------------------------------------------------------------------------
SCHEIBEN = [
    # (Datei, Offset, Laenge, Symbol, Beschreibung)
    ("STAGE2/ROOM2080.RDT", 0x00C86, 0x6C, "re15_r41c_org_2080_sub02",
     "ROOM2080 sub02 @0x00C86..@0x00CF1 (Rechner: Frage, Set(3,245), Wasser sinkt, Cut 7/3) - 108 B inkl. Evt_end"),
    ("STAGE2/ROOM2050.RDT", 0x023F2, 0x1A, "re15_r41c_org_2050_sub09",
     "ROOM2050 sub09 @0x023F2..@0x0240B (Pult-Einstieg: Cut 15, Slot 8 aus, (5,4..6)=1) - 26 B inkl. Evt_end"),
    ("STAGE2/ROOM2050.RDT", 0x01D38, 0x14, "re15_r41c_org_2050_slot8",
     "ROOM2050 main00 @0x01D38 Aot_set Slot 8 sce 3 Ereignis 9 (4000,-8450,1800,800) Band 3 - 20 B"),
    ("STAGE2/ROOM2050.RDT", 0x0201A, 0x0A, "re15_r41c_org_2050_kisten",
     "ROOM2050 sub01 @0x0201A Aot_reset Slot 6 sce 3 sat 0x31 -> Ereignis 2 (Kisten im Becken; Nachbesserung 1, M1) - 10 B"),
    ("STAGE2/ROOM2050.RDT", 0x01D4C, 0x22, "re15_r41c_org_2050_obj8",
     "ROOM2050 main00 @0x01D4C Obj_model_set obj 8 Typ 4 (-19554,0,22684) = Cursor - 34 B"),
    ("STAGE2/ROOM2000.RDT", 0x0169E, 0x20, "re15_r41c_org_2000_slot4",
     "ROOM2000 main00 @0x0169E Door_aot_set Slot 4 = Leiter nach ROOM2050 - 32 B"),
    ("STAGE2/ROOM2030.RDT", 0x01B6C, 0x22, "re15_r41c_org_2030_obj0",
     "ROOM2030 main00 @0x01B6C Obj_model_set obj 0 = Bruecke (-1075,-1800,-275) - 34 B"),
    ("STAGE2/ROOM2080.RDT", 0x00C1A, 0x14, "re15_r41c_org_2080_slot4",
     "ROOM2080 sub00 @0x00C1A Aot_set Slot 4 sce 3 Ereignis 2 = Rechner (-12050,-7250,1000,1000) Band 8 - 20 B"),
    ("STAGE2/ROOM2030.RDT", 0x01B8E, 0x14, "re15_r41c_org_2030_slot8",
     "ROOM2030 main00 @0x01B8E Aot_set Slot 8 sce 12 = Treppe Ring -> Nord-Landeplatz (-6050,-8400,3700,1400) Band 0x80 - 20 B"),
    ("STAGE2/ROOM2030.RDT", 0x01BA6, 0x30, "re15_r41c_org_2030_unten",
     "ROOM2030 main00 @0x01BA6..@0x01BD5 Ck(3,108)=0, Slot 9 Sicherung (Ereignis 6, Band 1), Endif, Slot 10 Item 0x16 x14 (Band 1) - 48 B"),
    ("STAGE2/ROOM2030.RDT", 0x01E10, 0x20, "re15_r41c_org_2030_sub01",
     "ROOM2030 sub01 @0x01E10..@0x01E2F Ck(3,126)=0 -> Lift-Rufzonen unten Slot 5/7 = Text msg 1 - 32 B"),
    ("STAGE2/ROOM2030.RDT", 0x01FE4, 0x26, "re15_r41c_org_2030_sub06",
     "ROOM2030 sub06 @0x01FE4..@0x02009 Frage msg 2, Ja -> Set(3,108) @0x01FF2, Slot 9 aus, msg 3 - 38 B"),
    ("STAGE2/ROOM2060.RDT", 0x01292, 0x20, "re15_r41c_org_2060_strom",
     "ROOM2060 sub01 @0x01292..@0x012B1 Ck(3,126)=0 + Schalter (5,6..9)=1,0,1,0 -> Evt_exec sub13 + Set(3,126) @0x012AE - 32 B"),
    ("STAGE1/ROOM11F0.RDT", 0x018D8, 0x19, "re15_r41c_org_11f0_msg1",
     "ROOM11F0 msg 1 @0x18D8 \"Will you push them? \" - 25 B"),
    ("STAGE1/ROOM11F0.RDT", 0x018F1, 0x18, "re15_r41c_org_11f0_msg2",
     "ROOM11F0 msg 2 @0x18F1 \"Power supply OK.\" - 24 B"),
]


def original():
    zeilen = ["/* AUTOMATISCH ERZEUGT von re15_port/tools/r41_ventilkette/gen_r41_ventilkette.py - nicht von Hand aendern.",
              " * Original-Byte-Scheiben (Runde 41 Spur C), Byte fuer Byte aus den ausgelieferten RDTs; Offset = Datei-Offset.",
              " * Der Riegel unit_r41_ventilkette vergleicht sie erneut mit den RDTs. */", ""]
    for datei, off, n, sym, besch in SCHEIBEN:
        d = open(os.path.join(CD, datei), "rb").read()
        b = d[off:off + n]
        zeilen.append("/* %s @0x%05X %d B md5 %s: %s */" % (datei, off, n, hashlib.md5(b).hexdigest(), besch))
        zeilen.append(inc_array(sym, b, 20))
        zeilen.append("#define %s_OFF 0x%05Xu" % (sym.upper(), off))
        zeilen.append("")
    ziel = os.path.join(GEN, "ventilkette_r41_original.inc")
    open(ziel, "w", newline="\n").write("\n".join(zeilen) + "\n")
    print("geschrieben:", ziel, "%d Scheiben" % len(SCHEIBEN))


if __name__ == "__main__":
    import sys
    sys.path.insert(0, HIER)
    from programme import programme   # 4. Port-Programme (SCD-Bytecode, Blocklaengen automatisch)
    wasser()
    texte()
    original()
    programme(CD, GEN)
