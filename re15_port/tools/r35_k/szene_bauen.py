#!/usr/bin/env python3
"""szene_bauen.py - Spur K (Runde 35): das SCD-Programm der ROOM10F0-Szene (Ada/Leon/Marvin) aus
ORIGINAL-Opcodes, mit Vorbild je Zeile, als gen/cut10f0_szene.inc.

Jede Zeile ist ein ausgelieferter Opcode in ausgelieferter Form; die Laengen sind die von
engine/src/scd_vm.c s_opcode_sizes (hier: tools/scd_dump_room.py SIZES). Positionen/Texte/
Choreografie = NUTZER-VORGABE / PORT-WAHL (include/re15_cut10f0.h), die FORM der Opcodes ist belegt
(Datei-Offsets in re15_port/shared_assets/PSX/STAGE1/ROOM*.RDT):
  Set(2,7)/Set(1,27)      ROOM1090 sub02 @0x02414/@0x02418 (Pad-Sperre + Balken), Ende @0x024BE/@0x024C2
  Set(9,71) als ERSTES    Einmal-Riegel wie ROOM11B0 sub06 @0x01478 `22 03 83 01`
  Sce_em_set NPC          ROOM11B0 main00 @0x01080 (Marvin 0x40, geparkt -30000) / @0x01094 (Ada 0x42)
  Cut_chg / Cut_auto      ROOM1050 sub03 @0x00D94 `29 06` / @0x00DF4 `3c 01`
  Work_set(1,0)/(2,n)+Nop ROOM11B0 sub06 @0x0148E / @0x014B0
  Pos_set / Dir_set       ROOM10D0 sub21 @0x01ABC `32 00 fb 1d 00 00 91 50` / @0x01AC4 `33 00 00 00 ea 06 00 00`
  Plc_dest Modus 6        ROOM11B0 sub06 @0x014A8 `40 00 06 3f 00 00 00 00` (stehen: Clip 1 -> Clip-2-Ruhe)
  Plc_dest Modus 4 (gehen) Spieler ROOM10D0 sub21 @0x01AAE `40 00 04 21 fb 1d 91 50`; NPC ROOM11B0 sub06 @0x0175C
  Plc_dest Modus 5 (rennen) ROOM11C0 sub02 @0x01868 (Spieler) / ROOM11B0 sub06 @0x015EA (NPC)
  Plc_dest Modus 9 (drehen) ROOM11C0 sub02 @0x0185E (Spieler) / @0x018CA (NPC)
  Warteschleife Ck(5,bit) ROOM1050 sub03 @0x00DCA `11 00 08 00 02 00 12 04 21 05 20 00` (Do/Evt_next/Edwhile/Ck);
                          Bits 0/1/2 wie ROOM11C0 sub05 @0x01C3E / ROOM11B0 sub10 @0x018AC / sub11 @0x018BE
  Message_on Maske 0      ROOM11B0 sub06 @0x014EE `2b 01 00 00`
  Plc_motion / Plc_flg    ROOM11C0 sub02 @0x018D8..@0x018E4 (Clip 19 vor + 19 rueckwaerts `43 00 80 00`)
  Geste + 23 Abschluss    ROOM11C0 sub02 @0x018A4 `3f 00 0f 00` Sleep 40 `3f 00 13 00` Sleep 40 `3f 00 17 00` Sleep 20
  ZEILENTAKT 40+50+20     ROOM11B0 sub06 @0x014EE `2b 01 00 00` Work_set `3f 00 0f 00` @0x014FA `09 0a 28 00` Sleep 40
                          @0x014FE `3f 00 10 00` (Clip 16) @0x01502 `09 0a 32 00` Sleep 50 @0x01506 `3f 00 17 00`
                          @0x0150A `09 0a 14 00` Sleep 20 = 110 Bilder je Zeile (ebenso msg 2 @0x0150E..@0x0152A, msg 8
                          @0x0166E..@0x01686); kuerzeste Original-Zeile 90 (msg 4 @0x01574, Sleep 40 @0x01580 + 50
                          @0x0158C). Zeilen mit nur EINER Geste halten sie ueber Sleep 40 + Sleep 50 (Geste B entfaellt).
  Plc_neck Modus 1        ROOM11C0 sub02 @0x01886 `41 01 fb dc 00 00 f5 c7 64 00` (Weltpunkt, Tempo 0x64)
  Blick+Drehung VOR der Zeile  ROOM11C0 sub02 @0x01886 (Plc_neck 1), @0x01890 `40 00 09 00 fb dc f5 c7` + `18 05`,
                          `29 0d` Cut_chg, @0x0189C `09 0a 14 00` Sleep 20, @0x018A0 `2b 00 00 00` + `3f 00 0f 00` -
                          so jetzt auch Leons erste Zeile (Nachbesserung 1, Mangel 3)
  Plc_neck Kopf gesenkt + Schuetteln  ROOM11B0 sub06 @0x0154E `41 02 00 00 00 00 2c 01 00 0a` Sleep 30
                          @0x0155C `41 04 03 00 00 00 00 00 64 00` Sleep 60 (ebenso ROOM10D0 sub21 @0x01BB4/@0x01BC2
                          fuer Leon und @0x01BEE/@0x01BFC fuer MARVIN, ROOM1170 @0x01766/@0x01774)
  Plc_neck Nicken         ROOM11B0 sub06 @0x016B4 `41 03 01 00 00 00 00 00 00 3c`
  Plc_neck loslassen      ROOM11B0 sub06 @0x01646 `41 00 00 00 00 00 00 00 64 00`
  Se_on                   ROOM10D0 sub21 @0x01A02 `36 02 0c 00 01 00 00 00 00 00 00 00` (Form; Bank/Satz hier
                          = Port-Bank RE15_CUT10F0_SE_BANK, Satz 1 = RE2 DOOR13 Door_exit, s. re15_cut10f0.h)
  Parken nach dem Abgang  ROOM10D0 sub21 @0x01D9E `32 00 1c 57 00 00 34 ef` (Pos_set weit weg)
  Schwanz                 ROOM1050 sub03 @0x00DF2 `42 00 3c 01 22 02 07 00 22 01 1b 00 01 00`

Aufruf: szene_bauen.py            -> schreibt engine/src/gen/cut10f0_szene.inc, druckt das Listing
"""
import os, struct, sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(os.path.dirname(HIER))
OUT = os.path.join(PORT, "engine", "src", "gen", "cut10f0_szene.inc")

# ---- Konstanten (muessen mit include/re15_cut10f0.h uebereinstimmen; der Riegel prueft das) ----
BANK_GESEHEN, BIT_GESEHEN = 9, 71          # VERTRAG Runde 35 §1.1 Spur K
SE_BANK = 0x0E                             # RE15_CUT10F0_SE_BANK (Port-Bank fuer die RE2-Tuerbank)
SE_TUERKNALL = 1                           # Door_exit-Satz des DOOR13-Tonteils
SPAWN_X, SPAWN_Z, SPAWN_DIR = 8400, -350, 2048      # ROOM10D0 main00 @0x1174 (Tuer nach 10F0)
ADA_X, ADA_Z, ADA_DIR = 6000, 11500, 3072             # PORT-WAHL "hinten rechts an den Monitoren bei Cut 2"
LEON_X, LEON_Z = 4800, 11500                          # PORT-WAHL "links von ihr"
WP_X, WP_Z = 5500, 2500                               # PORT-WAHL Wegpunkt im Gang vor der Konsole
MARVIN_X, MARVIN_Z = 3800, 9900                       # PORT-WAHL "schraeg links zu Leon und Ada"
ADA_WP_X, ADA_WP_Z = 5500, 3200                       # PORT-WAHL Adas Wegpunkt beim Abgang
PARK_X, PARK_Z = -30000, -30000                       # ROOM11B0 main00 @0x01080 (geparkt)
TUER_X, TUER_Z = 8800, -300                           # Blickpunkt Tuer = Rechteck Slot 0 @0x00F32
MSG = dict(hey=6, staff=7, destroyed=8, reach=9, m_made=10, l_made=11, intro=12, ada=13, adawong=14,
           m_hello=15, l_anyway=16, m_what=17, l_dots=18, l_car=19, m_yeah=20, l_okay=21, l_irons=22,
           m_alright=23)
BIT_LEON, BIT_ADA, BIT_MARVIN = 0, 1, 2   # Ankunftsbits Bank 5 (ROOM11C0 sub05 / ROOM11B0 sub10 / sub11)
ADA_SLOT, MARVIN_SLOT = 0, 1              # Sce_em_set-Slots (Aktor 1 / 2)

prog = []   # (bytes, kommentar)

def s16(v):
    return struct.pack("<h", v)

def op(b, c):
    prog.append((bytes(b), c))

def set_flag(bank, bit, val, c):       op([0x22, bank, bit, val], c)
def cut_chg(n, c):                     op([0x29, n], c)
def cut_auto(n, c):                    op([0x3C, n], c)
def sleep(n, c=""):                    op([0x09, 0x0A, n & 0xFF, (n >> 8) & 0xFF], c or "Sleep %d" % n)
def work_player(c="Work_set(1,0)+Nop Spieler"):   op([0x2E, 0x01, 0x00, 0x00], c)
def work_npc(i, c=None):               op([0x2E, 0x02, i, 0x00], c or "Work_set(2,%d)+Nop" % i)
def message(mid, c):                   op([0x2B, mid, 0x00, 0x00], c)
def motion(clip, c):                   op([0x3F, 0x00, clip, 0x00], c)
def motion_rev(c="Plc_flg(0,0x80) rueckwaerts"):  op([0x43, 0x00, 0x80, 0x00], c)
def neck_pt(x, z, c):                  op(b"\x41\x01" + s16(x) + s16(0) + s16(z) + b"\x64\x00", c)
def neck_release(c="Plc_neck loslassen"):          op(bytes([0x41, 0, 0, 0, 0, 0, 0, 0, 0x64, 0]), c)
def neck_down(c="Plc_neck Modus 2: Kopf leicht gesenkt (Pitch 300)"):  op(bytes([0x41, 0x02, 0, 0, 0, 0, 0x2C, 0x01, 0x00, 0x0A]), c)
def neck_shake(c="Plc_neck Modus 4: Kopfschuetteln (3 Schwuenge)"):   op(bytes([0x41, 0x04, 0x03, 0, 0, 0, 0, 0, 0x64, 0]), c)
def neck_nod(c="Plc_neck Modus 3: Nicken"):                            op(bytes([0x41, 0x03, 0x01, 0, 0, 0, 0, 0, 0x00, 0x3C]), c)
def dest(mode, bit, x, z, c):          op(b"\x40\x00" + bytes([mode, bit]) + s16(x) + s16(z), c)
def stand(c="Plc_dest Modus 6: stehen bleiben (Clip 1 -> Clip-2-Ruhe)"):   op(bytes([0x40, 0, 0x06, 0x3F, 0, 0, 0, 0]), c)
def wait(bit, c):
    op([0x11, 0x00, 0x08, 0x00], "Do  (Warteschleife %s)" % c)
    op([0x02, 0x00], "Evt_next + Nop")
    op([0x12, 0x04], "Edwhile")
    op([0x21, 0x05, bit, 0x00], "Ck(5,%d)==0 Ankunftsbit" % bit)
def pos_set(x, y, z, c):               op(b"\x32\x00" + s16(x) + s16(y) + s16(z), c)
def dir_set(yaw, c):                   op(b"\x33\x00" + s16(0) + s16(yaw) + s16(0), c)
def em_set(slot, typ, x, z, yaw, c):
    op(bytes([0x44, slot, typ, 0x40, 0, 0, 0, 0xFF]) + s16(x) + s16(0) + s16(z) + b"\x00\x00" + s16(yaw) + b"\x00\x00", c)
def se_on(bank, sat, c):               op(bytes([0x36, bank, sat] + [0] * 9), c)
def evt_end():                         op([0x01, 0x00], "Evt_end")
def plc_ret():                         op([0x42, 0x00], "Plc_ret + Nop")

# Eine Dialogzeile im Takt von ROOM11B0 sub06 @0x014EE..@0x0150A (= ROOM11C0 sub02 @0x018A4): Message_on,
# Geste A, Sleep 40, Geste B (optional - sonst wird A gehalten), Sleep 50, Clip 23, Sleep 20 = 110 Bilder.
def zeile(mid, wer, gesten, sprecher_work, c):
    message(mid, "Message_on %d  %s" % (mid, c))
    sprecher_work()
    g = gesten
    if g:
        motion(g[0][0], "Plc_motion(0,%d) %s" % (g[0][0], g[0][1]))
        sleep(40, "Sleep 40  (Zeilentakt ROOM11B0 @0x014FA)")
        if len(g) > 1 and g[1] == "rev":
            motion(g[0][0], "Plc_motion(0,%d) noch einmal" % g[0][0])
            motion_rev()
        elif len(g) > 1:
            motion(g[1][0], "Plc_motion(0,%d) %s" % (g[1][0], g[1][1]))
        sleep(50, "Sleep 50  (Zeilentakt ROOM11B0 @0x01502)")
        motion(23, "Plc_motion(0,23) Abschluss: Hand an die Huefte")
        sleep(20, "Sleep 20  (Zeilentakt ROOM11B0 @0x0150A)")
    else:
        sleep(110)

LEON = lambda: work_player()
ADA = lambda: work_npc(ADA_SLOT, "Work_set(2,0)+Nop Ada")
MARVIN = lambda: work_npc(MARVIN_SLOT, "Work_set(2,1)+Nop Marvin")

# ================================ DAS PROGRAMM ================================
set_flag(BANK_GESEHEN, BIT_GESEHEN, 1, "Set(9,71)=1 'Szene gesehen' als ERSTES (Einmal-Riegel)")
set_flag(2, 7, 1, "Set(2,7)=1 Pad-Sperre")
set_flag(1, 27, 1, "Set(1,27)=1 Balken")
em_set(ADA_SLOT, 0x42, ADA_X, ADA_Z, ADA_DIR, "Sce_em_set Ada 0x42 Slot 0 an den Monitoren (NE), Blick Nord")
em_set(MARVIN_SLOT, 0x40, PARK_X, PARK_Z, SPAWN_DIR, "Sce_em_set Marvin 0x40 Slot 1 GEPARKT (-30000,-30000)")
ADA(); stand("Ada: Plc_dest Modus 6 (stehen)")
work_player()
stand("Leon: Plc_dest Modus 6 (stehen, Tuer-Eintrittspose beenden)")
cut_chg(2, "Cut_chg 2  'Zuerst Kamera auf CUT2 - Ada'")
# Nachbesserung 1 (Abnahme 0, Mangel 3): Leon sieht Ada an und dreht sich zu ihr, BEVOR er sie anspricht - Form und
# Reihenfolge von ROOM11C0 sub02: @0x01886 Plc_neck Modus 1, @0x01890 Plc_dest Modus 9 + Warteschleife, `29 0d`
# Cut_chg, `09 0a 14 00` Sleep 20, @0x018A0 Message_on + Clip 15. Vorher zeigte der Arm bei Gierung 2048 an Ada
# (Soll 2942) vorbei auf die Westwand (state.log F76-F166, K_abnahme_0.md Mangel 3).
work_player(); neck_pt(ADA_X, ADA_Z, "Leon: Kopf zu Ada (Blick VOR der Zeile, ROOM11C0 sub02 @0x01886)")
dest(9, BIT_LEON, ADA_X, ADA_Z, "Leon: Plc_dest Modus 9 -> zu Ada drehen (ROOM11C0 sub02 @0x01890)")
wait(BIT_LEON, "Leon zu Ada gedreht")
sleep(50)
cut_chg(0, "Cut_chg 0  'Dann Kamera auf CUT0 - Leon'")
sleep(20, "Sleep 20  (ROOM11C0 sub02 @0x0189C `09 0a 14 00` vor der Zeile)")
# Leon: "Hey - how did you came in here?"  Arm strecken = Clip 15, jetzt auf Ada gerichtet
zeile(MSG["hey"], "Leon", [(15, "Arm strecken (Hey!-Griff nach vorn, zu Ada)")], LEON, "Leon: Hey - how did you came in here?")
# Ada dreht sich zu Leon um
ADA(); neck_pt(SPAWN_X, SPAWN_Z, "Ada: Kopf zu Leon an der Tuer")
dest(9, BIT_ADA, SPAWN_X, SPAWN_Z, "Ada: Plc_dest Modus 9 -> zu Leon drehen")
wait(BIT_ADA, "Ada gedreht")
# Leon laeuft zu ihr, Kamera folgt per RVD (Cut 0 -> 1 -> 2)
work_player()
cut_auto(1, "Cut_auto 1  Kamera wechselt ueber die RVD-Baender, solange Leon laeuft")
dest(4, BIT_LEON, WP_X, WP_Z, "Leon: Plc_dest Modus 4 (gehen) zum Wegpunkt im Gang")
wait(BIT_LEON, "Leon am Wegpunkt")
dest(4, BIT_LEON, LEON_X, LEON_Z, "Leon: Plc_dest Modus 4 (gehen) links neben Ada")
wait(BIT_LEON, "Leon bei Ada")
cut_chg(2, "Cut_chg 2  Dialogkamera")
dest(9, BIT_LEON, ADA_X, ADA_Z, "Leon: Plc_dest Modus 9 -> zu Ada drehen")
wait(BIT_LEON, "Leon zu Ada gedreht")
ADA(); neck_pt(LEON_X, LEON_Z, "Ada: Kopf zu Leon")
dest(9, BIT_ADA, LEON_X, LEON_Z, "Ada: Plc_dest Modus 9 -> zu Leon drehen")
wait(BIT_ADA, "Ada zu Leon gedreht")
sleep(10)
# Woman: staff card  (180-Grad-Armgeste = Clip 19 vor + rueckwaerts)
zeile(MSG["staff"], "Ada", [(19, "Arm seitlich hinaus (180-Grad-Geste)"), "rev"], ADA, "Woman: Did you really think there was only one staff card ...")
# Woman: destroyed  (Kopf gesenkt + Kopfschuetteln)
message(MSG["destroyed"], "Message_on %d  Woman: Anyway... the communication system is completely destroyed." % MSG["destroyed"])
ADA(); neck_down("Ada: Plc_neck Modus 2 Kopf leicht gesenkt")
sleep(30)
neck_shake("Ada: Plc_neck Modus 4 Kopfschuetteln")
sleep(80)
message(MSG["reach"], "Message_on %d  Woman: We won't reach anyone with it anymore..." % MSG["reach"])
sleep(30)
neck_pt(LEON_X, LEON_Z, "Ada: Kopf zurueck zu Leon")
sleep(80)
# Marvin kommt durch die Tuer: Tuerknall, Marvin laden (Pos_set aus der Parklage), Cut 0
work_player(); neck_pt(TUER_X, TUER_Z, "Leon: Kopf zur Tuer")
ADA(); neck_pt(TUER_X, TUER_Z, "Ada: Kopf zur Tuer")
se_on(SE_BANK, SE_TUERKNALL, "Se_on Port-Bank 0x0E Satz 1 = RE2 DOOR13 Door_exit (Tuerknall)")
sleep(12)
MARVIN(); pos_set(SPAWN_X, 0, SPAWN_Z, "Marvin: Pos_set an die Tuer (= Spawnpunkt der Tuer 10D0->10F0)")
dir_set(SPAWN_DIR, "Marvin: Dir_set 2048 (Blick in den Raum, wie Leons Eintritt)")
stand("Marvin: Plc_dest Modus 6 (stehen)")
cut_chg(0, "Cut_chg 0  Marvin an der Tuer")
sleep(20)
neck_pt(LEON_X, LEON_Z, "Marvin: Kopf zu Leon")
dest(9, BIT_MARVIN, LEON_X, LEON_Z, "Marvin: Plc_dest Modus 9 -> Richtung Cut 2 / Leon drehen")
wait(BIT_MARVIN, "Marvin gedreht")
zeile(MSG["m_made"], "Marvin", [(15, "Arm strecken")], MARVIN, "Marvin: Leon! You already made it!")
# Marvin laeuft Richtung Cut 2, steht schraeg links
cut_chg(2, "Cut_chg 2  Dialogkamera, Marvin kommt ins Bild")
MARVIN(); dest(4, BIT_MARVIN, WP_X, WP_Z, "Marvin: Plc_dest Modus 4 (gehen) zum Wegpunkt")
work_player(); neck_pt(WP_X, WP_Z, "Leon: Kopf zu Marvin")
ADA(); neck_pt(WP_X, WP_Z, "Ada: Kopf zu Marvin")
wait(BIT_MARVIN, "Marvin am Wegpunkt")
MARVIN(); dest(4, BIT_MARVIN, MARVIN_X, MARVIN_Z, "Marvin: Plc_dest Modus 4 (gehen) schraeg links vor Leon und Ada")
wait(BIT_MARVIN, "Marvin angekommen")
dest(9, BIT_MARVIN, LEON_X, LEON_Z, "Marvin: Plc_dest Modus 9 -> zu Leon drehen")
wait(BIT_MARVIN, "Marvin zu Leon gedreht")
ADA(); neck_pt(MARVIN_X, MARVIN_Z, "Ada: Kopf zu Marvin")
# Leon wendet sich Marvin zu (er stand zu Ada gewandt, Marvin schraeg hinter ihm - Dossier §8.2)
work_player(); neck_pt(MARVIN_X, MARVIN_Z, "Leon: Kopf zu Marvin")
dest(9, BIT_LEON, MARVIN_X, MARVIN_Z, "Leon: Plc_dest Modus 9 -> zu Marvin drehen")
wait(BIT_LEON, "Leon zu Marvin gedreht")
sleep(10)
zeile(MSG["l_made"], "Leon", [(15, "Arm strecken (zu Marvin)")], LEON, "Leon: Hey Marvin, glad you made it!")
# Leon stellt vor: er dreht sich zu Ada, Arm Richtung Ada, der Kopf bleibt bei Marvin
work_player(); dest(9, BIT_LEON, ADA_X, ADA_Z, "Leon: Plc_dest Modus 9 -> zu Ada drehen (Arm Richtung Ada)")
wait(BIT_LEON, "Leon zu Ada gedreht")
zeile(MSG["intro"], "Leon", [(15, "Arm strecken Richtung Ada")], LEON, "Leon: Allow me to introduce you. This is...")
zeile(MSG["ada"], "Ada", [(18, "Hand zur Brust")], ADA, "Ada: ... Ada, Ada Wong")
message(MSG["adawong"], "Message_on %d  Leon: Ada Wong." % MSG["adawong"])
work_player(); neck_nod("Leon: Nicken")
sleep(90)
MARVIN(); neck_pt(ADA_X, ADA_Z, "Marvin: Kopf zu Ada (er stellt sich IHR vor)")
zeile(MSG["m_hello"], "Marvin", [(15, "Arm strecken"), (18, "Hand zur Brust (I'm Marvin)")], MARVIN, "Marvin: Hello, glad to meet another Survivor! I'm Marvin.")
MARVIN(); neck_pt(LEON_X, LEON_Z, "Marvin: Kopf zurueck zu Leon")
# Leon wendet sich fuer den Rest des Gespraechs Marvin zu
work_player(); neck_pt(MARVIN_X, MARVIN_Z, "Leon: Kopf zu Marvin")
dest(9, BIT_LEON, MARVIN_X, MARVIN_Z, "Leon: Plc_dest Modus 9 -> zu Marvin drehen")
wait(BIT_LEON, "Leon zu Marvin gedreht")
# Leon: Anyway... (Kopfschuetteln, Kopf leicht gebeugt)
message(MSG["l_anyway"], "Message_on %d  Leon: Anyway... looks like we can't contact anyone ..." % MSG["l_anyway"])
work_player(); neck_down("Leon: Plc_neck Modus 2 Kopf leicht gesenkt")
sleep(30)
neck_shake("Leon: Plc_neck Modus 4 Kopfschuetteln")
sleep(80)
neck_pt(MARVIN_X, MARVIN_Z, "Leon: Kopf zu Marvin")
sleep(20)
zeile(MSG["m_what"], "Marvin", [(20, "Unterarm nach vorn (Frage)")], MARVIN, "Marvin: Ohh... what do we do then?...")
message(MSG["l_dots"], "Message_on %d  Leon: ..." % MSG["l_dots"])
sleep(70, "Sleep 70  'etwas Pause'")
sleep(60, "Sleep 60  Pause")
zeile(MSG["l_car"], "Leon", [(21, "Hand hoch (I know!)"), (17, "Arm-Schwung (The patrol car!)")], LEON, "Leon: I know! The patrol car! We can use it to get out of here!")
G16 = (16, "kleine Handflaechen-Geste (Original-Paar 15 -> 16, ROOM11B0 sub06 @0x014FE)")
zeile(MSG["m_yeah"], "Marvin", [(15, "Arm strecken"), G16], MARVIN, "Marvin: Yeah, you're right! That could be our way out!")
zeile(MSG["l_okay"], "Leon", [(15, "Arm strecken (zu Marvin: you go with Ada)"), G16], LEON, "Leon: Okay, Marvin, you go with Ada to the parking lot and wait there.")
zeile(MSG["l_irons"], "Leon", [(17, "Arm-Schwung (I'm going to get Chief Irons)")], LEON, "Leon: I'm going to get Chief Irons, and I'll be right behind you!")
zeile(MSG["m_alright"], "Marvin", [(15, "Arm strecken (Take care Leon!)"), G16], MARVIN, "Marvin: Alright! Sounds like a plan. Take care Leon!")
# Marvin und Ada rennen hintereinander zur Tuer
MARVIN(); neck_release("Marvin: Kopf loslassen")
dest(5, BIT_MARVIN, WP_X, WP_Z, "Marvin: Plc_dest Modus 5 (rennen) zum Wegpunkt")
sleep(25)
ADA(); neck_release("Ada: Kopf loslassen")
dest(5, BIT_ADA, ADA_WP_X, ADA_WP_Z, "Ada: Plc_dest Modus 5 (rennen) hinterher")
work_player(); neck_pt(WP_X, WP_Z, "Leon: schaut ihnen nach")
wait(BIT_MARVIN, "Marvin am Wegpunkt")
cut_chg(0, "Cut_chg 0  Abgang durch die Tuer")
MARVIN(); dest(5, BIT_MARVIN, SPAWN_X, SPAWN_Z, "Marvin: Plc_dest Modus 5 (rennen) zur Tuer")
wait(BIT_ADA, "Ada am Wegpunkt")
ADA(); dest(5, BIT_ADA, SPAWN_X, SPAWN_Z, "Ada: Plc_dest Modus 5 (rennen) zur Tuer")
wait(BIT_MARVIN, "Marvin an der Tuer")
MARVIN(); pos_set(PARK_X, 0, PARK_Z, "Marvin: Pos_set geparkt (durch die Tuer verschwunden)")
wait(BIT_ADA, "Ada an der Tuer")
ADA(); pos_set(PARK_X, 0, PARK_Z, "Ada: Pos_set geparkt")
se_on(SE_BANK, SE_TUERKNALL, "Se_on Tuer faellt hinter ihnen zu (RE2 DOOR13 Door_exit)")
sleep(10)
cut_chg(2, "Cut_chg 2  zurueck zu Leon (gemessen: nach Cut 0 laege Leon ausserhalb jedes 0->x RVD-Bands, Kamera bliebe stehen)")
sleep(45, "Sleep 45  'kurz weiter Cutscene Balken'")
work_player(); neck_release("Leon: Kopf loslassen")
plc_ret()
cut_auto(1, "Cut_auto 1  Kamera wieder frei (Schwanz ROOM1050 sub03 @0x00DF4)")
set_flag(2, 7, 0, "Set(2,7)=0")
set_flag(1, 27, 0, "Set(1,27)=0")
evt_end()

# ================================ AUSGABE ================================
blob = b"".join(b for b, _ in prog)
lines = ["/* AUTO-GENERATED by re15_port/tools/r35_k/szene_bauen.py -- DO NOT EDIT.",
         " * ROOM10F0-Szene (Runde 35 Spur K): %d Bytes, %d Opcodes. Vorbilder je Opcode-Form im Kopf des" % (len(blob), len(prog)),
         " * Generators; Konstanten (Positionen, Nachrichten-IDs, Bits) = include/re15_cut10f0.h. */",
         "#define RE15_CUT10F0_PROG_LEN %d" % len(blob),
         "static const uint8_t k_cut10f0_prog[RE15_CUT10F0_PROG_LEN] = {"]
off = 0
listing = []
for b, c in prog:
    hexs = ", ".join("0x%02x" % x for x in b)
    lines.append("    /* +%04X */ %s,%s /* %s */" % (off, hexs, " " * max(1, 46 - len(hexs)), c))
    listing.append("+%04X  %-36s %s" % (off, " ".join("%02x" % x for x in b), c))
    off += len(b)
lines.append("};")
# Marken, die der Riegel prueft (Offsets der Message_on-Opcodes)
msg_offs = []
off = 0
for b, c in prog:
    if b[0] == 0x2B:
        msg_offs.append((b[1], off))
    off += len(b)
lines.append("/* Offsets der Message_on-Opcodes (Riegel): id -> Offset */")
lines.append("static const struct { uint8_t id; uint16_t off; } k_cut10f0_msg_marken[] = {")
for mid, o in msg_offs:
    lines.append("    { %d, 0x%04X }," % (mid, o))
lines.append("};")
# Tuerknall-Offsets (Riegel: der erste Se_on)
se_offs = [o for (o, (b, c)) in zip([sum(len(x) for x, _ in prog[:i]) for i in range(len(prog))], prog) if b[0] == 0x36]
lines.append("#define RE15_CUT10F0_OFF_TUERKNALL 0x%04X" % se_offs[0])
lines.append("#define RE15_CUT10F0_OFF_TUER_ZU   0x%04X" % se_offs[1])
with open(OUT, "w", encoding="utf-8", newline="\n") as f:
    f.write("\n".join(lines) + "\n")
print("\n".join(listing))
print("-> %s: %d Bytes, %d Opcodes, %d Message_on" % (OUT, len(blob), len(prog), len(msg_offs)))
