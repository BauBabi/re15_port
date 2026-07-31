#!/usr/bin/env python
"""gen_debug_jump_table.py — erzeugt include/debug_jump_table.h aus DEBUG.BIN.

Satz = 26 Byte (13 Halbwoerter): 14 Byte Name + 12 Byte Nutzlast. Das Menue adressiert ihn mit
637*stage + 13*idx Halbwoertern (@0x8001476C-0x800147A8); Tabellenanfang in DEBUG.BIN = 0x2642,
verankert an BRIEFING ROOM = Index 0x14 (Hardware: zeigt 0x114 = Port-ROOM1140).
"""
import io, sys

BASE, REC, SLOTS, STAGES = 0x2642, 26, 49, 6
src = sys.argv[1] if len(sys.argv) > 1 else "info/Re1.5/PSX/BIN/DEBUG.BIN"
dst = sys.argv[2] if len(sys.argv) > 2 else "re15_port/include/debug_jump_table.h"
d = open(src, "rb").read()
rows = [[d[BASE + (st * 637 + i * 13) * 2:][:14].split(b"\x00")[0].decode("ascii", "replace").strip()
         for i in range(SLOTS)] for st in range(STAGES)]
print("%s: %s belegte Slots je Stage" % (src, [sum(1 for n in r if n) for r in rows]))
