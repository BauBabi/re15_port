#!/usr/bin/env python3
"""Welche Tueren fuehren nach ROOM5080/5081?  Walk aller SCD-Regionen aller RDTs
(scd_walk_lib = die eine Laengentabelle), Door_aot_set 0x3B: pc[22]=Stage (0-basiert,
aot_common.c:597-619), pc[23]=Raum, pc[14..19]=Ziel-Lage, pc[20..21]=Blickrichtung."""
import os, sys, glob, struct
HIER = os.path.dirname(os.path.abspath(__file__))
WURZEL = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
sys.path.insert(0, os.path.join(WURZEL, "re15_port", "tools"))
import scd_walk_lib as L

def s16(d, o): return struct.unpack_from("<h", d, o)[0]

for p in sorted(glob.glob(os.path.join(WURZEL, "re15_port", "shared_assets", "PSX", "STAGE*", "ROOM*.RDT"))):
    d = open(p, "rb").read()
    if len(d) < 0x100: continue
    name = os.path.basename(p)[:-4]
    for (kind, ri), ops in sorted(L.regionen(d).items()):
            for pc, op, sz in ops:
                if op != 0x3B: continue
                st, rm = d[pc+22], d[pc+23]
                ziel = ((st + 1) << 12) | (rm << 4)
                if ziel in (0x5080,):
                    print(f"{name} {kind}{ri:02d} @0x{pc:05X} slot={d[pc+1]} -> ROOM{ziel:04X} "
                          f"pos=({s16(d,pc+14)},{s16(d,pc+16)},{s16(d,pc+18)}) dir={s16(d,pc+20)} "
                          f"cut={d[pc+24]} bytes={d[pc:pc+sz].hex(' ')}")
