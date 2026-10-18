#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Runde 39 Spur P "johnruf1041": SCD-Programme und Text als engine/src/gen/johnruf_r39_programme.inc.

  python re15_port/tools/r39_johnruf/programme_bauen.py [--pruefen]   (druckt das Listing)

Dossier: analysis/befunde_runde39/P_johnruf1041.md, Konstanten: include/re15_johnruf_r39.h (der Riegel
test_r39_johnruf1041 prueft beide gegeneinander). Jede Zeile = ausgelieferte Original-Bytes (Datei-Offset in
re15_port/shared_assets/PSX/STAGE1/ROOM1041.RDT, vor dem Kopieren geprueft) oder eine eingefuegte Zeile in
ausgelieferter Opcode-Form (Vorbild je Zeile im Listing). Text-Kodierung/Breitenpruefung = Werkzeug der Runde 38 Spur F
(tools/r38_dining/programme_bauen.py: enc/breite), Textform = ROOM1041 msg 2 @0x022DC.

PROGRAMME (NUTZER-VORGABE Nachtrag 10, Ablauf des Orchestrators in AUFTRAG.md)
  RUF1041  ROOM1041 (Elza), gestartet vom Installer beim Betreten, solange John "verschwunden" ist ((4,0xF0)=1, ROOM1021
           sub02 @0x01FE0) und die Panikphase noch nicht lief (Bank 9 Bit 97 = 0). Form = ROOM1041 sub11 @0x021EA..@0x0223C
           (das Original "Elza hoert etwas" in diesem Raum: Szenen-Rahmen, Geraeusch, Drehen, Satz, Steuerung zurueck):
           zwei LEISE Hilferufe Johns (help03, help02 "Please help!"; RE2 Xa_vol 50 = ROOM11D0.RDT @0x01F84), Elza dreht
           sich zur Treppe, Xa_vol 120 (Standard), msg 40 "Elza: Oh, no, this is John! ...", Warte-Zeile 59 = Karte mit
           ROOM1141 (Kartenhinweis, Faden parkt bis zum Schliessen), dann Set(9,97)=1 = Panikphase, Steuerung zurueck.
  RUF1131  ROOM1131 (Flur vor 1141) in der Panikphase: ein Hilferuf help02 "Please help!", LAUTER als in 1041 (RE2 Xa_vol 100 =
           ROOM1020.RDT @0x01960 `5f 64`, Begruendung Dossier 4.2), ohne Szene (Elza laeuft weiter).
"""
import importlib.util, io, os, struct, sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(os.path.dirname(HIER))
REPO = os.path.dirname(PORT)
OUT = os.path.join(PORT, "engine", "src", "gen", "johnruf_r39_programme.inc")
sys.path.insert(0, os.path.join(PORT, "tools"))
import scd_walk_lib as L  # noqa: E402

_spec = importlib.util.spec_from_file_location("r38bau", os.path.join(PORT, "tools", "r38_dining", "programme_bauen.py"))
R = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(R)

D1041 = open(os.path.join(PORT, "shared_assets", "PSX", "STAGE1", "ROOM1041.RDT"), "rb").read()

# ---- Konstanten (muessen mit include/re15_johnruf_r39.h uebereinstimmen; der Riegel vergleicht) ----
MSG_SATZ = 40                   # RE15_R39P_MSG_SATZ        (VERTRAG 1.2: P = 1041 Id 40; Stimme room1041/main40.wav liegt)
HILFE_ID0 = 53                  # RE15_R39P_HILFE_ID0       Sprach-Ids 53..57 = help01..05 (dieselbe Zuordnung wie Spur J
                                #                           RE15_R39J_HILFE_ID0 in 1141; in 1041/1131 nur Stimme, kein Text)
MSG_KARTE = 59                  # RE15_R39P_MSG_KARTE       Port-Warte-Zeile ohne Text: Kartenhinweis + Parken
BIT_PANIK = 97                  # RE15_R39P_BIT_PANIK       Bank 9 (VERTRAG 1.1: P = 97)
XA_LEISE = 50                   # RE15_R39P_XA_LEISE        RE2 ROOM11D0.RDT @0x01F84 `5f 32`
XA_LAUTER = 100                 # RE15_R39P_XA_LAUTER       RE2 ROOM1020.RDT sub02 @0x01960 `5f 64` (dieselbe XA-Stimme
                                #   0x0B wie ROOM11D0 @0x01F7A mit 70 - RE2 regelt dieselbe Stimme je Raum; 100 statt 70
                                #   GEMESSEN: in 1131 laeuft MAIN01, mit 70 laege der Ruf nur +2,9 dB ueber der Musik gegen
                                #   +4,2 dB des leisen Rufs in 1041 ueber dessen Raumklang - Dossier 4.2)
XA_NORMAL = 120                 # RE15_R39P_XA_NORMAL       RE2-Standard @0x80012958 / @0x80053014 (`li v0,0x78`)
RUF1041 = (3, 2)                # RE15_R39P_RUF1041_A/_B    help03, dann help02 ("Please help!") - PORT-WAHL, Dossier 3
RUF1131 = 2                     # RE15_R39P_RUF1131         help02 "Please help!" ("das please help ... noch mal")
TREPPE = (-19650, -13000)       # RE15_R39P_TREPPE_X/_Z     Mitte Tuer slot 2 -> ROOM1061 (Treppe nach oben):
                                #   ROOM1041 main00 @0x010D8 `3b 02 02 31 00 00 50 b0 ec c8 dc 05 98 08` = x -20400 + 1500/2,
                                #   z -14100 + 2200/2
SUB11 = (0x021EA, 0x0223E)      # ROOM1041 sub11 (Form der ganzen 1041-Szene)

prog = []


def s16(v):
    return struct.pack("<h", v)


def op(b, c):
    prog.append((bytes(b), c))


def orig(off, erwartet, c):
    """Eine Original-Zeile aus ROOM1041 BYTEGLEICH (Bytes geprueft)."""
    b = D1041[off:off + len(erwartet)]
    if b != bytes(erwartet):
        sys.exit("ABBRUCH: ROOM1041 @0x%05X erwartet %s, ist %s" % (off, bytes(erwartet).hex(" "), b.hex(" ")))
    op(b, "ROOM1041 sub11 @0x%05X %s %s" % (off, L.NAMES.get(b[0], "?") if len(b) > 1 else "", c))


def sleep(n, c):
    op([0x09, 0x0A, n & 0xFF, (n >> 8) & 0xFF], c)


def xa_vol(v, c):
    if not 0 <= v < 0x80:
        sys.exit("ABBRUCH: Xa_vol %d (RE2 @0x800580a0 `sltiu v0,v1,0x80`)" % v)
    op([0x5F, v], "Xa_vol(%d) = RE2-Opcode 0x5F (Verteiler @0x800a74c8[0x5F] -> 0x80058090: `lbu v1,1(v0)` / "
                  "`sw v1,0x5b50(at)`, pc += 2) %s" % (v, c))


def ruf(k, c):
    op([0x2B, HILFE_ID0 + k - 1, 0x00, 0x00],
       "Message_on %d = Sprach-Zeile help%02d.wav OHNE Text (Port; Form `2b id 00 00` wie sub11 @0x02224; der "
       "Stimmen-Riegel in op_message_on laesst den vorigen Ruf ausreden) %s" % (HILFE_ID0 + k - 1, k, c))


def baue_ruf1041():
    del prog[:]
    orig(0x021F8, [0x22, 0x02, 0x07, 0x01], "Set(2,7)=1 Pad-Sperre (Szenen-Rahmen)")
    orig(0x021FC, [0x22, 0x01, 0x1B, 0x01], "Set(1,0x1B)=1 Balken")
    sleep(30, "Sleep 30 (Elza ist eben durch die Tuer; RE2 ROOM1060 sub05 +0x2E `09 0a 1e 00`, wie HAEMMERN39 der Spur J)")
    xa_vol(XA_LEISE, "- LEISE (NUTZER-VORGABE \"die Hilferufe sollen leise sein\"; Wert = RE2 ROOM11D0.RDT @0x01F84 `5f 32`)")
    ruf(RUF1041[0], "- erster Ruf (an der Stelle der Original-Meldung @0x02200 `2b 03 00 00` \"gunshot\" = Geraeusch)")
    orig(0x02204, [0x2E, 0x01, 0x00], "Work_set(1,0) Elza")
    orig(0x02207, [0x00], "Nop")
    orig(0x02208, [0x09, 0x0A, 0x19, 0x00], "Sleep 25")
    op(b"\x40\x00\x09\x20" + s16(TREPPE[0]) + s16(TREPPE[1]),
       "Plc_dest Elza Modus 9 (Drehen), Bit 0x20 = Form sub11 @0x0220C `40 00 09 20 3c ab 04 42`; Ziel = Tuer zur Treppe "
       "ROOM1061 (%d,%d) - PORT-WAHL: der Ruf kommt von oben (1041 hat im Original keinen Ruf Johns)" % TREPPE)
    orig(0x02214, [0x11, 0x00, 0x08, 0x00], "Do")
    orig(0x02218, [0x02], "Evt_next")
    orig(0x02219, [0x00], "Nop")
    orig(0x0221A, [0x12, 0x04], "Edwhile")
    orig(0x0221C, [0x21, 0x05, 0x20, 0x00], "Ck(5,0x20)==0 (bis die Drehung steht)")
    orig(0x02220, [0x09, 0x0A, 0x19, 0x00], "Sleep 25")
    ruf(RUF1041[1], "- zweiter Ruf \"Please help!\" (leise; derselbe Ruf kommt in 1131 lauter, NUTZER-VORGABE \"noch mal\")")
    sleep(90, "Sleep 90 (RE2 ROOM1060 sub05 +0x58 `09 0a 5a 00`; help02 1,96 s = 59 Bilder: Ruf ist aus, bevor Xa_vol wechselt)")
    xa_vol(XA_NORMAL, "- zurueck auf den Standard vor Elzas Zeile (Form RE2 ROOM1010 sub22: Xa_vol(120) vor dem Dialog)")
    b = bytearray(D1041[0x02224:0x02228])
    if bytes(b) != bytes([0x2B, 0x02, 0x00, 0x00]):
        sys.exit("ABBRUCH: ROOM1041 @0x02224")
    b[1] = MSG_SATZ
    op(bytes(b), "ROOM1041 sub11 @0x02224 Message_on 2 -> Id 40 \"Elza: Oh, no, this is John! ...\" (NUTZER-VORGABE; "
                 "Stimme synchro/STAGE1/room1041/main40.wav per Ordnerregel)")
    orig(0x02228, [0x09, 0x0A, 0x64, 0x00], "Sleep 100")
    op([0x2B, MSG_KARTE, 0x00, 0x00], "Message_on 59 = Port-Warte-Zeile ohne Text: der Stimmen-Riegel laesst Elzas Zeile "
                                      "ausreden (main40.wav 6,0 s), dann Kartenhinweis ROOM1041 -> ROOM1141 (RE2 Opcode 0x84 "
                                      "@0x800591C4; Port-Form Spur H: Faden parkt, bis der Schirm wieder zu ist)")
    op([0x22, 0x09, BIT_PANIK, 0x01], "Set(9,97)=1 Panikphase (VERTRAG 1.1; Set-Form sub11 @0x021EA `22 03 ad 01`) - Musik "
                                      "und Folgen (Spuren Q/R) haengen an diesem Bit")
    orig(0x0222C, [0x22, 0x02, 0x07, 0x00], "Set(2,7)=0")
    orig(0x02230, [0x22, 0x01, 0x1B, 0x00], "Set(1,0x1B)=0")
    orig(0x02234, [0x3C, 0x01], "Cut_auto 1")
    orig(0x02236, [0x2E, 0x01, 0x00], "Work_set(1,0)")
    orig(0x02239, [0x00], "Nop")
    orig(0x0223A, [0x42], "Plc_ret")
    orig(0x0223B, [0x00], "Nop")
    orig(0x0223C, [0x01, 0x00], "Evt_end")
    return list(prog)


def baue_ruf1131():
    del prog[:]
    sleep(30, "Sleep 30 (Elza ist eben durch die Tuer; RE2 ROOM1060 sub05 +0x2E)")
    xa_vol(XA_LAUTER, "- \"schon etwas lauter\" (NUTZER-VORGABE; Wert = RE2 ROOM1020.RDT @0x01960 `5f 64`, dieselbe XA-Stimme "
                      "0x0B wie ROOM11D0 @0x01F7A mit 70; Wahl gemessen, Dossier 4.2)")
    ruf(RUF1131, "- \"das please help von John auch noch mal\"")
    sleep(90, "Sleep 90 (RE2 ROOM1060 sub05 +0x58; der Ruf ist vor dem Zuruecksetzen aus)")
    xa_vol(XA_NORMAL, "- zurueck auf den Standard (Form RE2 ROOM1010 sub22)")
    op([0x01, 0x00], "Evt_end (keine Szene: Elza laeuft waehrenddessen weiter)")
    return list(prog)


# ---------------------------------------------------------------- Text
TEXTE = []


def text(name, kopf, zeilen, beschr):
    """Dialogzeile in der Form von ROOM1041 msg 2 @0x022DC: `04 00 05 01 <Elza:> 05 00 <Text> 04 01 01 63`."""
    for z in zeilen:
        if R.breite(z) > 271:
            sys.exit("ABBRUCH: Zeile zu breit (%d px > 271): %r" % (R.breite(z), z))
    sprecher, farbe = kopf
    erste = zeilen[0][len(sprecher):]
    b = bytes([0x04, 0x00, 0x05, farbe]) + R.enc(sprecher) + bytes([0x05, 0x00]) + R.enc(erste)
    for z in zeilen[1:]:
        b += b"\x08" + R.enc(z)
    b += bytes([0x04, 0x01, 0x01, 0x63])
    TEXTE.append((name, b, beschr + "  [Zeilenbreiten %s px]" % ", ".join(str(R.breite(z)) for z in zeilen)))


def baue_texte():
    msg2 = D1041[0x022DC:0x022DC + 31]
    if msg2[:4] != bytes([0x04, 0x00, 0x05, 0x01]) or msg2[-4:] != bytes([0x04, 0x01, 0x01, 0x63]):
        sys.exit("ABBRUCH: ROOM1041 msg 2 @0x022DC Form (%s)" % msg2.hex(" "))
    if msg2[4:4 + 5] != R.enc("Elza:"):
        sys.exit("ABBRUCH: ROOM1041 msg 2 Sprecher")
    text("k_r39p_msg40", ("Elza:", 0x01), ["Elza: Oh, no, this is John!", "He is in trouble, I have to help him!"],
         "ROOM1041 msg 40: NUTZER-VORGABE Nachtrag 10 (\"Oh, no, this is John! He is in Trouble, I have to help him!\"; "
         "Schreibung nach Nachtrag 6 \"trouble\" klein = derselbe Wortlaut wie Spur J msg 50); Form ROOM1041 msg 2 "
         "@0x022DC (Elza = Farbe 1 `05 01`), Zeilenumbruch `08`")


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


def main():
    pruefen = "--pruefen" in sys.argv
    programme = [("k_r39p_p_ruf1041", baue_ruf1041()), ("k_r39p_p_ruf1131", baue_ruf1131())]
    baue_texte()
    f = io.StringIO()
    f.write("/* GENERIERT von re15_port/tools/r39_johnruf/programme_bauen.py - NICHT HAND-EDITIEREN.\n"
            " * Runde 39 Spur P: SCD-Programme (Original-Bytes ROOM1041 sub11 + eingefuegte Zeilen, Listing beim\n"
            " * Werkzeugaufruf) und Text. Dossier: analysis/befunde_runde39/P_johnruf1041.md. */\n\n")
    f.write("#define RE15_R39P_ANKER_SUB11_OFF 0x%05Xu  /* ROOM1041 sub11 */\n" % SUB11[0])
    f.write("#define RE15_R39P_ANKER_SUB11_LEN %du\n" % (SUB11[1] - SUB11[0]))
    f.write("#define RE15_R39P_ANKER_SUB11_FNV 0x%08Xu\n\n" % fnv(D1041[SUB11[0]:SUB11[1]]))
    for name, zeilen in programme:
        b = b"".join(x for x, _ in zeilen)
        f.write("/* %s: %d Byte\n" % (name, len(b)))
        o = 0
        for x, k in zeilen:
            f.write(" *   +%04X %-36s %s\n" % (o, x.hex(" ")[:36], k.replace("*/", "* /")))
            o += len(x)
        f.write(" */\n")
        carr(f, name, b)
        f.write("\n")
    for name, b, beschr in TEXTE:
        f.write("/* %s */\n" % beschr.replace("*/", "* /"))
        carr(f, name, b)
        f.write("\n")
    text_out = f.getvalue()
    for name, zeilen in programme:
        print("== %s (%d B)" % (name, sum(len(x) for x, _ in zeilen)))
        o = 0
        for x, k in zeilen:
            print("  +%04X %-40s %s" % (o, x.hex(" ")[:40], k))
            o += len(x)
    for name, b, beschr in TEXTE:
        print("== %s: %s\n   %s" % (name, beschr, b.hex(" ")))
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
