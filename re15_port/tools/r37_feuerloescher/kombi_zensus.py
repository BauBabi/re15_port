# Zensus (Runde 37 Spur I, Nachbesserung 1): kann Item 0x31 (Feuerloescher) in RE1.5 kombiniert und dadurch
# aus dem Inventar genommen werden? Aufruf: python3 kombi_zensus.py info/Re1.5/PSX.EXE
import struct, sys
exe = open(sys.argv[1], 'rb').read()
T_ADDR = struct.unpack_from('<I', exe, 0x18)[0]   # PS-X-EXE-Kopf: t_addr @0x18, Text ab Datei 0x800
def off(a): return a - T_ADDR + 0x800
def u8(a): return exe[off(a)]
def u32(a): return struct.unpack_from('<I', exe, off(a))[0]
T = 0x80074DA8
hits = []
for ida in range(0x00, 0x80):
    row = T + ida * 12
    if off(row) + 12 > len(exe): break
    cnt = u8(row + 9); pl = u32(row + 4)
    if cnt == 0 or not (0x80010000 <= pl < 0x80010000 + len(exe) - 0x800): continue
    for i in range(cnt):
        p, r, a, pic = (u8(pl + i*4 + k) for k in range(4))
        if ida == 0x31 or p == 0x31 or r == 0x31:
            hits.append((ida, p, r, a, pic, pl + i*4))
row = T + 0x31 * 12
print('t_addr 0x%08X' % T_ADDR)
print('Zeile 0x31 @0x%08X: bytes %s  count=%d ptr=0x%08X' % (row, exe[off(row):off(row)+12].hex(' '), u8(row+9), u32(row+4)))
print('Paare mit 0x31 (als A, Partner oder Ergebnis):', ['A=%02x partner=%02x result=%02x action=%d pic=%d @0x%08X' % h for h in hits] or 'KEINE')
