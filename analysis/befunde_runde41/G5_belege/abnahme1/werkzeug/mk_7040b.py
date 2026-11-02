"""mk_7040b.py IN OUT X DIR CUT : Tuer 1 der Halle (ROOM2000 INIT @0x02558, im RAM @0x8011cbb4; AOT-Tabelle 0x800ce55c
zeigt auf Befehl+2) auf Stage 6 Raum 4 umbiegen und ihr Rechteck auf den ganzen Raum legen; Flag 4:0x89 setzen
(Tabelle @0x800a78c8[4] = 0x800d4854, Wort +16, Maske 0x80000000>>9). Ausloesen dann echt per Kreuz (Aktion)."""
import sys
sys.path.insert(0, '.')
from re2ss import SS
inp, out = sys.argv[1], sys.argv[2]
x = int(sys.argv[3]); dr = int(sys.argv[4], 0); cut = int(sys.argv[5])
s = SS(inp)
INS = 0x8011cbb4
assert s.u8(INS) == 0x3b and s.u8(INS + 1) == 1
assert s.u32(0x800ce55c) == INS + 2
print('alt ', s.bytes(INS, 32).hex(' '))
s.w8(INS + 4, 1); s.w16(INS + 6, -16000); s.w16(INS + 8, -6000); s.w16(INS + 10, 8000); s.w16(INS + 12, 8000)
s.w16(INS + 14, x); s.w16(INS + 16, 0); s.w16(INS + 18, -23570); s.w16(INS + 20, dr)
s.w8(INS + 22, 6); s.w8(INS + 23, 4); s.w8(INS + 24, cut); s.w8(INS + 25, 0)
s.w8(INS + 26, 0x2a); s.w8(INS + 27, 0x02); s.w8(INS + 28, 0); s.w8(INS + 29, 0); s.w8(INS + 30, 0); s.w8(INS + 31, 0)
print('neu ', s.bytes(INS, 32).hex(' '))
s.w32(0x800d4864, s.u32(0x800d4864) | 0x00400000)
s.save(out)
print('ok', out)
