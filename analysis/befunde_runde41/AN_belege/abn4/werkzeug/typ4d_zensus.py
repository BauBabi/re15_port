#!/usr/bin/env python3
"""typ4d_zensus.py - Abnahme 4 Spur AN: in welchen RDTs setzt das Skript eine 0x4D (Sce_em_set 0x44 [44 slot typ ...])?
Und: welche NPC-Typen (>= 0x40) setzt jedes Skript ueberhaupt (fuer die NB5-Regel 0x40..0x4D)?
Walker wie abnahme3/werkzeug/npc_gegner_zensus.py (Laengen aus tools/discard_zensus.py).
Aufruf: python typ4d_zensus.py <re15_port/tools> <shared_assets/PSX>"""
import sys, os, glob, collections
sys.path.insert(0, sys.argv[1])
from discard_zensus import op_size, fwd_target, rdt_section_end, section_regions, u32
root = sys.argv[2]
alle_npc = collections.Counter()
for path in sorted(glob.glob(os.path.join(root, "STAGE*", "ROOM*.RDT"))):
    d = open(path, "rb").read(); room = os.path.basename(path)[4:8]
    if len(d) < 0x48: continue
    typen = collections.OrderedDict()
    for sec in (u32(d, 0x40), u32(d, 0x44)):
        if sec == 0 or sec >= len(d): continue
        se = rdt_section_end(d, sec)
        for (o, e, idx) in section_regions(d, sec, se):
            pc, end = sec + o, sec + e; maxf = pc
            while pc < end:
                op = d[pc]; sz = op_size(d, pc)
                if sz is None or pc + sz > end: break
                if op == 0x44: typen.setdefault(d[pc + 2], []).append(pc)
                t = fwd_target(d, pc, op)
                if t is not None and t > maxf: maxf = t
                if op == 0x01 and pc + 2 > maxf: break
                pc += sz
    npc = [t for t in typen if t >= 0x40]
    for t in npc: alle_npc[t] += 1
    if 0x4D in typen:
        print(f"ROOM{room}: 0x4D Sce_em_set @" + ",".join(f"0x{p:05X}" for p in typen[0x4D]))
print("NPC-Typen (>= 0x40) ueber alle Skripte, Anzahl Raeume:", " ".join(f"{t:02X}:{n}" for t, n in sorted(alle_npc.items())))
