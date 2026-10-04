"""re2_los.py - RE2 FUN_80050858 (Sichtstrahl gegen die Kollisionssaetze, Decompile RE2_Quellcode_V2/FUN_80050858.c)
Zeile fuer Zeile in Python (int32-Arithmetik wie die R3000), angewandt auf ROOM2050 collision.sca und die
Blickziel-Suche FUN_8003DB38 (Maske 0x2080, a3 = 1 @0x8003dc10-14) der RE2-Mitschnitte. Runde 35 Spur H, NB4.
Aufruf: python re2_los.py"""
import os, struct, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import re2_frames as F

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.join(HERE, "..", "..", "..", "..")
SCA = open(os.path.join(ROOT, "info", "re2leon", "PL0", "RDT", "room2050", "collision.sca"), "rb").read()
EXE = open(os.path.join(ROOT, "info", "re2leon", "PSX.EXE"), "rb").read()
T_ADDR = struct.unpack_from("<I", EXE, 0x18)[0]
TAB = EXE[0x800 + 0x800A73B4 - T_ADDR: 0x800 + 0x800A73B4 - T_ADDR + 16]   # DAT_800a73b4[attr & 0xf]


def i32(v):
    v &= 0xFFFFFFFF
    return v - (1 << 32) if v & 0x80000000 else v


def u32(v):
    return v & 0xFFFFFFFF


def los(p1, p2, mask, p4):
    """FUN_80050858(p1, p2, mask, p4): 0 = frei, 1 = verdeckt."""
    n = struct.unpack_from("<i", SCA, 4)[0]
    for k in range(1, n):
        x, z, w, d, attr, hb = struct.unpack_from("<hhHHHH", SCA, k * 16)
        fl = struct.unpack_from("<I", SCA, k * 16 + 12)[0]
        if (attr & mask) == 0 or TAB[attr & 0xF] != 0:
            continue
        b80 = 3
        if u32(p1[0] - x) < w and u32(p1[2] - z) < d:
            b80 = 2
        elif u32(p2[0] - x) < w and u32(p2[2] - z) < d:
            b80 = 2
        else:
            i10, i4, u14, i2, i3, u1, i11, i13 = z, p1[2], w, x, p1[0], d, p2[2], p2[0]
            i12, i9, i8 = i3 - i13, i4 - i11, i10 + u1
            a = i32(i32(u14 * (i4 - i10) - u1 * (i3 - i2)) ^ i32(u14 * (i11 - i10) - u1 * (i13 - i2)))
            b = i32(i32(i12 * (i10 - i4) - i9 * (i2 - i3)) ^ i32(i12 * (i8 - i4) - i9 * ((i2 + u14) - i3)))
            c = i32(i32(u14 * (i8 - i4) - u1 * (i3 - i2)) ^ i32(u14 * (i8 - i11) - u1 * (i13 - i2)))
            e = i32(i32(i12 * (i8 - i4) - i9 * (i2 - i3)) ^ i32(i12 * (i10 - i4) - i9 * ((i2 + u14) - i3)))
            if i32(a & b) > -1 and i32(c & e) > -1:
                continue
        if p4 == 0:
            return 1
        i11 = 0
        v = fl
        while (v & 1) == 0:
            i11 -= 0x708
            v >>= 1
        i10 = (hb >> 11) * -100 + ((hb >> 6) & 0x1F) * -0x708
        if (u32(p1[1] - i10) < u32(i11 - i10) and u32(p1[2] - z) < d) or \
           (u32(p2[1] - i10) < u32(i11 - i10) and u32(p2[2] - z) < d):
            b80 ^= 2
        else:
            i9, u1, i4, i8, i15, i3, i2 = p1[1], d, z, p1[2], i11 - i10, p2[1], p2[2]
            i13, i12 = i8 - i2, i9 - i3
            a = i32(i32(u1 * (i9 - i10) - i15 * (i8 - i4)) ^ i32(u1 * (i3 - i10) - i15 * (i2 - i4)))
            b = i32(i32(i13 * (i10 - i9) - i12 * (i4 - i8)) ^ i32(i13 * (i11 - i9) - i12 * ((i4 + u1) - i8)))
            c = i32(i32(u1 * (i11 - i9) - i15 * (i8 - i4)) ^ i32(u1 * (i11 - i3) - i15 * (i2 - i4)))
            e = i32(i32(i13 * (i11 - i9) - i12 * (i4 - i8)) ^ i32(i13 * (i10 - i9) - i12 * ((i4 + u1) - i8)))
            if i32(a & b) > -1 and i32(c & e) > -1:
                continue
        if (u32(p1[1] - i10) < u32(i11 - i10) and u32(p1[0] - x) < w) or \
           (u32(p2[1] - i10) < u32(i11 - i10) and u32(p2[0] - x) < w):
            if b80 != 0:
                return 1
        else:
            i9, u1, i4, i8, i15, i3, i2 = p1[1], w, x, p1[0], i11 - i10, p2[1], p2[0]
            i13, i12 = i8 - i2, i9 - i3
            a = i32(i32(u1 * (i9 - i10) - i15 * (i8 - i4)) ^ i32(u1 * (i3 - i10) - i15 * (i2 - i4)))
            b = i32(i32(i13 * (i10 - i9) - i12 * (i4 - i8)) ^ i32(i13 * (i11 - i9) - i12 * ((i4 + u1) - i8)))
            c = i32(i32(u1 * (i11 - i9) - i15 * (i8 - i4)) ^ i32(u1 * (i11 - i3) - i15 * (i2 - i4)))
            e = i32(i32(i13 * (i11 - i9) - i12 * (i4 - i8)) ^ i32(i13 * (i10 - i9) - i12 * ((i4 + u1) - i8)))
            if i32(a & b) < 0 or i32(c & e) < 0:
                return 1
    return 0


SAETZE = [struct.unpack_from("<3h", open(os.path.join(ROOT, "info", "re2leon", "PL0", "RDT", "ROOM2050.RDT"), "rb").read(),
                             0x1970 + n * 0x16 + 10) for n in range(10)]

if __name__ == "__main__":
    D = os.path.join(HERE, "daten")
    for run in sorted(os.listdir(D)):
        p = os.path.join(D, run, "frames.bin")
        if not os.path.exists(p):
            continue
        fr = F.frames(p)
        _, kopf = F.part(fr[0]["lparts"], 8)          # Leons Kopf-Part +0x5C (Startpunkt @0x8003dc4c)
        satz0 = 0 if "west" in run or "_r0_" in run or "_r3_" in run else 5
        if run.startswith("push"):
            continue
        res = []
        for s in range(satz0, satz0 + 5):
            res.append("S%d:%s" % (s, "verdeckt" if los(kopf, SAETZE[s], 0x2080, 1) else "frei"))
        print("%-20s Kopf %s  %s" % (run, kopf, " ".join(res)))
