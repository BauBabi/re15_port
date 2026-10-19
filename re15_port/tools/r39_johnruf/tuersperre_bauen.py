#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Runde 39 Spur P "johnruf1041", Nachbesserung 1: Tuersperren (Texte, Szenen-Huelle 10B1, Anker) als
engine/src/gen/tuersperre_r39.inc.

  python re15_port/tools/r39_johnruf/tuersperre_bauen.py [--pruefen]   (druckt das Listing; --pruefen vergleicht nur)

Nutzer (AUFTRAG.md Nachtrag 12, woertlich): "beim Musikwechsel, wenn Elza John retten soll, soll beim anklciken der Tuer
von ROOM 1041 zu Room 1031 kommen "I have to rescue John! He is in danger!". Auch bei der Tuer 1051 zu 10A1." und
"Ausserdem moechte ich das in Elzas Szenario die Tuer im ROOM 10B1 ausnahmslos IMMER geschlossen ist. auch wenn sie nach
Sherry sucht." Dossier analysis/befunde_runde39/P_johnruf1041.md Abschnitt 8, Konstanten include/re15_tuersperre_r39.h
(der Riegel test_r39_johnruf1041_tuer prueft beide gegeneinander und gegen die ausgelieferten RDTs).

TUER-SAETZE (Door_aot_set, ausgeliefert, vom Werkzeug geprueft):
  ROOM1041 main00 @0x010B8 Slot 1 -> Raum 0x03 = ROOM1031 Cut 8
  ROOM1051 main00 @0x00B5A Slot 4 -> Raum 0x0A = ROOM10A1 Cut 0
  ROOM10B1 main00 @0x016F2 Slot 1 -> Raum 0x26 = ROOM1261 Cut 15 (Kanaldeckel; Slot 0 @0x016D2 ist die Leiter zum Dach)
TEXTE:
  RETTUNG  ROOM1041 msg 41 und ROOM1051 msg 40 (Ids: Orchestrator Nachbesserung 1): NUTZER-VORGABE "I have to rescue John!
           He is in danger!" in Erzaehlform `04 02 <Zeile> 08 <Zeile> 01 00` wie die Tuersperren 1141 (Runde 38 F msg 41) /
           11C0 (Spur O msg 40); Original-Form RE1.5 ROOM11B1 sub01 @0x011C0 msg 14 und ROOM1090 msg 7 @0x28E6 (Gedanke der
           Figur an einem MESSAGE-Satz, `04 02`), RE2 ROOM4040 sub03 @0x00E9E msg 9 "I have to catch / that woman!".
  DECKEL   ROOM10B1 msg 40: RE2 ROOM21A0 msg 4 @0x02963 (der Deckel, den die Figur nie oeffnen kann; MAIN sub03 @0x01EC6
           `46 02 04 31 04 00 00 00 ff ff`), Zeichen fuer Zeichen, RE2-Steuercodes in RE1.5-Form: `fa 02` -> `04 02`,
           `fc` -> `08` (Zeile), `fd 00` -> `02 00` (Seite; RE2 `fd` traegt immer ein Parameterbyte, 205x 00 im Zensus),
           RE2-Punkt `01` -> RE1.5 `57`, `fe 00` -> `01 00`. RE1.5 hat fuer einen Deckel keinen eigenen Text.
PROGRAMM:
  SZENE10B1  ersetzt das Ziel von ROOM10B1 sub01 @0x01824 `04 ff 18 04` (Evt_exec sub04, die Szene "John, wait!"), nur
             wenn der Deckel gesperrt ist: Gosub sub04 (Szene UNVERAENDERT; ihr Aot_on @0x01A8E `47 01` ruft den Handler,
             den Slot 1 GERADE hat = MESSAGE @0x80043084 -> Text statt Raumwechsel, LAB_800407bc @0x8004082c), dann das
             Szenen-Ende in RE1.5-Form (ROOM1051 sub02 @0x00CCC Evt_next hinter dem Text, @0x00D90..@0x00D9F Pad frei,
             Balken weg, Cut_auto, Plc_ret), weil sub04 Pad-Sperre @0x01966 und Balken @0x0196A nie loest (das tat im
             Original erst der Raumwechsel).
"""
import importlib.util, io, os, struct, sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(os.path.dirname(HIER))
REPO = os.path.dirname(PORT)
OUT = os.path.join(PORT, "engine", "src", "gen", "tuersperre_r39.inc")
sys.path.insert(0, os.path.join(PORT, "tools"))
import scd_walk_lib as L  # noqa: E402

_spec = importlib.util.spec_from_file_location("r38bau", os.path.join(PORT, "tools", "r38_dining", "programme_bauen.py"))
R = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(R)


def rdt(name):
    return open(os.path.join(PORT, "shared_assets", "PSX", "STAGE1", name), "rb").read()


D1041, D1051, D10B1, D10B0, D11A1, D11B1, D1090 = (rdt("ROOM1041.RDT"), rdt("ROOM1051.RDT"), rdt("ROOM10B1.RDT"),
                                                  rdt("ROOM10B0.RDT"), rdt("ROOM11A1.RDT"), rdt("ROOM11B1.RDT"),
                                                  rdt("ROOM1090.RDT"))
RE2_21A0 = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM21A0.RDT")

# ---- Konstanten (muessen mit include/re15_tuersperre_r39.h uebereinstimmen; der Riegel vergleicht) ----
MSG_1041 = 41          # RE15_R39P_TS_MSG_1041   (Orchestrator: 1041 = 41; Sprachdatei room1041/main41.wav)
MSG_1051 = 40          # RE15_R39P_TS_MSG_1051   (Orchestrator: 1051 = 40; Sprachdatei room1051/main40.wav)
MSG_10B1 = 40          # RE15_R39P_TS_MSG_10B1   (10B1 Original-Ids 0..5, msg_sec 0x1AB8; erste Port-Id wie 11C0/1051)
SUB_10B1 = 4           # RE15_R39P_TS_SUB_10B1   ROOM10B1 sub01 @0x01824 `04 ff 18 04`

ERWARTET = (   # (Datei, Offset, Bytes, Bedeutung) - das Werkzeug bricht ab, wenn das RDT anders aussieht
    (D1041, 0x010B8, "3b 01 02 31 00 00 50 b0 dc ec dc 05 98 08 9a 99 00 00 eb f0 00 00 00 03 08",
     "ROOM1041 main00 Door_aot_set Slot 1 -> Raum 0x03 (ROOM1031) Cut 8"),
    (D1051, 0x00B5A, "3b 04 02 31 00 00 3c 41 94 c6 e8 03 d0 07 58 66 c0 c7 e0 60 00 08 00 0a 00",
     "ROOM1051 main00 Door_aot_set Slot 4 -> Raum 0x0A (ROOM10A1) Cut 0"),
    (D10B1, 0x016F2, "3b 01 02 31 00 00 98 a9 e8 0d dc 05 34 08 e6 b0 f8 f8 d4 e5 00 04 00 26 0f",
     "ROOM10B1 main00 Door_aot_set Slot 1 -> Raum 0x26 (ROOM1261) Cut 15 (Kanaldeckel)"),
    (D10B1, 0x016D2, "3b 00 02 31 05 00", "ROOM10B1 main00 Door_aot_set Slot 0 (Leiter zum Dach, bleibt)"),
    (D10B1, 0x01816, "21 03 82 01", "ROOM10B1 sub01 Ck(3,0x82)==1 (Szene steht an, ROOM1171 sub02 @0x013A4)"),
    (D10B1, 0x01824, "04 ff 18 04", "ROOM10B1 sub01 Evt_exec sub04 (die Szene)"),
    (D10B1, 0x0195A, "22 03 82 00 22 03 83 01 22 03 41 01 22 02 07 01 22 01 1b 01",
     "ROOM10B1 sub04 Szenen-Kopf: (3,0x82)=0 (3,0x83)=1 (3,0x41)=1, Pad-Sperre (2,7)=1, Balken (1,0x1B)=1"),
    (D10B1, 0x01A8E, "47 01 01 00", "ROOM10B1 sub04 Aot_on Slot 1 + Evt_end (kein Set(2,7)=0)"),
    (D10B0, 0x016F2, "3b 01 02 31 00 00 98 a9 e8 0d dc 05 34 08 e6 b0 f8 f8 d4 e5 00 04 00 26 0f",
     "ROOM10B0 (Leon) derselbe Satz - bleibt offen"),
    (D11A1, 0x00FBC, "46 02 01 31 01 00 ff ff 00 00",
     "Form: RE1.5 ROOM11A1 main00 Aot_reset Slot 2 -> MESSAGE msg 1 'It's locked.' ohne Bedingung (Tuer immer zu)"),
    (D11B1, 0x011C0, "46 01 01 31 0e 00 ff ff 00 00",
     "Form: RE1.5 ROOM11B1 sub01 (je Bild) Aot_reset Slot 1 -> MESSAGE msg 14 unter Ck(4,0xF3)==0 @0x011A0"),
    (D1051, 0x00CCC, "02 00", "Form: ROOM1051 sub02 Evt_next + Nop hinter Message_on"),
    (D1051, 0x00D90, "22 02 07 00 22 01 1b 00 3c 01 2e 01 00 00 42 00 08 00 01 00",
     "Form: ROOM1051 sub02 Szenen-Ende Set(2,7)=0 Set(1,0x1B)=0 Cut_auto 1 Work_set(1,0) Plc_ret, Endif, Evt_end"),
)


def pruefe_rdts():
    for d, off, hexs, was in ERWARTET:
        soll = bytes.fromhex(hexs.replace(" ", ""))
        ist = d[off:off + len(soll)]
        if ist != soll:
            sys.exit("ABBRUCH: %s @0x%05X erwartet %s, ist %s" % (was, off, soll.hex(" "), ist.hex(" ")))


# ---------------------------------------------------------------- Texte
def text_rettung():
    satz = "I have to rescue John! He is in danger!"        # NUTZER-VORGABE, woertlich
    z1, z2 = "I have to rescue John!", "He is in danger!"     # Umbruch nach dem ersten Satz (wie 11C0/1141: 2 Zeilen)
    if z1 + " " + z2 != satz:
        sys.exit("ABBRUCH: Zeilen ergeben nicht den Nutzer-Satz")
    for z in (z1, z2):
        if R.breite(z) > 271:
            sys.exit("ABBRUCH: Zeile zu breit (%d px > 271): %r" % (R.breite(z), z))
    # Form-Probe: ROOM1090 msg 7 @0x28E6 (Gedanke an einem MESSAGE-Satz) beginnt `04 02`, bricht mit `08` um, endet `01 00`
    m7 = D1090[0x28E6:D1090.index(bytes([0x01, 0x00]), 0x28E6) + 2]
    if m7[:2] != b"\x04\x02" or m7[-2:] != b"\x01\x00" or b"\x08" not in m7:
        sys.exit("ABBRUCH: Form-Probe ROOM1090 msg 7 @0x28E6")
    return b"\x04\x02" + R.enc(z1) + b"\x08" + R.enc(z2) + b"\x01\x00", (z1, z2)


def text_deckel():
    d = open(RE2_21A0, "rb").read()
    re2 = d[0x02963:d.index(b"\xfe", 0x02963) + 2]
    # RE2-Original Zeichen fuer Zeichen umkodieren (Steuercodes s. Kopf); Glyphen A-Z/a-z/'/Leer sind in beiden gleich
    soll = bytes.fromhex("fa02fc304441004845400045 4f00 42454e494855003f484b4f414001fd00"
                         "25503a4f0045494c4b4f4f453e48410042 4b4e004941fc504b004b4c414a00455001fe00".replace(" ", ""))
    if re2 != soll:
        sys.exit("ABBRUCH: RE2 ROOM21A0 msg 4 @0x02963 ist %s" % re2.hex(" "))
    out = bytearray()
    i = 0
    while i < len(re2):
        b = re2[i]
        if b == 0xFA:   out += bytes([0x04, re2[i + 1]]); i += 2; continue      # Textkopf, Erzaehlform 2
        if b == 0xFC:   out.append(0x08); i += 1; continue                       # Zeile
        if b == 0xFD:   out += bytes([0x02, re2[i + 1]]); i += 2; continue      # Seite (Parameter 0)
        if b == 0xFE:   out += bytes([0x01, 0x00]); i += 2; continue            # Ende
        if b == 0x01:   out.append(0x57); i += 1; continue                       # RE2-Punkt -> RE1.5-Punkt
        out.append(b); i += 1
    zeilen = ("The lid is firmly closed.", "It's impossible for me", "to open it.")
    if bytes(out) != b"\x04\x02\x08" + R.enc(zeilen[0]) + b"\x02\x00" + R.enc(zeilen[1]) + b"\x08" + R.enc(zeilen[2]) \
            + b"\x01\x00":
        sys.exit("ABBRUCH: Umkodierung RE2 msg 4 -> %s" % bytes(out).hex(" "))
    for z in zeilen:
        if R.breite(z) > 271:
            sys.exit("ABBRUCH: Zeile zu breit (%d px > 271): %r" % (R.breite(z), z))
    return bytes(out), zeilen


# ---------------------------------------------------------------- Programm
def baue_szene10b1():
    z = []
    z.append((bytes([0x18, SUB_10B1]), "Gosub sub04 = die Szene UNVERAENDERT aus dem RDT (@0x0195A..@0x01A90, msg 0..5); ihr "
                                       "Aot_on @0x01A8E `47 01` trifft den gesperrten Slot 1 -> Text msg 40 statt Raumwechsel "
                                       "(Form Gosub-Huelle Spur O / Runde 38 F, Gosub @0x8003fc04)"))
    z.append((D1051[0x00CCC:0x00CCE], "ROOM1051 sub02 @0x00CCC Evt_next + Nop: die VM parkt unter RE15_PAUSE_SCD, bis der Text "
                                      "zu ist (FUN_8003f038 @0x8003f040-4c, Maske 0xffff0000 @0x800430a4)"))
    z.append((D1051[0x00D90:0x00D94], "ROOM1051 sub02 @0x00D90 Set(2,7)=0 Pad frei (sub04 @0x01966 setzt sie, loest sie nie)"))
    z.append((D1051[0x00D94:0x00D98], "ROOM1051 sub02 @0x00D94 Set(1,0x1B)=0 Balken weg (sub04 @0x0196A)"))
    z.append((D1051[0x00D98:0x00D9A], "ROOM1051 sub02 @0x00D98 Cut_auto 1"))
    z.append((D1051[0x00D9A:0x00D9E], "ROOM1051 sub02 @0x00D9A Work_set(1,0) Elza + Nop"))
    z.append((D1051[0x00D9E:0x00DA0], "ROOM1051 sub02 @0x00D9E Plc_ret + Nop (Elza wieder frei; sub04 @0x01986 Work_set(1,0))"))
    z.append((D1051[0x00DA2:0x00DA4], "ROOM1051 sub02 @0x00DA2 Evt_end"))
    return z


# ---------------------------------------------------------------- Ausgabe
def carr(f, name, b):
    f.write("static const uint8_t %s[%d] = {" % (name, len(b)))
    for i, x in enumerate(b):
        f.write(("\n    " if i % 16 == 0 else "") + "0x%02x," % x)
    f.write("\n};\n")


def fnv(b):
    h = 0x811C9DC5
    for x in b:
        h = ((h ^ x) * 0x01000193) & 0xFFFFFFFF
    return h


ANKER = (   # (Name, Datei, von, bis) - der C-Teil prueft diese Bereiche im GELADENEN RDT (FNV-1a), sonst bleibt alles Original
    ("T1041", D1041, 0x010B8, 0x010D8),      # Door_aot_set Slot 1 -> ROOM1031
    ("T1051", D1051, 0x00B5A, 0x00B7A),      # Door_aot_set Slot 4 -> ROOM10A1
    ("T10B1", D10B1, 0x016F2, 0x01712),      # Door_aot_set Slot 1 -> ROOM1261 (Kanaldeckel)
    ("S10B1", D10B1, 0x01812, 0x01A92),      # sub01 (Ausloeser) .. sub04 (Szene + Aot_on @0x01A8E)
)


def main():
    pruefen = "--pruefen" in sys.argv
    pruefe_rdts()
    t_rett, z_rett = text_rettung()
    t_deck, z_deck = text_deckel()
    szene = baue_szene10b1()
    f = io.StringIO()
    f.write("/* GENERIERT von re15_port/tools/r39_johnruf/tuersperre_bauen.py - NICHT HAND-EDITIEREN.\n"
            " * Runde 39 Spur P, Nachbesserung 1: Tuersperren 1041/1051 (Panikphase) und 10B1 (immer) - Texte, Szenen-Huelle,\n"
            " * Anker. Dossier: analysis/befunde_runde39/P_johnruf1041.md Abschnitt 8. */\n\n")
    for name, d, von, bis in ANKER:
        f.write("#define RE15_R39P_TS_ANKER_%s_OFF 0x%05Xu\n" % (name, von))
        f.write("#define RE15_R39P_TS_ANKER_%s_LEN %du\n" % (name, bis - von))
        f.write("#define RE15_R39P_TS_ANKER_%s_FNV 0x%08Xu\n" % (name, fnv(d[von:bis])))
    f.write("\n")
    b = b"".join(x for x, _ in szene)
    f.write("/* k_r39p_ts_szene10b1: %d Byte\n" % len(b))
    o = 0
    for x, k in szene:
        f.write(" *   +%04X %-24s %s\n" % (o, x.hex(" "), k.replace("*/", "* /")))
        o += len(x)
    f.write(" */\n")
    carr(f, "k_r39p_ts_szene10b1", b)
    f.write("\n/* ROOM1041 msg %d / ROOM1051 msg %d: NUTZER-VORGABE \"%s %s\" (AUFTRAG.md Nachtrag 12), Erzaehlform\n"
            " * `04 02 .. 08 .. 01 00` wie die Tuersperren 1141/11C0 und ROOM1090 msg 7 @0x28E6  [Zeilenbreiten %d, %d px <= 271] */\n"
            % (MSG_1041, MSG_1051, z_rett[0], z_rett[1], R.breite(z_rett[0]), R.breite(z_rett[1])))
    carr(f, "k_r39p_ts_msg_rettung", t_rett)
    f.write("\n/* ROOM10B1 msg %d: RE2 ROOM21A0 msg 4 @0x02963 \" / %s<fd>%s / %s\" in RE1.5-Kodierung (Kopf des Werkzeugs)\n"
            " *   [Zeilenbreiten %d, %d, %d px <= 271] */\n"
            % (MSG_10B1, z_deck[0], z_deck[1], z_deck[2], R.breite(z_deck[0]), R.breite(z_deck[1]), R.breite(z_deck[2])))
    carr(f, "k_r39p_ts_msg_deckel", t_deck)
    text_out = f.getvalue()
    print("== k_r39p_ts_szene10b1 (%d B)" % len(b))
    o = 0
    for x, k in szene:
        print("  +%04X %-24s %s" % (o, x.hex(" "), k))
        o += len(x)
    print("== k_r39p_ts_msg_rettung: %s\n   %s" % (" / ".join(z_rett), t_rett.hex(" ")))
    print("== k_r39p_ts_msg_deckel: %s\n   %s" % (" / ".join(z_deck), t_deck.hex(" ")))
    if pruefen:
        ok = os.path.exists(OUT) and open(OUT, encoding="utf-8").read() == text_out
        print("%s %s" % (OUT, "ok" if ok else "ANDERS"))
        return 0 if ok else 1
    with io.open(OUT, "w", encoding="utf-8", newline="\n") as fo:
        fo.write(text_out)
    print("->", OUT)
    return 0


if __name__ == "__main__":
    sys.exit(main())
