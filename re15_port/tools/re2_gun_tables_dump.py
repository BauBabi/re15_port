#!/usr/bin/env python3
"""re2_gun_tables_dump.py - Dump der RE2-Schuss-Applier-Tabellen aus info/re2leon/PSX.EXE
(Runde 16, Thema trefferhoehe). Alle Werte, die re15_damage.c (re15_re2_gun_probe) traegt,
kommen aus diesem Dump - nichts davon ist geraten.

  DAT_800A6DB4   Prioritaets-Zeilen (3 Zeilen x 3 Teile, Satz 1 / Satz 2 (+9) fuer Flag&8)
  DAT_800A6F8C   Zielhoehe-Bits (word0>>29) -> Gruppe DOWN 0 / LEVEL 1 / UP 2
  0x800A6A88     Zeigertabelle je Gegnertyp -> Schadensrecord-Basis ([0x10] = Zombie 0x800A412C)
  Record         (id-1)*20: w0 = Schaden je Klammer (10 Bit), w1 = Poise (3 Bit) + +0x1D3 (7 Bit),
                 +8/+0xC/+0x10 = dy-Fenster UP/LEVEL/DOWN
  0x800A68E8     Geometrie-Zeiger item*24 + grp*8 -> Record 0x1C Byte:
                 Byte 0, Flags[3], 3 x {start, 0, depth/4, halfw/4}

Aufruf: python re15_port/tools/re2_gun_tables_dump.py [repo-root]
"""
import struct, sys, os

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = sys.argv[1] if len(sys.argv) > 1 else os.path.abspath(os.path.join(HERE, "..", ".."))
exe = open(os.path.join(REPO, "info", "re2leon", "PSX.EXE"), "rb").read()
t_addr = struct.unpack_from("<I", exe, 0x18)[0]

def off(a): return a - t_addr + 0x800
def u32(a): return struct.unpack_from("<I", exe, off(a))[0]
def s16(a): return struct.unpack_from("<h", exe, off(a))[0]

print("t_addr=%08x" % t_addr)
print("DAT_800A6DB4:", exe[off(0x800A6DB4):off(0x800A6DB4) + 18].hex(" "))
print("DAT_800A6F8C:", exe[off(0x800A6F8C):off(0x800A6F8C) + 8].hex(" "))
zb = u32(0x800a6a88 + 0x10 * 4)
print("PTR_DAT_800a6a88[0x10]=%08x" % zb)
print("--- Zombie-Records @%08x + (id-1)*20" % zb)
for i in range(1, 20):
    a = zb + (i - 1) * 20
    w0 = u32(a); w1 = u32(a + 4)
    print(" id%2d @%08x w0=%08x dmg=%d/%d/%d w1=%08x poise=%d/%d/%d 1d3=%d/%d/%d UP[%d,%d] LV[%d,%d] DN[%d,%d]" % (
        i, a, w0, w0 & 0x3ff, (w0 >> 10) & 0x3ff, (w0 >> 20) & 0x3ff, w1,
        w1 & 7, (w1 >> 3) & 7, (w1 >> 6) & 7, (w1 >> 9) & 0x7f, (w1 >> 16) & 0x7f, (w1 >> 23) & 0x7f,
        s16(a + 8), s16(a + 10), s16(a + 12), s16(a + 14), s16(a + 16), s16(a + 18)))
print("--- Geometrie-Zeiger @0x800A68E8 + item*24 + grp*8 (Gruppe 0 DOWN / 1 LEVEL / 2 UP)")
seen = {}
for item in range(0, 20):
    row = []
    for grp in range(3):
        p = 0x800A68E8 + item * 24 + grp * 8
        row.append((u32(p), u32(p + 4)))
    print(" item%2d: DOWN %08x/%08x LEVEL %08x/%08x UP %08x/%08x" % (item, *row[0], *row[1], *row[2]))
    for grp, (rec, kf) in enumerate(row):
        if rec in seen or rec < t_addr: continue
        seen[rec] = 1
        b = exe[off(rec):off(rec) + 0x1c]
        boxes = [struct.unpack_from("<hhhh", b, 4 + 8 * k) for k in range(3)]
        print("    rec %08x: b0=%02x flags=%02x %02x %02x boxes=%s" % (rec, b[0], b[1], b[2], b[3], boxes))

# ---------------------------------------------------------------------------------------------
# ERWEITERUNG (Phase 3, Thema re-restposten): der Geometrie-Zeiger ist ein PAAR
#   {rec_base, pattern}  (FUN_800410CC: `iVar10 = *param_3 + uVar7 * 0x1c`, uVar7 aus
#   `param_3[1]`).  Das PATTERN ist eine Liste von Paaren {recIdx, count}: pro Aufruf des
#   Appliers (= pro Bild des Angriffs) zaehlt +0x1EC herunter; bei 0 rueckt +0x1ED eine Stelle
#   weiter und laedt die naechste count.  recIdx 0xFF = "kein Ziel in diesem Bild" (der Applier
#   kehrt sofort zurueck), 0xFE = Sprung an den Listenanfang.
#     @0x80041128-30  `lbu v0,492(t0)` (+0x1EC) / `bne v0,zero,0x80041168`
#     @0x80041138-44  `lbu v0,493(t0); addiu v0,v0,1; sb v0,493(t0)`      (+0x1ED = Listenindex++)
#     @0x8004114C-60  `lw v1,4(a2); sll v0,v0,1; addu; lbu v0,1(v0); sb v0,492(t0)`  (count)
#     @0x8004116C-7C  `lbu v0,493(t0); lw v1,4(a2); sll; addu; lbu a1,0(v0)`         (recIdx)
#     @0x80041180-9C  `addiu a0,zero,255; bne a1,a0` -> 0xFF-Zweig: `lbu v1,492; addiu v1,-1;
#                     sb v1,492; j 0x80041aec` mit v0 = 0 = KEIN Ziel in diesem Bild
#     @0x800411A0-E4  `addiu v0,zero,254; bne a1,v0` -> 0xFE-Zweig: `sb zero,493(t0)`, count =
#                     list[1], recIdx = list[0], und bei 0xFF sofort raus
# Das Listenende ist also NICHT 0xFF (das heisst nur "dieses Bild trifft nichts"), sondern eine
# count von 255 (= haelt fuer den Rest des Angriffs) bzw. ein 0xFE-Ruecksprung.
# Damit hat jede Waffe pro Zielhoehe EIN Record-ARRAY, nicht ein Record: das Messer (Item 1)
# faehrt fuenf verschiedene Boxen durch die Klinge, alle Schusswaffen stehen auf Record 0.
def _dump_patterns():
    print("--- Geometrie-Record-ARRAYS + Pattern je Item (Gruppe 0 DOWN / 1 LEVEL / 2 UP)")
    for item in range(0, 20):
        head = []
        for grp in range(3):
            p = 0x800A68E8 + item * 24 + grp * 8
            head.append((u32(p), u32(p + 4)))
        if all(rec < t_addr for rec, _ in head):
            print(" item%2d: (keine gueltigen Zeiger - Tabellenzeile 0 ist Datenwort)" % item)
            continue
        print(" item%2d:" % item)
        for grp, (rec, kf) in enumerate(head):
            pat = []
            o = off(kf)
            for k in range(0, 64, 2):
                a, c = exe[o + k], exe[o + k + 1]
                pat.append((a, c))
                if a == 0xfe or c == 0xff:      # 0xFF in .a ist KEIN Ende (s. Kopf oben)
                    break
            nrec = max([a for a, _ in pat if a < 0xfe] + [0]) + 1
            print("   grp%d rec@%08x pat@%08x = %s" % (
                grp, rec, kf, " ".join("%02x/%d" % (a, c) for a, c in pat)))
            for i in range(nrec):
                a = rec + i * 0x1c
                b = exe[off(a):off(a) + 0x1c]
                boxes = [struct.unpack_from("<hhhh", b, 4 + 8 * k) for k in range(3)]
                print("      [%d] %08x flags=%02x %02x %02x boxes=%s" % (
                    i, a, b[1], b[2], b[3], boxes))

_dump_patterns()
