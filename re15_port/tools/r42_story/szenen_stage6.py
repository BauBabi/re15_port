#!/usr/bin/env python3
"""szenen_stage6.py - Runde 42 Story, Stufe STAGE6: die Programme fuer Elzas Zug ROOM6031 als engine/src/gen/story6_r42_szenen.inc.

PORT-STORY nach dem freigegebenen Konzept (C:/workspace/git/reAi_v2_entscheidung_2026-11-06/11_story_v0831/KONZEPT.md, 4.2 E6/E9;
Ids VERTRAG analysis/befunde_runde42/VERTRAG.md 2.3, Bit 130 = RE15_R42_BIT_ZUG_ELZA). Leons Fassung (6030) bleibt: die Wagen-Szene
L21 baut tools/r41_story80/szenen_bauen.py, den Zugstart mit Annette engine/src/annette_r41.c.

Kein Asset-Patch: der Port tauscht nur das Ziel eines Evt_exec (engine/src/story6_r42.c, scd_vm.c op_evt_exec):
  ROOM6031 sub01 @0x00D6E `04 ff 18 02` -> sub02 @0x00D78   -> k_s6_6031_wagen  (E6: msg 0, 1, 2, 12, 13)
  ROOM6031 sub07 @0x00F54 `04 ff 18 05` -> sub05 @0x00E2A   -> k_s6_6031_zug    (E9) bei (3,89)=1, (9,130)=0, Cut 0
  ROOM6031 sub06 @0x00EEA `04 ff 18 05` -> sub05 @0x00E2A   -> k_s6_6031_tuer   bei (3,89)=1, (9,130)=0, Cut 6/8/12
Vor dem Umleiten prueft der Port die ersten 16 Bytes des Original-Subs (Signatur unten).

FORM (wie szenen_bauen.py): jede Zeile ist
  orig  lo..hi        ausgelieferter Byte-Bereich von ROOM6031 (Datei-Offsets), auf Opcode-Grenzen geprueft
  fremd raum lo..hi   ausgelieferter Byte-Bereich eines ANDEREN Raums (nur Gesten derselben Figur: John 0x49 hat in 6031 keine
                      Zeile; ROOM6030 sub02 nach msg 5 @0x00EF2..)
  op    raum off      Opcode in ausgelieferter Form (Vorlage geprueft, gleiche Laenge, gleicher Opcode) mit getauschtem Argument
  port                Port-Befehl in Original-Form fuer Port-Signale/PORT-STORY-Lagen (Vorlage im Text); dieselben Formen wie
                      Leons Zugstart (annette_r41.c k_zug_seg z_sig_an / z_sig_krach / z_schlaf25 / z_dreh_* / z_warte*)
Das Werkzeug geht jedes Programm mit der Opcode-Laengentabelle ab und bricht ab, wenn es nicht genau auf Evt_end endet oder die
Message_on-Folge nicht der Soll-Folge gleicht.

Aufruf: python re15_port/tools/r42_story/szenen_stage6.py          (schreibt die .inc, druckt das Listing)
        python re15_port/tools/r42_story/szenen_stage6.py --check  (prueft nur)
"""
import os
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(os.path.dirname(HIER))
sys.path.insert(0, os.path.join(PORT, "tools", "r41_story80"))
import szenen_bauen as S80  # noqa: E402  (SD = Opcode-Laengen/Namen aus tools/scd_dump_room.py, rdt())

OUT = os.path.join(PORT, "engine", "src", "gen", "story6_r42_szenen.inc")
RAUM = "6031"
SUB_TAB = 0x0D14                      # RDT-Kopf sub_scd (scd_dump_room "sub_scd=0xD14")
SUB_SOLL = {2: 0x0D78, 5: 0x0E2A}
SIG_N = 16


def LE16(v):
    v &= 0xFFFF
    return [v & 0xFF, v >> 8]


# ---- Bank-5-Signale / Bits (Werte = include/re15_story6_r42.h) -----------------------------------------------------------
SIG_ZUG_AN = 0x10      # 6031-Skripte nutzen nur (5,0x20)/(5,0x21) (sub02 @0x00D9A, sub08 @0x00F92) -> 0x10/0x11 frei
SIG_KRACH = 0x11
AN_ELZA = 0x20         # Plc_dest-Ankunftsbit Spieler (Form sub08 @0x00F72 `40 00 09 20`)
AN_NPC = 0x21          # Plc_dest-Ankunftsbit NPC (Form sub08 @0x00F82 `40 00 09 21`)
BIT_ZUG_ELZA = 130     # RE15_R42_BIT_ZUG_ELZA (Vertrag 1.2)
MSG = {"E6a": 12, "E6b": 13, "E9a": 14, "E9b": 15, "E9c": 16, "E9d": 17}
FILM_MSG = 60          # RE15_ZUG600_MSG_FILM (include/re15_zws_r41.h): Port-Zeile ohne Text, Zugfilm; nicht in den Soll-Folgen

# ---- PORT-STORY-Lagen (E9) -------------------------------------------------------------------------------------------------
# Elza steht am Eintritt der Tuer Slot 1 (main00 @0x00C52 Ziel `df f0 .. 41 a4` = (-3873,-23487)); Sherry am Pult (sub07 @0x00F1E
# Lage `02 f9 .. 00 a9` = (-1790,-22272)); die Wagentuer Slot 0 (main00 @0x00C32) liegt in der Wand x -4341: Ziel ihrer Wand-
# flaeche (-3873,-22550) = Leons Z_TUER (annette_r41.c, gemessen dort). John steht im Fuehrerstand (er ist mit Elza nach vorn
# gekommen): (-2900,-23800), 1022 von Elza und 1889 von Sherry (zwei Koerperradien 902), innerhalb der Waende x -4341..-1322,
# z -26410..-20430 (AN_belege zug6030.py) mit 450 Abstand; 141 neben Leons gemessenem Seitenplatz Z_LEON (-3000,-23700), dort
# in Cut 0 zu 100 % frei. Blick zum Pult (Gier 3481 = atan2 zu Sherry, wp_yaw_zu-Konvention 0 = +X, 3072 = +Z).
ELZA = (-3873, -23487)
SHERRY = (-1790, -22272)
TUER = (-3873, -22550)
JOHN = (-2900, -23800)
JOHN_GIER = 3481


def orig(lo, hi, was):
    return ("fremd", RAUM, lo, hi, was)


def fremd(raum, lo, hi, was):
    return ("fremd", raum, lo, hi, was)


def op(raum, off, vorlage, neu, was):
    return ("op", raum, off, bytes(vorlage), bytes(neu), was)


def port(b, was):
    return ("port", bytes(b), was)


def msg_on(mid, raum, off, vid, was):
    return op(raum, off, [0x2B, vid, 0x00, 0x00], [0x2B, mid, 0x00, 0x00], was)


def drehen(bit, xz, was):
    return port([0x40, 0x00, 0x09, bit] + LE16(xz[0]) + LE16(xz[1]), "Plc_dest Modus 9 (drehen) Bit 0x%02x auf (%d,%d), Form sub08 "
                "@0x00F72/@0x00F82 `40 00 09 2x`: %s" % (bit, xz[0], xz[1], was))


def warte(bit, was):
    # Do/Evt_next/Edwhile Ck(5,bit,0): Form sub08 @0x00F8A..@0x00F96 (bit 0x21), sub02 @0x00D92..@0x00D9E (bit 0x20)
    return port([0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, bit, 0x00],
                "warten bis (5,0x%02x) (Form sub08 @0x00F8A..@0x00F96): %s" % (bit, was))


WORK_ELZA = orig(0x0F6E, 0x0F72, "Work_set(1,0) Elza (sub08 @0x00F6E)")
WORK_SHERRY = orig(0x0F7E, 0x0F82, "Work_set(2,0) Sherry (sub08 @0x00F7E; Sherry = Sce_em_set-Slot 0)")
WORK_JOHN = op(RAUM, 0x0F7E, [0x2E, 0x02, 0x00, 0x00], [0x2E, 0x02, 0x01, 0x00],
               "Work_set(2,1) John (Form @0x00F7E; John = Sce_em_set-Slot 1, unten)")

# ------------------------------------------------------------------------------------------------------------------------------
# E6: Wagen-Szene sub02 - msg 0 (story_r36), 1, 2 wie heute, dann 12 Sherry, 13 Elza.
P_WAGEN = [
    orig(0x0D78, 0x0DFA, "sub02 @0x00D78..: Set(3,22) @0x00D78, Sperren, Cut 10, Elza geht zur Marke und dreht sich, msg 0 Elza + "
                         "Gesten 13/13/16, msg 1 Sherry + Sleep 100 @0x00DD6, msg 2 Elza + Gesten 12/12/16 (bis Sleep 20 @0x00DF6)"),
    msg_on(MSG["E6a"], RAUM, 0x0DD2, 1, "msg 12 Sherry (Form msg 1 @0x00DD2)"),
    orig(0x0DD6, 0x0DDA, "Sleep 100 wie nach msg 1 (@0x00DD6; Sherry hat in sub02 keinen Gestenblock)"),
    msg_on(MSG["E6b"], RAUM, 0x0DDA, 2, "msg 13 Elza (Form msg 2 @0x00DDA)"),
    orig(0x0DDE, 0x0DFA, "Elza-Gesten 12/12/16 wie nach msg 2 (@0x00DDE..)"),
    orig(0x0DFA, 0x0E0E, "Schluss wie Original: Cut 9, Sleep 30, Sperren auf, Cut_auto, Plc_ret, Evt_end @0x00E0C"),
]
FOLGE_WAGEN = [0, 1, 2, MSG["E6a"], MSG["E6b"]]

# ------------------------------------------------------------------------------------------------------------------------------
# E9: Zugstart im Fuehrerstand (Elzas Fassung; Ablauf nach Leons Zugstart annette_r41.c k_zug_seg).
P_ZUG = [
    op(RAUM, 0x0F5C, [0x22, 0x04, 0x17, 0x01], [0x22, 0x09, BIT_ZUG_ELZA, 0x01],
       "Set(9,130)=1 RE15_R42_BIT_ZUG_ELZA (Form Set(4,23) sub08 @0x00F5C), einmalig"),
    orig(0x0F60, 0x0F68, "Sperren (2,7)/(1,27) (sub08 @0x00F60/@0x00F64)"),
    orig(0x0F1E, 0x0F32, "Sce_em_set Sherry am Pult (sub07 @0x00F1E, Zweig (3,89)=0; ab (3,89)=1 setzt sub07 sie nicht mehr)"),
    op(RAUM, 0x0E56, [0x44, 0x01, 0x49, 0x40, 0x00, 0x00, 0x00, 0xFF, 0x39, 0x9A, 0x00, 0x00, 0x48, 0x9B, 0x00, 0x00, 0x00, 0x0C,
                      0x00, 0x00],
       [0x44, 0x01, 0x49, 0x40, 0x00, 0x00, 0x00, 0xFF] + LE16(JOHN[0]) + [0x00, 0x00] + LE16(JOHN[1]) + [0x00, 0x00]
       + LE16(JOHN_GIER) + [0x00, 0x00],
       "Sce_em_set John (Satz sub06 @0x00E56: Slot 1, Typ 0x49) im Fuehrerstand (-2900,-23800) Gier 3481 - PORT-STORY"),
    orig(0x0F68, 0x0F7E, "Sleep 1, Cut 1, Work_set(1,0), Elza dreht sich zu Sherry (Plc_dest @0x00F72), Sleep 10 (sub08 @0x00F68..)"),
    WORK_SHERRY,
    orig(0x0FC2, 0x0FC4, "Cut 3 Pult (sub08 @0x00FC2)"),
    orig(0x0FC8, 0x0FDC, "Sherry am Pult: Plc_motion 0, Sleep 50, Plc_motion 0, Plc_flg 0x80, Sleep 50 (sub08 @0x00FC8.., ohne msg 6)"),
    # Runde 42 Strang finale Stufe 2: kein Zug-an-Signal mehr (Set(5,0x10)) - der Zugfilm traegt das Hochfahren selbst (ROOM7020
    # snd0 Satz 0x0C bei Film-Bild 18 = ANNETTE_SE Satz 3), wie Leons z_film in annette_r41.c.
    orig(0x0FDC, 0x0FE6, "Pultlicht: Set(1,28,1), Cut 4, Sleep 15 (sub08 @0x00FDC..)"),
    orig(0x0FE8, 0x0FEC, "Sleep 15 (@0x00FE8; Cut 5 @0x00FE6 = rotes WARNING des gescheiterten Starts entfaellt wie bei Leon)"),
    orig(0x0FEC, 0x0FF0, "Set(1,28,0) (@0x00FEC)"),
    # Musik unter dem Zugfilm (Stufe 2 Runde 2, wie Leons z_musik_halt/z_musik_weiter in annette_r41.c): die laufende Spur MAIN vor
    # dem Film anhalten, danach weiter = RE2 ROOM1080 sub14 @0x02524 (Op 4) / @0x02536 (Op 3), ROOM4090 @0x02804/@0x0282B;
    # RE1.5-Handler FUN_80044da4 Fall 4 SsSeqPause @0x80044efc, Fall 3 SsSeqReplay @0x80044ec4. Nur Slot 0: bei (3,89)=1 spielt
    # 6031 nur MAIN (sub06 @0x00EB4 Ck(3,89)=1 -> @0x00EB8, sub07 @0x00F4A -> @0x00F4E); SUB (Slot 1) nur im (3,89)=0-Zweig
    # @0x00E86 -> @0x00E8A / @0x00F3A -> @0x00F3E, ein Op 3 darauf wuerde ihn einschalten.
    op(RAUM, 0x0EB8, [0x54, 0x00, 0x01, 0x00, 0x00, 0x00], [0x54, 0x00, 0x04, 0x00, 0x00, 0x00],
       "Sce_bgm_control MAIN (Slot 0) anhalten (Form sub06 @0x00EB8, Op 4 = SsSeqPause @0x80044efc)"),
    msg_on(FILM_MSG, RAUM, 0x0FC4, 6, "Zugfilm R600ZUG.STR (Port-Zeile ohne Text, Form der Pult-Zeile msg 6 @0x00FC4, die das Programm "
                                       "auslaesst; Haken re15_zws_r41_message_on Tabelle zug601) - nach dem Pultlicht, vor Cut 1"),
    op(RAUM, 0x0EB8, [0x54, 0x00, 0x01, 0x00, 0x00, 0x00], [0x54, 0x00, 0x03, 0x00, 0x00, 0x00],
       "Sce_bgm_control MAIN (Slot 0) weiter (Form sub06 @0x00EB8, Op 3 = SsSeqReplay @0x80044ec4)"),
    orig(0x1020, 0x1026, "Cut 1, Sleep 60 (sub08 @0x01020)"),
    orig(0x0F54, 0x0F58, "Evt_exec sub05 Licht-Schleife = Fahrt (sub07 @0x00F54, Zweig Ck(3,89)=1)"),
    orig(0x0F82, 0x0F96, "Sherry dreht sich zu Elza: Plc_dest 9 Bit 0x21 auf (-3873,-23487) + warten (sub08 @0x00F82..@0x00F96)"),
    msg_on(MSG["E9a"], RAUM, 0x0F96, 5, "msg 14 Sherry (Form msg 5 @0x00F96)"),
    orig(0x0F9A, 0x0FAE, "Sherry-Gesten 1/1 wie nach msg 5 (@0x00F9A..)"),
    port([0x22, 0x05, SIG_KRACH, 0x01], "Set(5,0x11)=1 Signal Krachen aus dem hinteren Wagen (ANNETTE_SE Satz 0; Form z_sig_krach)"),
    orig(0x0FF0, 0x100C, "WARNING beim Krachen (Vorschlag A7): Set(1,29,1), For 3 { Cut 4, Sleep 15, Cut 5 (rotes WARNING), Sleep 15 }, "
                         "Set(1,29,0) = gescheiterter Start sub08 @0x00FF0..@0x0100B unveraendert, wie Leons z_warnung"),
    orig(0x1020, 0x1022, "Cut 1 zurueck (sub08 @0x01020)"),
    port([0x09, 0x0A, 0x19, 0x00], "Sleep 25 (Form @0x00F7A; Zahl RE2 Claire ROOM6141 sub02 @0x0104C `0a 19 00`, wie z_schlaf25)"),
    WORK_ELZA, drehen(AN_ELZA, TUER, "Elza zur Wagentuer"), warte(AN_ELZA, "Elza"),
    WORK_SHERRY, drehen(AN_NPC, TUER, "Sherry zur Wagentuer"), warte(AN_NPC, "Sherry"),
    WORK_JOHN, drehen(AN_NPC, TUER, "John zur Wagentuer"), warte(AN_NPC, "John"),
    port([0x09, 0x0A, 0x24, 0x00], "Sleep 36 (RE2 Claire ROOM6141 sub02 @0x01072 `0a 24 00`, wie z_schlaf36)"),
    WORK_JOHN,
    msg_on(MSG["E9b"], "6030", 0x0EEA, 5, "msg 15 John (Form ROOM6030 msg 5 @0x00EEA)"),
    fremd("6030", 0x0EF2, 0x0F22, "John-Gesten 3/3/1/1/7 (ROOM6030 sub02 nach msg 5 @0x00EF2.., dieselbe Figur 0x49)"),
    WORK_SHERRY,
    msg_on(MSG["E9c"], RAUM, 0x0DD2, 1, "msg 16 Sherry (Form msg 1 @0x00DD2)"),
    orig(0x0DD6, 0x0DDA, "Sleep 100 wie nach msg 1 (@0x00DD6)"),
    WORK_ELZA, drehen(AN_ELZA, JOHN, "Elza zu John"), warte(AN_ELZA, "Elza"),
    msg_on(MSG["E9d"], RAUM, 0x109E, 10, "msg 17 Elza (Form msg 10 @0x0109E)"),
    orig(0x10A2, 0x10D2, "Elza-Gesten 12/12/9/9/16 wie nach msg 10 (@0x010A2..)"),
    orig(0x1116, 0x1128, "Schluss wie sub08: Sleep 30, Sperren auf, Cut_auto, Plc_ret, Evt_end (@0x01116..@0x01126)"),
]
FOLGE_ZUG = [MSG["E9a"], MSG["E9b"], MSG["E9c"], MSG["E9d"]]

# ------------------------------------------------------------------------------------------------------------------------------
# Wagen vor dem Zugstart: wie der (3,89)=0-Zweig von sub06 (Form Leons k_tuer, annette_r41.c).
P_TUER = [
    orig(0x0C72, 0x0C92, "Door_aot_set Slot 2 Ausgang zur Bahnsteighalle (main00 @0x00C72; sub06 @0x00EBE hatte ihn auf msg 11 gestellt)"),
    orig(0x0E90, 0x0E9A, "Aot_reset Slot 3 -> msg 4 'I have to get this thing moving first!' (sub06 @0x00E90, Zweig (3,89)=0)"),
    orig(0x0EF0, 0x0EF2, "Evt_end (sub06 @0x00EF0)"),
]

PROGRAMME = [
    # (C-Name, Sub der Signatur, Segmente, Soll-Folge, Messtext)
    ("k_s6_6031_wagen", 2, P_WAGEN, FOLGE_WAGEN, "Elza, Wagen-Szene E6"),
    ("k_s6_6031_zug", 5, P_ZUG, FOLGE_ZUG, "Elza, Zugstart E9"),
    ("k_s6_6031_tuer", 5, P_TUER, [], "Elza, Wagen vor dem Zugstart"),
]


def bauen(sub, segs):
    rdts = {}

    def rdt(r):
        if r not in rdts:
            rdts[r] = S80.rdt(r)
        return rdts[r]

    d = rdt(RAUM)
    o = d[SUB_TAB + 2 * sub] | d[SUB_TAB + 2 * sub + 1] << 8
    if SUB_TAB + o != SUB_SOLL[sub]:
        raise SystemExit("ROOM%s sub%02d beginnt @0x%05X, erwartet @0x%05X" % (RAUM, sub, SUB_TAB + o, SUB_SOLL[sub]))
    prog, zeilen = bytearray(), []
    for s in segs:
        if s[0] == "fremd":
            _, r, lo, hi, was = s
            q = rdt(r)
            pc = lo
            while pc < hi:
                n = S80.SD.op_size(q, pc)
                if not n:
                    raise SystemExit("ROOM%s @0x%05X: unbekannter Opcode %02x" % (r, pc, q[pc]))
                pc += n
            if pc != hi:
                raise SystemExit("ROOM%s 0x%05X..0x%05X endet nicht auf einer Opcode-Grenze" % (r, lo, hi))
            zeilen.append((len(prog), "orig ROOM%s @0x%05X..0x%05X (%d B) %s" % (r, lo, hi, hi - lo, was)))
            prog += q[lo:hi]
        elif s[0] == "op":
            _, r, off, vb, neu, was = s
            q = rdt(r)
            if q[off:off + len(vb)] != vb:
                raise SystemExit("ROOM%s Vorlage @0x%05X ist %s, erwartet %s" % (r, off, q[off:off + len(vb)].hex(" "), vb.hex(" ")))
            if len(neu) != len(vb) or neu[0] != vb[0]:
                raise SystemExit("ROOM%s op @0x%05X: Form weicht ab" % (r, off))
            zeilen.append((len(prog), "op   %s  (Vorlage ROOM%s @0x%05X `%s`) %s" % (neu.hex(" "), r, off, vb.hex(" "), was)))
            prog += neu
        else:
            _, b, was = s
            if S80.SD.op_size(b, 0) != len(b) and b[0] != 0x11:   # Do-Schleife: 4 Opcodes, unten im Ganzen geprueft
                raise SystemExit("port %s: Laenge %d passt nicht zum Opcode" % (b.hex(" "), len(b)))
            zeilen.append((len(prog), "port %s  %s" % (b.hex(" "), was)))
            prog += b
    pc, msgs, listing = 0, [], []
    while pc < len(prog):
        n = S80.SD.op_size(prog, pc)
        if not n:
            raise SystemExit("Programm sub%02d +0x%04X: unbekannter Opcode %02x" % (sub, pc, prog[pc]))
        if prog[pc] == 0x2B:
            msgs.append((pc, prog[pc + 1]))
        listing.append("+0x%04X %-14s %s" % (pc, S80.SD.NAMES.get(prog[pc], "?"), prog[pc:pc + n].hex(" ")))
        pc += n
    if pc != len(prog) or prog[-2:] != b"\x01\x00":
        raise SystemExit("Programm sub%02d endet nicht auf Evt_end" % sub)
    so = SUB_SOLL[sub]
    return bytes(prog), zeilen, msgs, listing, so, d[so:so + SIG_N]


def main():
    L = ["/* AUTO-GENERIERT von re15_port/tools/r42_story/szenen_stage6.py - NICHT von Hand aendern.",
         " * Runde 42 Story, Stufe STAGE6: Elzas Zug ROOM6031 (Konzept 4.2 E6/E9). PORT-STORY aus Original-Bausteinen (Belege je Zeile). */",
         ""]
    for name, sub, segs, folge, was in PROGRAMME:
        prog, zeilen, msgs, listing, so, sig = bauen(sub, segs)
        ist = [m for _, m in msgs if m != FILM_MSG]   # die Film-Zeile zeigt keinen Text (re15_zws_r41_message_on)
        if sum(1 for _, m in msgs if m == FILM_MSG) != (1 if name == "k_s6_6031_zug" else 0):
            raise SystemExit("%s: Zugfilm-Zeile %d nicht genau einmal im Zugstart" % (name, FILM_MSG))
        if ist != folge:
            raise SystemExit("%s: Message_on-Folge %s != Soll %s" % (name, ist, folge))
        print("== %s (%s): %d B, Folge %s" % (name, was, len(prog), ist))
        for z in listing:
            print("   " + z)
        L.append("/* ROOM%s %s - %s (%d B). Bausteine:" % (RAUM, name, was, len(prog)))
        for off, z in zeilen:
            L.append(" *   +0x%04X %s" % (off, z))
        L.append(" * Message_on im Programm: %s */" % (", ".join("+0x%04X msg %d" % (o, m) for o, m in msgs) or "-"))
        L.append("static const uint8_t %s[%d] = {" % (name, len(prog)))
        # Das Vertrags-Bit steht als Makro im Feld (re15_story_r42.h, vor dieser .inc eingebunden): der Zensus zensus_r42.py weist
        # die Stelle dann als EIGENE Nutzung aus (Set `22 09 <Bit> 01`, Form sub08 @0x00F5C).
        bit_stellen = set(o + 2 for o in range(len(prog) - 3) if prog[o:o + 4] == bytes([0x22, 0x09, BIT_ZUG_ELZA, 0x01]))
        for i in range(0, len(prog), 16):
            L.append("    " + ",".join("RE15_R42_BIT_ZUG_ELZA" if (i + k) in bit_stellen else "0x%02x" % b
                                       for k, b in enumerate(prog[i:i + 16])) + ",")
        L.append("};")
        L.append("/* Signatur: ROOM%s sub%02d @0x%05X, erste %d Bytes */" % (RAUM, sub, so, SIG_N))
        L.append("static const uint8_t %s_sig[%d] = { %s };" % (name, SIG_N, ",".join("0x%02x" % b for b in sig)))
        L.append("static const uint8_t %s_folge[%d] = { %s };" % (name, max(1, len(folge)), ", ".join(str(m) for m in folge) or "0"))
        L.append("#define %s_FOLGE_N %d" % (name.upper(), len(folge)))
        L.append("")
    text = "\n".join(L) + "\n"
    if "--check" in sys.argv:
        alt = open(OUT, encoding="utf-8").read() if os.path.exists(OUT) else ""
        print("gleich" if alt == text else "VERALTET")
        return 0 if alt == text else 1
    with open(OUT, "w", encoding="utf-8", newline="\n") as f:
        f.write(text)
    print("geschrieben:", os.path.relpath(OUT, os.path.dirname(PORT)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
