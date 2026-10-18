#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Runde 39 Spur J "dining1141": die SCD-Programme, Texte und der Klopf-Tonteil als engine/src/gen/dining_r39_programme.inc.

  python re15_port/tools/r39_dining/programme_bauen.py [--pruefen]   (druckt das Listing)

Baut auf dem Werkzeug der Runde 38 Spur F auf (tools/r38_dining/programme_bauen.py: Helfer kopie/op/ersetze, Text-
Kodierung, Breitenpruefung, die ROOM1141-Lagen der Runde 38). Jede Zeile = ausgelieferte Original-Bytes (Datei-Offset in
re15_port/shared_assets/PSX/STAGE1/ROOM1141.RDT, vor dem Kopieren geprueft) oder eine eingefuegte Zeile in ausgelieferter
Opcode-Form (Vorbild je Zeile im Listing). Dossier: analysis/befunde_runde39/J_dining1141.md; Konstanten:
include/re15_dining_r39.h (der Riegel test_r39_dining1141 prueft beide gegeneinander).

PROGRAMME (Nachbesserung 1: Ablauf nach Nutzer-Nachtrag 4)
  HAEMMERN39   ROOM1141 Phase H (Evt_exec sub02 @0x00B02 beim Betreten, statt HAEMMERN der Runde 38). John ist NICHT im
               Raum (auf dem Dach, Punkt 2; der Installer parkt ihn). Schleife solange die Dachtuer von innen verriegelt
               ist (While Ck(4,0xC3)==0 = main00 @0x00A76; vorher bis zum Rolladen): je Runde mehrfaches Klopfen (Punkt 4:
               RE2 DOOR0F Satz 0 dreimal, Abstand Sleep 5 = RE2 ROOM1090 @0x02BB0/@0x02BBC/@0x02BC0), Sleep 30 (RE2 ROOM1060
               sub05 +0x2E), ein Hilferuf help01..05 (Punkt 4, Reihenfolge MIX), Sleep 90 + Sleep 90 (+0x58/+0x5E). Nach dem
               ERSTEN Hilferuf (Bank 9 Bit 94 noch 0) Elzas Reaktion (Punkt 5), erst wenn keine Szene laeuft (Merker
               (5,0x10) des Gemaelde-Programms, Pad-Sperre (2,7)) und nur solange die Dachtuer verriegelt ist (M1).
  ENTRIEGELN39 ROOM1141 Ereignis 5 (Riegel der Dachtuer) in Phase H: sub05 (msg 12 "You've opened the lock."), dann
               Elza rennt zur Kamera Cut 7, dreht sich zur Tuer, hebt die Waffe (N4.2), Tuer auf, John innen vor die Tuer,
               Tuer zu, Kamera auf die Tuer (Cut 6, John steht schon da), Cut 5, Elza senkt die Waffe, John laeuft heraus;
               Szene der Runde 38 mit Punkt 6 (Glasbruch vor msg 6) und Punkt 7 (Skript wartet auf Johns Drehung vor msg 9);
               nach dem Tod der Gorillas Punkt 8 (zwei Zeilen Johns mit Gesten, Folge-Geste Clip 17), dann Johns Abgang.
  ROLLTOR39    ROOM1141 Ereignis 6 (Gemaelde) in Phase H: Rolladen hoch mit Ton (Runde 38), danach Steuerung zurueck (N4.1).
"""
import importlib.util, io, os, struct, sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(os.path.dirname(HIER))
REPO = os.path.dirname(PORT)
OUT = os.path.join(PORT, "engine", "src", "gen", "dining_r39_programme.inc")

_spec = importlib.util.spec_from_file_location("r38bau", os.path.join(PORT, "tools", "r38_dining", "programme_bauen.py"))
R = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(R)
prog = R.prog          # dieselbe Liste, die R.op/R.kopie fuellen

# ---- Konstanten (muessen mit include/re15_dining_r39.h uebereinstimmen; der Riegel vergleicht) ----
BANK = 0x3E                     # RE15_R39J_SE_BANK
SATZ_KLOPFEN = 0x00             # RE15_R39J_SATZ_KLOPFEN  DOOR0F Satz 0
SATZ_GLAS = 0x21                # RE15_R39J_SATZ_GLAS     RE2 ROOM1090 @0x02BB0 `36 02 21 ..`
SATZ_HILFE0 = 0x31              # RE15_R39J_SATZ_HILFE0   0x31..0x35 = help01..05
MSG_REAKTION, MSG_PUH, MSG_FOLGEN = 50, 51, 52
MSG_STIMME_WARTEN = 58          # RE15_R39J_MSG_STIMME_WARTEN (Port-Warte-Zeile, Punkt 6)
BIT_REAKTION = 94               # RE15_R39J_BIT_REAKTION (Bank 9, VERTRAG 1.1)
MIX = (3, 1, 4, 2, 5)           # PORT-WAHL "im Mix": Reihenfolge der help-Dateien je Durchlauf; die erste Runde ruft
                                #   help03 (1,57 s) - die Reaktion kommt nach Sleep 90 (3 s) sicher hinter dem Ruf
JOHN_TUER = (-7350, -1300)      # RE15_R39J_JOHN_TUER_X/Z
JOHN_TUER_YAW = 1024            # RE15_R39J_JOHN_TUER_YAW
ELZA_VORNE = (-7800, -12800)    # RE15_R39J_ELZA_VORNE_X/Z
ELZA_NACH = (-5800, -21500)     # RE15_R39J_ELZA_NACH_X/Z
ECKE = (-7550, -21500)          # RE15_R39J_ECKE_X/Z = sub02 @0x00F8E
CUT_ELZA, CUT_TUER, CUT_GESPRAECH = 7, 6, 8
import math
ELZA_BLICK_JOHN = int(round(math.atan2(-(ECKE[1] - ELZA_NACH[1]), ECKE[0] - ELZA_NACH[0]) / (2 * math.pi) * 4096)) % 4096
TUER_AUF_BILDER = 43            # DOOR1A Ton 0 = VAG 2, 1,43 s (tueren_02_re2.md 4: "1.43 (86)" in 60-Hz-Bildern) bei 30 Bildern/s
TUER_ZU_BILDER = 27             # DOOR1A Ton 1 = VAG 3, 0,89 s ("0.89 (53)") bei 30 Bildern/s
DOOR0F_TON = 0x2A88             # RE2 @0x8009A5D4 Halbwort 0 (gen/re2_tuer_tabelle.inc DOOR0F)
DOOR0F_CK = 0xDB                # RE2 @0x8009A5D4 +8: Pruefsumme Ton (XOR erstes Byte je 512 B, re2_tuer_ton.py)
DOOR0F_DATEI = 48076            # Dateigroesse laut derselben Tabelle
MERKER_BANK, MERKER_BIT = 5, 0x10  # RE15_R39J_MERKER_BANK/_BIT: Gemaelde-Programm laeuft (Nachbesserung 1 M1); Bank 5 =
                                #   Skript-Merker (ROOM1141 sub03/sub04 @0x00FA4/@0x00FB6 nutzen Bit 0/1, sonst keins)
MSG52_REST = 30                # RE15_R39J_MSG52_REST: Sleep nach Johns Folge-Geste (Abnahme H3: msg 52 stand 99 Bilder)
SCHUETZE_BANK, SCHUETZE_BIT = 5, 0x11  # RE15_R39J_SCHUETZE_BANK/_BIT: Johns Salve laeuft (Nachbesserung 1 N4.3)
SUB_SCHUETZE = 30               # RE15_R39J_SUB_SCHUETZE: Evt_exec-Nummer des Fadens SCHUETZE39 (< RE15_RDT_MAX_SUB_SCD 32;
                                #   ROOM1141 hat sub00..sub06; Port-Nummer wie ROOM1131 Ereignis 29, Runde 38)

D1141 = R.RDT["1141"]


def s16(v):
    return struct.pack("<h", v)


def op(b, c):
    R.op(b, c)


def se39(satz, c):
    op([0x36, BANK, satz] + [0] * 9, "Se_on(0x3E,0x%02X) %s (Form wie die Port-Baenke der Runde 38: `36 bank satz` + 9 x 0)" % (satz, c))


def ifel_c3(inner, c):
    """Ifel_ck { Ck(4,0xC3)==0  <inner> } Endif - Nachbesserung 1 (N4.1): Klopfen und Rufe laufen, solange die Dachtuer von
    innen verriegelt ist (Bedingung = ROOM1141 main00 @0x00A76 `21 04 c3 00`, die den Riegel-Slot 2 baut), nicht mehr nur
    bis zum Rolladen. Ifel-Form sub01 @0x00BFE (Laenge = Ck + Rumpf + Endif), Endif sub01 @0x00C10."""
    rumpf = 4 + len(inner) + 2
    op(b"\x06\x00" + struct.pack("<H", rumpf), "Ifel_ck Laenge %d (Form sub01 @0x00BFE)" % rumpf)
    op(b"\x21\x04\xc3\x00", "  Ck(4,0xC3)==0 = main00 @0x00A76 (Dachtuer noch verriegelt, John draussen)")
    op(bytes(inner), "  " + c)
    op(b"\x08\x00", "Endif = sub01 @0x00C10")


def sleep(n, c):
    R.sleep(n, c)


def warte_solange(bank, bit, c):
    """While Ck(bank,bit)==1 { Evt_next } - Form ROOM1141 sub03 @0x00FA8 `0f 04 08 00` + Ck + `02 00` + @0x00FB2 `10 00`
    (dort Ck(5,0)==0); hier mit Wert 1 (Ck pc[3] = Sollwert, sub00 @0x00AFE `21 04 f0 01`)."""
    op(b"\x0f\x04\x08\x00", "While Laenge 8 (Form sub03 @0x00FA8): %s" % c)
    op(bytes([0x21, bank, bit, 0x01]), "  Ck(%d,0x%02X)==1" % (bank, bit))
    op(b"\x02\x00", "  Evt_next + Nop (Form sub03 @0x00FB0)")
    op(b"\x10\x00", "Ewhile (Form sub03 @0x00FB2)")


def klopfen_gruppe():
    """Punkt 4: mehrfaches Klopfen = RE2 DOOR0F Satz 0 dreimal, Abstand 5 Bilder (Form RE2 ROOM1090 @0x02BB0 Se_on /
    @0x02BBC `09 0a 05 00` Sleep 5 / @0x02BC0 Se_on). Jeder Schlag in Ifel Ck(4,0xC3)==0: das Entriegeln der Dachtuer
    (sub05 @0x00FD2 `22 04 c3 01`) beendet das Klopfen sofort."""
    for k in range(3):
        ifel_c3([0x36, BANK, SATZ_KLOPFEN] + [0] * 9, "Se_on(0x3E,0) Klopfen %d/3 = RE2 DOOR0F Satz 0 (Holzschlag)" % (k + 1))
        if k < 2:
            sleep(5, "Sleep 5 = RE2 ROOM1090 sub15 @0x02BBC `09 0a 05 00`")


def reaktion():
    """Punkt 5: einmal (Bank 9 Bit 94) nach dem ersten Hilferuf beim Betreten. Formen aus ROOM1141 sub02.
    Nachbesserung 1 (M1): erst warten, bis keine Szene laeuft - das Gemaelde-Programm ROLLTOR39 haelt den Merker
    (5,0x10) (gesetzt SYNCHRON beim Ausloesen von Ereignis 6 in dining_r38.c, geloescht am Programmende), jede andere
    Szene haelt die Pad-Sperre (2,7) (sub02 @0x00C26 / sub06 @0x01004); und nur, solange die Dachtuer verriegelt ist
    (John ist dann noch draussen; nach dem Entriegeln waere die Zeile falsch)."""
    rumpf_start = len(prog)
    op([0x21, 0x09, BIT_REAKTION, 0x00], "  Ck(9,94)==0 (Elza hat noch nicht reagiert; Ck-Form sub00 @0x00B84 `21 03 6a 00`)")
    warte_solange(MERKER_BANK, MERKER_BIT, "Gemaelde-Programm laeuft (Merker, Nachbesserung 1 M1)")
    warte_solange(2, 0x07, "eine Szene haelt die Pad-Sperre (sub02 @0x00C26 Set(2,7)=1)")
    innen_start = len(prog)
    R.set_flag(2, 0x07, 1, "  Set(2,7)=1 Pad-Sperre = sub02 @0x00C26 (im selben Takt wie die Pruefungen davor: kein Fenster)")
    R.work_player("  Work_set(1,0)+Nop Elza = sub02 @0x00C2E")
    op(b"\x41\x01" + s16(JOHN_TUER[0]) + s16(0) + s16(JOHN_TUER[1]) + b"\x64\x00",
       "  Plc_neck Elza -> Dachtuer (%d,%d) Tempo 100 (Form sub02 @0x00C42 `41 01 x 0 z 64 00`)" % JOHN_TUER)
    sleep(10, "  Sleep 10 = sub02 @0x00C4C")
    op([0x2B, MSG_REAKTION, 0x00, 0x00], "  Message_on 50 \"Elza: Oh, no, this is John! ...\" (Form sub02 @0x00D0A `2b 02 00 00`)")
    sleep(90, "  Sleep 90 (Zeit zum Lesen; RE2 ROOM1060 sub05 +0x58 `09 0a 5a 00`)")
    R.set_flag(9, BIT_REAKTION, 1, "  Set(9,94)=1 Reaktion gesehen (VERTRAG 1.1)")
    op(b"\x41\x00" + b"\x00" * 6 + b"\x64\x00", "  Plc_neck aus = sub02 @0x00F30 `41 00 00 00 00 00 00 00 64 00`")
    R.set_flag(2, 0x07, 0, "  Set(2,7)=0 = sub02 @0x00F4E")
    innen = sum(len(b) for b, _ in prog[innen_start:]) + 2 + 4
    prog.insert(innen_start, (b"\x21\x04\xc3\x00", "  Ck(4,0xC3)==0 = main00 @0x00A76 (Dachtuer noch verriegelt)"))
    prog.insert(innen_start, (b"\x06\x00" + struct.pack("<H", innen),
                              "  Ifel_ck Laenge %d (Form sub01 @0x00BFE): nur solange John draussen ist" % innen))
    op(b"\x08\x00", "  Endif = sub01 @0x00C10")
    rumpf = sum(len(b) for b, _ in prog[rumpf_start:]) + 2
    prog.insert(rumpf_start, (b"\x06\x00" + struct.pack("<H", rumpf),
                              "Ifel_ck Laenge %d (Form sub01 @0x00BFE: Laenge = Ck + Rumpf + Endif)" % rumpf))
    op(b"\x08\x00", "Endif = sub01 @0x00C10")


def baue_haemmern39():
    del prog[:]
    op([0x54, 0x00, 0x01, 0x00, 0x00, 0x00], "Sce_bgm_control MAIN Play = ROOM1141 sub00 @0x00B4E (Raum-Titel MAIN16, wie Runde 38)")
    sleep(30, "Sleep 30 (Elza ist eben herein; RE2 ROOM1060 sub05 +0x2E `09 0a 1e 00`)")
    kopf = len(prog)
    op(b"\x0f\x04\x00\x00", "While (Laenge s.u.) = Form ROOM1141 sub03 @0x00FA8 `0f 04 08 00`")
    op(b"\x21\x04\xc3\x00", "  Ck(4,0xC3)==0 (Schleife laeuft, solange die Dachtuer verriegelt ist; main00 @0x00A76) - "
                            "Nachbesserung 1 N4.1: vorher Ck(3,0x6A)==0 (nur bis zum Rolladen)")
    for i, h in enumerate(MIX):
        klopfen_gruppe()
        sleep(30, "Sleep 30 = RE2 ROOM1060 sub05 +0x2E")
        ifel_c3([0x36, BANK, SATZ_HILFE0 + h - 1] + [0] * 9,
                "Se_on(0x3E,0x%02X) Hilferuf help%02d.wav (NUTZER-VORGABE Punkt 4; Sprach-Pfad wie main42)" % (SATZ_HILFE0 + h - 1, h))
        sleep(90, "Sleep 90 = RE2 ROOM1060 sub05 +0x58")
        if i == 0:
            reaktion()
        sleep(90, "Sleep 90 = RE2 ROOM1060 sub05 +0x5E")
    op(b"\x10\x00", "Ewhile = ROOM1141 sub03 @0x00FB2 (zurueck zum While-Test)")
    laenge = sum(len(b) for b, _ in prog[kopf + 1:])
    prog[kopf] = (b"\x0f\x04" + struct.pack("<H", laenge),
                  prog[kopf][1].replace("Laenge s.u.", "Laenge %d bis hinter das Ewhile" % laenge))
    op([0x01, 0x00], "Evt_end (Dachtuer entriegelt: Klopfen und Hilferufe sind zu Ende)")
    return list(prog)


def idx_bei(raum, off):
    return R.zeile_bei(raum, off)


def einfuegen_vor(raum, off, zeilen):
    i = idx_bei(raum, off)
    for k, z in enumerate(zeilen):
        prog.insert(i + k, z)


def einfuegen_nach(raum, off, zeilen):
    i = idx_bei(raum, off) + 1
    for k, z in enumerate(zeilen):
        prog.insert(i + k, z)


def baue_rolltor39():
    """Nachbesserung 1 N4.1 (NUTZER-VORGABE "Elza soll das Rolltor oeffnen und dann noch ganz normal laufen koennen"):
    Ereignis 6 (Gemaelde) in Phase H = ROOM1141 sub06 mit dem Rolladen-Ton der Runde 38 und OHNE die Gorilla-Bloecke
    (in Phase H sind die Gorillas heraus); am Ende gibt sub06 selbst die Steuerung zurueck (@0x010C8..@0x010D6). Danach
    Merker (5,0x10) aus (Nachbesserung 1 M1; dining_r38.c setzt ihn beim Ausloesen)."""
    del prog[:]
    R.kopie("1141", 0x00FE8, 0x0100E, [0x2B, 0x0D, 0x80, 0xFF], "(sub06: Gemaelde-Frage; Ja-Zweig)")
    i_ifel = idx_bei("1141", 0x00FEE)
    if prog[i_ifel][0] != bytes([0x06, 0x00, 0xE8, 0x00]):
        sys.exit("ABBRUCH: Ifel sub06 @0x00FEE")
    R.rolladen_ton_a()
    R.kopie("1141", 0x01032, 0x0107C, [0x2E, 0x03, 0x00], "(Work_set Prop 0 = Rolladen; Bewegung bytegleich)")
    R.rolladen_ton_b()
    R.kopie("1141", 0x0107C, 0x010A4, [0x37, 0x00, 0x0D, 0x00], "(Sca-Saetze: Rolladen-Zellen frei)")
    R.kopie("1141", 0x010C8, 0x010DA, [0x22, 0x02, 0x07, 0x00],
            "(sub06-Schluss: Pad-Sperre/Balken aus, Cut_auto 1, Plc_ret Spieler, Endif - Steuerung zurueck)")
    i_end = len(prog) - 1
    if prog[i_end][0] != b"\x08\x00":
        sys.exit("ABBRUCH: sub06 @0x010D8 Endif erwartet")
    R.set_flag(MERKER_BANK, MERKER_BIT, 0, "Set(5,0x10)=0 Merker 'Gemaelde-Programm laeuft' aus (Nachbesserung 1 M1; "
                                           "Form Set sub03 @0x00FA4 `22 05 00 00`)")
    op([0x01, 0x00], "Evt_end = sub06 @0x010DA")
    laenge = sum(len(b) for b, _ in prog[i_ifel + 1:i_end + 1])
    prog[i_ifel] = (b"\x06\x00" + struct.pack("<H", laenge),
                    prog[i_ifel][1] + "  -> Laenge 0xE8 -> 0x%X (Ja-Zweig mit Ton, ohne Gorilla-Bloecke @0x0100E/@0x010A4)" % laenge)
    return list(prog)


def tuer_auftritt():
    """Punkt 2 (NUTZER-VORGABE) mit Nachbesserung 1 N4.2: nach dem Entriegeln rennt Elza bis zur anderen Kamera (Cut 7,
    Blick nach Sueden, nicht mehr zur Tuer), dreht sich zur Tuer und hebt die Waffe; dann Tuer auf/zu, Kamera auf die Tuer
    (John steht schon da)."""
    R.set_flag(2, 0x07, 1, "Set(2,7)=1 Pad-Sperre (Form sub06 @0x01004)")
    R.set_flag(1, 0x1B, 1, "Set(1,0x1B)=1 Balken (Form sub06 @0x01008)")
    R.work_player("Work_set(1,0)+Nop Elza (sub02 @0x00CD4)")
    R.dest(5, 0, ELZA_VORNE, "Plc_dest RUN Elza vom Riegel in den Raum (%d,%d) (Spieler-RUN Form ROOM1021 sub02 @0x02046 "
                             "`40 00 05 20`; Bit 0 = Warte-Bit von sub03) - NUTZER-VORGABE \"rennt sie dann bis zur Szene mit "
                             "den anderen Kamera\"" % ELZA_VORNE)
    op([0x18, 0x03], "Gosub sub03 = sub02 @0x00CE0 (warten, bis Elza steht)")
    R.cut_chg(CUT_ELZA, "Cut_chg 7 = Kamera am Gangende, Blick nach Sueden: Elza von vorn, Dachtuer hinter der Kamera "
                        "(Form sub02 @0x00C24 `29 07`)")
    R.dest(9, 0, JOHN_TUER, "Plc_dest TURN Elza zur Dachtuer (Form sub02 @0x00CE2 `40 00 09 00 ..` / ROOM3060 sub03 @0x0108E)")
    op([0x18, 0x03], "Gosub sub03 = sub02 @0x00CEA")
    op([0x3F, 0x02, 0x06, 0x00], "Plc_motion(2,6) Elza hebt die Waffe = ROOM3060 sub03 @0x010BE `3f 02 06 00` (Spieler-Sub 2 "
                                 "0x80073e30[2] = 0x80050f00 auf +0x170/+0x174 = W-Bank @0x80036be4/@0x80036c04, Clip 6 = RAISE; "
                                 "Waffe gewaehlt beim Ausloesen, dining_r39.c) - NUTZER-VORGABE Nachtrag 4")
    sleep(40, "Sleep 40 = ROOM3060 sub03 @0x010C2 `09 0a 28 00`")
    R.se_on(R.BANK_TUER, 0, "Se_on(0x0E,0) DOOR1A Satz 0 = Tuer auf (Dachtuer 1141 = DOOR1A, tuer_zuordnung.inc S064; Form "
                            "ROOM10D0 sub21 @0x01A02)")
    R.work_npc(0, "Work_set(2,0)+Nop John (sub02 @0x00C5C)")
    R.pos_set(JOHN_TUER, "Pos_set John innen vor die Dachtuer (Form sub02 @0x00C32 / ROOM10D0 sub21 @0x01D9E) - hinter der "
                         "Kamera von Cut 7")
    op(b"\x33\x00\x00\x00" + s16(JOHN_TUER_YAW) + b"\x00\x00",
       "Dir_set John Blick %d = in den Gang (Form ROOM10D0 @0x01AC4 `33 00 00 00 ea 06 00 00`)" % JOHN_TUER_YAW)
    sleep(TUER_AUF_BILDER, "Sleep 43 = Laenge DOOR1A Ton 0 (1,43 s)")
    R.se_on(R.BANK_TUER, 1, "Se_on(0x0E,1) DOOR1A Satz 1 = Tuer zu (Form ROOM10D0 sub21)")
    sleep(TUER_ZU_BILDER, "Sleep 27 = Laenge DOOR1A Ton 1 (0,89 s)")
    op([0x54, 0x00, 0x02, 0x00, 0x00, 0x00], "Sce_bgm_control MAIN Stop = ROOM1141 sub01 @0x00C06 (John ist drin)")
    R.cut_chg(CUT_TUER, "Cut_chg 6 = Dachgang mit Dachtuer: John steht schon da (NUTZER-VORGABE)")
    sleep(20, "Sleep 20 = sub02 @0x00DC6 `09 0a 14 00`")
    R.cut_chg(5, "Cut_chg 5 = Blick nach Norden auf Rolladen und Gang (Runde 38: @0x00D08 -> 5): John laeuft heraus")
    R.work_player("Work_set(1,0)+Nop Elza")
    op([0x3F, 0x02, 0x06, 0x00], "Plc_motion(2,6) = ROOM1021 sub02 @0x020EA (Waffe senken: Clip 6 rueckwaerts) - es ist John")
    op([0x43, 0x00, 0x80, 0x00], "Plc_flg 0x80 rueckwaerts = ROOM1021 sub02 @0x020EE `43 00 80 00`")


def nach_dem_kampf():
    """Punkt 8 (NUTZER-VORGABE Wortlaut): nach dem Tod der Gorillas zwei Zeilen Johns mit Gesten, dann sein Abgang.
    Aufbau = ROOM1141 sub02 @0x00C24..@0x00C76 (Szenenbeginn: Kamera, Sperre, Pos_set, Kopf/Drehung zueinander),
    Erleichterung = Johns Gesten zu msg 1 "I'm glad to see you're still alive" (@0x00CB4..@0x00CD0).
    Nachbesserung 1: M3 - John steht im Schnittbild von Cut 8 schon an seinem Platz (Pos_set im selben Takt wie Elzas
    Pos_set, vor Sleep 1); M2/N4.4 - zu msg 52 die Folge-Geste Clip 17 (Armschwung) statt Plc_motion(2,6) (= Zielen,
    ROOM1021 @0x020C6): Johns eigene Folge aus 1141 sub02 @0x00DBA..@0x00DC6 (Clip 17, Sleep 50, Clip 23, Sleep 20), die
    Bibliotheks-Geste des Originals zu "Hurry up! They're coming!" (ROOM3070 sub03 @0x03724)."""
    R.cut_chg(CUT_GESPRAECH, "Cut_chg 8 = Suedwest-Ecke, John und Elza im Bild (Form sub02 @0x00C24 `29 07`)")
    R.set_flag(2, 0x07, 1, "Set(2,7)=1 = sub02 @0x00C26")
    R.set_flag(1, 0x1B, 1, "Set(1,0x1B)=1 Balken = sub02 @0x00C2A")
    R.work_npc(0, "Work_set(2,0)+Nop John (Form sub02 @0x00C5C)")
    R.pos_set(ECKE, "Pos_set John an seinen Platz im Kampf (Ziel sub02 @0x00F8E) IM SCHNITTBILD (Nachbesserung 1 M3: vorher "
                    "erst hinter Sleep 1/10/5 = 16 Bilder nach dem Schnitt sichtbar)")
    R.work_player("Work_set(1,0)+Nop Elza = sub02 @0x00C2E")
    R.pos_set(ELZA_NACH, "Pos_set Elza (%d,%d) = sub02 @0x00C32 (dort (-7600,-17600) beim Kamerawechsel)" % ELZA_NACH)
    op([0x34, 0x04] + list(s16(ELZA_BLICK_JOHN)),
       "Member_set(4,%d) = Elzas Blick (+0x6a, actor_common.c Id 4) zu John (Form sub02 @0x00C3A `34 04 00 00`; Yaw aus "
       "pos_advance @0x800245d8: atan2(-dz,dx))" % ELZA_BLICK_JOHN)
    sleep(1, "Sleep 1 = sub02 @0x00C3E")
    op(b"\x41\x01" + s16(ECKE[0]) + s16(0) + s16(ECKE[1]) + b"\x64\x00", "Plc_neck Elza -> John = sub02 @0x00C42")
    sleep(10, "Sleep 10 = sub02 @0x00C4C")
    R.dest(9, 0, ECKE, "Plc_dest TURN Elza -> John = sub02 @0x00C50")
    sleep(5, "Sleep 5 = sub02 @0x00C58")
    R.work_npc(0, "Work_set(2,0)+Nop John = sub02 @0x00C5C")
    op(b"\x41\x01" + s16(ELZA_NACH[0]) + s16(0) + s16(ELZA_NACH[1]) + b"\x64\x00", "Plc_neck John -> Elza = sub02 @0x00C60")
    sleep(5, "Sleep 5 = sub02 @0x00C6A")
    R.dest(9, 1, ELZA_NACH, "Plc_dest TURN John -> Elza = sub02 @0x00C6E")
    op([0x18, 0x04], "Gosub sub04 = sub02 @0x00C76 (warten, bis John Elza ansieht)")
    # Zeile 1 + Erleichterung (Gesten Johns zu msg 1, sub02 @0x00CB0..@0x00CD0)
    op([0x2B, MSG_PUH, 0x00, 0x00], "Message_on 51 \"John: Puuuh, that was close! ...\" (Form sub02 @0x00CB0 `2b 01 00 00`)")
    R.work_npc(0, "Work_set(2,0)+Nop John = sub02 @0x00CB4")
    op([0x3F, 0x00, 0x0F, 0x00], "Plc_motion(0,15) = sub02 @0x00CB8 (Johns Geste zu \"I'm glad to see you're still alive\")")
    sleep(50, "Sleep 50 = sub02 @0x00CBC")
    op([0x3F, 0x00, 0x0F, 0x00], "Plc_motion(0,15) = sub02 @0x00CC0")
    op([0x43, 0x00, 0x80, 0x00], "Plc_flg 0x80 (rueckwaerts) = sub02 @0x00CC4")
    sleep(50, "Sleep 50 = sub02 @0x00CC8")
    op([0x3F, 0x00, 0x17, 0x00], "Plc_motion(0,23) = sub02 @0x00CCC")
    sleep(20, "Sleep 20 = sub02 @0x00CD0")
    # Zeile 2 + Folge-Geste (Nachbesserung 1 M2/N4.4)
    op([0x2B, MSG_FOLGEN, 0x00, 0x00], "Message_on 52 \"John: Elza, follow me, ...\" (Form sub02 @0x00D9E `2b 05 00 00`)")
    R.work_npc(0, "Work_set(2,0)+Nop John = sub02 @0x00DA2")
    op([0x3F, 0x00, 0x11, 0x00], "Plc_motion(0,17) Folge-Geste Armschwung = sub02 @0x00DBA (John) / ROOM3070 sub03 @0x03724 "
                                 "\"Hurry up! They're coming!\" (Nachbesserung 1 M2: statt (2,6) = Zielen)")
    sleep(50, "Sleep 50 = sub02 @0x00DBE `09 0a 32 00`")
    op([0x3F, 0x00, 0x17, 0x00], "Plc_motion(0,23) = sub02 @0x00DC2 (Hand zurueck)")
    sleep(20, "Sleep 20 = sub02 @0x00DC6 `09 0a 14 00`")
    sleep(MSG52_REST, "Sleep %d = Rest der Standzeit von msg 52 (Abnahme H3: Text offen F4505..F4603 = 99 Bilder; "
                      "50 + 20 + %d = 100 >= 99): John laeuft erst los, wenn die Zeile weg ist" % (MSG52_REST, MSG52_REST))
    op(b"\x41\x00" + b"\x00" * 6 + b"\x64\x00", "Plc_neck aus (John) = sub02 @0x00F80")
    R.work_player("Work_set(1,0)+Nop Elza")
    op(b"\x41\x00" + b"\x00" * 6 + b"\x64\x00", "Plc_neck aus (Elza) = sub02 @0x00F30")


def baue_entriegeln39():
    """Nachbesserung 1 N4.2: Ereignis 5 (Riegel der Dachtuer, main00 @0x00A9A) in Phase H. Zuerst die Original-Zeilen von
    sub05 (Riegel-Slot aus, (4,0xC3)=1, msg 12 "You've opened the lock."), dann die Szene: Tuer-Auftritt (oben), Szene der
    Runde 38 mit Punkt 6/7, Kampf, Punkt 8, Johns Abgang. Die Dachtuer (Slot 1) wird erst nach Johns Abgang scharf."""
    del prog[:]
    R.kopie("1141", 0x00FC8, 0x00FDC, [0x46, 0x02, 0x00, 0x00],
            "(sub05: Riegel-Slot 2 aus, Set(4,0xC3)=1, msg 12 \"You've opened the lock.\", Evt_next)")
    tuer_auftritt()
    # ---- Szenen-Flags, Gorillas geparkt, John laeuft zu seinem Platz (wie Runde 38) ----
    R.kopie("1141", 0x00C18, 0x00C24, [0x22, 0x04, 0xF0, 0x00], "(sub02: Set(4,0xF0)=0 / (4,0xF2)=1 / (3,0xC2)=1)")
    R.kopie("1141", 0x00B1A, 0x00B42, [0x44, 0x01, 0x27, 0x30], "(Gorillas geparkt wie sub00 beim Original-Szenenstart)")
    R.work_npc(0)
    R.dest(5, 1, R.JOHN_PLATZ, "Plc_dest RUN John von der Dachtuer an seinen Platz (Form ROOM1141 sub02 @0x00F8E `40 00 05 01`)")
    op([0x18, 0x04], "Gosub sub04 = ROOM1141 sub02 @0x00F96 (warten, bis John steht)")
    # ---- Original sub02 ab @0x00CD4 (wie Runde 38) ----
    vor = len(prog)
    R.kopie("1141", 0x00CD4, 0x00F4E, [0x2E, 0x01, 0x00], "(sub02: Elza an ihren Platz, Drehungen, Szene ab msg 2)")
    R.ersetze_bei("1141", 0x00D08, [0x29, 0x00], [0x29, 0x05], "Cut_chg 5 (Runde 38: Cut 0 blickt auf die Doppeltuer)")
    R.ersetze_bei("1141", 0x00D0A, [0x2B, 0x02, 0x00, 0x00], [0x2B, R.MSG_1141_FRAGE, 0x00, 0x00],
                  "Port-Nachricht 40 = msg 2 ohne \"Same here...\" (Runde 38)")
    R.ersetze_bei("1141", 0x00E80, [0x29, 0x00], [0x29, 0x05], "Cut_chg 5 (wie @0x00D08)")
    # Punkt 6: Glasbruch vor "What was that?!" (msg 6 @0x00DCA), vor dem Original-Sleep 20 @0x00DC6
    einfuegen_vor("1141", 0x00DC6, [
        (bytes([0x2B, MSG_STIMME_WARTEN, 0x00, 0x00]), "Message_on 58 = Port-Warte-Zeile ohne Text: op_message_on parkt, "
                                                       "bis Johns Sprachzeile zu msg 5 endet (Stimmen-Riegel scd_vm.c), dann "
                                                       "verbraucht re15_dining_r39_message_on sie (Form sub02 @0x00D9E `2b 05 00 00`)"),
        (bytes([0x36, BANK, SATZ_GLAS] + [0] * 9), "Se_on(0x3E,0x21) Glasbruch = RE2 ROOM1090 sub15 @0x02BB0 (Satz 0x21, "
                                                   "GLAS1090) - NUTZER-VORGABE Punkt 6"),
        (bytes([0x09, 0x0A, 0x05, 0x00]), "Sleep 5 = RE2 ROOM1090 sub15 @0x02BBC"),
        (bytes([0x36, BANK, SATZ_GLAS] + [0] * 9), "Se_on(0x3E,0x21) Glasbruch = RE2 ROOM1090 sub15 @0x02BC0 (zwei Scheiben)"),
    ])
    # Punkt 7: das Skript wartet auf Johns Drehung zu Elza, bevor msg 9 seine Geste startet (Form @0x00CFE/@0x00D06)
    einfuegen_nach("1141", 0x00E94, [
        (bytes([0x18, 0x04]), "Gosub sub04 (Form sub02 @0x00D06 nach Johns Plc_dest Modus 9 @0x00CFE): John dreht sich zu "
                              "Elza fertig, bevor @0x00EC4 Plc_motion seinen Dreh-Sub 9 beendet"),
    ])
    op([0x46, 0x00, 0x01, 0x31, R.MSG_1141_SPERRE, 0x00, 0xFF, 0xFF, 0x00, 0x00],
       "Aot_reset Slot 0 (Tuer -> 1131) -> Text msg 41 (Runde 38; Form ROOM11B0 sub01 @0x01216)")
    op([0x46, 0x02, 0x01, 0x31, R.MSG_1141_SPERRE, 0x00, 0xFF, 0xFF, 0x00, 0x00],
       "Aot_reset Slot 2 (Riegel im Rechteck der Dachtuer) -> msg 41 (Runde 38)")
    R.kopie("1141", 0x00F4E, 0x00FA2, [0x22, 0x02, 0x07, 0x00], "(sub02-Schluss: Steuerung zurueck, Gorillas, John in die Ecke)")
    op([0x04, 0xFF, 0x18, SUB_SCHUETZE], "Evt_exec(0xFF, 0x18, %d) = Faden SCHUETZE39 (John schiesst ab und an; Form sub00 @0x00B02 "
                                         "`04 ff 18 02`) - Nachbesserung 1 N4.3" % SUB_SCHUETZE)
    xp = {R.ORIG_ELZA: R.ELZA_PLATZ, R.ORIG_JOHN: R.JOHN_PLATZ,
          (-1300, -15500): (R.ELZA_PLATZ[0] + R.BLICK_DX, R.ELZA_PLATZ[1]),
          (-850, -13350): (R.JOHN_PLATZ[0] + R.BLICK_DX, R.JOHN_PLATZ[1])}
    getauscht = 0
    for i in range(vor, len(prog)):
        b, k = prog[i]
        if b[0] == 0x41 and len(b) == 10 and b[1] == 0x01:
            x, z = struct.unpack_from("<h", b, 2)[0], struct.unpack_from("<h", b, 6)[0]
            if (x, z) in xp:
                nb = bytearray(b); nx, nz = xp[(x, z)]; nb[2:4] = s16(nx); nb[6:8] = s16(nz)
                prog[i] = (bytes(nb), k + "  -> Blickpunkt (%d,%d) statt (%d,%d)" % (nx, nz, x, z))
                getauscht += 1
        elif b[0] == 0x40 and len(b) == 8:
            x, z = struct.unpack_from("<hh", b, 4)
            if (x, z) in xp:
                nb = bytearray(b); nx, nz = xp[(x, z)]; nb[4:6] = s16(nx); nb[6:8] = s16(nz)
                prog[i] = (bytes(nb), k + "  -> Ziel (%d,%d) statt (%d,%d)" % (nx, nz, x, z))
                getauscht += 1
    if getauscht != 12:
        sys.exit("ABBRUCH: %d statt 12 Koordinaten umgesetzt" % getauscht)
    # ---- Gorillas tot -> Punkt 8, dann Doppeltuer frei und Johns Abgang (Runde 38) ----
    R.warte(4, 0xF2, 1, "Gorillas leben (sub01 @0x00C0C loescht (4,0xF2))")
    warte_solange(SCHUETZE_BANK, SCHUETZE_BIT, "Johns letzte Salve zu Ende (N4.3)")
    sleep(30, "Sleep 30 (Form ROOM1021 sub06 @0x021FE)")
    nach_dem_kampf()
    op([0x46, 0x00, 0x02, 0x31] + list(D1141[0x00A52 + 14:0x00A52 + 20]),
       "Aot_reset Slot 0 -> Tuer ROOM1131 (Nutzlast = main00 @0x00A52+14; Runde 38 Nachbesserung 1 M3)")
    R.work_player("Work_set(1,0)+Nop Spieler = Arbeitsfigur der Lage-Abfrage (Form ROOM1141 sub02 @0x00C2E)")
    R.while_elza_in_bahn()
    R.set_flag(2, 0x07, 1, "Set(2,7)=1 Pad-Sperre (Form ROOM1141 sub06 @0x01004)")
    R.set_flag(1, 0x1B, 1, "Set(1,0x1B)=1 Balken (Form ROOM1141 sub06 @0x01008)")
    R.cut_chg(7, "Cut_chg 7 = Kamera (-7465,-3024,-9613) Blick nach Sueden (sub02 @0x00C24): John laeuft aus der Ecke auf die "
                 "Kamera zu und verlaesst das Bild am unteren Rand (Runde 38)")
    R.work_npc(0)
    op([0x34, 0x06] + list(s16(R.JOHN_WORT0)),
       "Member_set(6, 0x%04X) John: kein Koerper-Schub (FUN_8002aec4 @0x8002af04; Runde 38 M3)" % R.JOHN_WORT0)
    R.dest(5, 1, R.GANG_AUSGANG, "Plc_dest RUN John -> Gang-Ausgang (Form sub02 @0x00F8E)")
    op([0x18, 0x04], "Gosub sub04 (Form sub02 @0x00F96)")
    R.dest(5, 1, R.TUER_INNEN, "Plc_dest RUN John -> vor die Dachtuer")
    op([0x18, 0x04], "Gosub sub04")
    R.se_on(R.BANK_TUER, 0, "Se_on(0x0E,0) DOOR1A oeffnen (Runde 38)")
    sleep(15, "Sleep 15 (Form ROOM1131 sub02 @0x00A54)")
    R.pos_set(R.PARK, "Pos_set John geparkt (Form ROOM10D0 sub21 @0x01D9E, Lage ROOM11B0 @0x01080)")
    R.se_on(R.BANK_TUER, 1, "Se_on(0x0E,1) DOOR1A zufallen")
    R.set_flag(9, R.BIT_JOHN_WEG, 1, "Set(9,67)=1 John hat 1141 verlassen (Runde 38)")
    R.kopie("1141", 0x00FC8, 0x00FD6, [0x46, 0x02, 0x00, 0x00],
            "(sub05 ohne Nachricht: Riegel-Slot 2 aus + Set(4,0xC3)=1 - die Dachtuer ist offen, John kam durch sie herein)")
    R.kopie("1141", 0x00FDC, 0x00FE6, [0x46, 0x01, 0x02, 0x31], "(sub05: Dachtuer Slot 1 scharf)")
    sleep(15, "Sleep 15")
    op([0x2A, 0x00], "Cut_old + Nop: zurueck auf die Kamera vor Cut 7 (Form ROOM1150 @0x010F2 `2a 00 3c 01`)")
    op([0x3C, 0x01], "Cut_auto 1 (Form ROOM1150 @0x010F4)")
    R.set_flag(2, 0x07, 0, "Set(2,7)=0 (Form sub06 @0x010C8)")
    R.set_flag(1, 0x1B, 0, "Set(1,0x1B)=0 (Form sub06 @0x010CC)")
    op([0x01, 0x00], "Evt_end = sub05 @0x00FE6")
    return list(prog)


def ifel_f2(zeilen_fn, c):
    """Ifel_ck { Ck(4,0xF2)==1 <Zeilen> } Endif - Gorillas leben noch (sub00 @0x00B4A `21 04 f2 01`); Ifel-Form sub01
    @0x00BFE (Laenge = Ck + Rumpf + Endif), Endif sub01 @0x00C10."""
    kopf = len(prog)
    op(b"\x06\x00\x00\x00", "Ifel_ck (Laenge s.u.) (Form sub01 @0x00BFE): %s" % c)
    op(b"\x21\x04\xf2\x01", "  Ck(4,0xF2)==1 = sub00 @0x00B4A (Gorillas leben)")
    zeilen_fn()
    op(b"\x08\x00", "Endif = sub01 @0x00C10")
    laenge = sum(len(b) for b, _ in prog[kopf + 1:])
    prog[kopf] = (b"\x06\x00" + struct.pack("<H", laenge), prog[kopf][1].replace("(Laenge s.u.)", "Laenge %d" % laenge))


ANKER = {}


def anker(name):
    ANKER[name] = sum(len(b) for b, _ in prog)


def baue_schuetze39():
    """Nachbesserung 1 N4.3 (NUTZER-VORGABE Nachtrag 4: "John sollte auch ab und an mal auf die Gorillas schiessen in
    ROOM 1141 und dann ihre Aufmerksamkeit erhalten"; Johns eigene Zeile msg 11 "I'll try to distract them"). Eigener
    Faden (Evt_exec aus ENTRIEGELN39, sobald John in seiner Ecke steht), solange die Gorillas leben: Pause, Drehung zum
    naechsten Gorilla (Ziel traegt dining_r39.c am Anker ZIEL ein), Zielen, zwei Schuesse, Senken - die Original-Folge
    von Johns Schuessen auf Roy, ROOM1021 sub02 @0x020C6..@0x020F2. Treffer, Ton und Aufmerksamkeit setzt dining_r39.c
    an den Ankern FEUER (Verzug 1 wie der Schuss-Ton der Runde 36 L, RE2 ROOM1010 sub30)."""
    del prog[:]
    ANKER.clear()
    sleep(60, "Sleep 60 (John ist eben in der Ecke; Form ROOM1171 sub02 @0x014D2 `09 0a 3c 00`)")
    kopf = len(prog)
    op(b"\x0f\x04\x00\x00", "While (Laenge s.u.) = Form ROOM1141 sub03 @0x00FA8 `0f 04 08 00`")
    op(b"\x21\x04\xf2\x01", "  Ck(4,0xF2)==1 = sub00 @0x00B4A (Gorillas leben)")
    sleep(90, "Sleep 90 (PORT-WAHL \"ab und an\": Takt der Ruf-Schleife, RE2 ROOM1060 sub05 +0x58)")
    sleep(90, "Sleep 90 (RE2 ROOM1060 sub05 +0x5E)")

    def salve():
        R.set_flag(SCHUETZE_BANK, SCHUETZE_BIT, 1, "  Set(5,0x11)=1 Salve laeuft (ENTRIEGELN39 wartet nach dem Kampf darauf)")
        R.work_npc(0, "  Work_set(2,0)+Nop John (sub02 @0x00C5C)")
        anker("ZIEL")
        R.dest(9, 1, (0, 0), "  Plc_dest TURN John -> naechster Gorilla (Form sub02 @0x00E94 `40 00 09 01 ..`; Ziel x/z traegt "
                             "dining_r39.c am Anker ZIEL ein)")
        op([0x18, 0x04], "  Gosub sub04 = sub02 @0x00D06 (warten, bis John ausgerichtet ist)")
        op([0x3F, 0x02, 0x06, 0x00], "  Plc_motion(2,6) Zielen = ROOM1021 sub02 @0x020C6 (eigene Bank Clip 6)")
        sleep(110, "  Sleep 110 = ROOM1021 sub02 @0x020CA `09 0a 6e 00`")

        def schuss(n):
            def zeilen():
                anker("FEUER%d" % n)
                op([0x3F, 0x02, 0x07, 0x00], "    Plc_motion(2,7) Feuer-Clip = ROOM1021 sub02 @0x%05X (eigene Bank Clip 7; Anker FEUER%d)"
                   % (0x020CE if n == 1 else 0x020D6, n))
                sleep(30, "    Sleep 30 = ROOM1021 sub02 @0x020D2 `09 0a 1e 00`")
            return zeilen
        ifel_f2(schuss(1), "erster Schuss")
        ifel_f2(schuss(2), "zweiter Schuss")
        R.work_npc(0, "  Work_set(2,0)+Nop John")
        op([0x3F, 0x02, 0x06, 0x00], "  Plc_motion(2,6) = ROOM1021 sub02 @0x020EA (senken)")
        op([0x43, 0x00, 0x80, 0x00], "  Plc_flg 0x80 rueckwaerts = ROOM1021 sub02 @0x020EE")
        sleep(20, "  Sleep 20 = ROOM1021 sub02 @0x020F2 `09 0a 14 00`")
        R.set_flag(SCHUETZE_BANK, SCHUETZE_BIT, 0, "  Set(5,0x11)=0 Salve fertig")
    ifel_f2(salve, "Salve nur, solange die Gorillas leben")
    op(b"\x10\x00", "Ewhile = ROOM1141 sub03 @0x00FB2")
    laenge = sum(len(b) for b, _ in prog[kopf + 1:])
    prog[kopf] = (b"\x0f\x04" + struct.pack("<H", laenge), prog[kopf][1].replace("Laenge s.u.", "Laenge %d bis hinter das Ewhile" % laenge))
    op([0x01, 0x00], "Evt_end (Gorillas tot)")
    return list(prog)


# ---------------------------------------------------------------- Texte
TEXTE = []


def text(name, kopf, zeilen, beschr):
    """Dialogzeile in der Form von ROOM1141 msg 2 @0x0115F: `04 00 05 xx <Name:> 05 00 <Text> 04 01 01 63`."""
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
    msg2 = D1141[0x0115F:0x0115F + 59]
    if msg2[:4] != bytes([0x04, 0x00, 0x05, 0x01]) or msg2[-4:] != bytes([0x04, 0x01, 0x01, 0x63]):
        sys.exit("ABBRUCH: ROOM1141 msg 2 @0x0115F Form")
    msg1 = D1141[0x0111D:0x0111D + 66]
    if msg1[:4] != bytes([0x04, 0x00, 0x05, 0x07]):
        sys.exit("ABBRUCH: ROOM1141 msg 1 @0x0111D Sprecherfarbe John")
    text("k_r39j_msg50", ("Elza:", 0x01), ["Elza: Oh, no, this is John!", "He is in trouble, I have to help him!"],
         "ROOM1141 msg 50: NUTZER-VORGABE Punkt 5 (Nachtrag 2026-10-18: John, nicht Roy; Nachtrag 6: \"trouble\" klein); Form msg 2 @0x0115F "
         "(Elza = Farbe 1 `05 01`), Zeilenumbruch `08` wie msg 2")
    text("k_r39j_msg51", ("John:", 0x07), ["John: Puuuh, that was close! I thought", "we end up as gorilla meal...."],
         "ROOM1141 msg 51: NUTZER-VORGABE Punkt 8 (Nutzer: \"meal\" + Auslassungszeichen + Punkt = vier Punkte; die Schrift hat kein Auslassungszeichen; Nachtrag 6: \"gorilla\" klein); "
         "Form msg 1 @0x0111D (John = Farbe 7 `05 07`)")
    text("k_r39j_msg52", ("John:", 0x07), ["John: Elza, follow me, I think I might", "have found a way out of here!"],
         "ROOM1141 msg 52: NUTZER-VORGABE Punkt 8; Form msg 1 @0x0111D")


# ---------------------------------------------------------------- Klopf-Tonteil
def klopfton():
    p = os.path.join(REPO, "info", "re2leon", "COMMON", "DOOR", "DOOR0F.DO2")
    d = open(p, "rb").read()
    if len(d) != DOOR0F_DATEI:
        sys.exit("ABBRUCH: DOOR0F.DO2 %d B statt %d" % (len(d), DOOR0F_DATEI))
    ton = d[:DOOR0F_TON]
    ck = 0
    for o in range(0, DOOR0F_TON, 512):
        ck ^= d[o]
    if ck != DOOR0F_CK:
        sys.exit("ABBRUCH: DOOR0F Pruefsumme 0x%02X statt 0x%02X" % (ck, DOOR0F_CK))
    if ton[:16] != bytes([0, 0, 0x14, 0x16, 0, 0, 0x24, 0x17]) + b"\xff" * 8 or ton[0x10:0x14] != b"pBAV":
        sys.exit("ABBRUCH: DOOR0F Tonkopf/VH weicht ab")
    if struct.unpack_from("<I", ton, 0xC30)[0] != 0x10:
        sys.exit("ABBRUCH: DOOR0F Nachspann @0xC30 != 0x10")
    return ton


def carr(f, name, b):
    f.write("static const uint8_t %s[%d] = {" % (name, len(b)))
    for i, x in enumerate(b):
        f.write(("\n    " if i % 16 == 0 else "") + "0x%02x," % x)
    f.write("\n};\n")


def main():
    pruefen = "--pruefen" in sys.argv
    programme = [("k_r39j_p_haemmern", baue_haemmern39()), ("k_r39j_p_entriegeln", baue_entriegeln39()),
                 ("k_r39j_p_rolltor", baue_rolltor39()), ("k_r39j_p_schuetze", baue_schuetze39())]
    baue_texte()
    ton = klopfton()
    f = io.StringIO()
    f.write("/* GENERIERT von re15_port/tools/r39_dining/programme_bauen.py - NICHT HAND-EDITIEREN.\n"
            " * Runde 39 Spur J: SCD-Programme (Original-Bytes + eingefuegte Zeilen, Listing beim Werkzeugaufruf), Texte und\n"
            " * der Klopf-Tonteil. Dossier: analysis/befunde_runde39/J_dining1141.md. */\n\n")
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
    for k in sorted(ANKER):
        f.write("#define RE15_R39J_SCHUETZE_OFF_%s 0x%04X   /* Anker im Faden SCHUETZE39 (dining_r39.c) */\n" % (k, ANKER[k]))
    f.write("\n")
    for name, b, beschr in TEXTE:
        f.write("/* %s */\n" % beschr.replace("*/", "* /"))
        carr(f, name, b)
        f.write("\n")
    f.write("/* RE2 DOOR0F.DO2 Tonteil [0 .. 0x%X) bytegleich aus info/re2leon/COMMON/DOOR/DOOR0F.DO2 (Archiv-Tabelle RE2\n"
            " * @0x8009A5D4 `88 2a cc 8b 06 00 00 00 db 36 00 00`: Tonteil 0x2A88, Pruefsumme 0xDB = XOR erstes Byte je 512 B,\n"
            " * geprueft). Aufbau wie TORSE (tueren_02_re2.md 4): Tonkopf 4 Saetze @0, VH \"pBAV\" @0x10, Nachspann @0xC30,\n"
            " * VB @0xC38. Satz 0 = VAG 2 (Treppe Holz, ein Holzschlag 0,31 s). */\n" % DOOR0F_TON)
    carr(f, "k_r39j_klopfton", ton)
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
