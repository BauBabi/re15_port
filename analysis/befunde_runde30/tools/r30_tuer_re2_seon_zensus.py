#!/usr/bin/env python3
"""Runde 30 / Thema tuer-verschlossen.
Zensus ALLER `jal 0x8005ba28` (RE2 Se_on) in info/re2leon/PSX.EXE.
Fuer jede Aufrufstelle wird der a0-Wert rueckwaerts gelesen (lui a0 / ori a0 / addiu a0)
innerhalb der letzten 12 Instruktionen + Delay-Slot. Ausgabe: Adresse, a0 (falls literal).
Kein Raten: ist a0 nicht literal, steht 'a0=?' da.
"""
import struct, sys
EXE = sys.argv[2] if len(sys.argv) > 2 else 'info/re2leon/PSX.EXE'
d = open(EXE, 'rb').read()
T_ADDR = struct.unpack_from('<I', d, 0x18)[0]
T_SIZE = struct.unpack_from('<I', d, 0x1c)[0]
def off(a): return 0x800 + a - T_ADDR
def word(a): return struct.unpack_from('<I', d, off(a))[0]
TARGET = int(sys.argv[1], 16) if len(sys.argv) > 1 else 0x8005ba28
JAL = 0x0C000000 | ((TARGET >> 2) & 0x03FFFFFF)
hits = []
for a in range(T_ADDR, T_ADDR + T_SIZE, 4):
    if word(a) == JAL:
        hits.append(a)
print('# jal 0x%08x = Wort 0x%08x, %d Aufrufstellen' % (TARGET, JAL, len(hits)))
def lit_a0(a):
    # Delay-Slot zuerst, dann rueckwaerts
    val = None; hi = None; src = []
    seq = [a + 4] + [a - 4 * i for i in range(1, 14)]
    lo_done = False
    for p in seq:
        w = word(p)
        op = w >> 26; rs = (w >> 21) & 31; rt = (w >> 16) & 31; imm = w & 0xffff
        if op == 0x0f and rt == 4:  # lui a0
            hi = imm << 16
            src.append('lui@%08x' % p)
            return (hi | (val or 0)), src
        if op == 0x0d and rt == 4 and rs == 4 and not lo_done:  # ori a0,a0,imm
            val = imm; lo_done = True; src.append('ori@%08x' % p)
            continue
        if op == 0x0d and rt == 4 and rs == 0:  # ori a0,zero,imm
            src.append('ori0@%08x' % p); return imm, src
        if op == 0x09 and rt == 4 and rs == 0:  # addiu a0,zero,imm
            src.append('addiu0@%08x' % p); return imm, src
        # andere Schreiber auf a0 -> nicht literal
        if op == 0 and ((w >> 11) & 31) == 4 and (w & 0x3f) in (0x21, 0x25, 0x20, 0x23, 0x24):
            src.append('reg@%08x' % p); return None, src
        if op in (0x23, 0x21, 0x25, 0x24, 0x20) and rt == 4:
            src.append('load@%08x' % p); return None, src
        if p != a + 4 and (op == 3 or (op == 0 and (w & 0x3f) in (8, 9))):
            break
    return None, src
for a in hits:
    v, src = lit_a0(a)
    if v is None:
        print('0x%08x  a0=?            %s' % (a, ','.join(src)))
    else:
        print('0x%08x  a0=0x%08x  bank=%d id=0x%02x low=0x%04x  %s' % (a, v, v >> 24, (v >> 16) & 0xff, v & 0xffff, ','.join(src)))
