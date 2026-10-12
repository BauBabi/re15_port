#!/usr/bin/env python3
"""savestate_11f0_pruefen.py - Runde 36 G2: prueft den Original-Spielstand room11f0_orig.sav (DuckStation, MZD-Disc,
Debug-Menue JUMP 11F, re15_quickload.py --left 5 --postload 10) gegen info/Re1.5/PSX.EXE und ROOM11F0.RDT und liest
den Generator-Cursor obj 0 aus dem Objekt-Pool.
  EXE-Stub      @0x80026e4c `08 00 e0 03` (Auslieferung; gepatcht waere `24 c2 01 08`)
  RDT-Basis     DAT_800ac778 (FUN_8002bd44 @0x8002bd48 `lw v0,-14472(v0)`, FUN_8003b558 @0x8003b5b4)
  SCA-Zeiger    RDT+0x20 (FUN_8003b558 @0x8003b5dc `lw v0,32(v1)`), Kopf ceil_x/ceil_z (@0x8003b5e4/e8),
                Bereichszeiger [4+4q]/[8+4q] (@0x8003b600-614), Zelle 12 B
  Pool          0x800b3f98 + 0x94*k (FUN_8002bd44 @0x8002bda8-c8; Installer LAB_80040914 @0x8004093c-58: k = pc[1])
  Bank 5        0x800b1028 (Tabelle @0x80074664[5], Set-Handler @0x8003fdf8-fe0c), Bit b = Wort b>>5, Maske 0x80000000>>(b&31)
Aufruf: python savestate_11f0_pruefen.py <sav> <ROOM11F0.RDT>"""
import os, struct, sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..",
                                ".claude", "skills", "re15-room-capture", "scripts"))
from re15_ss import Ram

POOL, OBJ_SZ = 0x800B3F98, 0x94
PL = 0x800ACA54


def main():
    r = Ram(sys.argv[1])
    rdt = open(sys.argv[2], "rb").read()
    print("EXE-Stub @0x80026e4c:", r.bytes(0x80026E4C, 4).hex(), "(Auslieferung 0800e003)")
    base = r.u32(0x800AC778)
    print("RDT-Basis DAT_800ac778 = %08x" % base)
    # SCD-Bytecode (main 0xCC4 .. sub-Ende 0x1810) im RAM == Datei?
    scd_ram = r.bytes(base + 0xCC4, 0x1810 - 0xCC4)
    print("SCD 0x00CC4..0x01810 im RAM == ROOM11F0.RDT:", scd_ram == rdt[0xCC4:0x1810])
    sca = r.u32(base + 0x20)
    cx, cz = r.s16(sca), r.s16(sca + 2)
    print("SCA @%08x ceil (%d,%d)" % (sca, cx, cz))
    sca_file = struct.unpack_from("<I", rdt, 0x20)[0]
    n_all = sum(struct.unpack_from("<5I", rdt, sca_file + 4))
    cells_ram = r.bytes(sca + 24, n_all * 12)
    print("SCA-Zellen (%d x 12 B) im RAM == Datei @0x%05X:" % (n_all, sca_file + 24),
          cells_ram == rdt[sca_file + 24:sca_file + 24 + n_all * 12])
    for q in range(5):
        a, e = r.u32(sca + 4 + 4 * q), r.u32(sca + 8 + 4 * q)
        print("  Bereich %d: Zeiger %08x .. %08x (%d Zellen)" % (q, a, e, (e - a) // 12 if e >= a else -1))
    q1 = r.u32(sca + 8)
    for i in range(13, 17):
        w, d, x, z, t, u0, u1, fl = struct.unpack_from("<HHhhBBBB", r.bytes(q1 + 12 * i, 12))
        print("  r1[%d] @%08x: x %d..%d z %d..%d typ %d u0 0x%02x u1 0x%02x fl 0x%02x" % (
            i, q1 + 12 * i, x, x + w, z, z + d, t, u0, u1, fl))
    print("Objekt-Pool (nOmodel = RDT-Kopf[2] = %d):" % r.u8(base + 2))
    for k in range(12):
        o = POOL + OBJ_SZ * k
        f0 = r.u32(o)
        if not f0 & 1:
            continue
        box = r.u32(o + 0x78)
        bx = struct.unpack("<6h", r.bytes(box, 12)) if 0x80000000 <= box < 0x80200000 else None
        print("  k=%2d @%08x +0=%04x typ %d band %d lage (%d,%d,%d) spiegel (%d,%d,%d) +0x8C=%d box@%08x %s%s" % (
            k, o, f0, r.u8(o + 8), r.u8(o + 0x82), r.s32(o + 0x34), r.s32(o + 0x38), r.s32(o + 0x3C),
            r.s16(o + 0x40), r.s16(o + 0x42), r.s16(o + 0x44), r.u8(o + 0x8C), box, bx,
            (" = RDT+0x%05X" % (box - base)) if box >= base and box - base < len(rdt) else ""))
    b5 = r.u32(0x800B1028)
    b4w7 = r.u32(0x800B1018 + 7 * 4)
    print("Bank 5 Wort 0 = %08x (Bits 0..12 = Raetsel scharf: %s); 4:238 = %d" % (
        b5, "ja" if b5 & 0xFFF80000 == 0xFFF80000 else "nein", 1 if b4w7 & (0x80000000 >> 14) else 0))
    print("Spieler +0x34/+0x38/+0x3C = (%d,%d,%d) Band %d Blick %d" % (
        r.s32(PL + 0x34), r.s32(PL + 0x38), r.s32(PL + 0x3C), r.u8(PL + 0x82), r.s16(PL + 0x6A)))
    print("DAT_800ac788 (Stand-Objekt) = %08x" % r.u32(0x800AC788))


if __name__ == "__main__":
    main()
