#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Runde 41 Spur C: die Port-Programme der Ventilkette als SCD-Bytecode (gen/ventilkette_r41_programme.inc).

Jede Zeile = (Bytes als Hex oder Marker, Kommentar mit Vorbild). Marker "IF" oeffnet einen Ifel_ck-Block
(`06 00 LL 00`), "ENDIF" schliesst ihn mit `08 00`; LL = Bytes zwischen dem Ifel_ck-Ende und dem Ende des Endif
(Blockende = Ifel_ck + 4 + LL = hinter dem Endif; rolltor_1050.c, ROOM2060 sub12 @0x015C0 `06 00 42 00`).
Wird von gen_r41_ventilkette.py aufgerufen (programme(CD, GEN)).
"""
import os


def le16(v):
    v &= 0xFFFF
    return "%02x %02x" % (v & 0xFF, v >> 8)


def assemble(name, zeilen):
    out = bytearray()
    kommentare = []
    stapel = []
    for hexe, kom in zeilen:
        if hexe == "IF":
            stapel.append(len(out))
            out += bytes([0x06, 0x00, 0x00, 0x00])
            kommentare.append((len(out) - 4, 4, "Ifel_ck (Laenge automatisch) " + kom))
            continue
        if hexe == "ENDIF":
            start = stapel.pop()
            out += bytes([0x08, 0x00])
            ll = len(out) - (start + 4)
            out[start + 2] = ll & 0xFF
            out[start + 3] = ll >> 8
            kommentare.append((len(out) - 2, 2, "Endif " + kom))
            continue
        b = bytes(int(x, 16) for x in hexe.split())
        kommentare.append((len(out), len(b), kom))
        out += b
    assert not stapel, name
    z = ["/* %s (%d B) */" % (name, len(out)), "static const unsigned char %s[%d] = {" % (name, len(out))]
    for off, n, kom in kommentare:
        hexs = ",".join("0x%02x" % x for x in out[off:off + n])
        z.append("    /* +%03X */ %s, /* %s */" % (off, hexs, kom))
    z.append("};")
    return out, "\n".join(z)


def msg_on(mid, kom):
    return ("2b %02x ff ff" % mid, "Message_on %d, Maske ffff - %s" % (mid, kom))


EVT_NEXT = ("02 00", "Evt_next + Nop (ROOM2050 sub04 @0x022EA)")
JA = ("21 0c 1f 00", "Ck(12,31)=0 = Ja (ROOM2050 sub04 @0x022F0)")
SPERRE_AN = ("22 02 07 01", "Set(2,7)=1 Pad-Sperre (ROOM2030 sub02 @0x01E30)")
SPERRE_AUS = ("22 02 07 00", "Set(2,7)=0 (ROOM2030 sub02 @0x01EC4)")
CUT_AUTO_0 = ("3c 00", "Cut_auto 0 (ROOM2030 sub02 @0x01E34)")
CUT_AUTO_1 = ("3c 01", "Cut_auto 1 (ROOM2030 sub02 @0x01EC2)")
CUT_OLD = ("2a 00", "Cut_old + Nop (ROOM11D0 sub02 @0x01884)")
ENDE = ("01 00", "Evt_end")


def bauen(cd):
    P = []
    # ---- 2030 Pult -------------------------------------------------------------------------------------------
    P.append(("re15_r41c_prg_2030_pult_ohne",
              [msg_on(40, "A control panel... gauge reads zero... valve 13 is closed."), EVT_NEXT, ENDE]))
    P.append(("re15_r41c_prg_2030_pult_unten", [msg_on(44, "The bridge is down."), EVT_NEXT, ENDE]))
    senken = [msg_on(43, "The pressure gauge reads 80. Lower the bridge? (Frage, Ja/Nein)"), EVT_NEXT,
              ("IF", "Ja-Zweig"), JA, SPERRE_AN, CUT_AUTO_0,
              ("29 04", "Cut_chg 4 Draufsicht (Fahrt-Kamera; Form ROOM2030 sub02 @0x01E60 `29 0a`)"),
              ("2e 03 00 00", "Work_set(3,0) Bruecke + Nop (Form sub03 @0x01ED2 `2e 03 01`)"),
              ("2f 01 " + le16(120), "Speed_set(1,+120) (sub03 @0x01EF2 `2f 01 78 00`)"),
              ("0d 00 06 00 " + le16(30), "For 30 (Form sub03 @0x01EDA `0d 00 06 00 2d 00`; 30 x 120 = 3600)"),
              ("30 02 02 00", "Add_speed, Evt_next, Evt_next, Nop (sub03 @0x01EE0..@0x01EE3)"),
              ("0e 00", "Next (sub03 @0x01EE4)"),
              ("34 01 " + le16(-1800), "Member_set(1,-1800) Endlage = Original-Lage @0x01B6C (Form sub03 @0x01EE6)"),
              ("37 00 0c 00", "Sca_id_set Region 0 Zelle 12 := 00 begehbar (Form ROOM2050 sub04 @0x02398 `37 00 32 00`)"),
              ("37 01 0c 00", "Region 1"), ("37 02 0c 00", "Region 2"), ("37 03 0c 00", "Region 3"),
              ("37 04 0c 00", "Region 4"),
              ("22 09 66 01", "Set(9,102)=1 Bruecke unten (Port-Bit, VERTRAG)"),
              ("09 0a 14 00", "Sleep 20 (ROOM2080 sub02 @0x00CD8 nach der Fahrt)"),
              CUT_OLD, CUT_AUTO_1, SPERRE_AUS,
              ("ENDIF", ""), ENDE]
    P.append(("re15_r41c_prg_2030_pult_senken", senken))
    # ---- 2030 Ventil 13 --------------------------------------------------------------------------------------
    P.append(("re15_r41c_prg_2030_v13_frage",
              [msg_on(41, "Intake valve 13. Will you open it?"), EVT_NEXT,
               ("IF", "Ja-Zweig"), JA, ("22 09 65 01", "Set(9,101)=1 Ventil 13 offen (Port-Bit)"),
               msg_on(42, "You've opened valve 13. The pressure is rising. (Form ROOM2030 sub06 @0x02000 msg 3 nach dem Set)"),
               EVT_NEXT, ("ENDIF", ""), ENDE]))
    P.append(("re15_r41c_prg_2030_v13_zustand", [msg_on(47, "Intake valve 13 is open."), EVT_NEXT, ENDE]))
    # ---- 2030 Ventilraetsel (Nachbesserung 4, Nachtrag 13): sechs Ventile (Raeder 01/02/03 Nordwand, 07/08/09 Suedwand) -------
    # Jede Drehung: Frage -> Ja -> Marker (5,0x1B); der C-Takt vermerkt sie in (5,0x10..0x13) (Raum-Scratch wie die Schalter-
    # bits ROOM11F0 (5,13..22) / ROOM2060 (5,6..9)) und prueft nach der VIERTEN (RE2 ROOM2130 sub04: Ergebnis erst am Ende,
    # @0x01752): richtig -> Ereignis 26 "2030_ventile_flut", falsch -> Ereignis 27 "2030_ventile_falsch".
    # Nachbesserung 5 (Nachtrag 18 Punkt 3): nach dem Ja dreht der Spieler das Rad mit der Ventil-Animation aus RE2 -
    # Byte-Folge von RE2 ROOM1080 sub08 @0x0210A..@0x0221E (Hubschrauber-Landeplatz, msg 0 @0x267A "A water pressure
    # valve"), Clips 15/16/17 aus dem RBJ derselben RDT @0x39AC (shared_assets/RE2/VENTIL1080.RBJ, vom PC-Lader als
    # Raum-Overlay von 2030/2031 gebunden). Lage: Pos_set auf die Radmitte x und 816 vor die Wandflaeche (RE2: pos_set
    # @0x02116 z -21250 bis zur SCA-Flaeche in Blickrichtung, Element 12 z -22066 = 816), Blick zur Wand (RE2 member_set
    # @0x0212C `34 0f 00 08`; RE1.5 Member 4 = rot_y +0x6a, actor_common.c). Se_on(2,5) @0x02168.. -> Marker (5,0x1A),
    # der C-Takt spielt RE2 ROOM1080 Satz 5 (Bank VENTIL1080). RE2s Plc_cnt @0x021EA/@0x021F4 (0x32/0x1E = genau die
    # Clip-Laengen 50/30) kennt RE1.5 nicht - der rueckwaerts gespielte Clip laeuft ohnehin ganz. RE2s Blick zum Tank
    # (plc_neck @0x021FE/@0x0220C, Sleep 180/60) und cut_be_set gehoeren zur Szene des Landeplatzes und entfallen.
    vx = (-1350, 150, 2350, 2600, 400, -2650)          # re15_ventilkette_r41.h RE15_R41C_VENTIL_X
    wz = (-8500, -8500, -8500, 7900, 7900, 7900)       # RE15_R41C_VENTIL_WAND_Z
    TON = ("22 05 1a 01", "Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt "
                          "RE2 ROOM1080 Satz 5, Bank VENTIL1080)")
    for k, nr in enumerate(("01", "02", "03", "07", "08", "09")):
        nord = wz[k] < 0
        pz = wz[k] + (816 if nord else -816)
        yaw = 1024 if nord else 3072                   # Vorwaerts (cos, -sin): 1024 = -z (Nordwand), 3072 = +z (Suedwand)
        dreh = []
        for c in range(5):                              # RE2 @0x02178 For 2 + @0x021AE For 2 + @0x021CE einzeln = 5 Takte
            dreh += [("3f 00 11 00", "Plc_motion(0,17,0) Drehtakt %d/5 (RE2 @0x02186/@0x021B4/@0x021CE)" % (c + 1)),
                     ("09 0a 12 00", "Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2)"), TON,
                     ("09 0a " + le16(15 if c == 4 else 12), "Sleep %d (RE2 %s)" % (15 if c == 4 else 12,
                      "@0x021E2" if c == 4 else "@0x021A4/@0x021C8"))]
        dreh += [("09 0a 0c 00", "Sleep 12 (RE2 @0x021E6)")]
        P.append(("re15_r41c_prg_2030_ventil_%s" % nr,
                  [msg_on(50 + k, "Valve %s ... Will you turn it? (Frage, Ja/Nein)" % nr), EVT_NEXT,
                   ("IF", "Ja-Zweig"), JA,
                   SPERRE_AN, ("22 02 02 01", "Set(2,2)=1 KI steht (ROOM2050 sub02 @0x0223C)"),
                   ("2e 01 00 00", "Work_set(1,0) Spieler + Nop (RE2 sub08 @0x0210A `2e 01 00`)"),
                   ("32 00 %s %s %s" % (le16(vx[k]), le16(-1800), le16(pz)),
                    "Pos_set (%d,-1800,%d) vor Rad %s (Form RE2 sub08 @0x02116 `32 00 30 b2 00 00 fe ac`)" % (vx[k], pz, nr)),
                   ("34 04 " + le16(yaw), "Member_set(4 = rot_y, %d) Blick zur Wand (Form RE2 @0x0212C `34 0f 00 08`)" % yaw),
                   ("09 0a 14 00", "Sleep 20 (RE2 @0x02130)"),
                   ("3f 00 0f 00", "Plc_motion(0,15,0) Hand zum Rad (RE2 @0x0213C; RBJ Clip 15 = 30 Bilder)"),
                   ("09 0a 1e 00", "Sleep 30 (RE2 @0x0214A)"),
                   ("3f 00 10 00", "Plc_motion(0,16,0) Rad fassen (RE2 @0x02156; Clip 16 = 50 Bilder)"),
                   ("09 0a 1c 00", "Sleep 28 (RE2 @0x02164)"), TON,
                   ("09 0a 16 00", "Sleep 22 (RE2 @0x02174)")] + dreh +
                  [("3f 00 10 c0", "Plc_motion(0,16,0xC0) loslassen rueckwaerts, ohne Ueberblendung (RE2 @0x021EC)"),
                   ("09 0a 32 00", "Sleep 50 (RE2 @0x021F0)"),
                   ("3f 00 0f c0", "Plc_motion(0,15,0xC0) Hand zurueck (RE2 @0x021F6)"),
                   ("09 0a 1e 00", "Sleep 30 (RE2 @0x021FA)"),
                   ("42 00", "Plc_ret + Nop (RE2 @0x0221E `42` `00`)"),
                   ("22 02 02 00", "Set(2,2)=0 (ROOM11F0 sub17 @0x016EC)"), SPERRE_AUS,
                   ("22 05 1b 01", "Set(5,0x1B)=1 Ventil gedreht (Marker fuer den C-Takt; Bank 5 Wort 0 = Raum-Scratch, "
                                   "Raumladen wischt @0x8003ed74; Form der Schalter ROOM2060 sub06 @0x014D8 `22 05 06 01`)"),
                   ("ENDIF", ""), ENDE]))
    # richtige Folge: Warnung -> Zaehler -> Flut im selben Faden (Form wie bisher; vom C-Takt als Ereignis 26 gefeuert)
    flut = [msg_on(46, "WARNING: The water will rise in one minute. Please leave the room immediately! (Nachtrag 19)"), EVT_NEXT,
            ("22 05 1d 01", "Set(5,0x1D)=1 Zaehler laeuft (Bank 5 Wort 0 = Raum-Scratch, Raumladen wischt @0x8003ed74; C-Takt liest)"),
            ("09 0a " + le16(1800), "Sleep 1800 = 60 s bei 30 Takten (Hintergrundfaden wie ROOM2000 sub07 @0x01B36)"),
            SPERRE_AN, ("22 02 02 01", "Set(2,2)=1 KI steht (ROOM2050 sub02 @0x0223C)"), CUT_AUTO_0,
            ("29 04", "Cut_chg 4 Draufsicht"),
            ("2e 03 04 00", "Work_set(3,4) Wasser A + Nop"),
            ("32 00 " + le16(-10800) + " " + le16(0) + " " + le16(-12400),
             "Pos_set (-10800,0,-12400) Beckengrund (Form ROOM2000 sub02 @0x0180C)"),
            ("2f 01 " + le16(-50), "Speed_set(1,-50) aufwaerts (Rate ROOM2080 sub02 @0x00CC8 `2f 01 32 00`)"),
            ("2e 03 05 00", "Work_set(3,5) Wasser B + Nop"),
            ("32 00 " + le16(-10800) + " " + le16(0) + " " + le16(3800), "Pos_set (-10800,0,3800)"),
            ("2f 01 " + le16(-50), "Speed_set(1,-50)"),
            ("22 05 1e 01", "Set(5,0x1E)=1 Flut laeuft (Bank 5 Wort 0; C-Takt: Ertrinken)"),
            ("0d 00 16 00 " + le16(324), "For 324, Block 0x16 = Koerper 20 + Next 2 (324 x 50 = 16200 bis zur Tuerebene; Form ROOM2080 @0x00CCC)"),
            ("2e 03 04 00", "Work_set(3,4) + Nop"), ("2f 01 " + le16(-50), "Speed_set(1,-50) (Form: Speed_set direkt vor Add_speed, sub03 @0x01ED6)"),
            ("30", "Add_speed A"),
            ("2e 03 05 00", "Work_set(3,5) + Nop"), ("2f 01 " + le16(-50), "Speed_set(1,-50)"),
            ("30", "Add_speed B"),
            EVT_NEXT, ("0e 00", "Next"),
            ("09 0a 3c 00", "Sleep 60 (Form ROOM20B0 sub05 @0x01FB2)"),
            ENDE]
    P.append(("re15_r41c_prg_2030_ventile_flut", flut))
    # falsche Folge: zuruecksetzen (Form ROOM11F0 sub17 @0x016C0..@0x016E4 `22 05 nn 00`), Text 45 (Fehlerton 0x0B spielt der
    # C-Takt, RE2 ROOM2130 sub04 @0x01800 `36 02 0b 01`).
    P.append(("re15_r41c_prg_2030_ventile_falsch",
              [("22 05 10 00", "Set(5,0x10)=0 Drehung 1 zurueck (Form ROOM11F0 sub17 @0x016C0 `22 05 0d 00`)"),
               ("22 05 11 00", "Set(5,0x11)=0"), ("22 05 12 00", "Set(5,0x12)=0"), ("22 05 13 00", "Set(5,0x13)=0"),
               msg_on(45, "The valves turned back to their original position."), EVT_NEXT, ENDE]))
    # waehrend des Zaehlers zeigen die Ventile die Warnung 46 erneut (Zustand = die Warnung selbst); vor der Bruecke
    # (Bit 102, Nachtrag 9) bzw. ohne die Sicherung im Ventil-Sicherungskasten 2010 (Bit (9,125), Nachtrag 16 Folge 4)
    # Satz 49 = Original "It's locked." (ROOM20A0 msg 1 @0x203C, Scheibe re15_r41c_org_20a0_msg1).
    P.append(("re15_r41c_prg_2030_ventile_laeuft",
              [msg_on(46, "WARNING: ... Please leave the room immediately! (Zustand waehrend des Zaehlers)"), EVT_NEXT, ENDE]))
    P.append(("re15_r41c_prg_2030_ventile_vorher",
              [msg_on(49, "It is locked. = Original ROOM20A0 msg 1 @0x203C"), EVT_NEXT, ENDE]))
    # Plakat-Nahaufnahme (Nachtrag 13 Folge 3; Form der Nahaufnahmen Runde 40: Pad-Sperre ROOM1051 sub03 @0x00DA4, KI steht
    # ROOM2050 sub02 @0x0223C). Bild, Warten auf die Taste und Freigabe (2,7)/(2,2) := 0 (Form ROOM11F0 sub17 @0x016E8/
    # @0x016EC, ROOM1051 sub03 @0x00DD2) macht der C-Takt.
    P.append(("re15_r41c_prg_2030_plakat",
              [SPERRE_AN, ("22 02 02 01", "Set(2,2)=1 KI steht (ROOM2050 sub02 @0x0223C)"),
               ("22 05 1c 01", "Set(5,0x1C)=1 Plakat-Nahaufnahme (Marker fuer den C-Takt, Bank 5 Wort 0)"), ENDE]))
    # ---- 2010 Ventil-Sicherungskasten (Nachbesserung 5, Nachtrag 16) ----------------------------------------
    # Form des Original-Kastens ROOM2060: sub00 @0x0109E..@0x010DE (ohne aufgehobene Sicherung Text 3, mit Sicherung
    # Ereignis 18 = sub18 Frage msg 4, eingesetzt Text 6), sub19 @0x0169E Set(3,144)=1 -> Cut_chg 9 (Einstellung auf die
    # Wirkung) -> Message_on 5 "You've used the Fuse." -> Evt_next -> Cut_chg 10 -> Cut_auto 1. Die "Einstellung auf
    # die Wirkung" ist hier das Standbild der Ventil-Kamera 2030 Cut 7 (BSS/ROOM2030/BG07.BSS) ueber die Vollbild-Ebene
    # des PC-Renderers + Aufschliess-Ton (RE2 Satz 0x26, EXE @0x800515f8, KARTSE.VBS AUF_HIER), gesetzt vom C-Takt ab dem
    # Marker (5,0x1F); geschlossen wird es wie die Nahaufnahmen mit der Bestaetigen-/Abbrechen-Flanke (andi 0xc000
    # @0x80028698), danach der Satz (die Vollbild-Ebene liegt ueber dem Text). Warten = While-Form ROOM2000 @0x01AD4
    # `0f 04 08 00` `21 05 00 00` `02 00` `10 00`.
    P.append(("re15_r41c_prg_2010_kasten_leer",
              [msg_on(40, "A fuse box for the valve controls. | One of the fuses is missing... (ROOM2060 msg 3)"), EVT_NEXT, ENDE]))
    P.append(("re15_r41c_prg_2010_kasten_frage",
              [msg_on(40, "A fuse box ... | One of the fuses is missing..."), EVT_NEXT,
               msg_on(41, "Will you use the Fuse? = ROOM2060 msg 4 @0x1855 (Form sub18 @0x0167E)"), EVT_NEXT,
               ("IF", "Ja-Zweig"), JA,
               SPERRE_AN, ("22 02 02 01", "Set(2,2)=1 KI steht (ROOM2050 sub02 @0x0223C)"),
               ("22 09 7d 01", "Set(9,125)=1 Sicherung im Ventilkasten (Form ROOM2060 sub19 @0x0169E `22 03 90 01`)"),
               ("22 05 1f 01", "Set(5,0x1F)=1 Standbild der Ventile + Aufschliess-Ton (Marker fuer den C-Takt; Bank 5 Wort 0, "
                               "ROOM2010 benutzt Bank 5 nicht)"),
               ("0f 04 08 00", "While (Form ROOM2000 @0x01AD4): solange das Standbild offen ist"),
               ("21 05 1f 01", "Ck(5,0x1F)=1"),
               EVT_NEXT, ("10 00", "Ewhile"),
               msg_on(42, "You've used the Fuse. = ROOM2060 msg 5 @0x1875 (sub19 @0x016C8)"), EVT_NEXT,
               ("22 02 02 00", "Set(2,2)=0 (ROOM11F0 sub17 @0x016EC)"), SPERRE_AUS,
               ("ENDIF", ""), ENDE]))
    P.append(("re15_r41c_prg_2010_kasten_drin",
              [msg_on(43, "The fuse is in place. (ROOM2060 msg 6) | Will you take the Fuse? (ROOM2030 msg 2)"), EVT_NEXT,
               ("IF", "Ja-Zweig"), JA,
               ("22 09 7d 00", "Set(9,125)=0 Sicherung wieder genommen (Form ROOM2030 sub06 @0x01FF2 `22 03 6c 01`)"),
               msg_on(44, "You've taken the Fuse. = ROOM2030 msg 3 @0x20AC (sub06 @0x02000)"), EVT_NEXT,
               ("ENDIF", ""), ENDE]))
    # ---- 2050 ------------------------------------------------------------------------------------------------
    P.append(("re15_r41c_prg_2050_hebel_sperre",
              [msg_on(40, "The pump has no power. The switchboard must be set first. (Form ROOM2060 sub12 @0x015D0)"),
               EVT_NEXT, ENDE]))
    P.append(("re15_r41c_prg_2050_pult_ein",
              [msg_on(43, "A high voltage switchboard. The pump needs 80 volts."), EVT_NEXT,
               msg_on(41, "Will you push them? = ROOM11F0 msg 1 @0x18D8 (Form ROOM11F0 sub16 @0x015A8)"), EVT_NEXT,
               ("IF", "Ja-Zweig"), JA,
               ("29 0f", "Cut_chg 15 = ROOM2050 sub09 @0x023F2"),
               ("46 08 00 00 00 00 00 00 00 00", "Aot_reset Slot 8 sce 0 = sub09 @0x023F4"),
               ("22 05 04 01", "Set(5,4)=1 Cursor-Tasten = sub09 @0x023FE"),
               ("22 05 06 01", "Set(5,6)=1 Spieler/KI stehen = sub09 @0x02406 (OHNE (5,5): SQUARE beendet sonst, sub01 @0x021E2)"),
               ("ENDIF", ""), ENDE]))
    P.append(("re15_r41c_prg_2050_pult_zustand",
              [msg_on(44, "The switchboard is set to 80 volts. The pump is powered."), EVT_NEXT, ENDE]))
    # ---- 2080 Rechner: seit Nachtrag 7 nur Original (sub02 @0x00C86), keine Port-Programme mehr (Nachbesserung 3) -----
    return P


def programme(cd, gen):
    zeilen = ["/* AUTOMATISCH ERZEUGT von re15_port/tools/r41_ventilkette/programme.py - nicht von Hand aendern.",
              " * Port-Programme der Ventilkette (Runde 41 Spur C) als SCD-Bytecode; jede Zeile mit ihrem Vorbild.",
              " * Blocklaengen der Ifel_ck automatisch (Blockende = Ifel_ck + 4 + Laenge = hinter dem Endif). */", ""]
    tabelle = []
    for name, defs in bauen(cd):
        b, txt = assemble(name, defs)
        zeilen.append(txt)
        zeilen.append("")
        tabelle.append((name, len(b)))
    zeilen.append("typedef struct { const char *name; const unsigned char *b; int n; } re15_r41c_programm_t;")
    zeilen.append("static const re15_r41c_programm_t re15_r41c_programme[] = {")
    for name, n in tabelle:
        zeilen.append("    { \"%s\", %s, %d }," % (name.replace("re15_r41c_prg_", ""), name, n))
    zeilen.append("};")
    zeilen.append("#define RE15_R41C_PROGRAMME_N ((int)(sizeof re15_r41c_programme / sizeof re15_r41c_programme[0]))")
    ziel = os.path.join(gen, "ventilkette_r41_programme.inc")
    open(ziel, "w", newline="\n").write("\n".join(zeilen) + "\n")
    print("geschrieben:", ziel, "%d Programme" % len(tabelle))
