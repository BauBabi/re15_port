#!/usr/bin/env python3
"""Zensus grid & 0x60 (Entity +0x09) ueber ALLE ausgelieferten RDTs (Runde 38 Integration,
versteckte Schachtspinne).

Die Zielhilfe FUN_8003703c ueberspringt JEDES Wesen mit +0x09 & 0x60 (`lbu a0,-145(s0)` @0x80037104,
`andi v0,a0,0x60` @0x8003710c, `bne v0,zero,0x80037174` @0x80037110). Quellen fuer +0x09:
  * Sce_em_set (0x44, 20 B): pc[2] = Typ, pc[3] = +0x09 (Handler 0x800420A0, `sb` grid), pc[4] = floor
  * Member_set (0x34, 4 B): Member 0x0C = +0x09 (actor_common.c case 12 `sb`), Wert s16 pc[2..3]
  * Member_set2 (0x35, 3 B): Member pc[1], Wert = work_vars[pc[2]] (dynamisch)
  * Plc_cnt (0x5B, 4 B): ALU pc[1] auf Member pc[2] mit work_vars[pc[3]] (dynamisch)
Ziel des Member_* = der Work_set (0x2E kind/idx) davor im selben Unterprogramm (kind 2 = Aktor idx+1).
Walk = scd_dump_room.py (Laengen aus scd_vm.c); ein Abbruch wird gemeldet.

Aufruf: python zensus_grid60.py <PSX-Baum> x <re15_port/tools/scd_dump_room.py> [--alle]
(Ausgabe zensus_grid60.txt = Lauf ueber info/Re1.5/PSX, ohne --alle)
"""
import os, sys, glob, struct, importlib.util, io, contextlib

HERE = os.path.dirname(os.path.abspath(__file__))
TOOL = sys.argv[3] if len(sys.argv) > 3 else None

def lade_walk(tool_path):
    src = open(tool_path, encoding="utf-8").read()
    src = src.replace("\nmain()\n", "\n")          # nur die Funktionen, ohne den Aufruf
    ns = {"__name__": "scd_dump_room"}
    exec(compile(src, tool_path, "exec"), ns)
    return ns

def walk(ns, d, start, end):
    """Opcodes (pc, op, bytes) einer Region; None am Ende = Abbruch."""
    out = []
    pc = start; maxf = start
    while pc < end:
        op = d[pc]
        sz = ns["op_size"](d, pc)
        if sz is None or pc + sz > end:
            out.append((pc, None, None)); return out
        out.append((pc, op, d[pc:pc+sz]))
        t = ns["fwd_target"](d, pc, op)
        if t is not None and t > maxf: maxf = t
        if op == 0x01 and pc + 2 > maxf: return out
        pc += sz
    return out

def s16(b, o): return struct.unpack_from("<h", b, o)[0]

TYPNAMEN = {0x10:"Zombie",0x11:"Zombie",0x12:"Zombie",0x13:"Zombie-Girl",0x15:"Zombie",0x16:"Zombie",
            0x18:"Zombie",0x1a:"Gitterhand",0x20:"Hund",0x21:"Kraehe",0x22:"Stub",0x23:"Alligator",
            0x24:"FX-Emitter",0x25:"Spinne",0x26:"Feuer/Baby",0x27:"Gorilla",0x29:"Affe",0x2b:"Spiderman",
            0x2d:"Efeu",0x30:"Birkin",0x36:"Birkin"}

def typname(t):
    if t >= 0x40: return "NPC"
    return TYPNAMEN.get(t, "?")

def main():
    root = sys.argv[1]
    alle = "--alle" in sys.argv
    ns = lade_walk(TOOL)
    rdts = sorted(glob.glob(os.path.join(root, "STAGE*", "ROOM*.RDT")))
    sce_rows, mem_rows, dyn_rows, abbruch = [], [], [], []
    for p in rdts:
        d = open(p, "rb").read()
        room = os.path.basename(p)[4:8]
        if len(d) < 0x60:
            print("  (uebersprungen: %s %d B)" % (os.path.basename(p), len(d))); continue
        ms, ss = struct.unpack_from("<I", d, 0x40)[0], struct.unpack_from("<I", d, 0x44)[0]
        for sec, tag in ((ms, "main"), (ss, "sub")):
            if sec == 0 or sec >= len(d): continue
            se = ns["rdt_section_end"](d, sec)
            for (o, e, idx) in ns["section_regions"](d, sec, se):
                lbl = f"{tag}{idx:02d}"
                ws = None                     # (kind, idx) des letzten Work_set
                for (pc, op, b) in walk(ns, d, sec + o, sec + e):
                    if op is None:
                        abbruch.append((room, lbl, pc)); break
                    if op == 0x2E:
                        ws = (b[1], b[2])
                    elif op == 0x44:
                        slot, typ, grid, fl = b[1], b[2], b[3], b[4]
                        x, y, z = s16(b, 8), s16(b, 10), s16(b, 12)
                        if alle or (grid & 0x60):
                            sce_rows.append((room, lbl, pc, slot, typ, grid, fl, x, y, z))
                    elif op == 0x34 and b[1] == 0x0C:
                        val = s16(b, 2)
                        if alle or (val & 0x60):
                            mem_rows.append((room, lbl, pc, ws, val))
                    elif op == 0x35 and b[1] == 0x0C:
                        dyn_rows.append((room, lbl, pc, ws, "Member_set2 var %d" % b[2]))
                    elif op == 0x5B and b[2] == 0x0C:
                        dyn_rows.append((room, lbl, pc, ws, "Plc_cnt alu %d var %d" % (b[1], b[3])))
    print("RDTs: %d" % len(rdts))
    print("\n== Sce_em_set (0x44) mit pc[3] & 0x60 ==")
    for r in sce_rows:
        room, lbl, pc, slot, typ, grid, fl, x, y, z = r
        print("  ROOM%s %-6s @Datei 0x%05X  em %2d Typ 0x%02X %-12s grid 0x%02X (&0x60=0x%02X) floor %d  (%d,%d,%d)"
              % (room, lbl, pc, slot, typ, typname(typ), grid, grid & 0x60, fl, x, y, z))
    print("\n== Member_set (0x34) Member 0x0C mit Wert & 0x60 ==")
    for r in mem_rows:
        room, lbl, pc, ws, val = r
        print("  ROOM%s %-6s @Datei 0x%05X  Work_set %s  -> grid 0x%02X (&0x60=0x%02X)"
              % (room, lbl, pc, ("(%d,%d)" % ws) if ws else "-", val & 0xff, val & 0x60))
    print("\n== dynamische Schreiber auf Member 0x0C (Member_set2 / Plc_cnt) ==")
    for r in dyn_rows:
        room, lbl, pc, ws, txt = r
        print("  ROOM%s %-6s @Datei 0x%05X  Work_set %s  %s" % (room, lbl, pc, ("(%d,%d)" % ws) if ws else "-", txt))
    print("\n== Walk-Abbrueche ==")
    for r in abbruch:
        print("  ROOM%s %-6s @0x%05X" % r)

main()
