import struct, os, json

def depack(src, spos, outlen):
    """byte-true port of FUN_800c47e8 @0x800C47E8 (DEBUG.BIN +0x47E8)"""
    out = bytearray(outlen)
    t3 = 0          # src cursor
    t4 = 0          # flag mask
    t0 = 0          # dst cursor
    t5 = 0          # flag byte
    if outlen <= 0:
        return bytes(out)
    while True:
        if t4 == 0:                       # 0x47f8 bne t4,zero
            t4 = 0x80                     # 0x4800
            t5 = src[spos+t3]             # 0x4804
            t3 += 1                       # 0x4808
        a3 = src[spos+t3]                 # 0x4810
        t3 += 1                           # 0x4820 delay slot
        if (a3 & 0x80) == 0:              # 0x4818/0x481c
            out[t0] = a3; t0 += 1         # 0x4828/0x4830
            # j 0x48e4 -> NO flag shift, NO andi
        else:
            v1 = (a3 << 24) & 0xffffffff  # 0x483c
            if (t5 & t4) == 0:            # 0x4834/0x4838
                out[t0] = a3; t0 += 1     # 0x4844/0x4848
                t4 >>= 1                  # 0x4850  (j 0x48e0 -> andi)
                t4 &= 0xffff
            else:
                a3 = src[spos+t3]         # 0x4858
                v0 = ((a3 << 16) | v1) & 0xffffffff   # 0x4860/0x4864
                t2 = (v0 >> 20) if v0 < 0x80000000 else ((v0 >> 20) - (1<<12))  # sra 20
                t3 += 1                   # 0x4874
                if (a3 & 0x0f) != 0:      # 0x486c/0x4870
                    t1 = (a3 & 0x0f) + 2  # 0x48a4
                else:
                    a3 = src[spos+t3]     # 0x487c
                    t3 += 1               # 0x4880
                    t1 = (a3 & 0x3f) + 3  # 0x4884/0x4888
                    v0 = ((a3 >> 6) | ((t2 << 2) & 0xffffffff)) & 0xffffffff  # 0x488c/90/94
                    v0 = (v0 << 16) & 0xffffffff                              # 0x4898
                    t2 = (v0 >> 16) if v0 < 0x80000000 else ((v0 >> 16) - (1<<16))  # sra 16
                t4 >>= 1                  # 0x48a8
                srcp = t0 + t2            # 0x48ac
                tend = t0 + t1            # 0x48b4
                if t1 != 0:
                    for _ in range(t1):
                        out[t0] = out[srcp]
                        srcp += 1; t0 += 1
                t0 = tend                 # 0x48dc
                t4 &= 0xffff              # 0x48e0
        if not (t0 < outlen):             # 0x48e4 slt / 0x48e8 bne
            break
    return bytes(out)

STAGE_TBL = {1:(0x1EAE4,), 2:(0x1713C,), 3:(0x1D0F8,), 4:(0x18590,), 5:(0x1DF18,), 6:(0x01E3C,)}
ROOT = os.path.join(os.path.dirname(__file__), '..', '..', '..')
PSX  = os.path.join(ROOT, 'info', 'Re1.5', 'PSX')

def load_tbl(st):
    d = open(os.path.join(PSX,'BIN','STAGE%d.BIN'%st),'rb').read()
    o = STAGE_TBL[st][0]
    return d, o
