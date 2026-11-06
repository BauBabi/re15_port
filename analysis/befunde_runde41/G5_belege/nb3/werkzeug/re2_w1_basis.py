"""re2_w1_basis.py - RE2-RAM: [T0]-Basis der Intro-Kopplung (sh v0,0x2bb0(at) nach lui at,hi) und Gewicht 1 je Savestate."""
import sys, struct
sys.path.insert(0, sys.argv[1])
from re2ss import SS
SN = sys.argv[2]
B = open(sys.argv[3], "rb").read()     # CDEMD0_EM36_ai0.BIN (Link 0x80100000)
muster = bytes.fromhex("3000e28c" "2528" "0002")  # lw v0,48(a3) ; addu a1,s0,zero (Teil)
muster = B[0x11f4:0x11fc]              # @0x801011f4 lw v0,48(a3) / @0x801011f8 addu a1,s0,zero (lui/sh danach relokiert)
for i in range(1, 27):
    s = SS(SN % i)
    ram = s.ram()
    p = ram.find(muster)
    if p < 0:
        print("s%02d Overlay nicht gefunden" % i); continue
    lui = struct.unpack_from("<I", ram, p + 8)[0]; sh = struct.unpack_from("<I", ram, p + 12)[0]
    hi = lui & 0xffff; lo = struct.unpack("<h", struct.pack("<H", sh & 0xffff))[0]
    adr = ((hi << 16) + lo) & 0xffffffff
    g5 = s.u32(0x800cfe1c)
    teile = s.u32(g5 + 0x198) if g5 else 0
    t2 = teile + 2 * 172
    tab = s.u32(t2 + 0x88) if teile else 0
    w = [s.s16(tab + 12 + 2 * k) for k in range(4)] if tab else None
    t0y = s.s32(teile + 0x30) if teile else None
    print("s%02d Overlay-Basis 0x%08x [T0]-Basis @0x%08x = %d | G5 0x%08x Teil0.y %s Gewichte %s clipwort 0x%08x" % (
        i, 0x80100000 + p - 0x11f4, adr, s.s16(adr), g5, t0y, w, s.u32(g5 + 0x14c) if g5 else 0))
