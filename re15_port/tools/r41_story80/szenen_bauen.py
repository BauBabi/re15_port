#!/usr/bin/env python3
"""szenen_bauen.py - Runde 41 Spur ST80-T "Story-Texte umsetzen": die Szenen ROOM30C1 sub03 (Elza trifft Marvin, kurze
Fassung), ROOM6030 sub02 (Leon, Wagen-Szene) und ROOM6030 sub08 (Leon, Fuehrerstand mit Annette) aus ORIGINAL-Bytes,
als engine/src/gen/story80_szenen.inc.

Kein Asset-Patch: der Port leitet nur den Start des Original-Subs um (op_evt_exec, engine/src/story80_r41.c):
  ROOM30C1 sub00 @0x00C02 `04 ff 18 03` Evt_exec sub03  -> k_s80_30c1_sub03
  ROOM6030 sub01 @0x00D70 `04 ff 18 02` Evt_exec sub02  -> k_s80_6030_sub02
  ROOM6030 sub07 @0x010E8 `04 ff 18 08` Evt_exec sub08  -> k_s80_6030_sub08        bei (3,22)=1 (Wagen-Szene gelaufen)
                                                        -> k_s80_6030_sub08_vorne  bei (3,22)=0 (Nachbesserung 1)
  ROOM6030 sub07 @0x0110C `04 ff 18 09` Evt_exec sub09  -> k_s80_6030_sub09_nachtrag bei (9,128)=1 und (3,22)=1
  ROOM6030 sub07 @0x01132 `04 ff 18 05` Evt_exec sub05  -> k_s80_6030_sub05_nachtrag_tor bei (9,128)=1, (3,89)=1, Cut 0
                                                        (Integration A2: Nachtrag nach dem Tor, vor dem AN-Zugstart)
Vor dem Umleiten prueft der Port die ersten 16 Bytes des Original-Subs (Signatur unten).

NACHBESSERUNG 1 (Abnahme 1 M1, Entscheid Orchestrator Variante a): der Fuehrerstand ist vor dem Wagen erreichbar
(Treppe main00 @0x00CB2 Aot_set Slot 4 sce 12, Tuer Slot 1 main00 @0x00C52 Ziel 6030 Cut 0 -> sub00 @0x00D2C Case 0
-> Gosub sub07; sub07 @0x010D0 `21 04 17 00` Ck(4,23)=0 -> @0x010E8 Evt_exec sub08 fragt das Wagen-Flag NICHT ab).
Wagen-Flag = (3,22): einziger Schreiber sub02 @0x00D7A `22 03 16 01`, einziger Leser sub01 @0x00D62 `21 03 16 00`
(Ausloeser der Wagen-Szene). Die Gates liest story80_r41.c (re15_game_flag_get, dieselbe Tabelle wie Ck @0x8003fcf4):
  vorne  (3,22)=0: msg 11, msg 31 "Everyone we could find. Can you start this thing?", dann 13..19 wie Original;
         setzt das Port-Bit (9,128) "Sherry-Nachtrag offen" (Bank-9-Zensus frei, test_r36_kartenbits k_port[]).
  nachtrag  naechstes Betreten des Fuehrerstands mit (4,23)=1 (sub07 @0x010EC Else_ck), (3,89)=0 (@0x010F4, Annette da),
         (3,22)=1 und (9,128)=1: msg 40 Leon "There's a little girl in the back... Sherry.", msg 25 Annette, Schluss
         wie sub08 @0x0131E (Annette zurueck ans Pult = sub09-Haltung @0x01338..@0x01348, Leon dreht ab, Sperren auf);
         loescht (9,128) -> laeuft hoechstens einmal.

FORM: Jede Zeile eines Programms ist entweder ein ausgelieferter Byte-Bereich DESSELBEN Raums (Datei-Offsets in
re15_port/shared_assets/PSX/STAGE<n>/ROOM<id>.RDT, "orig lo..hi") oder ein Opcode in ausgelieferter Form mit
getauschtem Argument ("op", Vorlage-Offset angegeben):
  Message_on  `2b id 00 00`   ROOM30C1 @0x00CB2 / ROOM6030 @0x00E9A (Maske 0 = Untertitel, die Welt laeuft weiter)
  Plc_dest    `40 00 09 21 x z` ROOM6030 sub08 @0x01160 (NPC dreht sich, Ankunftsbit (5,0x21)); x/z = Leons Marke
                              sub02 @0x00E0E `40 00 04 20 11 9e bb a1` (-25071,-24133)
Sprung-Opcodes (Do/Edwhile, For/Next) liegen nur INNERHALB kopierter Bereiche; das Werkzeug geht jedes Programm mit
der Opcode-Laengentabelle ab (tools/scd_dump_room.py = engine/src/scd_vm.c s_opcode_sizes) und bricht ab, wenn ein
Programm nicht genau auf Evt_end `01 00` endet.

ABLAUF = NUTZERENTSCHEID (analysis/story80/TEXTE_UMSETZUNG.md); Gesten/Blicke = PORT-WAHL aus den Original-Bloecken
desselben Sprechers.

Aufruf: python re15_port/tools/r41_story80/szenen_bauen.py          (schreibt die .inc, druckt das Listing)
        python re15_port/tools/r41_story80/szenen_bauen.py --check  (prueft nur)
"""
import os, sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(os.path.dirname(HIER))
PSX = os.path.join(PORT, "shared_assets", "PSX")
OUT = os.path.join(PORT, "engine", "src", "gen", "story80_szenen.inc")


class _SD:
    """SIZES/NAMES/op_size/u16/u32 aus tools/scd_dump_room.py (das Skript ruft main() ohne __name__-Waechter, deshalb
    nur der Teil vor 'def main(' ausgefuehrt)."""
    def __init__(self):
        src = open(os.path.join(PORT, "tools", "scd_dump_room.py"), encoding="utf-8").read()
        ns = {}
        exec(compile(src[:src.index("\ndef main(")], "scd_dump_room.py", "exec"), ns)
        self.__dict__.update(ns)


SD = _SD()


def rdt(room):
    return open(os.path.join(PSX, "STAGE%X" % int(room[0], 16), "ROOM%s.RDT" % room), "rb").read()


# Datei-Offsets der Sub-Tabelle (RDT-Kopf, scd_dump_room "sub_scd") und erwartete Sub-Anfaenge.
SUB_TAB = {"30C1": 0x0BD8, "6030": 0x0D14}
SUB_SOLL = {("30C1", 3): 0x0C2C, ("6030", 2): 0x0D7A, ("6030", 8): 0x113A, ("6030", 9): 0x1376, ("6030", 5): 0x1004}


def orig(lo, hi, was):
    return ("orig", lo, hi, was)


def op(vorlage_off, vorlage_bytes, neu_bytes, was):
    return ("op", vorlage_off, bytes(vorlage_bytes), bytes(neu_bytes), was)


def msg_on(mid, vorlage_off, vorlage_id, was):
    return op(vorlage_off, [0x2B, vorlage_id, 0x00, 0x00], [0x2B, mid, 0x00, 0x00], was)


# ------------------------------------------------------------------------------------------------------------------
# ROOM30C1 sub03 (Elza): kurze Fassung, msg 1..5, danach Marvins Abgang unveraendert.
P_30C1 = [
    orig(0x0C2C, 0x0D9E, "sub03 @0x00C2C..: Sperren, Cut 1, Elza/Marvin gehen an den Tresen, Cut 9, "
                         "msg 1 Elza 'Who are you?' + Gesten, msg 2 Marvin + Gesten, msg 3 Elza + Gesten, "
                         "msg 4 Marvin + Gesten, msg 5 Elza + Gesten (bis Sleep 20 @0x00D9A)"),
    orig(0x0D9E, 0x0DA2, "Work_set(2,0) Marvin (@0x00D9E, im Original vor msg 6)"),
    orig(0x0E16, 0x0E6A, "Marvins Abgang wie im Original nach msg 8: Plc_motion(0,6) + Sleep 50 @0x00E16, Blick "
                         "zurueck, Plc_dest 9/5 + Gosub sub05, Evt_exec sub06, Pos_set, Cut 3, Sperren auf, Cut_auto, "
                         "Plc_ret, Evt_end @0x00E68"),
]
FOLGE_30C1 = [1, 2, 3, 4, 5]

# ------------------------------------------------------------------------------------------------------------------
# ROOM6030 sub02 (Leon): Wagen-Szene, dreizehn Zeilen.
LEON_SIEHT_JOHN = orig(0x0E88, 0x0E9A, "Leon blickt zu John: Work_set 1 @0x00E88, Plc_neck (-26055,0,-25784) @0x00E8C "
                                       "(= John, Sce_em_set sub06 @0x0101C), Sleep 1")
LEON_SIEHT_SHERRY = [orig(0x0E88, 0x0E8C, "Work_set 1 (Leon) @0x00E88"),
                     orig(0x0DEC, 0x0DFA, "Plc_neck (-24471,0,-25806) @0x00DEC (= Sherry, Sce_em_set sub06 @0x01044) "
                                          "+ Sleep 1 @0x00DF6")]
P_6030_WAGEN = [
    orig(0x0D7A, 0x0E5C, "sub02 @0x00D7A..: Sperren, Leon geht zu Marvin, msg 0 'Hang on, Marvin...' + Gesten, Cut 9, "
                         "Leon blickt zu Sherry @0x00DEC, geht zur Marke, Cut 10, John und Sherry blicken zu Leon"),
    orig(0x0E5C, 0x0E80, "Zeile 2: msg 1 Leon + Gesten 4/4/7 (@0x00E5C..)"),
    # Runde 42 Story STAGE6 (Konzept L21, VERTRAG 2.3): Zeilen 3..8 = 21 John, 2 Leon, 3 John, 22 Sherry, 4 Leon, 5 John.
    # Sprecherwechsel msg 2 Sherry -> Leon und msg 22 John -> Sherry: Blick und Gesten nach dem neuen Sprecher (Bloecke
    # desselben Sprechers aus sub02).
    # Zeile 3 (msg 21 John)
    LEON_SIEHT_JOHN,
    msg_on(21, 0x0E9A, 3, "Zeile 3: msg 21 John (Form msg 3 @0x00E9A)"),
    orig(0x0EEE, 0x0F22, "John-Gesten 3/3/1/1/7 wie nach msg 5 (@0x00EEE..)"),
    # Zeile 4 (msg 2, jetzt Leon "Where is she now?"): Leon blickt weiter zu John
    orig(0x0E80, 0x0E84, "Zeile 4: Message_on msg 2 (@0x00E80, Original-Bytes)"),
    orig(0x0ECA, 0x0EEA, "Leon-Gesten 1/1/7 wie nach msg 4 (Work_set 1 @0x00ECA..)"),
    # Zeile 5 (msg 3 John)
    orig(0x0E88, 0x0EC6, "Zeile 5: Leon blickt zu John, msg 3 John + Gesten 0/0/2/7 (@0x00E88.., Original)"),
    # Zeile 6 (msg 22, jetzt Sherry "Elza's still out there...!"): Leon blickt zu Sherry
    LEON_SIEHT_SHERRY[0], LEON_SIEHT_SHERRY[1],
    msg_on(22, 0x0E80, 2, "Zeile 6: msg 22 Sherry (Form msg 2 @0x00E80)"),
    orig(0x0E84, 0x0E88, "Sleep 100 wie nach msg 2 (@0x00E84; Sherry hat in sub02 keinen Gestenblock)"),
    # Zeile 7 (msg 4 Leon, antwortet Sherry) und Zeile 8 (msg 5 John): Leon blickt vor msg 5 zu John
    orig(0x0EC6, 0x0EEA, "Zeile 7: msg 4 Leon + Gesten 1/1/7 (@0x00EC6.., Original)"),
    # Runde 42 namen Stufe 4: Leons zweiter Satz als eigene Nachricht 32 (vorher Seite 2 von msg 4, stand nur ~20 Bilder bis msg 5).
    # Form msg 4 @0x00EC6 `2b 04 00 00`, danach dieselben Leon-Gesten 1/1/7 (@0x00ECA..@0x00EEA: Sleep 50+50+20 = 120 Bilder) - so steht
    # msg 32 ihren Nachlauf `04 01 01 63` (99 Bilder, ROOM6030 @0x13F5) ganz aus wie jede Original-Zeile des Raums; zwischen dem Ende
    # von msg 4 und msg 32 liegen ~20 Bilder ohne Text (wie zwischen allen Zeilen von sub02).
    msg_on(32, 0x0EC6, 4, "Zeile 7b: msg 32 Leon (Form msg 4 @0x00EC6), Runde 42 namen Stufe 4"),
    orig(0x0ECA, 0x0EEA, "Leon-Gesten 1/1/7 wie nach msg 4 (Work_set 1 @0x00ECA..)"),
    LEON_SIEHT_JOHN,
    orig(0x0EEA, 0x0F22, "Zeile 8: msg 5 John + Gesten 3/3/1/1/7 (@0x00EEA.., Original)"),
    # Zeilen 9..10 (Original-Folge msg 6..7)
    orig(0x0F22, 0x0F46, "Zeile 9: msg 6 Leon + Gesten 4/4/7 (@0x00F22.., Original)"),
    orig(0x0F46, 0x0F7E, "Zeile 10: msg 7 John + Gesten 0/0/1/1/7 (@0x00F46.., Original)"),
    # Zeile 11 (neu, msg 23 Sherry): Sherry dreht sich zu Leon, Leon blickt zu ihr
    orig(0x0E4A, 0x0E4E, "Work_set(2,2) Sherry (@0x00E4A)"),
    op(0x01160, [0x40, 0x00, 0x09, 0x21, 0xDF, 0xF0, 0x41, 0xA4],
       [0x40, 0x00, 0x09, 0x21, 0x11, 0x9E, 0xBB, 0xA1],
       "Plc_dest Modus 9 (drehen) Bit 0x21 wie Annette sub08 @0x01160; Ziel = Leons Marke sub02 @0x00E0E "
       "(-25071,-24133)"),
    orig(0x01168, 0x01174, "warten bis (5,0x21): Do/Evt_next/Edwhile Ck(5,0x21)==0 (sub08 @0x01168..)"),
    LEON_SIEHT_SHERRY[0], LEON_SIEHT_SHERRY[1],
    msg_on(23, 0x0E80, 2, "Zeile 11: msg 23 Sherry (Form msg 2 @0x00E80)"),
    orig(0x0E84, 0x0E88, "Sleep 100 wie nach msg 2 (@0x00E84)"),
    # Zeile 12 (neu, msg 24 John): John blickt zu Sherry, Leon zu John
    orig(0x0E9E, 0x0EA2, "Work_set(2,0) John (@0x00E9E)"),
    orig(0x0DEC, 0x0DFA, "Plc_neck auf Sherrys Platz (-24471,0,-25806) @0x00DEC + Sleep 1"),
    LEON_SIEHT_JOHN,
    msg_on(24, 0x0E9A, 3, "Zeile 12: msg 24 John (Form msg 3 @0x00E9A)"),
    orig(0x0F4A, 0x0F7E, "John-Gesten 0/0/1/1/7 wie nach msg 7 (@0x00F4A..)"),
    # Zeile 13 (msg 8 Leon) und Schluss
    orig(0x0F7E, 0x0FA2, "Zeile 13: msg 8 Leon + Gesten 5/5/7, Sleep 30 (@0x00F7E.., Original)"),
    orig(0x0FA2, 0x0FE8, "Schluss wie Original: Blicke zurueck (John, Sherry, Leon), Sperren auf, Cut 12, Cut_auto, "
                         "Plc_ret, Evt_end @0x00FE6"),
]
FOLGE_6030_WAGEN = [0, 1, 21, 2, 3, 22, 4, 32, 5, 6, 7, 23, 24, 8]

# ------------------------------------------------------------------------------------------------------------------
# ROOM6030 sub08 (Leon): Fuehrerstand, a..d vor "Certainly." (msg 13).
P_6030_PULT = [
    orig(0x113A, 0x11CC, "sub08 @0x0113A..: Sperren, Cut 1, Leon/Annette drehen sich zueinander, "
                         "a = msg 11 Annette + Gesten 13/13/16, b = msg 12 Leon + Gesten 1/1/0/0/7 (Original-Folge)"),
    orig(0x11CC, 0x11D0, "Work_set(2,0) Annette (@0x011CC)"),
    msg_on(25, 0x11D0, 13, "c = msg 25 Annette (Form msg 13 @0x011D0)"),
    orig(0x12EA, 0x1306, "Annette-Gesten 14/14/16 wie nach msg 18 (@0x012EA..)"),
    orig(0x1306, 0x130A, "Work_set 1 Leon (@0x01306)"),
    msg_on(26, 0x130A, 19, "d = msg 26 Leon (Form msg 19 @0x0130A)"),
    orig(0x12C6, 0x12E2, "Leon-Gesten 3/3/7 wie nach msg 17 (@0x012C6..)"),
    orig(0x11CC, 0x1376, "ab Work_set(2,0) + msg 13 'Certainly.' @0x011D0 bis Evt_end @0x01374 (Original)"),
]
FOLGE_6030_PULT = [11, 12, 25, 26, 13, 14, 15, 16, 17, 18, 19]

# ------------------------------------------------------------------------------------------------------------------
# Nachbesserung 1: Port-Bit (9,128) "Sherry-Nachtrag offen". Set-Form `22 <Bank> <Bit> <Wert>` (Opcode 0x22, Handler
# 0x8003fdd0); Vorlagen sub08 @0x0113A `22 04 17 01` (setzen) und @0x0136C `22 01 1b 00` (loeschen).
BIT_NACHTRAG = 0x80          # = 128 = re15_story80_r41.h RE15_S80_BIT_NACHTRAG
SET_NACHTRAG = op(0x0113A, [0x22, 0x04, 0x17, 0x01], [0x22, 0x09, BIT_NACHTRAG, 0x01],
                  "Set(9,128)=1 Port-Bit 'Sherry-Nachtrag offen' (Form Set(4,23) @0x0113A)")
LOESCH_NACHTRAG = op(0x0136C, [0x22, 0x01, 0x1B, 0x00], [0x22, 0x09, BIT_NACHTRAG, 0x00],
                     "Set(9,128)=0 Nachtrag verbraucht (Form Set(1,27)=0 @0x0136C)")

# ROOM6030 sub08, Fuehrerstand VOR dem Wagen ((3,22)=0): a = msg 11, b' = msg 31 statt 12/25/26, dann wie Original.
P_6030_PULT_VORNE = [
    orig(0x113A, 0x113E, "Set(4,23)=1 @0x0113A (Szene gesehen, Original)"),
    SET_NACHTRAG,
    orig(0x113E, 0x1198, "Sperren @0x0113E, Cut 1, Leon/Annette drehen sich zueinander, a = msg 11 Annette + Gesten "
                         "13/13/16, Work_set 1 Leon @0x01194 (Original)"),
    msg_on(31, 0x1198, 12, "b' = msg 31 Leon (Form msg 12 @0x01198)"),
    orig(0x119C, 0x11CC, "Leon-Gesten 1/1/0/0/7 wie nach msg 12 (@0x0119C.., Original)"),
    orig(0x11CC, 0x1376, "ab Work_set(2,0) + msg 13 'Certainly.' @0x011D0 bis Evt_end @0x01374 (Original)"),
]
FOLGE_6030_PULT_VORNE = [11, 31, 13, 14, 15, 16, 17, 18, 19]

# ROOM6030 sub09 (Annette am Pult, gerufen sub07 @0x0110C), Nachtrag NACH dem Wagen ((9,128)=1, (3,22)=1).
P_6030_NACHTRAG = [
    LOESCH_NACHTRAG,
    orig(0x113E, 0x1174, "Sperren (2,7)/(1,27) @0x0113E, Sleep 1, Cut 1, Leon dreht sich zu Annette @0x01150, Annette "
                         "zu Leon @0x01160, warten bis (5,0x21) @0x01168..@0x01170 (sub08, ohne Set(4,23))"),
    orig(0x1194, 0x1198, "Work_set 1 Leon (@0x01194)"),
    msg_on(40, 0x1198, 12, "msg 40 Leon (Form msg 12 @0x01198)"),
    orig(0x119C, 0x11CC, "Leon-Gesten 1/1/0/0/7 wie nach msg 12 (@0x0119C..)"),
    orig(0x11CC, 0x11D0, "Work_set(2,0) Annette (@0x011CC)"),
    msg_on(25, 0x11D0, 13, "msg 25 Annette (Form msg 13 @0x011D0)"),
    orig(0x12EA, 0x1306, "Annette-Gesten 14/14/16 wie nach msg 18 (@0x012EA..; wie im Programm sub08)"),
    orig(0x131E, 0x1376, "Schluss wie sub08: Annette dreht zum Pult @0x01322, Cut 0 @0x01336, Pult-Haltung 18/19 + "
                         "Plc_flg 4 @0x01338..@0x01348 (= sub09 @0x0137A..@0x0138A), Leon dreht ab @0x01350, Sperren auf "
                         "@0x01368/@0x0136C, Cut_auto, Plc_ret, Evt_end @0x01374"),
]
FOLGE_6030_NACHTRAG = [40, 25]

# Integration A2 (Gegenpruefung Welle 2 Befund 2, Weg F gemessen): Fuehrerstand vorne zuerst -> Wagen -> ohne Rueckkehr ->
# Tor. Der Torschalter ROOM6000 sub07 setzt @0x011B4 `22 03 59 01` (3,89)=1 (und @0x011C0 (3,22), @0x011CE (4,23)); danach
# erreicht sub07 den Nachtrag-Zweig nie mehr (@0x010F4 `21 03 59 00` Ck(3,89)=0), sondern nur noch @0x01128 Ck(3,89)=1 ->
# @0x01132 `04 ff 18 05` Evt_exec sub05. Ohne diesen Eintrag lief dort die Zugstart-Szene der Spur AN (msg 28 "...Sherry is
# back there!") ohne den Nachtrag, und (9,128) blieb 1 stehen. Der Nachtrag laeuft deshalb an dieser Stelle (Regel NB1:
# naechstes Betreten des Fuehrerstands nach dem Wagen). Weil sub07 die Frau ab (3,89)=1 nicht mehr setzt, setzt das
# Programm sie selbst mit den Bytes des Nachtrag-Zweigs @0x010F8 und startet die Licht-Schleife wie sub07 @0x01132 (der
# Haken der Spur AN laesst sub05 dann unveraendert, weil die 0x4D steht); der Zugstart folgt beim naechsten Betreten.
P_6030_NACHTRAG_TOR = [
    LOESCH_NACHTRAG,
    orig(0x10F8, 0x110C, "Sce_em_set Annette am Pult (sub07 @0x010F8, Bytes des Nachtrag-Zweigs)"),
    orig(0x1132, 0x1136, "Evt_exec sub05 Licht-Schleife wie sub07 @0x01132 (Zweig Ck(3,89)=1 @0x01128)"),
] + P_6030_NACHTRAG[1:]
FOLGE_6030_NACHTRAG_TOR = [40, 25]

PROGRAMME = [
    # (Raum, Sub, C-Name, Segmente, Folge, Messtext)
    ("30C1", 3, "k_s80_30c1_sub03", P_30C1, FOLGE_30C1, "Elza trifft Marvin, kurze Fassung"),
    ("6030", 2, "k_s80_6030_sub02", P_6030_WAGEN, FOLGE_6030_WAGEN, "Leon, Wagen-Szene"),
    ("6030", 8, "k_s80_6030_sub08", P_6030_PULT, FOLGE_6030_PULT, "Leon, Fuehrerstand"),
    ("6030", 8, "k_s80_6030_sub08_vorne", P_6030_PULT_VORNE, FOLGE_6030_PULT_VORNE,
     "Leon, Fuehrerstand vor dem Wagen (NB1)"),
    ("6030", 9, "k_s80_6030_sub09_nachtrag", P_6030_NACHTRAG, FOLGE_6030_NACHTRAG,
     "Leon, Fuehrerstand nach dem Wagen, Nachtrag (NB1)"),
    ("6030", 5, "k_s80_6030_sub05_nachtrag_tor", P_6030_NACHTRAG_TOR, FOLGE_6030_NACHTRAG_TOR,
     "Leon, Fuehrerstand nach dem Tor, Nachtrag (Integration A2)"),
]
SIG_N = 16


def bauen(room, sub, segs):
    d = rdt(room)
    tab = SUB_TAB[room]
    o = d[tab + 2 * sub] | d[tab + 2 * sub + 1] << 8
    if tab + o != SUB_SOLL[(room, sub)]:
        raise SystemExit("ROOM%s sub%02d beginnt @0x%05X, erwartet @0x%05X" % (room, sub, tab + o, SUB_SOLL[(room, sub)]))
    prog = bytearray()
    zeilen = []
    for s in segs:
        if s[0] == "orig":
            _, lo, hi, was = s
            b = d[lo:hi]
            # Bereich muss auf Opcode-Grenzen liegen: ab lo mit der Laengentabelle bis genau hi
            pc = lo
            while pc < hi:
                n = SD.op_size(d, pc)
                if not n:
                    raise SystemExit("ROOM%s @0x%05X: unbekannter Opcode %02x" % (room, pc, d[pc]))
                pc += n
            if pc != hi:
                raise SystemExit("ROOM%s orig 0x%05X..0x%05X endet nicht auf einer Opcode-Grenze (0x%05X)" % (room, lo, hi, pc))
            zeilen.append((len(prog), "orig @0x%05X..0x%05X (%d B) %s" % (lo, hi, hi - lo, was)))
            prog += b
        else:
            _, voff, vbytes, neu, was = s
            if d[voff:voff + len(vbytes)] != vbytes:
                raise SystemExit("ROOM%s Vorlage @0x%05X ist %s, erwartet %s" % (room, voff, d[voff:voff + len(vbytes)].hex(" "),
                                                                             vbytes.hex(" ")))
            if len(neu) != len(vbytes) or neu[0] != vbytes[0]:
                raise SystemExit("ROOM%s op @0x%05X: Form weicht ab" % (room, voff))
            zeilen.append((len(prog), "op   %s  (Vorlage @0x%05X `%s`) %s" % (neu.hex(" "), voff, vbytes.hex(" "), was)))
            prog += neu
    # Ganzes Programm abgehen
    pc, msgs, listing = 0, [], []
    while pc < len(prog):
        n = SD.op_size(prog, pc)
        if not n:
            raise SystemExit("ROOM%s sub%02d Programm +0x%04X: unbekannter Opcode %02x" % (room, sub, pc, prog[pc]))
        if prog[pc] == 0x2B:
            msgs.append((pc, prog[pc + 1]))
        listing.append("+0x%04X %-14s %s" % (pc, SD.NAMES.get(prog[pc], "?"), prog[pc:pc + n].hex(" ")))
        pc += n
    if pc != len(prog) or prog[-2:] != b"\x01\x00":
        raise SystemExit("ROOM%s sub%02d: Programm endet nicht auf Evt_end" % (room, sub))
    sig_off = SUB_SOLL[(room, sub)]
    return bytes(prog), zeilen, msgs, listing, sig_off, d[sig_off:sig_off + SIG_N]


def schreiben(erg):
    L = ["/* AUTO-GENERIERT von re15_port/tools/r41_story80/szenen_bauen.py - NICHT von Hand aendern.",
         " * Runde 41 Spur ST80-T: Szenen aus Original-Bytes (Belege je Zeile), Dossier analysis/story80/TEXTE_UMSETZUNG.md.",
         " * Ablauf = NUTZERENTSCHEID; Gesten/Blicke = PORT-WAHL aus Original-Bloecken desselben Sprechers. */", ""]
    for (room, sub, name, segs, folge, was), (prog, zeilen, msgs, listing, sig_off, sig) in erg:
        L.append("/* ROOM%s sub%02d - %s (%d B). Bausteine:" % (room, sub, was, len(prog)))
        for off, z in zeilen:
            L.append(" *   +0x%04X %s" % (off, z))
        L.append(" * Message_on im Programm: %s */" % ", ".join("+0x%04X msg %d" % (o, m) for o, m in msgs))
        L.append("static const uint8_t %s[%d] = {" % (name, len(prog)))
        for i in range(0, len(prog), 16):
            L.append("    " + ",".join("0x%02x" % b for b in prog[i:i + 16]) + ",")
        L.append("};")
        L.append("/* Signatur: ROOM%s sub%02d @0x%05X, erste %d Bytes */" % (room, sub, sig_off, SIG_N))
        L.append("static const uint8_t %s_sig[%d] = { %s };" % (name, SIG_N, ",".join("0x%02x" % b for b in sig)))
        L.append("#define %s_SIG_OFF 0x%05Xu" % (name.upper(), sig_off))
        L.append("static const uint8_t %s_folge[%d] = { %s };" % (name, len(folge), ", ".join(str(m) for m in folge)))
        L.append("")
    data = "\n".join(L) + "\n"
    with open(OUT, "w", encoding="utf-8", newline="\n") as f:
        f.write(data)


def main():
    erg = []
    for p in PROGRAMME:
        room, sub, name, segs, folge, was = p
        prog, zeilen, msgs, listing, sig_off, sig = bauen(room, sub, segs)
        ist = [m for _, m in msgs]
        if ist != folge:
            raise SystemExit("ROOM%s sub%02d: Message_on-Folge %s != Soll %s" % (room, sub, ist, folge))
        print("== ROOM%s sub%02d %s: %d B, Folge %s" % (room, sub, was, len(prog), ist))
        for z in listing:
            print("   " + z)
        erg.append((p, (prog, zeilen, msgs, listing, sig_off, sig)))
    if "--check" not in sys.argv:
        schreiben(erg)
        print("geschrieben:", os.path.relpath(OUT, os.path.dirname(PORT)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
