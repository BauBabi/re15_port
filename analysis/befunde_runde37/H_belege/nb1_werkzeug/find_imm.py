# Sucht in einem STAGE*.BIN alle Instruktionen mit 16-Bit-Immediate == lo, deren Basisregister zuvor per lui hi gesetzt wurde
# (einfacher Vorwaerts-Tracker je Funktion), sowie rohe Worte == ziel.
import struct, sys
binp, ziel = sys.argv[1], int(sys.argv[2], 16)
d = open(binp, 'rb').read()
BASE = 0x80100000
n = len(d)//4
hi = {}
hits = []
for i in range(n):
    w = struct.unpack_from('<I', d, i*4)[0]
    a = BASE + i*4
    op = w >> 26; rs = (w >> 21) & 31; rt = (w >> 16) & 31; imm = w & 0xffff
    simm = imm - 0x10000 if imm & 0x8000 else imm
    if op == 0xf:
        hi[rt] = (imm << 16, a)
        continue
    if op in (9, 0x20, 0x21, 0x23, 0x24, 0x25, 0x28, 0x29, 0x2b) and rs in hi:
        v = (hi[rs][0] + simm) & 0xffffffff
        if v == ziel:
            hits.append((a, w, op))
    if w == ziel:
        hits.append((a, w, 'WORT'))
print(f"{binp}: Laenge 0x{len(d):x} -> Ende 0x{BASE+len(d):08x}; Ziel 0x{ziel:08x}")
for a, w, op in hits:
    print(f"  0x{a:08x}: 0x{w:08x} op={op}")
