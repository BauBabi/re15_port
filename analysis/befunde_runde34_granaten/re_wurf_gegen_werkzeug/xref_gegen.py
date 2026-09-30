#!/usr/bin/env python3
"""Gegenpruefung R34: unabhaengiger Xref-Scanner (lui + imm16) ueber PSX.EXE und alle BIN-Overlays.
Verfolgt je Register den letzten lui-Wert (zurueckgesetzt bei jeder anderen Schreibung auf das Register
und an Sprungzielen nicht - konservativ: Fenster 64 Instruktionen). Meldet jede Instruktion
(Load/Store/addiu/ori), deren effektive Adresse == Ziel (oder im Bereich [Ziel, Ziel+span)).
Aufruf: xref_gegen.py 0x800b5358 [span]
"""
import struct, sys, os, glob
REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
def bins():
    d = open(os.path.join(REPO, 'info/Re1.5/PSX.EXE'), 'rb').read()
    t_addr, t_size = struct.unpack_from('<II', d, 0x18)
    yield 'PSX.EXE', t_addr, d[0x800:0x800 + t_size]
    for p in sorted(glob.glob(os.path.join(REPO, 'info/Re1.5/PSX/BIN/*.BIN'))):
        yield os.path.basename(p), 0x80100000, open(p, 'rb').read()
R = ["zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7",
     "s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","fp","ra"]
OPN = {0x20:'lb',0x21:'lh',0x23:'lw',0x24:'lbu',0x25:'lhu',0x28:'sb',0x29:'sh',0x2b:'sw',0x22:'lwl',0x26:'lwr',0x2a:'swl',0x2e:'swr',0x09:'addiu',0x0d:'ori'}
def main():
    tgt = int(sys.argv[1], 16); span = int(sys.argv[2], 0) if len(sys.argv) > 2 else 1
    for name, base, data in bins():
        n = len(data) // 4
        luiv = {}  # reg -> (value, index)
        for i in range(n):
            w = struct.unpack_from('<I', data, i * 4)[0]
            op = w >> 26; rs = (w >> 21) & 31; rt = (w >> 16) & 31; imm = w & 0xffff
            simm = imm - 0x10000 if imm & 0x8000 else imm
            if op == 0x0f:
                luiv[rt] = (imm << 16, i); continue
            if op in OPN and rs in luiv and i - luiv[rs][1] <= 64:
                hi = luiv[rs][0]
                ea = (hi | imm) if op == 0x0d else (hi + simm) & 0xffffffff
                if tgt <= ea < tgt + span:
                    print('%-11s %08x  %-5s %s,%d(%s)  -> %08x' % (name, base + i * 4, OPN[op], R[rt], simm, R[rs], ea))
            # clobber tracking: any instruction writing rt/rd kills lui value
            if op == 0:
                rd = (w >> 11) & 31
                if rd in luiv: del luiv[rd]
            elif op in (0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x20,0x21,0x23,0x24,0x25,0x22,0x26):
                if rt in luiv and not (op in OPN and OPN[op] in ('addiu','ori') and False): 
                    if rt != rs or op not in (0x09,0x0d): del luiv[rt]
                    else: del luiv[rt]
main()
