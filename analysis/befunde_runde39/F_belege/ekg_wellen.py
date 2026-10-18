import struct, sys
def lade(p):
    d = open(p,'rb').read()
    t_addr = struct.unpack_from('<I', d, 0x18)[0]
    t_size = struct.unpack_from('<I', d, 0x1c)[0]
    print(p, 't_addr=%08x t_size=%x' % (t_addr, t_size))
    return d, t_addr
def rd(d, t, addr, n):
    o = 0x800 + addr - t
    return d[o:o+n]
# RE1.5
d15, t15 = lade(r'C:/workspace/git/reAi_v2/.claude/worktrees/r39_zustand/info/Re1.5/PSX.EXE')
ptr = [struct.unpack_from('<I', rd(d15, t15, 0x80076214 + 4*i, 4))[0] for i in range(6)]
print('RE1.5 Wellen-Zeiger @0x80076214:', ['%08x' % p for p in ptr])
tabs15 = [rd(d15, t15, p, 0x1b8) for p in ptr]
for i in range(6):
    for j in range(i+1, 6):
        if tabs15[i] == tabs15[j]: print('  RE1.5 Tabelle %d == Tabelle %d (bytegleich)' % (i, j))
for i in (0, 3):
    e = [struct.unpack_from('<hh', tabs15[i], 4*k) for k in range(110)]
    print('  RE1.5 Tabelle %d (idx 0x20..0x6c, {y,h}):' % i, e[0x20:0x6d])
print('  Farben @0x8007622c:', rd(d15, t15, 0x8007622c, 24).hex(' '))
# RE2 Leon
d2, t2 = lade(r'C:/workspace/git/reAi_v2/.claude/worktrees/r39_zustand/info/re2leon/SLUS_007.48')
ptr2 = [struct.unpack_from('<I', rd(d2, t2, 0x800ab180 + 4*i, 4))[0] for i in range(5)]
print('RE2 Wellen-Zeiger @0x800ab180:', ['%08x' % p for p in ptr2])
tabs2 = [rd(d2, t2, p, 0xa0) for p in ptr2]
for i in range(5):
    for j in range(i+1, 5):
        if tabs2[i] == tabs2[j]: print('  RE2 Tabelle %d == Tabelle %d (bytegleich)' % (i, j))
for i in range(5):
    e = [(tabs2[i][2*k], tabs2[i][2*k+1]) for k in range(80)]
    print('  RE2 Tabelle %d @%08x ({y,h} u8):' % (i, ptr2[i]), e)
print('  RE2 Farben @0x800ab160:', rd(d2, t2, 0x800ab160, 30).hex(' '))
print('  RE2 Tabelle 4 roh:', tabs2[4].hex(' '))
