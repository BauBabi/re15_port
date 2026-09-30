#!/usr/bin/env python3
"""belege.py - Spur A (Runde 34 Nacht): HERKUNFTSMARKE fuer den Bauplan "Sicherung einsetzen".

Prueft JEDE Byte-Behauptung des Dossiers analysis/befunde_runde34_nacht/A_rolltor.md gegen die
ausgelieferten Dateien, damit keine Adresse nur zitiert, aber nie gelesen ist
(memory reai-v2-zitierte-adresse-ist-kein-beleg). Ausgabe: je Beleg OK/FEHLT, am Ende Summe;
Exit 1, sobald ein Beleg nicht stimmt.

Quellen (nur lesen):
  re15_port/shared_assets/PSX/STAGE1/ROOM1050.RDT, ROOM1051.RDT, STAGE2/ROOM2060.RDT, ROOM2061.RDT
  info/Re1.5/PSX.EXE     (RE1.5, PS-X-EXE: Datei = 0x800 + addr - t_addr, t_addr @Kopf+0x18)
  info/re2leon/PSX.EXE   (RE2 Retail Leon, dieselbe Abbildung)

Aufruf:  python belege.py
"""
import os, struct, sys

HIER = os.path.dirname(os.path.abspath(__file__))
BAUM = os.path.normpath(os.path.join(HIER, "..", "..", ".."))
PSX = os.path.join(BAUM, "re15_port", "shared_assets", "PSX")


def lies(pfad):
    with open(pfad, "rb") as f:
        return f.read()


def rdt(stage, name):
    return lies(os.path.join(PSX, "STAGE%d" % stage, name + ".RDT"))


class Exe:
    def __init__(self, pfad):
        self.d = lies(pfad)
        self.t_addr = struct.unpack_from("<I", self.d, 0x18)[0]

    def bytes(self, addr, n):
        o = 0x800 + addr - self.t_addr
        return self.d[o:o + n]


RE15 = Exe(os.path.join(BAUM, "info", "Re1.5", "PSX.EXE"))
RE2 = Exe(os.path.join(BAUM, "info", "re2leon", "PSX.EXE"))

N_OK = 0
N_FEHL = 0


def pruefe(text, ist, soll):
    global N_OK, N_FEHL
    soll_b = bytes.fromhex(soll.replace(" ", "")) if isinstance(soll, str) else soll
    if ist == soll_b:
        N_OK += 1
        print("  OK     %s" % text)
    else:
        N_FEHL += 1
        print("  FEHLT  %s\n         ist  %s\n         soll %s" % (text, ist.hex(" "), soll_b.hex(" ")))


def wort(w):
    return struct.pack("<I", w)


def main():
    r1050, r1051 = rdt(1, "ROOM1050"), rdt(1, "ROOM1051")
    r2060, r2061 = rdt(2, "ROOM2060"), rdt(2, "ROOM2061")

    print("== ROOM1050 / ROOM1051 - was ausgeliefert ist")
    pruefe("1050 sub00 @0x0C1E Ck(3,121,0) Tor zu?", r1050[0xC1E:0xC22], "21 03 79 00")
    pruefe("1050 sub00 @0x0C22 Aot_set Slot 7 sce 3 -> sub02 (Schalter)", r1050[0xC22:0xC36],
           "2c 07 03 31 00 00 a0 41 0a dd 20 03 20 03 ff 00 18 02 00 00")
    pruefe("1050 sub02 @0x0CAC Message_on 0 Maske 0xFF80 (Frage)", r1050[0xCAC:0xCB0], "2b 00 80 ff")
    pruefe("1050 sub02 @0x0CB6 Ck(12,31,0) = Ja", r1050[0xCB6:0xCBA], "21 0c 1f 00")
    pruefe("1050 sub02 @0x0CBA Set(3,121,1) Tor offen", r1050[0xCBA:0xCBE], "22 03 79 01")
    pruefe("1050 sub02 @0x0CC8 Set(2,7,1) Spieler-Sperre", r1050[0xCC8:0xCCC], "22 02 07 01")
    pruefe("1050 sub02 @0x0CD0 Cut_chg 3", r1050[0xCD0:0xCD2], "29 03")
    pruefe("1050 sub02 @0x0D74 Set(2,7,0)", r1050[0xD74:0xD78], "22 02 07 00")
    pruefe("1050 sub02 @0x0D7C Cut_auto 1", r1050[0xD7C:0xD7E], "3c 01")
    pruefe("1050 msg 2 @0x0ED2 'I need a fuse to run the shutter.'", r1050[0xED2:0xEF7],
           "04 02 25 00 4a 41 41 40 00 3d 00 42 51 4f 41 00 50 4b 00 4e 51 4a 00 50 44 41 00"
           " 4f 44 51 50 50 41 4e 57 01 00")
    pruefe("1051 msg 2 @0x0E68 derselbe Satz", r1051[0xE68:0xE8D], r1050[0xED2:0xEF7])
    pruefe("1051 sub02 @0x0CC8..0x0DA4 == 1050 sub02 @0x0CAC..0x0D88", r1051[0xCC8:0xDA4],
           r1050[0xCAC:0xD88])
    pruefe("1051 sub00 @0x0C4C Schalter-Zone == 1050 @0x0C22", r1051[0xC4C:0xC60], r1050[0xC22:0xC36])
    pruefe("Kameratabelle @0x60, 10 Cuts, 1050 == 1051", r1051[0x60:0x60 + 320], r1050[0x60:0x60 + 320])
    pruefe("Cut 7 @0x140 und Cut 8 @0x160: erste 28 Byte gleich", r1050[0x160:0x17C], r1050[0x140:0x15C])
    pruefe("Cut 7 pri_offset @0x15C = 0x518", r1050[0x15C:0x160], wort(0x518))
    pruefe("Cut 8 pri_offset @0x17C = 0x51C", r1050[0x17C:0x180], wort(0x51C))
    pruefe("1051 main00 @0x0C0E Slot 11 BELEGT (Leiche -> sub03)", r1051[0xC0E:0xC22],
           "2c 0b 03 31 00 00 92 3b ae e3 e8 03 e8 03 ff 00 18 03 00 00")
    pruefe("1051 main00 @0x0C22 Slot 12 BELEGT (geparkte SIG P228)", r1051[0xC22:0xC38],
           "50 0c 09 31 00 00 00 00 00 00 00 00 00 00 04 00 0f 00 a5 00 ff 00")
    pruefe("1051 sub03 @0x0DA4 Set(2,7,1)", r1051[0xDA4:0xDA8], "22 02 07 01")
    pruefe("1051 sub03 @0x0DB4 Cut_chg 9 (Nahansicht Leiche)", r1051[0xDB4:0xDB6], "29 09")
    pruefe("1051 sub03 @0x0DB6 Message_on 5 Maske 0xFFFF", r1051[0xDB6:0xDBA], "2b 05 ff ff")
    pruefe("1051 sub03 @0x0DC0 Cut_chg 3 (Rueckweg)", r1051[0xDC0:0xDC2], "29 03")
    pruefe("1051 sub03 @0x0DC2 Cut_auto 1", r1051[0xDC2:0xDC4], "3c 01")
    pruefe("1051 sub03 @0x0DD2 Set(2,7,0)", r1051[0xDD2:0xDD6], "22 02 07 00")
    # Rolltor-Modell obj 0 bei (15432,0,-10424) und Kollisionszelle 19 (Rolltor)
    pruefe("1050 sub00 @0x0C36 Obj_model_set obj 0 (Rolltor) pos x/z", r1050[0xC40:0xC48],
           "48 3c 00 00 48 d7 00 00")

    print("== ROOM2060 - das vollstaendige RE1.5-Sicherungsraetsel (Vorbild)")
    pruefe("2060 sub00 @0x010A2 Ck(3,108,0) Sicherung nicht genommen", r2060[0x10A2:0x10A6], "21 03 6c 00")
    pruefe("2060 sub00 @0x010C2 Ck(3,144,0) nicht eingesetzt -> Slot 9 sce 3 sub18",
           r2060[0x10C2:0x10DA], "21 03 90 00 2c 09 03 31 00 00 a4 ed 6e 0f 78 05 e8 03 ff 00 18 12 00 00")
    pruefe("2060 sub12 @0x015BA Message_on 0 (Generator-Frage)", r2060[0x15BA:0x15BE], "2b 00 ff ff")
    pruefe("2060 sub12 @0x015C4 Ck(12,31,0) Ja", r2060[0x15C4:0x15C8], "21 0c 1f 00")
    pruefe("2060 sub12 @0x015CC Ck(3,144,0) + @0x015D0 Message_on 1 (Sperrsatz)",
           r2060[0x15CC:0x15D4], "21 03 90 00 2b 01 ff ff")
    pruefe("2060 sub18 @0x01678 Message_on 3 / Evt_next / Message_on 4 (Frage)",
           r2060[0x1678:0x1682], "2b 03 ff ff 02 00 2b 04 ff ff")
    pruefe("2060 sub18 @0x01688 Ck(12,31,0) + Cut_chg 8 (Nahansicht vorher)",
           r2060[0x1688:0x168E], "21 0c 1f 00 29 08")
    pruefe("2060 sub19 @0x0169E Set(3,144,1) + Cut_chg 9 (Nahansicht nachher)",
           r2060[0x169E:0x16A4], "22 03 90 01 29 09")
    pruefe("2060 sub19 @0x016C8 Message_on 5 / Evt_next / Cut_chg 10 / Cut_auto 1",
           r2060[0x16C8:0x16D4], "2b 05 ff ff 02 00 29 0a 3c 01 01 00")
    pruefe("2060 msg 4 @0x1855 'Will you use the Fuse?' (Ja/Nein)", r2060[0x1855:0x1875],
           "04 02 33 45 48 48 00 55 4b 51 00 51 4f 41 00 50 44 41 00 05 01 22 51 4f 41 05 00"
           " 1b 03 02 01 00")
    pruefe("2060 msg 5 @0x1875 \"You've used the Fuse.\"", r2060[0x1875:0x1892],
           "04 02 35 4b 51 3a 52 41 00 51 4f 41 40 00 50 44 41 00 05 01 22 51 4f 41 05 00 57 01 00")
    pruefe("2061 msg 4/5 == 2060", r2061[0x1855:0x1892], r2060[0x1855:0x1892])

    print("== RE1.5 PSX.EXE - die Opcodes, die der Bauplan benutzt")
    pruefe("Opcode-Tabelle 0x800744a8[0x29] -> 0x800402a0 Cut_chg", RE15.bytes(0x8007454C, 4), wort(0x800402A0))
    pruefe("Opcode-Tabelle [0x2A] -> 0x8004032c Cut_old", RE15.bytes(0x80074550, 4), wort(0x8004032C))
    pruefe("Opcode-Tabelle [0x2B] -> 0x800404f4 Message_on", RE15.bytes(0x80074554, 4), wort(0x800404F4))
    pruefe("Opcode-Tabelle [0x3C] -> 0x800403ac Cut_auto", RE15.bytes(0x80074598, 4), wort(0x800403AC))
    pruefe("Opcode-Tabelle [0x5E] -> 0x80042b04 = LETZTER Eintrag (95)", RE15.bytes(0x80074620, 4), wort(0x80042B04))
    pruefe("Cut_chg @0x800402d4 ori v0,v0,0x100 (Auto-Kamera AUS)", RE15.bytes(0x800402D4, 4), "00 01 42 34")
    pruefe("Cut_chg @0x800402e4 sb a1,DAT_800b3f7b (alter Cut gemerkt)", RE15.bytes(0x800402E4, 4), "7b 3f 25 a0")
    pruefe("Cut_old @0x8004033c lbu a0,DAT_800b3f7b", RE15.bytes(0x8004033C, 4), "7b 3f 84 90")
    pruefe("Cut_old @0x80040378 addiu v1,zero,-257 (Bit 0x100 loeschen)", RE15.bytes(0x80040378, 4), "ff fe 03 24")
    pruefe("Message_on @0x80040500 ori a1,zero,0x300", RE15.bytes(0x80040500, 4), "00 03 05 34")
    pruefe("Message_on @0x8004051c sll a3,a3,16 (Maske)", RE15.bytes(0x8004051C, 4), "00 3c 07 00")
    pruefe("Evt_next @0x8003f26c ori v0,zero,0x2 (Ertrag)", RE15.bytes(0x8003F26C, 4), "02 00 02 34")
    pruefe("Ck @0x8003fd24 addiu at,at,0x4664 (Bank-Tabelle)", RE15.bytes(0x8003FD24, 4), "64 46 21 24")
    pruefe("Bank-Tabelle 0x80074664[9] = 0x800b1078", RE15.bytes(0x80074688, 4), wort(0x800B1078))
    pruefe("Bank-Tabelle 0x80074664[12] = 0x800b8520 (Ja/Nein)", RE15.bytes(0x80074694, 4), wort(0x800B8520))

    print("== RE1.5 PSX.EXE - Inventar-Grundbausteine (Verbrauch Heil-Item)")
    pruefe("FUN_8004dfec @0x8004dff0 lbu v0,0x800b0fbc (Anzahl Plaetze)", RE15.bytes(0x8004DFF0, 4), "bc 0f 42 90")
    pruefe("FUN_8004dfec @0x8004e048 addiu v0,zero,-1 (nicht gefunden)", RE15.bytes(0x8004E048, 4), "ff ff 02 24")
    pruefe("Verbrauch @0x8004aef0 sb zero (Id)", RE15.bytes(0x8004AEF0, 4), "00 00 20 a0")
    pruefe("Verbrauch @0x8004af2c jal 0x8004dadc (Nachruecken)", RE15.bytes(0x8004AF2C, 4), "b7 36 01 0c")
    pruefe("Eigenschaft Item 0x40 @0x800750a8: cap 1, Paarzeiger 0x80074c88, 0 Paare",
           RE15.bytes(0x800750A8, 12), "01 00 00 00 88 4c 07 80 00 00 00 00")

    print("== RE2 Retail PSX.EXE - Sce_item_lost (0x62) und Keep_Item_ck (0x5E)")
    pruefe("RE2 Opcode-Tabelle 0x800a74c8[0x62] -> 0x800585e4", RE2.bytes(0x800A7650, 4), wort(0x800585E4))
    pruefe("RE2 Opcode-Tabelle [0x5E] -> 0x800584f0 (Keep_Item_ck)", RE2.bytes(0x800A7640, 4), wort(0x800584F0))
    pruefe("RE2 @0x800585fc lbu a0,1(v0) (Item-Id)", RE2.bytes(0x800585FC, 4), "01 00 44 90")
    pruefe("RE2 @0x80058600 jal 0x800696cc (Platz suchen)", RE2.bytes(0x80058600, 4), "b3 a5 01 0c")
    pruefe("RE2 @0x80058608 bltz v0 (kein Treffer -> nur Nachruecken)", RE2.bytes(0x80058608, 4), "0a 00 40 04")
    pruefe("RE2 @0x80058618 sb zero,0x4a3c(at) (Id)", RE2.bytes(0x80058618, 4), "3c 4a 20 a0")
    pruefe("RE2 @0x80058634 jal 0x80069714 (Nachruecken)", RE2.bytes(0x80058634, 4), "c5 a5 01 0c")
    pruefe("RE2 @0x80058644 addiu v1,v1,2 (Satzlaenge 2)", RE2.bytes(0x80058644, 4), "02 00 63 24")

    print("\nSUMME: %d OK, %d FEHLT" % (N_OK, N_FEHL))
    return 1 if N_FEHL else 0


if __name__ == "__main__":
    sys.exit(main())
