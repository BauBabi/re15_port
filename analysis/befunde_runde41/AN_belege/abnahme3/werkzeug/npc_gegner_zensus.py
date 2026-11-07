#!/usr/bin/env python3
"""npc_gegner_zensus.py - Abnahme 3 Spur AN: in welchen RDTs setzt das Skript NPCs (Typ >= 0x40) UND Gegner (0x10..0x3F)?
Walker wie tools/cutscene_latch_zensus.py (Laengen aus scd_vm.c s_opcode_sizes ueber tools/discard_zensus.py).
Sce_em_set = 0x44: [44 slot typ grid ...]. Ausgabe je Raum: NPC-Typen mit/ohne Port-Kasten, Gegner-Typen.
Aufruf: python npc_gegner_zensus.py <re15_port/tools> <shared_assets/PSX>"""
import sys, os, glob, collections
sys.path.insert(0, sys.argv[1])
from discard_zensus import op_size, fwd_target, rdt_section_end, section_regions, u32
KASTEN = {0x40, 0x42, 0x45, 0x47, 0x49, 0x4b, 0x4d}   # re15_enemy_apply_hitbox (re15_damage.c) NPC-Zweige
root = sys.argv[2]
n_both = 0
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
    npc = [t for t in typen if t >= 0x40]; geg = [t for t in typen if 0x10 <= t < 0x40]
    if npc and geg:
        n_both += 1
        print(f"ROOM{room}: NPC " + " ".join(f"{t:02X}{'' if t in KASTEN else '(ohne Kasten)'}" for t in npc)
              + " | Gegner " + " ".join(f"{t:02X}" for t in geg))
print(f"Raeume mit NPC und Gegner im selben Skript: {n_both}")
