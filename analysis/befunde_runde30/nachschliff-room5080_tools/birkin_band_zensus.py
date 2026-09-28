#!/usr/bin/env python3
"""Band-Zensus fuer alle Birkin-Spawns (Sce_em_set 0x44, Typ 0x30/0x36) in allen RDTs.

Frage: Wo weicht das Band, das der Port heute fuer die Birkin-Wandklemme nimmt
(re15_collision_band_from_y(y) = -(y/1800), re15_collision.c:186), vom Band ab, das das
Original nimmt (+0x82 = Spawn-Byte pc[4]: Sce_em_set @0x800421c8 `lbu v0,2(s2)` mit
s2 = pc+2, @0x800421d0 `sb v0,130(s0)`; gelesen von FUN_8003b0a4 @0x8003b234
`lbu v1,130(a3)` / @0x8003b23c `bne v1,v0` gegen Zell-floor>>4)?

Dazu je Raum: welche Baender die SCA-Zellen tragen (floor>>4, Layout wie sca_5080.py).
Record-Layout (FUN_800420a0): pc[1]=Slot, pc[2]=Typ(+0x8), pc[3]=grid(+0x9 @0x80042164),
pc[4]=+0x82, pc[8..9]=x(+0x34 @0x8004217c), pc[10..11]=y(+0x38 @0x80042188),
pc[12..13]=z(+0x3c @0x80042194).  Walk = scd_walk_lib (die eine Laengentabelle)."""
import os, sys, glob, struct
HIER = os.path.dirname(os.path.abspath(__file__))
WURZEL = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
sys.path.insert(0, os.path.join(WURZEL, "re15_port", "tools"))
import scd_walk_lib as L

def sca_baender(d):
    sca = struct.unpack_from("<I", d, 0x20)[0]
    if sca == 0 or sca + 24 > len(d):
        return set()
    cnt = struct.unpack_from("<5I", d, sca + 4)
    if sum(cnt) > 4096:
        return set()
    o, b = sca + 24, set()
    for _ in range(sum(cnt)):
        b.add(d[o + 11] >> 4)
        o += 12
    return b

def band_from_y(y):          # re15_collision.c:186, C-Division (Richtung 0)
    q = abs(y) // 1800
    return -(q if y >= 0 else -q)

n = 0
for p in sorted(glob.glob(os.path.join(WURZEL, "re15_port", "shared_assets", "PSX", "STAGE*", "ROOM*.RDT"))):
    d = open(p, "rb").read()
    if len(d) < 0x100:
        continue
    name = os.path.basename(p)[:-4]
    baender = sca_baender(d)
    for (kind, ri), ops in sorted(L.regionen(d).items()):
        for pc, op, sz in ops:
            if op != 0x44 or d[pc + 2] not in (0x30, 0x36):
                continue
            n += 1
            typ, grid, b82 = d[pc + 2], d[pc + 3], d[pc + 4]
            x, y, z = struct.unpack_from("<hhh", d, pc + 8)
            by = band_from_y(y)
            diff = "ABWEICHUNG" if by != b82 else "gleich"
            print(f"{name} {kind}{ri:02d} @0x{pc:05X} typ=0x{typ:02X} grid=0x{grid:02X} "
                  f"pc[4](+0x82)={b82} y={y} band_from_y={by} -> {diff}; SCA-Baender={sorted(baender)}")
print(f"# {n} Birkin-Spawns")
