# Abnahme 4 G5A: eigener RE2-RAM-Leser (DuckStation-Savestate, zstd ab der ersten Magie 28 B5 2F FD; RAM-Basis ueber
# SLUS_007.48[0x800:0x840] = Text bei 0x80010000). Liest G5 und die vier Arme je Savestate.
import sys, struct, zstandard, json
EXE = r"C:\workspace\git\reAi_v2\info\re2leon\SLUS_007.48"
SN = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/re2/snaps/s%02d.sav"
SIG = open(EXE, "rb").read()[0x800:0x840]
class Ram:
    def __init__(s, pfad):
        d = open(pfad, "rb").read()
        k = d.find(b"\x28\xb5\x2f\xfd"); assert k > 0
        s.b = zstandard.ZstdDecompressor().stream_reader(d[k:]).read()
        p = s.b.find(SIG); assert p >= 0 and s.b.find(SIG, p + 1) < 0
        s.base = p - 0x10000
    def _o(s, a): return s.base + (a & 0x1fffff)
    def u8(s, a): return s.b[s._o(a)]
    def s16(s, a): return struct.unpack_from("<h", s.b, s._o(a))[0]
    def u32(s, a): return struct.unpack_from("<I", s.b, s._o(a))[0]
    def s32(s, a): return struct.unpack_from("<i", s.b, s._o(a))[0]
def w12(v):
    v &= 0xfff
    return v - 4096 if v >= 2048 else v
def lese(n):
    r = Ram(SN % n)
    g5 = r.u32(0x800cfe1c)
    out = {"s": n, "g5": {"adr": g5, "w4": r.u32(g5 + 4), "sub": r.u8(g5 + 5), "ph": r.u8(g5 + 6), "t": r.s16(g5 + 0x158),
                          "clip": r.u8(g5 + 0x14c), "bild": r.u8(g5 + 0x14d), "x": r.s32(g5 + 0x38), "z": r.s32(g5 + 0x40)},
           "pl_x": r.s32(r.u32(0x800cfe14) + 0x38), "pl_z": r.s32(r.u32(0x800cfe14) + 0x40), "arme": {}, "slots3c": []}
    for k in range(4):
        out["slots3c"].append(r.u32(0x800d3c38 + 4 * k))
    for i in range(1, 5):
        e = r.u32(0x800cfe1c + 4 * i)
        if e < 0x80000000: continue
        idx = r.u8(e + 0x218); teil0 = r.u32(e + 0x198)
        out["arme"][idx] = {"slot": i, "adr": e, "w4": r.u32(e + 4), "sub": r.u8(e + 5), "ph": r.u8(e + 6),
                            "sc": r.s16(teil0 + 0x8c), "t": r.s16(e + 0x158),
                            "rx": w12(r.s16(e + 0x74)), "ry": w12(r.s16(e + 0x76)), "rz": w12(r.s16(e + 0x78)),
                            "x": r.s32(e + 0x38), "y": r.s32(e + 0x3c), "z": r.s32(e + 0x40),
                            "rx_roh": r.s16(e + 0x74), "rz_roh": r.s16(e + 0x78)}
    return out
if __name__ == "__main__":
    alle = [lese(n) for n in range(1, 27)]
    json.dump(alle, open(sys.argv[1], "w"), indent=1)
    for o in alle:
        g = o["g5"]
        print("s%02d G5 w4 %08x sub %d ph %d t %d clip %d bild %d x %d | Leon x %d z %d | 3c38 %s" % (o["s"], g["w4"], g["sub"], g["ph"], g["t"], g["clip"], g["bild"], g["x"], o["pl_x"], o["pl_z"],
              " ".join("%08x" % a for a in o["slots3c"])))
        for idx in sorted(o["arme"]):
            a = o["arme"][idx]
            print("    idx %d slot %d %08x w4 %08x sub %d ph %d sc %d t %d rot %d %d %d (roh %d/%d)" % (idx, a["slot"], a["adr"], a["w4"], a["sub"], a["ph"], a["sc"], a["t"], a["rx"], a["ry"], a["rz"], a["rx_roh"], a["rz_roh"]))
