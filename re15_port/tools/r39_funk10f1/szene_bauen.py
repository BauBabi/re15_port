#!/usr/bin/env python3
"""szene_bauen.py - Runde 39 Spur I (funk10f1): die Funkszene ROOM10F1 (Elza am Kommunikationssystem) aus ORIGINAL-
Opcodes, als engine/src/gen/funk10f1_r39_szene.inc (Programm + drei Nachrichten + Marken fuer die Riegel).

Auftrag (analysis/befunde_runde39/AUFTRAG.md, NUTZER-VORGABE): "In Room 10F1 moechte ich, das wenn Elza ihn betritt eine
Cutscene laeuft wo sie Rechts zum Kommunikationssystem geht und von der Animation her drauf drueckt und sagt "Hello, anyone,
can you hear me? We Need help!" / Dann kommt "..." / Dann "Uhh, this Thing is broken!". / Suche die dir passenden Animationen
dafuer heraus."  Dossier: analysis/befunde_runde39/I_funk10f1.md.

FORM = Elzas eigene, ausgelieferte Szene ROOM1031 sub13 @0x02982..@0x02A58 (re15_port/shared_assets/PSX/STAGE1/ROOM1031.RDT,
tools/scd_dump_room.py); jede Zeile ist ein ausgelieferter Opcode in ausgelieferter Form (Laengen = scd_vm.c s_opcode_sizes):
  Set(9,70) als ERSTES     Einmal-Riegel wie ROOM11B0 sub06 @0x01478 `22 03 83 01` (VERTRAG Runde 39 1.1: Bit 70 = Spur I)
  Set(2,7)/Set(1,27)       ROOM1031 sub13 @0x02986 `22 02 07 01` / @0x0298A `22 01 1b 01` (Pad-Sperre + Balken)
  Work_set(1,0)+Nop        @0x02990 `2e 01 00 00` (Spieler = Elza)
  Cut_auto 1               ROOM1050 sub03 @0x00DF4 `3c 01` (Kamera folgt den RVD-Baendern, solange Elza geht; Form wie
                           tools/r35_k/szene_bauen.py "Leon laeuft zu ihr")
  Plc_dest 9/4/9 + Warten  @0x029CC `40 00 09 20 ..` (drehen) / @0x029D6 `40 00 04 20 ..` (gehen) / @0x029E0 (drehen),
                           Ankunftsbit 0x20; Warten = sub14 @0x02A5A `11 00 08 00 02 00 12 04 21 05 20 00` (Do / Evt_next+
                           Nop / Edwhile / Ck(5,0x20)==0) - hier eingebettet, weil ROOM10F1 keinen sub14 hat (sub-Tabelle
                           @0x10D4 = 2 Eintraege)
  Cut_chg                  @0x0298E `29 04` (hier Cut 1 = die Dialogkamera vor der Konsole)
  DRUECKEN                 @0x029A4 `3f 00 0b 00` Plc_motion(0,11) = Haltepose "Arm vorn" + @0x029A8 `43 00 04 00` Plc_flg 4
                           (halten) + @0x029AC Sleep 20 ... @0x029BE `3f 00 00 00` Plc_motion(0,0) + @0x029C2 `43 00 80 00`
                           Plc_flg 0x80 (rueckwaerts = Arm senken) + @0x029C6 `09 0a 1e 00` Sleep 30.
                           Im Original ist das Elzas Druck auf den Notschalter des Rolltors (Elza steht vor Aot_set Slot 7
                           @0x01D88 msg 2 "The emergency shutters have already been activated.", Member_set @0x02994..@0x0299C
                           (-13097,-21855) Gierung 1024; Bilder Cut 4 F455..F515, Dossier §2.4).
  ZEILE 1 waehrend des     Message_on @0x02A08 `2b 12 00 00` + Sleep 100 @0x02A14 `09 0a 64 00` (Elzas Zeilentakt)
  Drueckens (die Hand bleibt auf der Konsole, bis die Zeile gesprochen ist, dann senkt sie den Arm)
  ZEILE 2 "..."            = Elzas "Keine Antwort"-Form nach "Hey, is anyone here?!": @0x02A32 `3f 00 09 00` Plc_motion(0,9)
                           (Hand an die Huefte), @0x02A36 `09 0a 05 00` Sleep 5, @0x02A3A `41 04 03 00 00 00 00 00 3c 00`
                           Plc_neck Modus 4 (3 Schwuenge = sich umsehen/horchen), @0x02A44 `09 0a 64 00` Sleep 100
  ZEILE 3 "broken"         @0x02A00 `3f 00 0c 00` Plc_motion(0,12) (Hand an die Stirn, 50 Bilder) + @0x02A04 Sleep 100
  Schwanz                  @0x02A48 `3c 01` Cut_auto 1, @0x02A4A `22 02 07 00`, @0x02A4E `22 01 1b 00`, @0x02A52 `2e 01 00 00`,
                           @0x02A56 `42 00` Plc_ret+Nop, @0x02A58 `01 00` Evt_end
Wege/Plaetze = PORT-WAHL (Lage im Raum aus Kollision + Text-Zone Slot 2 @0x00F72 + Kamerabild, Dossier §2.1/§3).
Clip-Nummern = Record 0 des geliehenen ROOM1031-Blocks (RDT+0x5C = 0x307C; Elzas Bibliothek, rbj_zensus.py).

Aufruf: szene_bauen.py   -> schreibt engine/src/gen/funk10f1_r39_szene.inc, druckt Listing + Zeilenbreiten
"""
import os, struct, sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(os.path.dirname(HIER))
OUT = os.path.join(PORT, "engine", "src", "gen", "funk10f1_r39_szene.inc")
RDT_1031 = os.path.join(PORT, "shared_assets", "PSX", "STAGE1", "ROOM1031.RDT")
sys.path.insert(0, os.path.join(HIER, "..", "r35_k"))
import texte_bauen as T                                            # noqa: E402  (Glyphen + Breiten-Korpus)

# ---- Konstanten (muessen mit include/re15_funk10f1_r39.h uebereinstimmen; der Riegel prueft das) ----
BANK_GESEHEN, BIT_GESEHEN = 9, 70          # VERTRAG Runde 39 1.1: Spur I = Bit 70
BIT_ANKUNFT = 0x20                         # ROOM1031 sub13 @0x029CC `40 00 09 20` / sub14 @0x02A62 `21 05 20 00`
MSG_HALLO, MSG_PUNKTE, MSG_KAPUTT = 40, 41, 42   # VERTRAG Runde 39 1.2: 10F1 Ids 40..44
CUT_DIALOG = 1                             # Cut 1 (5806,-3627,10200) Blick nach Sueden auf die Konsole (Dossier §2.1)
CLIP_HALTEN, CLIP_ARM, CLIP_HUEFTE, CLIP_STIRN = 11, 0, 9, 12   # ROOM1031 rec0 (Dossier §2.3/§2.4)
# Weg (PORT-WAHL, frei von allen SCA-Zellen + Spielerradius ~460 gemessen, Dossier §3.1):
WEG = [(6250, 2900),   # rechts (nach Norden) aus dem Tuer-Erker heraus: Ecke Zelle 10 (7400,2500) + Radius frei
       (6250, 5900),   # den Gang entlang, westlich am Konsolentisch Zelle 13 (x 6800..8300, z 3800..5300) vorbei
       (6900, 6400)]   # vor die Konsole zwischen den Tischen 13 und 12 (Zelle 10 Vorderkante x 7400, Slot 2 z 2600..12300)
KONSOLE = (7400, 6400)  # Blickpunkt: Vorderkante der Konsole (Zelle 10 x 7400) auf Hoehe des Standplatzes

prog = []
marke = {}


def s16(v):
    return struct.pack("<h", v)


def op(b, c):
    prog.append((bytes(b), c))


def set_flag(bank, bit, val, c):  op([0x22, bank, bit, val], c)
def work_elza():                  op([0x2E, 0x01, 0x00, 0x00], "Work_set(1,0)+Nop  Elza (@0x02990 / @0x02A52)")
def message(mid, c):              op([0x2B, mid, 0x00, 0x00], c)
def motion(clip, c):              op([0x3F, 0x00, clip, 0x00], c)
def flg(v, c):                    op([0x43, 0x00, v, 0x00], c)
def sleep(n, c=""):               op([0x09, 0x0A, n & 0xFF, (n >> 8) & 0xFF], c or "Sleep %d" % n)
def cut_chg(n, c):                op([0x29, n], c)
def cut_auto(n, c):               op([0x3C, n], c)
def dest(mode, x, z, c):          op(b"\x40\x00" + bytes([mode, BIT_ANKUNFT]) + s16(x) + s16(z), c)
def neck_umsehen(c):              op(bytes([0x41, 0x04, 0x03, 0, 0, 0, 0, 0, 0x3C, 0]), c)


def warten(c):
    op([0x11, 0x00, 0x08, 0x00], "Do  (Warteschleife = sub14 @0x02A5A: %s)" % c)
    op([0x02, 0x00], "Evt_next + Nop  (@0x02A5E)")
    op([0x12, 0x04], "Edwhile  (@0x02A60)")
    op([0x21, 0x05, BIT_ANKUNFT, 0x00], "Ck(5,0x20)==0 Ankunftsbit  (@0x02A62)")


# ================================ DAS PROGRAMM (Ereignis 27) ================================
set_flag(BANK_GESEHEN, BIT_GESEHEN, 1, "Set(9,70)=1 'Funkszene gesehen' als ERSTES (Einmal-Riegel, ROOM11B0 @0x01478)")
set_flag(2, 7, 1, "Set(2,7)=1  Pad-Sperre (ROOM1031 sub13 @0x02986)")
set_flag(1, 27, 1, "Set(1,27)=1 Balken (@0x0298A)")
work_elza()
marke["WEG"] = len(prog)
cut_auto(1, "Cut_auto 1  Kamera folgt den RVD-Baendern (Cut 0 -> 1 bei z 2900..3900), solange Elza geht")
dest(9, WEG[0][0], WEG[0][1], "Plc_dest Modus 9  Elza dreht nach RECHTS (Norden) zum Weg (@0x029CC)")
warten("gedreht")
for i, (x, z) in enumerate(WEG):
    dest(4, x, z, "Plc_dest Modus 4  Elza geht -> (%d,%d) (@0x029D6)" % (x, z))
    warten("Wegpunkt %d" % i)
dest(9, KONSOLE[0], KONSOLE[1], "Plc_dest Modus 9  Elza dreht sich zur Konsole (Osten) (@0x029E0)")
warten("zur Konsole gedreht")
cut_chg(CUT_DIALOG, "Cut_chg 1  Dialogkamera: die Konsole von Norden (@0x0298E Form)")
marke["DRUECKEN"] = len(prog)
motion(CLIP_HALTEN, "Plc_motion(0,11)  Arm vorn auf die Konsole = Elzas Druck auf den Rolltor-Schalter (@0x029A4)")
flg(0x04, "Plc_flg(0,4)  Pose halten (@0x029A8)")
sleep(20, "Sleep 20  Hand erreicht die Taste (@0x029AC)")
marke["ZEILE1"] = len(prog)
message(MSG_HALLO, "Message_on 40  Elza: Hello, anyone, can you hear me? / We need help!  (NUTZER-VORGABE)")
sleep(100, "Sleep 100  Zeile gesprochen, Taste gehalten (Elzas Zeilentakt @0x02A14)")
marke["LOSLASSEN"] = len(prog)
motion(CLIP_ARM, "Plc_motion(0,0)  Arm senken (@0x029BE)")
flg(0x80, "Plc_flg(0,0x80)  rueckwaerts (@0x029C2)")
sleep(30, "Sleep 30  (@0x029C6)")
marke["ZEILE2"] = len(prog)
message(MSG_PUNKTE, "Message_on 41  Elza: ...  (NUTZER-VORGABE)")
motion(CLIP_HUEFTE, "Plc_motion(0,9)  Hand an die Huefte (@0x02A32)")
sleep(5, "Sleep 5  (@0x02A36)")
neck_umsehen("Plc_neck Modus 4  3 Schwuenge: horchen/sich umsehen, keine Antwort (@0x02A3A)")
sleep(100, "Sleep 100  (@0x02A44)")
marke["ZEILE3"] = len(prog)
message(MSG_KAPUTT, "Message_on 42  Elza: Uhh, this thing is broken!  (NUTZER-VORGABE)")
motion(CLIP_STIRN, "Plc_motion(0,12)  Hand an die Stirn (@0x02A00)")
sleep(100, "Sleep 100  (@0x02A04)")
marke["SCHWANZ"] = len(prog)
cut_auto(1, "Cut_auto 1  Kamera wieder frei (@0x02A48)")
set_flag(2, 7, 0, "Set(2,7)=0  (@0x02A4A)")
set_flag(1, 27, 0, "Set(1,27)=0  (@0x02A4E)")
work_elza()
op([0x42, 0x00], "Plc_ret + Nop  (@0x02A56)")
op([0x01, 0x00], "Evt_end  (@0x02A58)")

# ================================ Nachrichten ================================
ELZA_KOPF = bytes([0x04, 0x00, 0x05, 0x01]) + T.enc("Elza") + bytes([0x16, 0x05, 0x00, 0x00])
ENDE = bytes([0x04, 0x01, 0x01, 0x63])
NACHRICHTEN = [   # (id, [Zeilen])  -- NUTZER-VORGABE woertlich (AUFTRAG.md); Umbruch = Port-Satz unter dem Budget
    (MSG_HALLO,  ["Hello, anyone, can you hear me?", "We need help!"]),
    (MSG_PUNKTE, ["..."]),
    (MSG_KAPUTT, ["Uhh, this thing is broken!"]),
]


def bauen(zeilen):
    return ELZA_KOPF + b"\x08".join(T.enc(z) for z in zeilen) + ENDE


def c_array(name, b, kommentar):
    out = ["/* %s */" % kommentar, "static const uint8_t %s[%d] = {" % (name, len(b))]
    hexs = ["0x%02x" % x for x in b]
    for i in range(0, len(hexs), 16):
        out.append("    " + ", ".join(hexs[i:i + 16]) + ",")
    out.append("};")
    return out


def main():
    d = open(RDT_1031, "rb").read()
    # Belege gegen die Datei (bricht ab, wenn die Vorlage nicht mehr stimmt)
    pruef = {0x02986: "22 02 07 01", 0x0298A: "22 01 1b 01", 0x02990: "2e 01 00 00", 0x029A4: "3f 00 0b 00",
             0x029A8: "43 00 04 00", 0x029BE: "3f 00 00 00", 0x029C2: "43 00 80 00", 0x029C6: "09 0a 1e 00",
             0x029CC: "40 00 09 20", 0x029D6: "40 00 04 20", 0x029E0: "40 00 09 20", 0x02A00: "3f 00 0c 00",
             0x02A04: "09 0a 64 00", 0x02A08: "2b 12 00 00", 0x02A14: "09 0a 64 00", 0x02A32: "3f 00 09 00",
             0x02A36: "09 0a 05 00", 0x02A3A: "41 04 03 00 00 00 00 00 3c 00", 0x02A44: "09 0a 64 00",
             0x02A48: "3c 01", 0x02A4A: "22 02 07 00", 0x02A4E: "22 01 1b 00", 0x02A52: "2e 01 00 00",
             0x02A56: "42 00", 0x02A58: "01 00", 0x02A5A: "11 00 08 00 02 00 12 04 21 05 20 00 01 00",
             0x2FA7: "04 00 05 01 21 48 56 3d 16 05 00 00"}
    for off, hx in pruef.items():
        soll = bytes.fromhex(hx.replace(" ", ""))
        if d[off:off + len(soll)] != soll:
            sys.exit("ROOM1031 @0x%05X weicht ab: %s statt %s" % (off, d[off:off + len(soll)].hex(" "), hx))
    if d[0x2FA7 + 0x21:0x2FA7 + 0x25] != ENDE:
        sys.exit("ROOM1031 msg 19 Ende weicht ab")
    fw = T.breite()
    korp = T.korpus()
    maxpx = 0
    for raum, mid, s, blob in korp:
        if blob[:3] == b"\x04\x00\x05":
            maxpx = max(maxpx, max(T.zeilen_px(blob, fw)))
    blob = b"".join(b for b, _ in prog)
    starts, o = [], 0
    for b, _ in prog:
        starts.append(o)
        o += len(b)
    L = ["/* ERZEUGT von tools/r39_funk10f1/szene_bauen.py - NICHT von Hand aendern.",
         " * Runde 39 Spur I: Funkszene ROOM10F1 (Elza am Kommunikationssystem), Form = ROOM1031 sub13 @0x02982..@0x02A58.",
         " * Herleitung je Zeile im Generator-Kopf und in analysis/befunde_runde39/I_funk10f1.md. */",
         "#define RE15_FUNK10F1_PROG_LEN %d" % len(blob)]
    for k in sorted(marke):
        L.append("#define RE15_FUNK10F1_OFF_%-10s 0x%04X" % (k, starts[marke[k]]))
    for (b, c), s in zip(prog, starts):
        L.append("/*  +0x%04X  %-44s %s */" % (s, b.hex(" "), c))
    L += c_array("k_funk10f1_prog", blob, "Ereignis 27: die Funkszene")
    print("Budget: breiteste ausgelieferte Dialogzeile %d px" % maxpx)
    for mid, zeilen in NACHRICHTEN:
        b = bauen(zeilen)
        pxs = [sum(fw[c] for c in T.enc("Elza: " + zeilen[0]))] + [sum(fw[c] for c in T.enc(z)) for z in zeilen[1:]]
        if max(pxs) > maxpx:
            sys.exit("msg %d ueber dem Budget: %s > %d" % (mid, pxs, maxpx))
        print("msg %d: %d B, Zeilen-px %s" % (mid, len(b), pxs))
        L += c_array("k_funk10f1_msg%d" % mid, b, "msg %d Elza: %s  (Kopf/Ende = ROOM1031 msg 19 @0x2FA7, Zeilen-px %s, "
                     "Budget %d)" % (mid, " / ".join(zeilen), pxs, maxpx))
    open(OUT, "w", encoding="utf-8", newline="\n").write("\n".join(L) + "\n")
    for (b, c), s in zip(prog, starts):
        print("+%04X  %-44s %s" % (s, b.hex(" "), c))
    print("-> %s: Programm %d B, %d Opcodes" % (OUT, len(blob), len(prog)))


if __name__ == "__main__":
    main()
