import sys
sys.path.insert(0, '.')
from re2ss import SS
SN = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/re2/snaps/s%02d.sav"
s = SS(SN % 4)
EDD = 0x8014ffb0
n = (s.u32(EDD) >> 16) // 4
for c in range(n):
    w = s.u32(EDD + 4*c); cnt = w & 0xffff; off = w >> 16
    f0 = s.u32(EDD+off) & 0xfff; fl = s.u32(EDD+off+4*(cnt-1)) & 0xfff
    nxt = s.u32(EDD+off+4*cnt)
    print("clip %2d cnt %3d off 0x%04x kf %d..%d  entry[cnt]=%08x" % (c, cnt, off, f0, fl, nxt))
