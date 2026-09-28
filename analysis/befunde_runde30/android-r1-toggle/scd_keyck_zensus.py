#!/usr/bin/env python3
"""Zensus aller Sce_key_ck (0x51, HELD) / 0x52 (FLANKE) in den ausgelieferten RDTs.

Frage: liest irgendein Raumskript das R1-Bit? Im virtuellen Pad-Wort liegt R1 auf
Bit 8 (0x0100) und Bit 10 (0x0400) (Preset-Tabelle @0x80073dbc, Eintraege [8]/[10]).
Ein Skript, das eine solche Maske abfragt, saehe einen gerasteten R1 als "gehalten".

Walker: opcode-exakt ueber re15_port/tools/scd_dump_room.py (Laengen aus
scd_vm.c s_opcode_sizes). Ein Walk, der abbricht, wird GEZAEHLT und gemeldet
(Abdeckung), nicht verschwiegen.
"""
import os, sys, glob, struct, io, contextlib

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
TOOL = os.path.join(ROOT, "re15_port", "tools", "scd_dump_room.py")
src = open(TOOL, encoding="utf-8").read()
src = src.rsplit("\nmain()", 1)[0]          # main() nicht ausfuehren
ns = {}
exec(compile(src, TOOL, "exec"), ns)
u16, u32 = ns["u16"], ns["u32"]
op_size, rdt_section_end, section_regions = ns["op_size"], ns["rdt_section_end"], ns["section_regions"]

fwd_target = ns["fwd_target"]

def walk(d, start, end):
    """Wie dump() im Werkzeug: Ende = erstes Evt_end HINTER dem weitesten Vorwaertsziel.
    Was dahinter in der Region liegt, sind Daten (Nachrichtentext), kein Skript."""
    pc = start; out = []; ok = True; maxf = start
    while pc < end:
        op = d[pc]; sz = op_size(d, pc)
        if sz is None or pc + sz > end:
            ok = False; break
        t = fwd_target(d, pc, op)
        if t is not None and t > maxf: maxf = t
        if op in (0x51, 0x52):
            out.append((pc, op, d[pc+1], u16(d, pc+2)))
        if op == 0x01 and pc + 2 > maxf:
            return out, True
        pc += sz
    return out, ok

def main():
    base = os.path.join(ROOT, "re15_port", "shared_assets", "PSX")
    files = sorted(glob.glob(os.path.join(base, "STAGE*", "ROOM*.RDT")))
    n_reg = n_ok = 0
    masks = {}
    r1_hits = []
    for f in files:
        d = open(f, "rb").read()
        if len(d) < 0x60: continue
        ms, ss = u32(d, 0x40), u32(d, 0x44)
        for sec, tag in ((ms, "main"), (ss, "sub")):
            if sec == 0 or sec >= len(d): continue
            se = rdt_section_end(d, sec)
            for (o, e, idx) in section_regions(d, sec, se):
                n_reg += 1
                hits, ok = walk(d, sec + o, sec + e)
                if ok: n_ok += 1
                for (pc, op, par, m) in hits:
                    masks.setdefault((op, m), []).append((os.path.basename(f), "%s%02d" % (tag, idx), pc))
                    if m & 0x0500:
                        r1_hits.append((os.path.basename(f), "%s%02d" % (tag, idx), pc, op, par, m))
    print("RDT-Dateien: %d, SCD-Regionen: %d, davon sauber bis Evt_end/Regionsende gelaufen: %d" % (len(files), n_reg, n_ok))
    print("\nMaske   Opcode  Anzahl  Beispiel")
    for (op, m), lst in sorted(masks.items(), key=lambda kv: (kv[0][1], kv[0][0])):
        print("0x%04x  0x%02x    %5d   %s %s @0x%05X" % (m, op, len(lst), lst[0][0], lst[0][1], lst[0][2]))
    print("\nAbfragen mit R1-Bit (virtuell 0x0100 oder 0x0400): %d" % len(r1_hits))
    for h in r1_hits:
        print("  %s %s @0x%05X op=0x%02x param=%d maske=0x%04x" % h)

main()
