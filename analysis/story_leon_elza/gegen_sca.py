#!/usr/bin/env python3
"""Gegenpruefung D: SCA-Zellen eines Raums je (Region, Index) ausgeben.
Format (re15_port/include/re15_rdt.h re15_sca_entry_t, rdt_common.c:250ff):
  RDT+0x20 -> SCA-Start; Header 24 B: ceilX u16, ceilZ u16, count[5] u32;
  dann sum(count) Eintraege a 12 B: width u16, density u16, x s16, z s16, type u8, u0 u8, u1 u8, floor u8.
Aufruf: python -I gegen_sca.py <ROOMxxxx.RDT> [x z]   (optional: Punkt-Test, welche Zellen (x,z) enthalten)
"""
import struct, sys

def main():
    p = sys.argv[1]
    d = open(p, "rb").read()
    sca = struct.unpack_from("<I", d, 0x20)[0]
    cx, cz = struct.unpack_from("<HH", d, sca)
    counts = struct.unpack_from("<5I", d, sca + 4)
    print(f"{p}: SCA @0x{sca:X} ceil=({cx},{cz}) counts={counts}")
    off = sca + 24
    px = pz = None
    if len(sys.argv) >= 4:
        px, pz = int(sys.argv[2]), int(sys.argv[3])
    for r, c in enumerate(counts):
        for i in range(c):
            w, dn, x, z, ty, u0, u1, fl = struct.unpack_from("<HHhhBBBB", d, off)
            hit = ""
            if px is not None and x <= px <= x + w and z <= pz <= z + dn:
                hit = "  <== enthaelt Punkt"
            print(f"  r{r} i{i:02d} @0x{off:05X}  x={x:6d}..{x+w:6d}  z={z:6d}..{z+dn:6d}  w={w:5d} d={dn:5d} type={ty} u0=0x{u0:02x} u1=0x{u1:02x} floor=0x{fl:02x}{hit}")
            off += 12

if __name__ == "__main__":
    main()
