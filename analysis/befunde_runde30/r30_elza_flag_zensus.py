#!/usr/bin/env python3
"""Runde 30 / Thema G: wer LIEST (Ck 0x21) und wer SCHREIBT (Set 0x22) ein Story-Flag?
Opcode-exakter Walk ueber alle ausgelieferten RDT (Walker aus re15_port/tools/scd_dump_room.py).

Aufruf: r30_elza_flag_zensus.py <bank>:<bit> [<bank>:<bit> ...]
"""
import os, sys, glob

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.join(HERE, "..", "..")
src = open(os.path.join(ROOT, "re15_port", "tools", "scd_dump_room.py"), encoding="utf-8").read()
src = src.rsplit("\nmain()", 1)[0]
ns = {}
exec(compile(src, "scd_dump_room", "exec"), ns)
op_size, rdt_section_end, section_regions, u32 = ns["op_size"], ns["rdt_section_end"], ns["section_regions"], ns["u32"]

def main():
    want = set()
    for a in sys.argv[1:]:
        b, i = a.split(":")
        want.add((int(b, 0), int(i, 0)))
    hits = {w: [] for w in want}
    for p in sorted(glob.glob(os.path.join(ROOT, "re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT"))):
        nm = os.path.basename(p)[4:8]
        d = open(p, "rb").read()
        if len(d) < 0x60: continue
        for sec, tag in ((u32(d, 0x40), "main"), (u32(d, 0x44), "sub")):
            if sec == 0 or sec >= len(d): continue
            se = rdt_section_end(d, sec)
            for (o, e, idx) in section_regions(d, sec, se):
                pc = sec + o; end = sec + e
                while pc < end:
                    sz = op_size(d, pc)
                    if sz is None or pc + sz > end: break
                    op = d[pc]
                    if op in (0x21, 0x22):
                        k = (d[pc + 1], d[pc + 2])
                        if k in want:
                            hits[k].append((nm, "%s%02d" % (tag, idx), pc, "Ck " if op == 0x21 else "Set", d[pc + 3]))
                    pc += sz
    for k in sorted(hits):
        print("=== flag(%d,%d) = 0x%02x : %d Fundstellen" % (k[0], k[1], k[1], len(hits[k])))
        for h in hits[k]:
            print("   ROOM%s %-7s @0x%05X  %s val=%d" % h)

main()
