#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Runde 38 Spur F "dining1141": die SCD-Programme und Texte der Spur als engine/src/gen/dining_r38_programme.inc.

  python re15_port/tools/r38_dining/programme_bauen.py [--pruefen]   (druckt das Listing)

Jedes Programm besteht aus AUSGELIEFERTEN Original-Bytes (Datei-Offset in re15_port/shared_assets/PSX/STAGE1/ROOMxxxx.RDT,
vor dem Kopieren gegen die erwarteten Bytes geprueft) und eingefuegten Zeilen in ausgelieferter Opcode-Form (Vorbild je
Zeile). Laengen = re15_port/tools/scd_dump_room.py SIZES (= engine/src/scd_vm.c s_opcode_sizes). Dossier:
analysis/befunde_runde38/F_dining1141.md; Konstanten: include/re15_dining_r38.h (der Riegel test_r38_dining1141 prueft
beide gegeneinander).

PROGRAMME
  ROLL1140   ROOM1140 Ereignis 3 (Leon, Strom an): sub03 @0x00CA6 + Rolladen-Ton in der Form von ROOM1131 sub02.
  ROLL1141   ROOM1141 Ereignis 6 (Elza, Gemaelde-Schalter): sub06 @0x00FE8 + derselbe Ton.
             Ton-Form ROOM1131 sub02: @0x00A48 Se_on(2,0x0C) / @0x00A54 Sleep 15 / @0x00A58 Se_on(2,0x0A) /
             @0x00A64 Sleep 15 unmittelbar VOR dem Work_set auf das Prop (@0x00A68), @0x00AB2 Se_on(2,0x0B) /
             @0x00ABE Sleep 10 unmittelbar NACH der letzten Next (@0x00AB0); Ifel-Laenge +0x30 (1131 0xd0, 1140 0xa0).
             Bank = Port-Bank 0x3B (die snd0-Bank von ROOM1131, die einzige mit den Rolladen-Wellen).
  LEICHE1021 ROOM1021 Ereignis 6 (Roys Leiche): sub06 @0x021EC, Message_on 9 @0x02202 -> Port-Nachricht 40
             (= msg 9 + Seite + NUTZER-VORGABE "There's an empty key tag on his belt. / It reads "Dining Room".").
  HAEMMERN   ROOM1141 Phase H (John sitzt im Dachgang hinter dem geschlossenen Notrolladen fest): ersetzt das
             Evt_exec sub02 @0x00B02 beim Betreten. John steht schon im Dachgang HINTER dem Rolladen (Nachtrag 2: er ist
             da, wenn Elza den Raum betritt — kein Auftauchen): der Installer setzt den Spawn sub00 @0x00B06 vor dem
             ersten Bild auf JOHN_GANG.
             Musik = Sce_bgm_control in der Form sub00 @0x00B4E (MAIN-Slot Play); Klopfen = RE2 ROOM1060 sub05
             (Satz 0x0A, Sleep 30, 0x0B, Sleep 30, 0x0B, Sleep 90 ... Sleep 90) mit der Port-Bank 0x3C
             (KLOPF1060 = RE2-ROOM1060-snd0); "Help" = Port-Bank 0x3D Satz 0 (john_help.wav) in der zweiten
             90er-Pause. Schleife = While Ck(3,0x6A)==0 (Form sub03 @0x00FA8 `0f 04 ..`, Ck sub00 @0x00B84), jede
             Ton-Zeile in Ifel_ck Ck(3,0x6A)==0 (Form sub01 @0x00BFE `06 00 ..`): das Original-Set(3,0x6A)=1 von
             sub06 @0x00FF6 (Antwort "ja" am Gemaelde) beendet Klopfen und Hilferuf ohne C-Eingriff.
  BEFREIUNG  ROOM1141 Ereignis 6 in Phase H (Gemaelde-Schalter): sub06 @0x00FE8.. mit dem Rolladen-Ton wie ROLL1141,
             ohne die Gorilla-Einfrierbloecke @0x0100E/@0x010A4 (in Phase H sind keine Gorillas da); nach den
             Sca-Saetzen @0x0107C..@0x010A3: Musik aus (Form sub01 @0x00C06), Flags sub02 @0x00C18/@0x00C1C/@0x00C20,
             Gorillas geparkt (sub00 @0x00B1A..@0x00B41), John laeuft aus dem Gang (sichtbar seit dem Hochfahren)
             zu seinem Platz (Form sub02 @0x00F8E/@0x00F96), dann sub02 @0x00CD4..@0x00FA1 = Elza an ihren Platz +
             Drehungen + Szene ab msg 2 (-> Port-Nachricht 40), Plaetze/Blick-/Drehziele auf die neuen Plaetze
             (Original-Regel: Blickpunkt = eigener Platz + (5900,0)), Cut_chg 0 @0x00D08/@0x00E80 -> Cut 5 (Cut 0
             blickt nach Suedwesten auf die Doppeltuer, der neue Platz liegt am Gemaelde im Bild von Cut 5);
             Tuersperre msg 41 (Slot 0 + Riegel-Slot 2) bis die Gorillas tot sind, dann Johns Abgang durch den Gang
             und die Dachtuer AUSSERHALB des Bildes (Cut 7 blickt nach Sueden, John laeuft auf die Kamera zu und
             verlaesst das Bild am unteren Rand; Tuer-Toene DOOR1A im Off), Riegel-Zustand wie nach sub05.
"""
import io, os, struct, sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(os.path.dirname(HIER))
STAGE1 = os.path.join(PORT, "shared_assets", "PSX", "STAGE1")
OUT = os.path.join(PORT, "engine", "src", "gen", "dining_r38_programme.inc")
sys.path.insert(0, os.path.join(PORT, "tools"))
import scd_walk_lib as L  # noqa: E402

RDT = {r: open(os.path.join(STAGE1, "ROOM%s.RDT" % r), "rb").read() for r in ("1021", "1131", "1140", "1141")}

# ---- Konstanten (muessen mit include/re15_dining_r38.h uebereinstimmen; der Riegel vergleicht) ----
BANK_ROLLADEN = 0x3B        # RE15_R38F_SE_BANK_ROLLADEN
BANK_KLOPFEN = 0x3C         # RE15_R38F_SE_BANK_KLOPFEN
BANK_HILFE = 0x3D           # RE15_R38F_SE_BANK_HILFE
BANK_TUER = 0x0E            # RE15_CUT10F0_SE_BANK (RE2-Tuerbank; DOOR1A geladen vom C-Teil)
MSG_1021_LEICHE = 40        # RE15_R38F_MSG_1021_LEICHE
MSG_1141_FRAGE = 40         # RE15_R38F_MSG_1141_FRAGE
MSG_1141_SPERRE = 41        # RE15_R38F_MSG_1141_SPERRE
BIT_JOHN_WEG = 67           # RE15_R38F_BIT_JOHN_WEG (Bank 9, VERTRAG 1.1)
# ---- Nachtrag 2 (Nutzer 2026-10-16 21:33: "keine plötzlich rein poppenden Charaktere") — PORT-WAHL-Lagen ----
JOHN_GANG = (-7124, -6150)      # RE15_R38F_JOHN_GANG_X/Z   hinter dem Rolladen: x = Prop obj 0 x (sub00 @0x00B9C+10
                                #   `2c e4` = -7124), z im Dachgang (SCA x -8600..-5750, z -6700..-250) 550 hinter der
                                #   Rolladen-Zelle r[13] (z -7200..-6700)
JOHN_GANG_YAW = 1024            # RE15_R38F_JOHN_GANG_YAW   Blick -Z = zum Rolladen (pos_advance @0x800245d8:
                                #   x += cos(yaw)*v, z -= sin(yaw)*v; 1024 = -Z)
ELZA_PLATZ = (-8000, -10250)    # RE15_R38F_ELZA_PLATZ_X/Z  vor dem Gemaelde: Ostkante + z-Mitte der Gemaelde-Zone
                                #   Aot slot 3 sub00 @0x00B88 (x -9000..-8000, z -11000..-9500)
JOHN_PLATZ = (-5800, -10250)    # RE15_R38F_JOHN_PLATZ_X/Z  2200 oestlich = Original-Abstand der Dialogplaetze
                                #   ((-7200,-15500) @0x00CD8 / (-6750,-13350) @0x00B06: 2196)
BLICK_DX = 5900                 # Original-Regel Blickpunkt = Platz + (5900,0): (-1300,-15500) @0x00DF6 zu
                                #   (-7200,-15500), (-850,-13350) @0x00E06 zu (-6750,-13350)
GANG_AUSGANG = (-7350, -7600)   # RE15_R38F_GANG_X/Z        Ausgang des Dachgangs (Rolladen-Zelle z -7200..-6700)
TUER_INNEN = (-7350, -700)      # RE15_R38F_TUER_INNEN_X/Z  vor der Dachtuer (Rechteck z -750..250)
PARK = (-30000, -30000)         # ROOM11B0 main00 @0x01080 `d0 8a` (geparkt)
ORIG_ELZA = (-7200, -15500)     # Original-Dialogplatz Elza (sub02 @0x00CD8)
ORIG_JOHN = (-6750, -13350)     # Original-Dialogplatz John (sub00 @0x00B06)

prog = []


def s16(v):
    return struct.pack("<h", v)


def op(b, c):
    prog.append((bytes(b), c))


def kopie(raum, von, bis, erwartet_kopf=None, c=""):
    d = RDT[raum]
    b = d[von:bis]
    if erwartet_kopf is not None and b[:len(erwartet_kopf)] != bytes(erwartet_kopf):
        sys.exit("ABBRUCH: ROOM%s @0x%05X erwartet %s, ist %s" % (raum, von, bytes(erwartet_kopf).hex(" "),
                                                                b[:len(erwartet_kopf)].hex(" ")))
    # opcode-genau zerlegen (Kontrolle: der Bereich endet auf einer Opcode-Grenze)
    pc = von
    while pc < bis:
        sz = L.op_size(d, pc)
        if sz is None:
            sys.exit("ABBRUCH: ROOM%s @0x%05X unbekannter Opcode 0x%02X" % (raum, pc, d[pc]))
        prog.append((d[pc:pc + sz], "ROOM%s @0x%05X %s%s" % (raum, pc, L.NAMES.get(d[pc], "?"),
                                                          (" " + c) if c and pc == von else "")))
        pc += sz
    if pc != bis:
        sys.exit("ABBRUCH: ROOM%s Bereich 0x%05X..0x%05X endet nicht auf einer Opcode-Grenze" % (raum, von, bis))


def ersetze(alt, neu, c):
    """Ersetzt GENAU eine bereits kopierte Zeile (Bytes alt) durch neu."""
    treffer = [i for i, (b, _k) in enumerate(prog) if b == bytes(alt)]
    if len(treffer) != 1:
        sys.exit("ABBRUCH: Ersetzung %s: %d Treffer" % (bytes(alt).hex(" "), len(treffer)))
    i = treffer[0]
    prog[i] = (bytes(neu), prog[i][1] + "  -> " + c)


def set_flag(bank, bit, val, c):  op([0x22, bank, bit, val], c)
def cut_chg(n, c):                op([0x29, n], c)
def sleep(n, c):                  op([0x09, 0x0A, n & 0xFF, (n >> 8) & 0xFF], c)
def work_player(c="Work_set(1,0)+Nop Spieler (Form ROOM1141 sub02 @0x00C2E)"):  op([0x2E, 0x01, 0x00, 0x00], c)
def work_npc(i, c=None):          op([0x2E, 0x02, i, 0x00], c or "Work_set(2,%d)+Nop (Form ROOM1141 sub02 @0x00C5C)" % i)
def se_on(bank, satz, c):         op([0x36, bank, satz] + [0] * 9, c)
def dest(mode, bit, xz, c):       op(b"\x40\x00" + bytes([mode, bit]) + s16(xz[0]) + s16(xz[1]), c)
def pos_set(xz, c):               op(b"\x32\x00" + s16(xz[0]) + s16(0) + s16(xz[1]), c)
def warte(bank, bit, wert, c):
    op([0x11, 0x00, 0x08, 0x00], "Do  (Warteschleife %s; Form ROOM1021 sub04 @0x021D0 / ROOM1050 sub03 @0x00DCA)" % c)
    op([0x02, 0x00], "Evt_next + Nop")
    op([0x12, 0x04], "Edwhile")
    op([0x21, bank, bit, wert], "Ck(%d,0x%02X)==%d" % (bank, bit, wert))


def zeile_1131(off, laenge, erwartet, bank=None):
    """Eine Zeile aus ROOM1131 sub02 BYTEGLEICH, optional mit Port-Bank statt Bank 2 (pc[1])."""
    b = bytearray(RDT["1131"][off:off + laenge])
    if bytes(b) != bytes(erwartet):
        sys.exit("ABBRUCH: ROOM1131 @0x%05X %s" % (off, bytes(b).hex(" ")))
    if bank is not None:
        b[1] = bank
    return bytes(b)


def rolladen_ton_a():
    op(zeile_1131(0x00A48, 12, [0x36, 0x02, 0x0C, 0x00, 0x01] + [0] * 7, BANK_ROLLADEN),
       "Se_on(0x3B,0x0C) = ROOM1131 sub02 @0x00A48 bytegleich, Bank 2 -> Port-Bank 0x3B")
    op(zeile_1131(0x00A54, 4, [0x09, 0x0A, 0x0F, 0x00]), "Sleep 15 = ROOM1131 sub02 @0x00A54")
    op(zeile_1131(0x00A58, 12, [0x36, 0x02, 0x0A, 0x00, 0x03] + [0] * 7, BANK_ROLLADEN),
       "Se_on(0x3B,0x0A) = ROOM1131 sub02 @0x00A58 bytegleich, Port-Bank")
    op(zeile_1131(0x00A64, 4, [0x09, 0x0A, 0x0F, 0x00]), "Sleep 15 = ROOM1131 sub02 @0x00A64")


def rolladen_ton_b():
    op(zeile_1131(0x00AB2, 12, [0x36, 0x02, 0x0B, 0x00, 0x03] + [0] * 7, BANK_ROLLADEN),
       "Se_on(0x3B,0x0B) = ROOM1131 sub02 @0x00AB2 bytegleich, Port-Bank")
    op(zeile_1131(0x00ABE, 4, [0x09, 0x0A, 0x0A, 0x00]), "Sleep 10 = ROOM1131 sub02 @0x00ABE")


def baue_rolladen(raum, von, ifel, ifel_alt, ifel_neu, vor_work, nach_next, bis):
    """Original-sub [von,bis) mit eingefuegtem Ton vor `vor_work` (Work_set Prop 0) und nach `nach_next`."""
    del prog[:]
    d = RDT[raum]
    if d[ifel:ifel + 4] != bytes([0x06, 0x00, ifel_alt & 0xFF, ifel_alt >> 8]):
        sys.exit("ABBRUCH: ROOM%s Ifel @0x%05X" % (raum, ifel))
    kopie(raum, von, vor_work)
    ersetze([0x06, 0x00, ifel_alt & 0xFF, ifel_alt >> 8], [0x06, 0x00, ifel_neu & 0xFF, ifel_neu >> 8],
            "Ifel-Laenge 0x%02X -> 0x%02X (+0x30 = 3 Se_on + 3 Sleep, wie ROOM1131 @0x00A28 0xa0->0xd0)" % (ifel_alt, ifel_neu))
    rolladen_ton_a()
    kopie(raum, vor_work, nach_next, [0x2E, 0x03, 0x00], "(Work_set Prop 0 = Rolladen)")
    rolladen_ton_b()
    kopie(raum, nach_next, bis, [0x37, 0x00, 0x0D, 0x00], "(Sca_id_set = Original hinter der letzten Next)")
    return list(prog)


def baue_leiche():
    del prog[:]
    kopie("1021", 0x021EC, 0x02222, [0x46, 0x06], "(sub06 Roys Leiche)")
    ersetze([0x2B, 0x09, 0xFF, 0xFF], [0x2B, MSG_1021_LEICHE, 0xFF, 0xFF], "Port-Nachricht 40 (msg 9 + Hinweis)")
    return list(prog)


def ifel_6a(inner, c):
    """Ifel_ck { Ck(3,0x6A)==0  <inner> } Endif — Form ROOM1141 sub01 @0x00BFE `06 00 10 00` (Laenge = Ck + Rumpf +
    Endif, gemessen 0x10 = 4+6+4+2 bis hinter das Endif @0x00C10), Ck = sub00 @0x00B84 `21 03 6a 00`."""
    rumpf = 4 + len(inner) + 2
    op(b"\x06\x00" + struct.pack("<H", rumpf), "Ifel_ck Laenge %d (Form sub01 @0x00BFE)" % rumpf)
    op(b"\x21\x03\x6a\x00", "  Ck(3,0x6A)==0 = sub00 @0x00B84 (Rolladen noch zu)")
    op(inner, "  " + c)
    op(b"\x08\x00", "Endif = sub01 @0x00C10")


def baue_haemmern():
    del prog[:]
    # John steht schon im Dachgang: der Installer (dining_r38.c raum_1141) setzt den Spawn sub00 @0x00B06 nach dem
    # Init-Lauf und VOR dem ersten Bild auf JOHN_GANG/JOHN_GANG_YAW (die Lage traegt auch dieses Werkzeug, der Riegel
    # vergleicht sie mit include/re15_dining_r38.h).
    op([0x54, 0x00, 0x01, 0x00, 0x00, 0x00], "Sce_bgm_control MAIN Play = ROOM1141 sub00 @0x00B4E (Raum-Titel MAIN16)")
    kopf = len(prog)
    op(b"\x0f\x04\x00\x00", "While (Laenge s.u.) = Form ROOM1141 sub03 @0x00FA8 `0f 04 08 00`")
    op(b"\x21\x03\x6a\x00", "  Ck(3,0x6A)==0 (Schleife laeuft, solange der Rolladen zu ist)")
    ifel_6a(bytes([0x36, BANK_KLOPFEN, 0x0A] + [0] * 9), "Se_on(0x3C,0x0A) Klopfen = RE2 ROOM1060 sub05 +0x22 (Datei 0x0F48)")
    sleep(30, "Sleep 30  = RE2 ROOM1060 sub05 +0x2E")
    ifel_6a(bytes([0x36, BANK_KLOPFEN, 0x0B] + [0] * 9), "Se_on(0x3C,0x0B) Klopfen = RE2 ROOM1060 sub05 +0x32")
    sleep(30, "Sleep 30  = RE2 ROOM1060 sub05 +0x3E")
    ifel_6a(bytes([0x36, BANK_KLOPFEN, 0x0B] + [0] * 9), "Se_on(0x3C,0x0B) Klopfen = RE2 ROOM1060 sub05 +0x4C")
    sleep(90, "Sleep 90  = RE2 ROOM1060 sub05 +0x58")
    ifel_6a(bytes([0x36, BANK_HILFE, 0x00] + [0] * 9), "Se_on(0x3D,0) \"Help me out.\" (NUTZER-VORGABE: Help als Ton; john_help.wav)")
    sleep(90, "Sleep 90  = RE2 ROOM1060 sub05 +0x5E")
    op(b"\x10\x00", "Ewhile = ROOM1141 sub03 @0x00FB2 (zurueck zum While-Test; RE2 ROOM1060 sub05 +0x62 Goto)")
    laenge = sum(len(b) for b, _ in prog[kopf + 1:])
    prog[kopf] = (b"\x0f\x04" + struct.pack("<H", laenge),
                  prog[kopf][1].replace("Laenge s.u.", "Laenge %d bis hinter das Ewhile" % laenge))
    op([0x01, 0x00], "Evt_end (Rolladen offen: Klopfen und Hilferuf sind zu Ende)")
    return list(prog)


def zeile_bei(raum, off):
    """Index der von kopie() an Datei-Offset `off` eingefuegten Zeile."""
    k = "ROOM%s @0x%05X " % (raum, off)
    t = [i for i, (_b, c) in enumerate(prog) if c.startswith(k)]
    if len(t) != 1:
        sys.exit("ABBRUCH: Zeile ROOM%s @0x%05X %d mal" % (raum, off, len(t)))
    return t[0]


def ersetze_bei(raum, off, alt, neu, c):
    i = zeile_bei(raum, off)
    if prog[i][0] != bytes(alt):
        sys.exit("ABBRUCH: ROOM%s @0x%05X ist %s statt %s" % (raum, off, prog[i][0].hex(" "), bytes(alt).hex(" ")))
    prog[i] = (bytes(neu), prog[i][1] + "  -> " + c)


def baue_befreiung():
    del prog[:]
    d = RDT["1141"]
    # ---- sub06: Frage am Gemaelde, "ja" -> Rolladen hoch (mit Ton), OHNE die Gorilla-Bloecke ----
    kopie("1141", 0x00FE8, 0x0100E, [0x2B, 0x0D, 0x80, 0xFF], "(sub06: Gemaelde-Frage; Ja-Zweig)")
    i_ifel = zeile_bei("1141", 0x00FEE)
    if prog[i_ifel][0] != bytes([0x06, 0x00, 0xE8, 0x00]):
        sys.exit("ABBRUCH: Ifel sub06 @0x00FEE")
    rolladen_ton_a()
    kopie("1141", 0x01032, 0x0107C, [0x2E, 0x03, 0x00], "(Work_set Prop 0 = Rolladen; Bewegung bytegleich)")
    rolladen_ton_b()
    kopie("1141", 0x0107C, 0x010A4, [0x37, 0x00, 0x0D, 0x00], "(Sca-Saetze: Rolladen-Zellen frei)")
    # ---- John ist frei: Musik aus, Szenen-Flags, Gorillas geparkt, John laeuft heraus ----
    op([0x54, 0x00, 0x02, 0x00, 0x00, 0x00], "Sce_bgm_control MAIN Stop = ROOM1141 sub01 @0x00C06 (John ist frei)")
    kopie("1141", 0x00C18, 0x00C24, [0x22, 0x04, 0xF0, 0x00], "(sub02: Set(4,0xF0)=0 / (4,0xF2)=1 / (3,0xC2)=1)")
    kopie("1141", 0x00B1A, 0x00B42, [0x44, 0x01, 0x27, 0x30], "(Gorillas geparkt wie sub00 beim Original-Szenenstart)")
    work_npc(0)
    dest(5, 1, JOHN_PLATZ, "Plc_dest RUN John aus dem Gang an seinen Platz (Form ROOM1141 sub02 @0x00F8E `40 00 05 01`)")
    op([0x18, 0x04], "Gosub sub04 = ROOM1141 sub02 @0x00F96 (warten, bis John steht)")
    # ---- Original sub02 ab @0x00CD4: Elza an ihren Platz, Drehungen, Szene ab msg 2 ----
    vor = len(prog)
    kopie("1141", 0x00CD4, 0x00F4E, [0x2E, 0x01, 0x00], "(sub02: Elza an ihren Platz, Drehungen, Szene ab msg 2)")
    ersetze_bei("1141", 0x00D08, [0x29, 0x00], [0x29, 0x05],
                "Cut_chg 5: Cut 0 blickt nach Suedwesten (Ziel (-17202,-26058)), der Platz am Gemaelde liegt im Bild von Cut 5")
    ersetze_bei("1141", 0x00D0A, [0x2B, 0x02, 0x00, 0x00], [0x2B, MSG_1141_FRAGE, 0x00, 0x00],
                "Port-Nachricht 40 = msg 2 ohne \"Same here...\" (Nachtrag 14 Punkt 5)")
    ersetze_bei("1141", 0x00E80, [0x29, 0x00], [0x29, 0x05], "Cut_chg 5 (wie @0x00D08)")
    # Tuersperre VOR der Rueckgabe der Steuerung (@0x00F4E Set(2,7)=0) — sonst koennte Elza in den Bildern bis
    # zu Johns Ankunft in der Ecke noch durch die Tuer.
    op([0x46, 0x00, 0x01, 0x31, MSG_1141_SPERRE, 0x00, 0xFF, 0xFF, 0x00, 0x00],
       "Aot_reset Slot 0 (Tuer -> 1131) -> Text msg 41 (Form ROOM11B0 sub01 @0x01216 `46 01 01 31 0f 00 ff ff 00 00`)")
    op([0x46, 0x02, 0x01, 0x31, MSG_1141_SPERRE, 0x00, 0xFF, 0xFF, 0x00, 0x00],
       "Aot_reset Slot 2 (Riegel im Rechteck der Dachtuer main00 @0x00A9A; Slot 1 ist bei zu Riegel sce 0 @0x00A7A) -> msg 41")
    kopie("1141", 0x00F4E, 0x00FA2, [0x22, 0x02, 0x07, 0x00], "(sub02-Schluss: Steuerung zurueck, Gorillas, John in die Ecke)")
    xp = {ORIG_ELZA: ELZA_PLATZ, ORIG_JOHN: JOHN_PLATZ,
          (-1300, -15500): (ELZA_PLATZ[0] + BLICK_DX, ELZA_PLATZ[1]),
          (-850, -13350): (JOHN_PLATZ[0] + BLICK_DX, JOHN_PLATZ[1])}
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
    # ---- Gorillas tot -> Tueren frei, John geht durch den Gang aufs Dach (ausserhalb des Bildes) ----
    warte(4, 0xF2, 1, "Gorillas leben (sub01 @0x00C0C loescht (4,0xF2))")
    sleep(30, "Sleep 30 (Form ROOM1021 sub06 @0x021FE)")
    set_flag(2, 0x07, 1, "Set(2,7)=1 Pad-Sperre (Form ROOM1141 sub06 @0x01004)")
    set_flag(1, 0x1B, 1, "Set(1,0x1B)=1 Balken (Form ROOM1141 sub06 @0x01008)")
    cut_chg(7, "Cut_chg 7 = Kamera (-7465,-3024,-9613) Blick nach Sueden (sub02 @0x00C24): John laeuft auf die Kamera "
               "zu und verlaesst das Bild am unteren Rand")
    work_npc(0)
    dest(5, 1, GANG_AUSGANG, "Plc_dest RUN John -> Gang-Ausgang (Form sub02 @0x00F8E)")
    op([0x18, 0x04], "Gosub sub04 (Form sub02 @0x00F96)")
    dest(5, 1, TUER_INNEN, "Plc_dest RUN John -> vor die Dachtuer")
    op([0x18, 0x04], "Gosub sub04")
    se_on(BANK_TUER, 0, "Se_on(0x0E,0) RE2-Tuerbank DOOR1A Satz 0 = oeffnen (Dachtuer 1141 = DOOR1A, tuer_zuordnung.inc "
                        "S064; Form ROOM10D0 sub21 @0x01A02)")
    sleep(15, "Sleep 15 (Form ROOM1131 sub02 @0x00A54)")
    pos_set(PARK, "Pos_set John geparkt (Form ROOM10D0 sub21 @0x01D9E, Lage ROOM11B0 @0x01080) — ausserhalb jedes Bildes")
    se_on(BANK_TUER, 1, "Se_on(0x0E,1) DOOR1A zufallen")
    set_flag(9, BIT_JOHN_WEG, 1, "Set(9,67)=1 John hat 1141 verlassen (VERTRAG 1.1)")
    kopie("1141", 0x00FC8, 0x00FD6, [0x46, 0x02, 0x00, 0x00],
          "(sub05 ohne Nachricht: Riegel-Slot 2 aus + Set(4,0xC3)=1 — John hat den Riegel beim Hinausgehen geoeffnet)")
    kopie("1141", 0x00FDC, 0x00FE6, [0x46, 0x01, 0x02, 0x31], "(sub05: Dachtuer Slot 1 scharf)")
    op([0x46, 0x00, 0x02, 0x31] + list(d[0x00A52 + 14:0x00A52 + 20]),
       "Aot_reset Slot 0 -> Tuer ROOM1131 (Nutzlast = main00 @0x00A52+14)")
    sleep(15, "Sleep 15")
    op([0x3C, 0x01], "Cut_auto 1 (Form ROOM1141 sub06 @0x010D0)")
    set_flag(2, 0x07, 0, "Set(2,7)=0 (Form sub06 @0x010C8)")
    set_flag(1, 0x1B, 0, "Set(1,0x1B)=0 (Form sub06 @0x010CC)")
    op([0x08, 0x00], "Endif = Ende des Ja-Zweigs (Form sub06 @0x010D8)")
    op([0x01, 0x00], "Evt_end = sub06 @0x010DA")
    # Ifel-Laenge des Ja-Zweigs (sub06 @0x00FEE): von hinter dem Ifel-Kopf bis hinter das Endif.
    laenge = sum(len(b) for b, _ in prog[i_ifel + 1:-1])
    if laenge > 0xFFFF:
        sys.exit("ABBRUCH: Ja-Zweig %d Byte" % laenge)
    prog[i_ifel] = (b"\x06\x00" + struct.pack("<H", laenge),
                    prog[i_ifel][1] + "  -> Laenge 0xE8 -> 0x%X (Ja-Zweig mit Ton, Szene, Sperre, Abgang)" % laenge)
    return list(prog)


# ---------------------------------------------------------------- Texte
ENC = {" ": 0x00, ":": 0x16, ",": 0x18, "\"": 0x19, "!": 0x1A, "?": 0x1B, "'": 0x3A, "-": 0x3B, ".": 0x57}
for i in range(10): ENC[chr(ord("0") + i)] = 0x0C + i
for i in range(26):
    ENC[chr(ord("A") + i)] = 0x1D + i
    ENC[chr(ord("a") + i)] = 0x3D + i
AUF = 0x61   # oeffnendes Anfuehrungszeichen (ROOM1011 msg 18 @0x1231 `61 .. 19`)


def enc(t):
    out = bytearray()
    for c in t:
        if c == "“":
            out.append(AUF)
        else:
            out.append(ENC[c])
    return bytes(out)


def breite(t):
    src = open(os.path.join(PORT, "include", "font_width.h"), encoding="utf-8", errors="replace").read()
    import re
    w = [int(x, 0) for x in re.findall(r"\b(0x[0-9a-fA-F]+|\d+)\b", src.split("{", 1)[1].split("}", 1)[0])]
    return sum(w[b] for b in enc(t))


TEXTE = []   # (C-Name, Bytes, Beschreibung)


def text(name, b, beschr, zeilen):
    for z in zeilen:
        if breite(z) > 271:
            sys.exit("ABBRUCH: Zeile zu breit (%d px > 271): %r" % (breite(z), z))
    TEXTE.append((name, bytes(b), beschr + "  [Zeilenbreiten %s px]" % ", ".join(str(breite(z)) for z in zeilen)))


def baue_texte():
    d1021 = RDT["1021"]
    msg9 = d1021[0x024FB:0x024FB + 27]
    if msg9 != bytes.fromhex("04 02 24 41 3a 4f 00 40 41 3d 40 57 02 00 2c 4b 4b 4e 00 2e 4b 55 57 57 57 01 00"):
        sys.exit("ABBRUCH: ROOM1021 msg 9 @0x024FB")
    z1, z2 = "There's an empty key tag on his belt.", "It reads “Dining Room\"."
    text("k_r38f_msg1021_leiche", msg9[:-2] + b"\x02\x00" + enc(z1) + b"\x08" + enc(z2) + b"\x01\x00",
         "ROOM1021 msg 40: msg 9 @0x024FB (\"He's dead.[Seite]Poor Roy...\") + Seite `02 00` + NUTZER-VORGABE", [z1, z2])
    z1, z2 = "The door is marked", "“Dining Room\"."
    text("k_r38f_msg1131_zu", b"\x04\x02" + enc("It's locked.") + b"\x02\x00" + enc(z1) + b"\x08" + enc(z2) + b"\x01\x00",
         "ROOM1131 msg 40: \"It's locked.[Seite]The door is marked / \"Dining Room\".\" (RE2 Text 0x13 @0x8009F2BB; "
         "Form wie Runde 36 Spur Q msg 28, Punkt hinter dem Anfuehrungszeichen wie im Auftragstext)", ["It's locked.", z1, z2])
    text("k_r38f_msg1131_benutzt",
         b"\x04\x02" + enc("You've used the ") + b"\x05\x01" + enc("Dining Room Key") + b"\x05\x00" + enc(".") + b"\x01\x00",
         "ROOM1131 msg 41: \"You've used the Dining Room Key.\" (Form ROOM11E0 msg 12 @0x238C = RE2 Text 5 @0x8009F0E5)",
         ["You've used the Dining Room Key."])
    d1141 = RDT["1141"]
    msg2 = d1141[0x0115F:0x0115F + 59]
    if msg2[:12] != bytes.fromhex("04 00 05 01 21 48 56 3d 16 05 00 00") or msg2[-4:] != bytes.fromhex("04 01 01 63"):
        sys.exit("ABBRUCH: ROOM1141 msg 2 @0x0115F")
    lauf = enc("Same here... ")
    if msg2[12:12 + len(lauf)] != lauf:
        sys.exit("ABBRUCH: \"Same here... \" nicht am Anfang von msg 2")
    rest = bytearray(msg2[12 + len(lauf):])
    if rest[0] != ENC["a"]:
        sys.exit("ABBRUCH: msg 2 Rest beginnt nicht mit 'a'")
    rest[0] = ENC["A"]
    text("k_r38f_msg1141_frage", msg2[:12] + bytes(rest),
         "ROOM1141 msg 40: msg 2 @0x0115F ohne den Lauf \"Same here... \" (13 Glyphen), erster Buchstabe gross",
         ["Elza: Are you okay?", "what's happened?"])
    z1, z2 = "I cannot leave him alone", "with those monsters."
    text("k_r38f_msg1141_sperre", b"\x04\x02" + enc(z1) + b"\x08" + enc(z2) + b"\x01\x00",
         "ROOM1141 msg 41: NUTZER-VORGABE \"I cannot leave him alone with those monsters.\" (Text-Platz-Form "
         "ROOM1131 msg 1 @0x00B42 `04 02 .. 01 00`)", [z1, z2])
    # Item 0x43: Name (Namensblob-Form `.. 07`, DEBUG.BIN @0x800c4a28) + CHECK-Text (Beschreibungsbank `.. 01 00`)
    text("k_r38f_name", enc("Dining Room Key") + b"\x07", "Name 0x43 (Nachtrag 14: \"Dining Room Key\")", ["Dining Room Key"])
    z1, z2 = "It's in the shape of", "a heart."
    text("k_r38f_desc", enc(z1) + b"\x08" + enc(z2) + b"\x01\x00",
         "CHECK 0x43 = RE2-Beschreibung des Modells 0x5B @0x800A1CDD \"It's in the shape of<FC>a heart.\"", [z1, z2])


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


QUELLEN = (   # (Name, Raum, von, bis) — der C-Teil prueft diese Bereiche im GELADENEN RDT (FNV-1a)
    ("ROLL1140_SUB03", "1140", 0x00CA6, 0x00D52),
    ("ROLL1141_SUB06", "1141", 0x00FE8, 0x010DC),
    ("LEICHE1021_SUB06", "1021", 0x021EC, 0x02222),
    ("SZENE1141_SUB02_05", "1141", 0x00C18, 0x00FE8),   # sub02 + sub03/sub04 (Gosub-Ziele) + sub05
    ("SZENE1141_SUB00", "1141", 0x00AFA, 0x00B42),      # Evt_exec sub02 + Spawns John/Gorillas
    ("SZENE1141_MAIN00", "1141", 0x00A52, 0x00AEA),     # Tueren Slot 0/1, Riegel Slot 2, Tisch
)


def main():
    pruefen = "--pruefen" in sys.argv
    programme = [
        ("k_r38f_p_roll1140", baue_rolladen("1140", 0x00CA6, 0x00CAC, 0xA0, 0xD0, 0x00CCC, 0x00D16, 0x00D52)),
        ("k_r38f_p_roll1141", baue_rolladen("1141", 0x00FE8, 0x00FEE, 0xE8, 0x118, 0x01032, 0x0107C, 0x010DC)),
        ("k_r38f_p_leiche1021", baue_leiche()),
        ("k_r38f_p_haemmern", baue_haemmern()),
        ("k_r38f_p_befreiung", baue_befreiung()),
    ]
    baue_texte()
    f = io.StringIO()
    f.write("/* GENERIERT von re15_port/tools/r38_dining/programme_bauen.py - NICHT HAND-EDITIEREN.\n"
            " * Runde 38 Spur F: SCD-Programme (Original-Bytes + eingefuegte Zeilen, Listing beim Werkzeugaufruf) und\n"
            " * Texte. Dossier: analysis/befunde_runde38/F_dining1141.md. */\n\n")
    for name, raum, von, bis in QUELLEN:
        f.write("#define RE15_R38F_ANKER_%s_OFF 0x%05Xu  /* ROOM%s */\n" % (name, von, raum))
        f.write("#define RE15_R38F_ANKER_%s_LEN %du\n" % (name, bis - von))
        f.write("#define RE15_R38F_ANKER_%s_FNV 0x%08Xu\n" % (name, fnv(RDT[raum][von:bis])))
    f.write("\n")
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
