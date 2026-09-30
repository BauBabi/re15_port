#!/usr/bin/env python3
"""Gegenpruefung R34: sucht Stores durch einen Zeiger, der per `lw rt,<off>(rs)` geladen wurde
(z.B. off=124 = Aktor+0x7c Hitbox-Versatz). Meldet lw + jeden s[bhw] ..(rt) in den naechsten N Instruktionen,
solange rt nicht ueberschrieben wird. Aufruf: ptr_store_gegen.py 124 [N]"""
import struct, sys, os, glob
REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
R = ["zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7",
     "s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","fp","ra"]
ST = {0x28:'sb',0x29:'sh',0x2b:'sw',0x2a:'swl',0x2e:'swr'}
def bins():
    d = open(os.path.join(REPO, 'info/Re1.5/PSX.EXE'), 'rb').read()
    t_addr, t_size = struct.unpack_from('<II', d, 0x18)
    yield 'PSX.EXE', t_addr, d[0x800:0x800 + t_size]
    for p in sorted(glob.glob(os.path.join(REPO, 'info/Re1.5/PSX/BIN/*.BIN'))):
        yield os.path.basename(p), 0x80100000, open(p, 'rb').read()
off = int(sys.argv[1], 0); N = int(sys.argv[2]) if len(sys.argv) > 2 else 12
for name, base, data in bins():
    n = len(data) // 4
    W = [struct.unpack_from('<I', data, i*4)[0] for i in range(n)]
    for i, w in enumerate(W):
        if w >> 26 == 0x23 and (w & 0xffff) == off:
            rt = (w >> 16) & 31; rs = (w >> 21) & 31
            for j in range(i+1, min(n, i+1+N)):
                x = W[j]; op = x >> 26
                if op in ST and ((x >> 21) & 31) == rt:
                    imm = x & 0xffff; imm = imm - 0x10000 if imm & 0x8000 else imm
                    print('%-11s %08x lw %s,%d(%s) -> %08x %s %s,%d(%s)' % (name, base+i*4, R[rt], off, R[rs], base+j*4, ST[op], R[(x>>16)&31], imm, R[rt]))
                # clobber
                if op == 0 and ((x >> 11) & 31) == rt: break
                if op in (0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x20,0x21,0x23,0x24,0x25) and ((x >> 16) & 31) == rt: break
