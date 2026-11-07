# Abnahme 3 Spur W: Bytevergleich Mod-EXE (info/Re1.5/PSX.EXE) gegen Version_40%/PSX.EXE in den Bereichen, die ein Unterprogramm starten.
import struct, sys
A = open(sys.argv[1],'rb').read(); B = open(sys.argv[2],'rb').read()
def off(d, a): return 0x800 + a - struct.unpack_from('<I', d, 0x18)[0]
BEREICHE = [
 ('FUN_8003ee3c Thread-Start', 0x8003ee3c, 0x8003ef6c),
 ('FUN_8003ef6c Raum-Init (main00/sub00)', 0x8003ef6c, 0x8003f038),
 ('FUN_8003f038 sub01 je Bild', 0x8003f038, 0x8003f0a0),
 ('Opcode 0x04 Evt_exec LAB_8003f2a0', 0x8003f2a0, 0x8003f2d0),
 ('AOT sce 3 LAB_800430f0', 0x800430f0, 0x80043120),
 ('Opcode-Tabelle 0x800744a8 (95 Eintraege)', 0x800744a8, 0x80074624),
 ('AOT-Typ-Tabelle 0x8007469c (16 Eintraege)', 0x8007469c, 0x800746dc),
 ('Aufrufer @0x8004c720..@0x8004c740', 0x8004c720, 0x8004c740),
 ('Aufrufer @0x8004cd00..@0x8004cd20', 0x8004cd00, 0x8004cd20),
 ('Aufrufer @0x8004d740..@0x8004d7b8', 0x8004d740, 0x8004d7b8),
]
for name, a0, a1 in BEREICHE:
    x = A[off(A,a0):off(A,a1)]; y = B[off(B,a0):off(B,a1)]
    diff = [a0 + i for i in range(len(x)) if x[i] != y[i]]
    print('%-45s @0x%08X..@0x%08X  %4d Bytes  %s' % (name, a0, a1, len(x), 'GLEICH' if not diff and len(x) == len(y) else 'ABWEICHUNG %d Bytes ab @0x%08X' % (len(diff), diff[0])))
# Opcode 0x04 und 0x18 Eintraege
for op in (0x04, 0x18):
    a = 0x800744a8 + 4*op
    print('Opcode 0x%02X: Tabelle @0x%08X -> Mod 0x%08X, 40%% 0x%08X' % (op, a, struct.unpack_from('<I',A,off(A,a))[0], struct.unpack_from('<I',B,off(B,a))[0]))
a = 0x8007469c + 4*3
print('AOT sce 3: Tabelle @0x%08X -> Mod 0x%08X, 40%% 0x%08X' % (a, struct.unpack_from('<I',A,off(A,a))[0], struct.unpack_from('<I',B,off(B,a))[0]))
# Evt_exec-Handler roh
x = A[off(A,0x8003f2a0):off(A,0x8003f2d0)]
for i in range(0, len(x), 4):
    print('  @0x%08X  %s' % (0x8003f2a0+i, ' '.join('%02x'%b for b in x[i:i+4])))
