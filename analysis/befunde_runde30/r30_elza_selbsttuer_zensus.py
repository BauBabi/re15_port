#!/usr/bin/env python3
"""Runde 30 / Thema G: Zensus der SELBST-TUEREN (Door_aot_set, dessen Ziel der eigene Raum ist).

Der Port entscheidet in aot_common.c (aot_fire_door) ueber den Skript-Neueinstieg mit
    d->dest_room != 0 && (0x1000 | dest_room<<4) == g_current_room_id
also OHNE Variante (niedrigste Hex-Ziffer) und MIT fest verdrahteter Stage 1. Dieser
Zensus zaehlt, welche ausgelieferten Tueren davon betroffen sind.

Walker/Laengen: re15_port/tools/scd_dump_room.py (wird als Quelltext geladen, ohne main()).
Door_aot_set-Satz (32 B): +1 Slot, +2 sce, +14..+19 next_pos X/Y/Z (s16), +20 Yaw(u16),
+22 Ziel-Stage, +23 Ziel-Raum, +24 Ziel-Cut  (Leser FUN_8001d600: lbu 8/9/10(a0) auf den
Payload ab Satz+14, @0x8001d960/@0x8001d94c/@0x8001d930).
"""
import os, sys, glob, struct

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.join(HERE, "..", "..")
src = open(os.path.join(ROOT, "re15_port", "tools", "scd_dump_room.py"), encoding="utf-8").read()
src = src.rsplit("\nmain()", 1)[0]
ns = {}
exec(compile(src, "scd_dump_room", "exec"), ns)
op_size, rdt_section_end, section_regions, u32 = ns["op_size"], ns["rdt_section_end"], ns["section_regions"], ns["u32"]
s16 = ns["s16"]

def walk(d, start, end):
    pc = start
    while pc < end:
        sz = op_size(d, pc)
        if sz is None or pc + sz > end:
            return
        yield pc, d[pc], sz
        pc += sz

def main():
    rows = []
    n_rdt = 0
    for p in sorted(glob.glob(os.path.join(ROOT, "re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT"))):
        nm = os.path.basename(p)[4:8]
        d = open(p, "rb").read()
        if len(d) < 0x60: continue
        n_rdt += 1
        rid = int(nm, 16)
        stage0 = (rid >> 12) - 1; room = (rid >> 4) & 0xFF; var = rid & 0xF
        for sec, tag in ((u32(d, 0x40), "main"), (u32(d, 0x44), "sub")):
            if sec == 0 or sec >= len(d): continue
            se = rdt_section_end(d, sec)
            for (o, e, idx) in section_regions(d, sec, se):
                for pc, op, sz in walk(d, sec + o, sec + e):
                    if op != 0x3B: continue
                    ds, dr, dc = d[pc + 22], d[pc + 23], d[pc + 24]
                    if ds == stage0 and dr == room:
                        rect = (s16(d, pc + 6), s16(d, pc + 8), s16(d, pc + 10), s16(d, pc + 12))
                        port_alt = (dr != 0) and ((0x1000 | (dr << 4)) == rid)
                        rows.append((nm, var, "%s%02d" % (tag, idx), pc, d[pc + 1], dc, rect, port_alt))
    print("RDT durchlaufen: %d" % n_rdt)
    print("Selbst-Tueren gesamt: %d" % len(rows))
    print("%-6s %-3s %-7s %-7s %-4s %-3s %-28s %s" % ("Raum", "Var", "Region", "Datei", "Slot", "Cut", "Rechteck", "Port-Regel greift?"))
    for r in rows:
        print("%-6s %-3d %-7s 0x%05X %-4d %-3d %-28s %s" % (r[0], r[1], r[2], r[3], r[4], r[5], str(r[6]), "JA" if r[7] else "NEIN"))
    ja = sum(1 for r in rows if r[7]); nein = len(rows) - ja
    print("Port-Regel greift: %d   greift NICHT: %d" % (ja, nein))
    print("  davon Variante 1 (Elza): %d" % sum(1 for r in rows if not r[7] and r[1] == 1))
    print("  davon Variante 0 ausserhalb Stage 1 oder Raum 0: %d" % sum(1 for r in rows if not r[7] and r[1] == 0))

main()
