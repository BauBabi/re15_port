#!/usr/bin/env python3
"""Runde 41 Spur I: Zensus der Zombie-Spawns (Sce_em_set 0x44) ueber alle ausgelieferten RDTs.

Walker = re15_port/tools/aot_sce_census.py (opcode-exakt, main+sub, mit den dort belegten
Sektionsend-Fixes). Gesammelt je Sce_em_set: Raum, Skript, Offset, Slot (pc[1]), Typ (pc[2]),
Behavior/Deskriptor (pc[3]) = Entity +0x9 (RE1.5-Handler 0x800420A0), Position (LE pc[8..13]),
Yaw (LE pc[16..17]). Zusaetzlich Member_set (0x34) auf Member 12 (= +0x9, `sb a2,9(a0)`
@0x800411F8) als Skript-Wecker-Kandidaten.

Aufruf: python fresser_zensus.py [--rooms 2000,2001,...] [--all-types]
"""
import os, sys, glob, struct
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(HERE))
import aot_sce_census as C  # noqa: E402

ROOT = os.path.normpath(os.path.join(HERE, "..", "..", "shared_assets", "PSX"))
ZOMBIE_TYPES = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
                0x1c, 0x1d, 0x1e, 0x1f}

def walk_collect(data, start, end, room, script, out):
    pc = start
    while pc < end:
        op = data[pc]
        if op == 0x01:
            # Evt_end: weitergehen (Regionen koennen mehrere Evt_end tragen), Walker wie census
            pass
        sz = C.op_size(data, pc) if pc + 4 <= len(data) else C.SIZES.get(op)
        if sz is None or pc + sz > end:
            break
        if op == 0x44:
            out.append(dict(room=room, script=script, off=pc, kind="em_set", slot=data[pc+1],
                            type=data[pc+2], beh=data[pc+3],
                            x=C.s16(data, pc+8), y=C.s16(data, pc+10), z=C.s16(data, pc+12),
                            rot=C.u16(data, pc+16), raw=data[pc:pc+20].hex()))
        elif op == 0x34:
            out.append(dict(room=room, script=script, off=pc, kind="member_set",
                            member=data[pc+1], val=C.u16(data, pc+2), raw=data[pc:pc+4].hex()))
        elif op == 0x2E:
            out.append(dict(room=room, script=script, off=pc, kind="work_set",
                            wkind=data[pc+1], idx=data[pc+2], raw=data[pc:pc+3].hex()))
        pc += sz

def census(rooms=None):
    out = []
    for f in sorted(glob.glob(os.path.join(ROOT, "STAGE*", "ROOM*.RDT"))):
        room = os.path.basename(f)[4:-4]
        if rooms and room not in rooms:
            continue
        data = open(f, "rb").read()
        if len(data) < 0x48:
            continue
        main_s, sub_s = C.u32(data, 0x40), C.u32(data, 0x44)
        for (sec, tag) in ((main_s, "main"), (sub_s, "sub")):
            if sec == 0 or sec >= len(data):
                continue
            send = C.rdt_section_end(data, sec)
            for (o, e, idx) in C.section_regions(data, sec, send):
                walk_collect(data, sec + o, sec + e, room, f"{tag}{idx:02d}", out)
    return out

def main():
    rooms = None
    all_types = "--all-types" in sys.argv
    if "--rooms" in sys.argv:
        rooms = set(sys.argv[sys.argv.index("--rooms") + 1].split(","))
    recs = census(rooms)
    cur_work = {}
    for r in recs:
        if r["kind"] == "em_set":
            if not all_types and r["type"] not in ZOMBIE_TYPES:
                continue
            sel = r["beh"] & 0x1f
            print(f"ROOM{r['room']} {r['script']} @0x{r['off']:05x} EM_SET slot={r['slot']} "
                  f"type=0x{r['type']:02x} beh=0x{r['beh']:02x} (sel {sel}, gate80={int(bool(r['beh']&0x80))}) "
                  f"pos=({r['x']},{r['y']},{r['z']}) rot={r['rot']} raw={r['raw']}")
        elif r["kind"] == "member_set" and r["member"] == 12:
            print(f"ROOM{r['room']} {r['script']} @0x{r['off']:05x} MEMBER_SET(12 = +0x9) val=0x{r['val']:04x} raw={r['raw']}")

if __name__ == "__main__":
    main()
