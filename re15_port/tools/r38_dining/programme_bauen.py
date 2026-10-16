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
  HAEMMERN   ROOM1141 Phase H (John sitzt auf dem Dach fest): ersetzt das Evt_exec sub02 @0x00B02 beim Betreten.
             Musik = Sce_bgm_control in der Form sub00 @0x00B4E (MAIN-Slot Play); Klopfen = RE2 ROOM1060 sub05
             (Satz 0x0A, Sleep 30, 0x0B, Sleep 30, 0x0B, Sleep 90 ... Sleep 90, Goto) mit der Port-Bank 0x3C
             (KLOPF1060 = RE2-ROOM1060-snd0); "Help" = Port-Bank 0x3D Satz 0 (john_help.wav) in der zweiten
             90er-Pause. Die Schleife beendet der C-Teil beim Riegel (dining_r38.c).
  RIEGEL     ROOM1141 Ereignis 5 in Phase H: MAIN-Stopp (Form sub01 @0x00C06) + sub05 @0x00FC8..@0x00FE5 ("You've
             opened the lock.", Dachtuer scharf) + Einbruch Johns (PORT-WAHL-Choreografie, Formen je Zeile) +
             sub02 @0x00D0A..@0x00FA1 mit msg 2 -> Port-Nachricht 40 und vertauschten Plaetzen (John steht, wo Elza
             stand, und umgekehrt) + Tuersperre/Abgang Johns (PORT-WAHL, Formen je Zeile).
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
ELZA_SEITE = (-6650, -2600)     # RE15_R38F_ELZA_SEITE_X/Z   Elza weicht zurueck (Dachgang, Ostseite, 700 neben Johns Bahn)
JOHN_TUER = (-7350, -400)       # RE15_R38F_JOHN_TUER_X/Z   Johns Spawn in der Dachtuer
JOHN_TUER_YAW = 1024            # RE15_R38F_JOHN_TUER_YAW   Blick -Z (in den Raum)
GANG_AUSGANG = (-7350, -7600)   # RE15_R38F_GANG_X/Z        Ausgang des Dachgangs (Rolladen-Zelle z -7200..-6700)
JOHN_PLATZ = (-7200, -15500)    # = Elzas Dialogplatz im Original (sub02 @0x00CD8)
ELZA_PLATZ = (-6750, -13350)    # = Johns Dialogplatz im Original (sub00 @0x00B06)
TUER_INNEN = (-7350, -700)      # RE15_R38F_TUER_INNEN_X/Z  vor der Dachtuer (Rechteck z -750..250)
PARK = (-30000, -30000)         # ROOM11B0 main00 @0x01080 `d0 8a` (geparkt)

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


def baue_haemmern():
    del prog[:]
    op([0x54, 0x00, 0x01, 0x00, 0x00, 0x00], "Sce_bgm_control MAIN Play = ROOM1141 sub00 @0x00B4E (Raum-Titel MAIN16)")
    start = sum(len(b) for b, _ in prog)
    se_on(BANK_KLOPFEN, 0x0A, "Se_on(0x3C,0x0A) Klopfen  = RE2 ROOM1060 sub05 +0x22 (Datei 0x0F48) Satz 0x0A")
    sleep(30, "Sleep 30  = RE2 ROOM1060 sub05 +0x2E")
    se_on(BANK_KLOPFEN, 0x0B, "Se_on(0x3C,0x0B) Klopfen  = RE2 ROOM1060 sub05 +0x32")
    sleep(30, "Sleep 30  = RE2 ROOM1060 sub05 +0x3E")
    se_on(BANK_KLOPFEN, 0x0B, "Se_on(0x3C,0x0B) Klopfen  = RE2 ROOM1060 sub05 +0x4C")
    sleep(90, "Sleep 90  = RE2 ROOM1060 sub05 +0x58")
    se_on(BANK_HILFE, 0x00, "Se_on(0x3D,0) \"Help me out.\" (NUTZER-VORGABE: Help als Ton; john_help.wav)")
    sleep(90, "Sleep 90  = RE2 ROOM1060 sub05 +0x5E")
    hier = sum(len(b) for b, _ in prog)
    op(b"\x17\xff\xff\x00" + s16(start - hier), "Goto zur Klopf-Zeile = RE2 ROOM1060 sub05 +0x62 `17 ff ff 00 e0 ff`")
    return list(prog)


def baue_riegel():
    del prog[:]
    op([0x54, 0x00, 0x02, 0x00, 0x00, 0x00], "Sce_bgm_control MAIN Stop = ROOM1141 sub01 @0x00C06")
    kopie("1141", 0x00FC8, 0x00FE6, [0x46, 0x02], "(sub05 ohne Evt_end: Riegel auf, msg 12, Dachtuer scharf)")
    # ---- Einbruch Johns (PORT-WAHL; Formen je Zeile) ----
    set_flag(4, 0xF0, 0, "Set(4,0xF0)=0 = ROOM1141 sub02 @0x00C18")
    set_flag(4, 0xF2, 1, "Set(4,0xF2)=1 = ROOM1141 sub02 @0x00C1C (Gorillas leben)")
    set_flag(3, 0xC2, 1, "Set(3,0xC2)=1 = ROOM1141 sub02 @0x00C20 (John in ROOM1171, sub07 @0x016C4)")
    set_flag(2, 0x07, 1, "Set(2,7)=1 Pad-Sperre = ROOM1141 sub02 @0x00C26")
    set_flag(1, 0x1B, 1, "Set(1,0x1B)=1 Balken = ROOM1141 sub02 @0x00C2A")
    kopie("1141", 0x00B1A, 0x00B42, [0x44, 0x01, 0x27, 0x30], "(Gorillas geparkt wie sub00 beim Original-Szenenstart)")
    cut_chg(6, "Cut_chg 6 = Dachgang (Kamera (-8292,-838,-6606) -> Dachtuer)")
    work_player()
    dest(8, 0, ELZA_SEITE, "Plc_dest RUECKWAERTS (Modus 8) Elza weicht von der Tuer zurueck = Form ROOM1211 sub02 @0x0212A "
                           "`40 00 08 00 ..` (Elza in der Zellen-Szene)")
    warte(5, 0, 0, "Elza zurueckgewichen")
    d = RDT["1141"]
    john = bytearray(d[0x00B06:0x00B1A])
    if john[:4] != bytes([0x44, 0x00, 0x49, 0x40]):
        sys.exit("ABBRUCH: John-Spawn @0x00B06")
    john[8:10] = s16(JOHN_TUER[0]); john[12:14] = s16(JOHN_TUER[1]); john[16:18] = s16(JOHN_TUER_YAW)
    op(john, "Sce_em_set John = ROOM1141 sub00 @0x00B06, Lage in der Dachtuer + Blick -Z (PORT-WAHL)")
    se_on(BANK_TUER, 0, "Se_on(0x0E,0) RE2-Tuerbank DOOR1A Satz 0 = oeffnen (gen/szl11b0_tuerton.inc; Form ROOM10D0 sub21 @0x01A02)")
    work_npc(0)
    dest(5, 1, GANG_AUSGANG, "Plc_dest RUN Bit 1 zum Gang-Ausgang (Form ROOM1141 sub02 @0x00F8E `40 00 05 01`)")
    warte(5, 1, 0, "John am Gang-Ausgang")
    cut_chg(7, "Cut_chg 7 = Nordende, Blick nach Sueden (Original-Szenenkamera sub02 @0x00C24)")
    dest(5, 1, JOHN_PLATZ, "Plc_dest RUN John -> (-7200,-15500) = Elzas Original-Dialogplatz")
    work_player()
    dest(5, 0, ELZA_PLATZ, "Plc_dest RUN Elza -> (-6750,-13350) = Johns Original-Dialogplatz (Form ROOM1171 sub02 @0x013D4)")
    warte(5, 0, 0, "Elza am Platz")
    warte(5, 1, 0, "John am Platz")
    work_npc(0)
    dest(9, 1, ELZA_PLATZ, "Plc_dest TURN John -> Elza (Form ROOM1141 sub02 @0x00C6E `40 00 09 01`)")
    work_player()
    dest(9, 0, JOHN_PLATZ, "Plc_dest TURN Elza -> John (Form ROOM1141 sub02 @0x00CE2 `40 00 09 00`)")
    warte(5, 0, 0, "Elza gedreht")
    warte(5, 1, 0, "John gedreht")
    cut_chg(0, "Cut_chg 0 = ROOM1141 sub02 @0x00D08")
    # ---- Original-Szene ab msg 2, Plaetze vertauscht ----
    vor = len(prog)
    kopie("1141", 0x00D0A, 0x00FA2, [0x2B, 0x02, 0x00, 0x00], "(sub02 ab msg 2)")
    ersetze([0x2B, 0x02, 0x00, 0x00], [0x2B, MSG_1141_FRAGE, 0x00, 0x00],
            "Port-Nachricht 40 = msg 2 ohne \"Same here...\" (Nachtrag 14 Punkt 5)")
    xp = {(-1300, -15500): (-850, -13350), (-850, -13350): (-1300, -15500),
          (-7200, -15500): (-6750, -13350), (-6750, -13350): (-7200, -15500)}
    getauscht = 0
    for i in range(vor, len(prog)):
        b, k = prog[i]
        if b[0] == 0x41 and len(b) == 10:
            x, z = struct.unpack_from("<hh", b, 2)[0], struct.unpack_from("<h", b, 6)[0]
            if (x, z) in xp:
                nb = bytearray(b); nx, nz = xp[(x, z)]; nb[2:4] = s16(nx); nb[6:8] = s16(nz)
                prog[i] = (bytes(nb), k + "  -> Blickpunkt (%d,%d) statt (%d,%d) (Plaetze getauscht)" % (nx, nz, x, z))
                getauscht += 1
        elif b[0] == 0x40 and len(b) == 8 and b[2] == 0x09:
            x, z = struct.unpack_from("<hh", b, 4)
            if (x, z) in xp:
                nb = bytearray(b); nx, nz = xp[(x, z)]; nb[4:6] = s16(nx); nb[6:8] = s16(nz)
                prog[i] = (bytes(nb), k + "  -> Drehziel (%d,%d) statt (%d,%d) (Plaetze getauscht)" % (nx, nz, x, z))
                getauscht += 1
    if getauscht != 8:
        sys.exit("ABBRUCH: %d statt 8 Koordinaten getauscht" % getauscht)
    # ---- Tuersperre, Warten auf die Gorillas, Abgang Johns (PORT-WAHL; Formen je Zeile) ----
    op([0x46, 0x00, 0x01, 0x31, MSG_1141_SPERRE, 0x00, 0xFF, 0xFF, 0x00, 0x00],
       "Aot_reset Slot 0 -> Text msg 41 (Form ROOM11B0 sub01 @0x01216 `46 01 01 31 0f 00 ff ff 00 00`)")
    op([0x46, 0x01, 0x01, 0x31, MSG_1141_SPERRE, 0x00, 0xFF, 0xFF, 0x00, 0x00], "Aot_reset Slot 1 -> Text msg 41")
    warte(4, 0xF2, 1, "Gorillas leben (sub01 @0x00C0C loescht (4,0xF2))")
    sleep(30, "Sleep 30 (Form ROOM1021 sub06 @0x021FE)")
    set_flag(2, 0x07, 1, "Set(2,7)=1 Pad-Sperre (Form ROOM1141 sub06 @0x01004)")
    set_flag(1, 0x1B, 1, "Set(1,0x1B)=1 Balken (Form ROOM1141 sub06 @0x01008)")
    cut_chg(5, "Cut_chg 5 = vom Sueden auf Rolladen/Dachgang (Kamera (-7266,-3806,-17278))")
    work_npc(0)
    dest(5, 1, GANG_AUSGANG, "Plc_dest RUN John -> Gang-Ausgang")
    warte(5, 1, 0, "John am Gang-Ausgang")
    cut_chg(6, "Cut_chg 6 = Dachgang")
    dest(5, 1, TUER_INNEN, "Plc_dest RUN John -> Dachtuer")
    warte(5, 1, 0, "John an der Dachtuer")
    se_on(BANK_TUER, 0, "Se_on(0x0E,0) DOOR1A oeffnen")
    sleep(15, "Sleep 15 (Form ROOM1131 sub02 @0x00A54)")
    pos_set(PARK, "Pos_set John geparkt (Form ROOM10D0 sub21 @0x01D9E, Lage ROOM11B0 @0x01080)")
    se_on(BANK_TUER, 1, "Se_on(0x0E,1) DOOR1A zufallen (Door_exit)")
    set_flag(9, BIT_JOHN_WEG, 1, "Set(9,67)=1 John hat 1141 verlassen (VERTRAG 1.1)")
    op([0x46, 0x00, 0x02, 0x31] + list(RDT["1141"][0x00A52 + 14:0x00A52 + 20]),
       "Aot_reset Slot 0 -> Tuer ROOM1131 (Nutzlast = main00 @0x00A52+14)")
    op(list(RDT["1141"][0x00FDC:0x00FE6]), "Aot_reset Slot 1 -> Dachtuer = ROOM1141 sub05 @0x00FDC")
    sleep(15, "Sleep 15")
    op([0x3C, 0x01], "Cut_auto 1   (Schwanz ROOM1050 sub03 @0x00DF4)")
    set_flag(2, 0x07, 0, "Set(2,7)=0")
    set_flag(1, 0x1B, 0, "Set(1,0x1B)=0")
    op([0x01, 0x00], "Evt_end")
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
    ("RIEGEL1141_SUB05", "1141", 0x00FC8, 0x00FE8),
    ("RIEGEL1141_SUB02", "1141", 0x00C18, 0x00FA4),
    ("RIEGEL1141_SUB00", "1141", 0x00AFA, 0x00B42),
)


def main():
    pruefen = "--pruefen" in sys.argv
    programme = [
        ("k_r38f_p_roll1140", baue_rolladen("1140", 0x00CA6, 0x00CAC, 0xA0, 0xD0, 0x00CCC, 0x00D16, 0x00D52)),
        ("k_r38f_p_roll1141", baue_rolladen("1141", 0x00FE8, 0x00FEE, 0xE8, 0x118, 0x01032, 0x0107C, 0x010DC)),
        ("k_r38f_p_leiche1021", baue_leiche()),
        ("k_r38f_p_haemmern", baue_haemmern()),
        ("k_r38f_p_riegel", baue_riegel()),
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
