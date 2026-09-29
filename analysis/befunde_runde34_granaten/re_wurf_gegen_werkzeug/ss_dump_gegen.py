#!/usr/bin/env python3
"""Gegenpruefung R34: RAM-Bereich aus Savestate als Binaerdatei schreiben (fuer Disassembly von Laufzeit-Code).
Aufruf: ss_dump_gegen.py <sav> <addr> <len> <out.bin>"""
import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', '..', '.claude', 'skills', 're15-savestate-ghidra', 'scripts'))
from re15_ss import Ram
r = Ram(sys.argv[1]); a = int(sys.argv[2], 16); n = int(sys.argv[3], 0)
o = r.base + (a - 0x80000000)
open(sys.argv[4], 'wb').write(r.blob[o:o + n])
print('ok', hex(a), n)
