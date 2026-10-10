#!/usr/bin/env python3
"""szene11b0_bauen.py - Spur J (Runde 36): die ERWEITERUNG der ROOM11B0-Szene sub06 (Marvin schliesst die
Werkstatt-Tuer ab) aus ORIGINAL-Opcodes, als engine/src/gen/szl11b0_szene.inc.

Auftrag (AUFTRAG.md Z.37-44 + Nachtrag 14:00 Z.119/146): am Ende der Szene Marvin "Hold on" -> rennt zur
Innenseite der Werkstatt-Tuer -> Zuschliess-Animation + Zuschliess-Ton (RE2 Robert Kendo, Spielanfang) -> Tuer
bleibt zu (Elzas Meldung) -> zurueck zu Leon und Ada -> "I just want to make sure we don't have any unwanted
followers."; die Tuer danach beim Anklicken mit Elzas Text. Parkplatz-Tuer NICHT (Spur Q).

Jede Zeile ist ein ausgelieferter Opcode in ausgelieferter Form (Laengen = engine/src/scd_vm.c s_opcode_sizes);
Wege/Positionen/Texte/Gesten = NUTZER-VORGABE bzw. PORT-WAHL, die FORM ist belegt (Datei-Offsets in
re15_port/shared_assets/PSX/STAGE1/ROOM11B0.RDT, RE2: info/re2leon/PL0/RDT/ROOM1010.RDT):
  Set(9,84) als ERSTES       Einmal-Riegel wie ROOM11B0 sub06 @0x01478 `22 03 83 01` (VERTRAG §1.1: Bit 84 = J)
  Work_set(2,0)+Nop Marvin   sub06 @0x016D0 `2e 02 00 00` (Sce_em_set main00 @0x01080 `44 00 40` = Slot 0)
  Message_on                 sub06 @0x014EE `2b 01 00 00`
  Geste + Abschluss          sub06 @0x014F6 `3f 00 0f 00` Sleep 40 @0x014FA ... @0x01506 `3f 00 17 00` Sleep 20
  Plc_dest Modus 5 (rennen)  sub08 @0x01852 `40 00 05 02 89 a6 a0 c0` (Marvin, Ankunftsbit 2)
  Warten                     sub08 @0x0185A `18 0b` = Gosub sub11 @0x018BE (Set(5,2)=0, While Ck(5,2)==0 Evt_next)
  Plc_dest Modus 9 (drehen)  sub08 @0x01884 `40 00 09 02 05 f8 59 a9`
  Cut_chg                    sub06 @0x016C2 `29 06`
  ABSCHLIESSEN = RE2 ROOM1010 sub09 @0x01C18..@0x01C44 (Kendo, Gun Shop):
     @0x01C18 `3f 00 00 00` Clip 0 == ROOM11B0 RBJ Record 1 Clip 15 (bytegleich, Dossier §2.1) -> `3f 00 0f 00`
     @0x01C1C `09 0a 14 00` Sleep 20
     @0x01C20 `36 02 0a 01 ...` Se_on Raumbank Satz 0x0A -> Port-Bank RE15_SZL_SE_BANK Satz 0x0A (SCHLOSS1010)
     @0x01C2C `09 0a 0a 00` Sleep 10
     @0x01C30 `3f 00 00 c0` rueckwaerts -> RE1.5-Form `3f 00 0f 00` + `43 00 80 00` (ROOM11C0 sub02 @0x018D8..@0x018E4)
     @0x01C34..@0x01C44 Sleep 5 / `3f 00 00 40` / Plc_cnt 10 / Sleep 5 / Plc_stop / Sleep 25 -> Sleep 35 (RE1.5
     kennt Plc_cnt 0x5B / Plc_stop 0x66 nicht: Opcode-Tabelle 0x00..0x5E, 0x5B ist dort ein Feld-Setter)
  TUER ZU                    Sca_id_set/Sca_floor_set wie sub06 @0x017DA `37 02 04 ff` / @0x017DE `39 02 04 03`
                             auf die Zelle (3,0x0C) @0x00EC8 (Auslieferungszustand u0=ff fl=03); Text-Platz 8 =
                             ROOM11B1 sub00 @0x0112A `2c 08 01 31 00 00 80 a8 d0 bc b0 04 a4 06 0c 00 ff ff 00 00`
                             mit Nachricht 22 statt 12
  Rueckgabe                  sub06 @0x017EA `47 05` Aot_on 5 (Gully) + @0x017EC `01 00` Evt_end

Aufruf: szene11b0_bauen.py  -> schreibt engine/src/gen/szl11b0_szene.inc, druckt das Listing
"""
import os, struct, sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(os.path.dirname(HIER))
OUT = os.path.join(PORT, "engine", "src", "gen", "szl11b0_szene.inc")
RDT_11B0 = os.path.join(PORT, "shared_assets", "PSX", "STAGE1", "ROOM11B0.RDT")
RDT_11B1 = os.path.join(PORT, "shared_assets", "PSX", "STAGE1", "ROOM11B1.RDT")

# ---- Konstanten (muessen mit include/re15_szenenleon_r36.h uebereinstimmen; der Riegel prueft das) ----
BANK_ZU, BIT_ZU = 9, 84                  # VERTRAG Runde 36 §1.1 Spur J
SE_BANK = 0x3A                           # RE15_SZL_SE_BANK (Port-Bank SCHLOSS1010, 0x30 + Spur J = 10)
SE_SCHLOSS = 0x0A                        # RE2 ROOM1010 sub09 @0x01C20 `36 02 0a 01` -> Satz 0x0A
MSG_HOLD, MSG_FOLLOW, MSG_ZU = 20, 21, 22   # VERTRAG §1.2 ROOM11B0 J 20..27
MARVIN = 0                               # Work_set(2,0)
BIT_M = 2                                # Ankunftsbit Marvin (sub11 = Gosub 0x0B)
GOSUB_WARTEN = 0x0B
# Weg (PORT-WAHL, frei von allen SCA-Zellen + 450 Aktorradius, Dossier §2.2); Rueckweg = umgekehrt
WEG = [(1250, -21571),        # suedlich am Gully vorbei: Zelle (3,9) @0x00EA4 z..-21121 + 450
       (-2600, -21571),       # nach Westen zwischen Leon (-1639,-19999) und Ada (-1418,-22490)
       (-6103, -25224),       # = Leons Original-Weg sub07 @0x0181A
       (-16603, -25224),      # = sub07 @0x01810
       (-20860, -16230)]      # vor der Tuer: Zelle (3,0x0C) Ostkante -21410 - 450 - 100, z = Zellenmitte
TUER_BLICK = (-22903, -16230)  # x = sub07 @0x017FC, z = Zellenmitte -> Blick nach Westen auf die Tuer
HEIM = (1100, -19999)         # PORT-WAHL: Marvins Platz am Ende von sub06 (Messung §2.4)
LEON_BLICK = (-1549, -19868)  # sub06 @0x0174C `40 00 09 02 f3 f9 64 b2` (Marvin dreht zu Leon)
CUT_CONTAINER, CUT_WEG, CUT_TUER = 6, 10, 8   # Cut 10 = Cut 4 mit Licht (RVD @0x00488 9->10), Cut 8 = Cut 2
                                               # mit Licht (sub00 @0x01188 `4b 02 08`)
TEXTPLATZ = 8
TUER_RECHT = (-22400, -17200, 1200, 1700)       # ROOM11B1 sub00 @0x0112A

prog = []


def s16(v):
    return struct.pack("<h", v)


def op(b, c):
    prog.append((bytes(b), c))


def set_flag(bank, bit, val, c):  op([0x22, bank, bit, val], c)
def work_marvin():                op([0x2E, 0x02, MARVIN, 0x00], "Work_set(2,0)+Nop Marvin")
def message(mid, c):              op([0x2B, mid, 0x00, 0x00], c)
def motion(clip, c):              op([0x3F, 0x00, clip, 0x00], c)
def motion_rev():                 op([0x43, 0x00, 0x80, 0x00], "Plc_flg(0,0x80) rueckwaerts (ROOM11C0 @0x018E4)")
def sleep(n, c=""):               op([0x09, 0x0A, n & 0xFF, (n >> 8) & 0xFF], c or "Sleep %d" % n)
def cut_chg(n, c):                op([0x29, n], c)
def dest(mode, x, z, c):          op(b"\x40\x00" + bytes([mode, BIT_M]) + s16(x) + s16(z), c)
def warten():                     op([0x18, GOSUB_WARTEN], "Gosub sub11 (warten auf Ck(5,2))")
def se_on(bank, sat, c):          op(bytes([0x36, bank, sat] + [0] * 9), c)
def sca_id(r, i, v, c):           op([0x37, r, i, v], c)
def sca_floor(r, i, v, c):        op([0x39, r, i, v], c)
def aot_text(slot, rect, mid, c):
    x, z, w, d = rect
    op(bytes([0x2C, slot, 0x01, 0x31, 0x00, 0x00]) + s16(x) + s16(z) + s16(w) + s16(d) +
       bytes([mid, 0x00, 0xFF, 0xFF, 0x00, 0x00]), c)


def tuer_zu():
    sca_id(3, 0x0C, 0xFF, "Sca_id_set(3,0x0C)=0xff  Durchgangszelle wieder solid (@0x00EC8 u0=ff)")
    sca_floor(3, 0x0C, 0x03, "Sca_floor_set(3,0x0C)=0x03  Band 0 (@0x00EC8 fl=03)")
    aot_text(TEXTPLATZ, TUER_RECHT, MSG_ZU, "Aot_set Platz 8 Text msg 22 (ROOM11B1 sub00 @0x0112A, msg 12 -> 22)")


# ================================ ERWEITERUNG (Ereignis 22) ================================
marke = {}
set_flag(BANK_ZU, BIT_ZU, 1, "Set(9,84)=1 als ERSTES (Einmal-Riegel)")
work_marvin()
message(MSG_HOLD, "Message_on 20  Marvin: \"Hold on.\"  (NUTZER-VORGABE)")
motion(15, "Plc_motion(0,15) Geste 'Hey!' (sub06 @0x014F6)")
sleep(40, "Sleep 40  (Zeilentakt @0x014FA)")
motion(23, "Plc_motion(0,23) Abschluss (sub06 @0x01506)")
sleep(20, "Sleep 20  (Zeilentakt @0x0150A)")
marke["LAUF_HIN"] = len(prog)
for i, (x, z) in enumerate(WEG):
    if i == 2:
        cut_chg(CUT_WEG, "Cut_chg 10  Weg durch die Werkstatt (Cut 4 mit Licht)")
    if i == 4:
        cut_chg(CUT_TUER, "Cut_chg 8  die Werkstatt-Tuer (Cut 2 mit Licht)")
    dest(5, x, z, "Plc_dest Modus 5 Marvin -> (%d,%d)" % (x, z))
    warten()
dest(9, TUER_BLICK[0], TUER_BLICK[1], "Plc_dest Modus 9 Marvin dreht zur Tuer (%d,%d)" % TUER_BLICK)
warten()
marke["SCHLIESSEN"] = len(prog)
motion(15, "Plc_motion(0,15) = RE2 Kendo Clip 0 (ROOM1010 sub09 @0x01C18, bytegleich)")
sleep(20, "Sleep 20  (@0x01C1C)")
marke["SCHLOSS_TON"] = len(prog)
se_on(SE_BANK, SE_SCHLOSS, "Se_on Port-Bank 0x3A Satz 0x0A = RE2 ROOM1010 snd0 (@0x01C20)")
sleep(10, "Sleep 10  (@0x01C2C)")
motion(15, "Plc_motion(0,15) noch einmal")
motion_rev()
sleep(35, "Sleep 35  (@0x01C34..@0x01C44: 5 + 5 + 25)")
marke["TUER_ZU"] = len(prog)
tuer_zu()
marke["LAUF_ZURUECK"] = len(prog)
zurueck = list(reversed(WEG[:-1])) + [HEIM]
for i, (x, z) in enumerate(zurueck):
    if i == 1:
        cut_chg(CUT_WEG, "Cut_chg 10  zurueck durch die Werkstatt")
    if i == 3:
        cut_chg(CUT_CONTAINER, "Cut_chg 6  zurueck am Container (Cut der Original-Szene @0x016C2)")
    dest(5, x, z, "Plc_dest Modus 5 Marvin -> (%d,%d)" % (x, z))
    warten()
dest(9, LEON_BLICK[0], LEON_BLICK[1], "Plc_dest Modus 9 Marvin dreht zu Leon (sub06 @0x0174C)")
warten()
marke["ZEILE"] = len(prog)
message(MSG_FOLLOW, "Message_on 21  Marvin: \"I just want to make sure ...\"  (NUTZER-VORGABE)")
motion(16, "Plc_motion(0,16) Geste (sub06 @0x014FE)")
sleep(40, "Sleep 40  (Zeilentakt @0x014FA)")
sleep(50, "Sleep 50  (Zeilentakt @0x01502, Geste gehalten)")
motion(23, "Plc_motion(0,23) Abschluss (sub06 @0x01506)")
sleep(20, "Sleep 20  (Zeilentakt @0x0150A)")
marke["RUECKGABE"] = len(prog)
op([0x47, 0x05], "Aot_on 5  = sub06 @0x017EA (Gully -> ROOM1260)")
op([0x01, 0x00], "Evt_end  = sub06 @0x017EC")
erweiterung = prog

# ================================ TUER ZU beim Raumaufbau (Ereignis 23) ================================
prog = []
tuer_zu()
op([0x01, 0x00], "Evt_end")
tuerprog = prog

# ================================ Nachrichten ================================
ENC = {" ": 0x00, ":": 0x16, ",": 0x18, '"': 0x19, "!": 0x1A, "?": 0x1B, "'": 0x3A, "-": 0x3B, ".": 0x57}
for i in range(10):
    ENC[chr(ord("0") + i)] = 0x0C + i
for i in range(26):
    ENC[chr(ord("A") + i)] = 0x1D + i
    ENC[chr(ord("a") + i)] = 0x3D + i


def enc(s):
    return bytes(ENC[c] for c in s)


def marvin_zeile(zeilen):
    # Form ROOM11B0 msg 12 @0x1BD0: `04 00 05 07` "Marvin" `16 05 00 00` Text `04 01 01 5f`
    return (bytes([0x04, 0x00, 0x05, 0x07]) + enc("Marvin") + bytes([0x16, 0x05, 0x00, 0x00]) +
            b"\x08".join(enc(z) for z in zeilen) + bytes([0x04, 0x01, 0x01, 0x5F]))


rdt0 = open(RDT_11B0, "rb").read()
if rdt0[0x1BD0:0x1BDE] != bytes([0x04, 0x00, 0x05, 0x07]) + enc("Marvin") + bytes([0x16, 0x05, 0x00, 0x00]):
    sys.exit("ROOM11B0 msg 12 @0x1BD0: Kopf weicht ab")
if rdt0[0x1BFA:0x1BFE] != bytes([0x04, 0x01, 0x01, 0x5F]):
    sys.exit("ROOM11B0 msg 12 @0x1BFA: Ende weicht ab")
rdt1 = open(RDT_11B1, "rb").read()
MSG22 = rdt1[0x179F:0x17B6]                         # ROOM11B1 msg 12 "The door is locked." bis `01 00`
if MSG22 != bytes([0x04, 0x02]) + enc("The door is locked.") + bytes([0x01, 0x00]):
    sys.exit("ROOM11B1 msg 12 @0x179F weicht ab")
if rdt1[0x112A:0x113E] != bytes([0x2C, 0x08, 0x01, 0x31, 0x00, 0x00, 0x80, 0xA8, 0xD0, 0xBC, 0xB0, 0x04, 0xA4, 0x06,
                                 0x0C, 0x00, 0xFF, 0xFF, 0x00, 0x00]):
    sys.exit("ROOM11B1 sub00 @0x0112A weicht ab")
if rdt0[0x017EA:0x017EE] != bytes([0x47, 0x05, 0x01, 0x00]):
    sys.exit("ROOM11B0 sub06 @0x017EA weicht ab")
MSG20 = marvin_zeile(["Hold on."])
MSG21 = marvin_zeile(["I just want to make sure", "we don't have any unwanted followers."])


def c_array(name, b, kommentar):
    out = ["/* %s */" % kommentar, "static const uint8_t %s[%d] = {" % (name, len(b))]
    hexs = ["0x%02x" % x for x in b]
    for i in range(0, len(hexs), 16):
        out.append("    " + ", ".join(hexs[i:i + 16]) + ",")
    out.append("};")
    return out


def listing(p):
    out, off = [], 0
    for b, c in p:
        out.append("/*  +0x%04X  %-60s %s */" % (off, b.hex(" "), c))
        off += len(b)
    return out, off


def main():
    erw_bytes = b"".join(b for b, _ in erweiterung)
    tuer_bytes = b"".join(b for b, _ in tuerprog)
    offs, o = {}, 0
    starts = []
    for b, _ in erweiterung:
        starts.append(o)
        o += len(b)
    for k, idx in marke.items():
        offs[k] = starts[idx]
    L = ["/* ERZEUGT von tools/r36_j/szene11b0_bauen.py - NICHT von Hand aendern.",
         " * Runde 36 Spur J: Erweiterung der ROOM11B0-Szene sub06 + Tuer-zu-Programm + drei Nachrichten.",
         " * Herleitung je Zeile im Generator-Kopf und in analysis/befunde_runde36/J_szenenleon.md §2. */",
         "#define RE15_SZL_PROG_LEN      %d" % len(erw_bytes),
         "#define RE15_SZL_TUERPROG_LEN  %d" % len(tuer_bytes)]
    for k in sorted(offs):
        L.append("#define RE15_SZL_OFF_%-14s 0x%04X" % (k, offs[k]))
    lst, _ = listing(erweiterung)
    L += lst
    L += c_array("k_szl_prog", erw_bytes, "Ereignis 22: die Erweiterung (im Faden von sub06 ab @0x017EA)")
    lst, _ = listing(tuerprog)
    L += lst
    L += c_array("k_szl_tuerprog", tuer_bytes, "Ereignis 23: Tuer zu beim Raumaufbau, solange (9,84)=1")
    L += c_array("k_szl_msg20", MSG20, "msg 20 Marvin: Hold on.  (NUTZER-VORGABE, Form ROOM11B0 msg 12 @0x1BD0)")
    L += c_array("k_szl_msg21", MSG21, "msg 21 Marvin: I just want to make sure / we don't have any unwanted followers.")
    L += c_array("k_szl_msg22", MSG22, "msg 22 = ROOM11B1 msg 12 @0x179F \"The door is locked.\" (Bytes 1:1)")
    open(OUT, "w", newline="\n").write("\n".join(L) + "\n")
    print("\n".join(listing(erweiterung)[0]))
    print("Erweiterung %d B, Tuerprogramm %d B, msg20 %d B, msg21 %d B, msg22 %d B -> %s" % (
        len(erw_bytes), len(tuer_bytes), len(MSG20), len(MSG21), len(MSG22), OUT))


if __name__ == "__main__":
    main()
