# -*- coding: utf-8 -*-
"""Runde 35 Spur A (Nachbesserung 1) — alle `jal <ziel>` in einer PSX-EXE / einem Overlay finden und
die 8 Instruktionen davor + Delay-Slot als Rohworte ausgeben (Argument-Ladungen a1/a2/a3).

    python jal_scan.py <datei> <ladeadresse hex> <kopf bytes> <ziel hex> [ziel2 ...]
    EXE:     jal_scan.py info/re2leon/PSX.EXE 0x80010000 0x800 0x8004fba0
    Overlay: jal_scan.py info/Re1.5/PSX/BIN/STAGE1.BIN 0x80100000 0 0x8003dcc4
"""
import struct, sys
REG = ['zero','at','v0','v1','a0','a1','a2','a3','t0','t1','t2','t3','t4','t5','t6','t7',
       's0','s1','s2','s3','s4','s5','s6','s7','t8','t9','k0','k1','gp','sp','fp','ra']
def kurz(w):
    op = w >> 26; rs = (w >> 21) & 31; rt = (w >> 16) & 31; rd = (w >> 11) & 31; imm = w & 0xffff
    simm = imm - 0x10000 if imm & 0x8000 else imm
    if op == 9:  return 'addiu %s,%s,%d' % (REG[rt], REG[rs], simm)
    if op == 13: return 'ori %s,%s,0x%x' % (REG[rt], REG[rs], imm)
    if op == 15: return 'lui %s,0x%x' % (REG[rt], imm)
    if op == 0 and (w & 0x3f) == 0x21: return 'addu %s,%s,%s' % (REG[rd], REG[rs], REG[rt])
    if op == 0x23: return 'lw %s,%d(%s)' % (REG[rt], simm, REG[rs])
    if op == 0x21: return 'lh %s,%d(%s)' % (REG[rt], simm, REG[rs])
    if op == 0x25: return 'lhu %s,%d(%s)' % (REG[rt], simm, REG[rs])
    if op == 0x24: return 'lbu %s,%d(%s)' % (REG[rt], simm, REG[rs])
    if op == 3: return 'jal 0x%08x' % (0x80000000 | ((w & 0x3ffffff) << 2))
    return '.%08x' % w
def main():
    pfad, base, kopf = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3], 16)
    ziele = [int(a, 16) for a in sys.argv[4:]]
    d = open(pfad, 'rb').read()[kopf:]
    n = len(d) // 4
    ws = struct.unpack_from('<%dI' % n, d, 0)
    for z in ziele:
        code = (3 << 26) | ((z & 0x0fffffff) >> 2)
        for i, w in enumerate(ws):
            if w != code: continue
            print('--- jal 0x%08x @0x%08x' % (z, base + 4 * i))
            for k in range(max(0, i - 8), min(n, i + 2)):
                r = kurz(ws[k])
                if any(t in r for t in ('a1', 'a2', 'a3', 'jal')):
                    print('   %08x: %s' % (base + 4 * k, r))
if __name__ == '__main__':
    main()
