import sys
sys.path.insert(0, '.')
from re2ss import SS
SN = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/re2/snaps/s%02d.sav"
def angles(s, EMR, kf, nb):
    foff = s.u16(EMR+2); esz = (s.u32(EMR+4) >> 18) * 4
    a = EMR + foff + esz*kf
    hdr = [s.s16(a+2*k) for k in range(6)]
    # 12-bit packed angles: 3 per bone, starting at a+12
    raw = s.bytes(a+12, esz-12)
    bits = int.from_bytes(raw, 'little')
    angs = [(bits >> (12*k)) & 0xfff for k in range(3*nb)]
    return hdr, [tuple(angs[3*b:3*b+3]) for b in range(nb)]
def main():
  pass
for n in [int(a) for a in (sys.argv[1:] if __name__ == "__main__" else [])]:
    s = SS(SN % n)
    print("== s%02d" % n)
    for i in range(1, 5):
        e = s.u32(0x800cfe1c + 4*i)
        EDD = s.u32(e+0x17c); EMR = s.u32(e+0x108); fp = s.u32(e+0x178)
        fw = s.u32(fp); kf = fw & 0xfff
        hdr, ang = angles(s, EMR, kf, 4)
        parts = s.u32(e+0x198)
        ram = [(s.s16(parts+p*172+104)&0xfff, s.s16(parts+p*172+106)&0xfff, s.s16(parts+p*172+108)&0xfff) for p in range(4)]
        print("  arm%d mot %d fr %d EDD %08x EMR %08x fp %08x fw %08x kf %d hdr %s" % (i-1, s.u8(e+0x14c), s.u8(e+0x14d), EDD, EMR, fp, fw, kf, hdr))
        print("     kf-ang  %s" % ang)
        print("     ram-ang %s" % ram)
