#!/usr/bin/env python3
"""re2_hp_minus1.py — sucht in info/re2leon/PSX.EXE und info/re2leon/COMMON/BIN/STAGE*.BIN nach
`addiu rX,zero,-1` gefolgt (<=6 Instr.) von `sh rX,342(rY)` (+0x156 = RE2-HP := -1) und nach
`sh zero,342`. RAM-Adresse: EXE t_addr+off-0x800, Overlays @0x80100000 (RE2-Stages laden dort)."""
import struct, os, glob
root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
def scan(path, base, hdr):
    d = open(path, 'rb').read()
    n = (len(d) - hdr) // 4
    W = [struct.unpack_from('<I', d, hdr + 4*i)[0] for i in range(n)]
    out = []
    for i, w in enumerate(W):
        if (w >> 26) == 0x29 and (w & 0xffff) == 342:
            rt = (w >> 16) & 31
            for j in range(1, 7):
                if i - j < 0: break
                p = W[i - j]
                if (p >> 26) == 0x09 and ((p >> 21) & 31) == 0 and ((p >> 16) & 31) == rt:
                    if (p & 0xffff) == 0xffff: out.append(base + 4*i)
                    break
    return out
exe = os.path.join(root, 'info', 're2leon', 'PSX.EXE')
t = struct.unpack_from('<I', open(exe, 'rb').read(), 0x18)[0]
print('PSX.EXE:', ' '.join('0x%08x' % a for a in scan(exe, t, 0x800)))
for p in sorted(glob.glob(os.path.join(root, 'info', 're2leon', 'COMMON', 'BIN', 'STAGE*.BIN'))):
    print(os.path.basename(p) + ':', ' '.join('0x%08x' % a for a in scan(p, 0x80100000, 0)))
