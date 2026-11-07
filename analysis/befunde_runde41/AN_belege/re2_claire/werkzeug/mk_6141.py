"""mk_6141.py IN OUT [AOT] : RE2 Claire PAL (SLES_109.72). Im ersten Raum (ROOM1001, Strasse) die Tuer-AOT AOT (Default 0)
auf Stage 6 Raum 0x14 (ROOM6141, Annette/William) umbiegen und ihr Rechteck um Claire legen; Flag (0x22,0x2B) setzen.
Belege (Adressen der PAL-EXE, siehe zeitachse.md Abschnitt Weg):
  AOT-Tabelle 0x800c6130 (Aot_set-Handler @0x80051080 `3c 03 80 0c` / @0x80051084 `24 63 61 30`; Eintrag = Befehl+2)
  Spieler-Zeiger 0x800c79a8 (Work_set-Fall 1 @0x80051cc0 `3c 02 80 0c` / @0x80051cc4 `8c 42 79 a8`), Lage +0x38/+0x40
  Ck-Tabelle 0x8009f8e4 (Ck @0x8004e164 `3c 01 80 0a` / @0x8004e16c `8c 23 f8 e4`), [0x22] = 0x800cc470 (EXE-Daten)
    Flag (g,b): Wort = tab[g] + ((b>>3)&0x1c), Maske 0x80000000>>(b&31)  -> (0x22,0x2B): 0x800cc474 | 0x00100000
Tuer-Bytes (Ziel, Lage, Richtung, Schnitt, Tuerbild) = die echte Tuer ROOM6151 -> ROOM6141
  (PL1/RDT/ROOM6151.RDT INIT sub00 @0x02B96 `3b 00 01 31 00 00 20 93 0f ca 08 07 9a 10 1b b0 00 00 84 a2 14 f8 05 14 06 00 26 00 ..`).
Ausloesen dann echt per Kreuz (Aktion)."""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from re2c_ss import SS
inp, out = sys.argv[1], sys.argv[2]
k = int(sys.argv[3]) if len(sys.argv) > 3 else 0
s = SS(inp)
ptr = s.u32(0x800c6130 + 4 * k)
INS = ptr - 2
assert s.u8(INS) == 0x3b and s.u8(INS + 1) == k, s.bytes(INS, 4).hex()
pl = s.u32(0x800c79a8)
px, py, pz = s.s32(pl + 0x38), s.s32(pl + 0x3c), s.s32(pl + 0x40)
print('Claire', px, py, pz, 'Spieler-Zeiger %08x' % pl)
print('alt ', s.bytes(INS, 32).hex(' '))
s.w8(INS + 4, 0)
s.w16(INS + 6, px - 3000); s.w16(INS + 8, pz - 3000); s.w16(INS + 10, 6000); s.w16(INS + 12, 6000)
# Datei-Bytes ab +14: 1b b0 (x=-20453) 00 00 (y=0) 84 a2 (z=-23932) 14 f8 (Richtung) 05 (Stage 6) 14 (Raum) 06 (Schnitt)
#                    00 (Stockwerk) 26 (Tuerbild) 00 (Tuertyp) 00 00 00 00 (Klopfen/Schluessel)
door =bytes.fromhex("1b b0 00 00 84 a2 14 f8 05 14 06 00 26 00 00 00 00 00")
s.wbytes(INS + 14, door)
print('neu ', s.bytes(INS, 32).hex(' '))
w = s.u32(0x800cc474)
s.w32(0x800cc474, w | 0x00100000)
print('Flag (0x22,0x2B): 0x800cc474 %08x -> %08x' % (w, s.u32(0x800cc474)))
print('Flag (1,1) Wort 0x800c776c = %08x (Maske 0x40000000 -> %d)' % (s.u32(0x800c776c), (s.u32(0x800c776c) >> 30) & 1))
print('Flag (3,0x42) Wort 0x800cc3d0 = %08x (Maske 0x20000000 -> %d)' % (s.u32(0x800cc3d0), (s.u32(0x800cc3d0) >> 29) & 1))
if "--weg" in sys.argv:
    # nur Startraum: Zombies (Gegner-Zeiger 0x800c79b0[i], Work_set-Fall 3 @0x80051ce4..ec `3c 01 80 0c` / `8c 22 79 b0`)
    # an das Westende der Strasse setzen, damit Claire bis zur Tuer nicht gepackt wird. Betrifft ROOM1001, nicht 6141.
    for i in range(8):
        e = s.u32(0x800c79b0 + 4 * i)
        if e and e != 0x800cbde0 and s.u8(e + 8) == 0x1f:
            print('  em%d %08x (%d,%d) -> (-15000,%d)' % (i, e, s.s32(e + 0x38), s.s32(e + 0x40), s.s32(e + 0x40)))
            s.w32(e + 0x38, -15000)
s.save(out)
print('ok', out)
