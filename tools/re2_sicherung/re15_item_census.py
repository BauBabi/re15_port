#!/usr/bin/env python3
"""re15_item_census.py - opcode-exakter Item-Zensus ueber alle RE1.5-RDTs.

Der rohe Bytescan nach Opcode 0x50 taugt bei RE1.5 NICHT: die RDT-Textbloecke
enthalten massenhaft 0x50 ('m'), und die Treffer sehen teilweise plausibel aus
(z.B. ROOM1050+0x0F4A dekodiert zu "...s of riots and cannib..."). Deshalb wird
hier ueber den SCD-Blockwalker gelaufen.

Walker + Laengentabelle: analysis/nutzer_batch_2026-08-27/tools/re15_scd_walk.py
(Laengen byte-true aus re15_port/engine/src/scd_vm.c:166 / PTR_LAB_800744a8).
Feldlage von Item_aot_set (0x50) nach re15_port/engine/src/scd_vm.c:3800-3841:
    Kurzform  (pc[3]&0x80 == 0, 22 B): item=pc[14] amt=pc[16] tk=pc[18] prop=pc[20]
    Langform  (pc[3]&0x80 != 0, 30 B): item=pc[22] amt=pc[24] tk=pc[26] prop=pc[28]

Aufruf: re15_item_census.py [<id> ...]
"""
import sys, os, glob, struct, collections

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, os.path.join(REPO, "analysis", "nutzer_batch_2026-08-27", "tools"))
sys.path.insert(0, HERE)
import re15_scd_walk as W
import re15_items as I

want = set(int(a, 0) for a in sys.argv[1:])
NM = {i: t for i, _, t in I.names()}

hist = collections.Counter()
bad = []
nblocks = 0
rooms = sorted(glob.glob(os.path.join(REPO, "info", "Re1.5", "PSX", "STAGE*", "ROOM*.RDT")))
for path in rooms:
    d = open(path, "rb").read()
    if len(d) < 0x60:
        continue
    room = os.path.splitext(os.path.basename(path))[0]
    try:
        tbl = struct.unpack_from("<24I", d, 0x20)
        blks = W.blocks(d)
    except Exception:
        continue
    for name, base, subs in blks:
        for i, o in enumerate(subs):
            start = base + o
            end = base + subs[i + 1] if i + 1 < len(subs) else None
            if end is None:
                cand = [x for x in tbl if start < x <= len(d)]
                end = min(cand) if cand else len(d)
            nblocks += 1
            pc = start
            while pc < end:
                op = d[pc]
                try:
                    n = W.oplen(d, pc)
                except IndexError:
                    break
                if n == 0 or pc + n > end:
                    bad.append("%s %s%02d @0x%X op=0x%02X" % (room, name, i, pc, op))
                    break
                if op == 0x50:
                    lf = bool(d[pc + 3] & 0x80)
                    it = d[pc + (22 if lf else 14)]
                    amt = d[pc + (24 if lf else 16)]
                    tk = d[pc + (26 if lf else 18)]
                    prop = d[pc + (28 if lf else 20)]
                    hist[it] += 1
                    if not want or it in want:
                        print("%-12s %s%02d +0x%06X  %s  id=0x%02X %-20s n=%-3d "
                              "tk=%-3d prop=%-3d  %s"
                              % (room, name, i, pc, "LANG" if lf else "kurz", it,
                                 NM.get(it, "?"), amt, tk, prop,
                                 " ".join("%02x" % b for b in d[pc:pc + n])))
                pc += n

print()
print("--- %d RDTs, %d SCD-Bloecke, %d Item_aot_set, %d verschiedene Ids, "
      "%d Bloecke abgebrochen ---"
      % (len(rooms), nblocks, sum(hist.values()), len(hist), len(bad)))
for iid in sorted(hist):
    print("  id 0x%02X  x%-3d %s" % (iid, hist[iid], NM.get(iid, "?")))
if bad:
    print("abgebrochen:", bad[:25])
