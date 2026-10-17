"""Runde 38 Spur N: Wasser-Zensus je Raum (FLR-Typ-Byte +8, sce-8-Zonen, Kameras).

Liest NUR den Arbeitsbaum (shared_assets/PSX). FLR: RDT u32 @0x34 -> u16 count, 12-B-Eintraege
(x z w d s16, Typ u8 +8, FUN_800437d4 @0x800437f8-0x80043828). sce 8 = Aot_set (0x2C) pc[2] == 8,
Nutzlast u16 @rec+0xc (Wasserspiegel, Handler 0x8004330c `sh v0,136(entity)`).
"""
import glob, os, struct, sys
BAUM = r"C:/workspace/git/reAi_v2/.claude/worktrees/r38_wasser"
sys.path.insert(0, BAUM + "/re15_port/tools")
import aot_sce_census as A  # noqa: E402

PSX = BAUM + "/re15_port/shared_assets/PSX"

def flr(data):
    off = A.u32(data, 0x34)
    out = []
    if off == 0 or off + 2 > len(data):
        return out
    n = A.u16(data, off)
    e = off + 2
    while e + 12 <= len(data):
        x = A.s16(data, e)
        if x == 0:
            break
        z, w, d = A.s16(data, e + 2), A.s16(data, e + 4), A.s16(data, e + 6)
        out.append((x, z, w, d, data[e + 8], data[e + 9], data[e + 10], data[e + 11]))
        e += 12
    return n, out

def sce8(data, room):
    inst = []
    stats = dict(walked=0, total=0, residue=0, stops=[])
    for (sec, tag) in ((A.u32(data, 0x40), "main"), (A.u32(data, 0x44), "sub")):
        if sec == 0 or sec >= len(data):
            continue
        send = A.rdt_section_end(data, sec)
        for (o, e, idx) in A.section_regions(data, sec, send):
            A.walk(data, sec + o, sec + e, room, f"{tag}{idx:02d}", inst, stats)
    return [i for i in inst if i.get("sce") == 8 and i["op"] == 0x2C]

def main():
    rooms = sys.argv[1:] or None
    for path in sorted(glob.glob(PSX + "/STAGE*/ROOM*.RDT")):
        room = os.path.basename(path)[4:8]
        if rooms and room not in rooms:
            continue
        data = open(path, "rb").read()
        if len(data) < 0x60:
            continue
        r = flr(data)
        if not r:
            continue
        n, ents = r
        wasser = [e for e in ents if e[4] & 0x80]
        z8 = sce8(data, room)
        if not wasser and not z8 and rooms is None:
            continue
        print(f"ROOM{room}: FLR n={n} eintraege={len(ents)} wasser(0x80)={len(wasser)} sce8={len(z8)}")
        for i, e in enumerate(ents):
            tag = "WASSER" if e[4] & 0x80 else ""
            print(f"   FLR[{i:2}] x {e[0]}..{e[0]+e[2]} z {e[1]}..{e[1]+e[3]} typ={e[4]:#04x} b9..11={e[5]:02x} {e[6]:02x} {e[7]:02x} {tag}")
        for z in z8:
            x, zz, w, d = z["rect"]; w &= 0xffff; d &= 0xffff
            print(f"   sce8 {z['script']}@{z['off']:#x} slot={z['slot']} flags={z['flags']:#04x} band={z['floor']:#04x} "
                  f"x {x}..{x+w} z {zz}..{zz+d} spiegel={struct.unpack('<h', bytes(z['pb'][0:2]))[0]}")

if __name__ == "__main__":
    main()
