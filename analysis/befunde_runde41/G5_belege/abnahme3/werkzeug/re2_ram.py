# Abnahme 3: RE2-RAM selbst lesen (Savestates s01..s26 der Abnahme 1): G5 Slot 0, Teil 2 Weltlage, Morph-Gewichte, Vertexpuffer Mesh 2
import sys, struct, zstandard
EXE = r"C:\workspace\git\reAi_v2\info\re2leon\SLUS_007.48"
SN = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/re2/snaps/s%02d.sav"
class SS:
    def __init__(s, p):
        d = open(p, "rb").read(); off = struct.unpack_from("<I", d, 0xd4)[0]
        s.b = zstandard.ZstdDecompressor().stream_reader(d[off:]).read()
        sig = open(EXE, "rb").read()[0x800:0x830]; pos = s.b.find(sig); assert pos >= 0; s.base = pos - 0x10000
    def o(s, a): return s.base + (a & 0x1fffff)
    def u8(s, a): return s.b[s.o(a)]
    def u16(s, a): return struct.unpack_from("<H", s.b, s.o(a))[0]
    def s16(s, a): return struct.unpack_from("<h", s.b, s.o(a))[0]
    def u32(s, a): return struct.unpack_from("<I", s.b, s.o(a))[0]
    def s32(s, a): return struct.unpack_from("<i", s.b, s.o(a))[0]
if __name__ == "__main__":
    for n in [int(a) for a in sys.argv[1:]]:
        s = SS(SN % n); g5 = s.u32(0x800cfe1c); parts = s.u32(g5 + 0x198); p2 = parts + 2 * 172
        w = (s.s32(p2 + 92), s.s32(p2 + 96), s.s32(p2 + 100)); ang = (s.s16(p2 + 104), s.s16(p2 + 106), s.s16(p2 + 108))
        rec = s.u32(p2 + 136); gw = [s.s16(rec + 12 + 4 * i) for i in range(4)]
        vb = [(s.s16(0x80132be0 + 8 * i), s.s16(0x80132be0 + 8 * i + 2), s.s16(0x80132be0 + 8 * i + 4)) for i in range(635)]
        ys = [v[1] for v in vb]
        print("s%02d Schnitt %d G5 x %d | Teil2 welt %s ang %s | Gewichte %s | VB y %d..%d x %d..%d z %d..%d" % (n, s.u16(0x800d4820), s.s32(g5 + 0x38), w, ang, gw,
              min(ys), max(ys), min(v[0] for v in vb), max(v[0] for v in vb), min(v[2] for v in vb), max(v[2] for v in vb)))
        with open("re2ram_v_s%02d.txt" % n, "w") as f:
            f.write("635 %d %d %d %d\n" % tuple(gw)); f.write("%d %d %d\n" % w)
            for v in vb: f.write("%d %d %d\n" % v)
