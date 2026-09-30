#!/usr/bin/env python3
"""Spur C (Runde 34 Nacht): alle sce_espr_on2 (0x64) / sce_espr_kill2 (0x65) in RE2 ROOM2130.RDT
auflisten — mit Sub, Datei-Offset und den dekodierten Feldern des Handlers @0x80056644:
  a0 = (pc[2] << 24) | (pc[3] << 16) | u16(pc[6..7])   -> ESP-Id | Unterindex | scale16
  Unterindex: &7 = Zeilenstrom (Sub-Offset-Tabelle), >>3 = CLUT-Zeilen-Addend (@0x8001c9e0-fc)
  Lage = s16 pc[8..9], pc[10..11], pc[12..13]; Platz = pc[1] (sce_espr_kill2 `65 nn`).
Benutzt den RE2-Walker aus analysis/nutzer_batch_2026-08-27/tools (Laengen aus @0x800a74c8)."""
import os, sys, struct
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, "analysis", "nutzer_batch_2026-08-27", "tools"))
from re2_scd_walk import scd_blocks, block_end, walk
p = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM2130.RDT")
d = open(p, "rb").read()
offs = struct.unpack_from("<23I", d, 8)
for name, base, subs in scd_blocks(d):
    for i, so in enumerate(subs):
        s = base + so; e = block_end(d, base, subs, i, offs)
        ops, st = walk(d, s, e, last=(i + 1 == len(subs)))
        for (a, op, ln) in ops:
            if op == 0x64:
                pc = d[a:a + ln]
                esp, sub = pc[2], pc[3]
                sc = struct.unpack_from("<H", pc, 6)[0]
                x, y, z = struct.unpack_from("<hhh", pc, 8)
                print("%s sub%02d @0x%05X  %s  sce_espr_on2 platz=0x%02X esp=0x%02X unter=0x%02X"
                      " (strom %d, CLUT+%d) scale16=0x%04X lage=(%d,%d,%d)"
                      % (name, i, a, pc.hex(" "), pc[1], esp, sub, sub & 7, sub >> 3, sc, x, y, z))
            elif op == 0x65:
                print("%s sub%02d @0x%05X  %s  sce_espr_kill2 platz=0x%02X"
                      % (name, i, a, d[a:a + ln].hex(" "), d[a + 1]))
