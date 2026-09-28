#!/usr/bin/env python3
# Runde 30, Thema F: disassembliert einen Ausschnitt aus RE2s MEM_CARD.BIN.
# Das Overlay laedt NICHT bei 0x80100000 wie die STAGE-Overlays, sondern bei
# 0x801BFA18 (gemessen: 19 von 31 jal-Zielen im eigenen Adressraum treffen dort
# einen Funktionsanfang `addiu sp,sp,-N`; dieselbe Basis wie OPENING.BIN).
# Aufruf: python r30_karten_re2_memcard_dis.py <adresse-hex> <anzahl>
import os, sys, struct, importlib.util
REPO = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
spec = importlib.util.spec_from_file_location(
    'rd', os.path.join(REPO, '.claude', 'skills', 're15-psx-disasm', 'scripts', 're2_disasm.py'))
rd = importlib.util.module_from_spec(spec); spec.loader.exec_module(rd)
BASIS = 0x801BFA18
d = open(os.path.join(REPO, 'info', 're2leon', 'COMMON', 'BIN', 'MEM_CARD.BIN'), 'rb').read()
a = int(sys.argv[1], 16); n = int(sys.argv[2])
for i in range(n):
    adr = a + 4 * i
    off = adr - BASIS
    w = struct.unpack_from('<I', d, off)[0]
    print('  %08x: %08x  %s   ; Datei 0x%04X' % (adr, w, rd.dis_one(w, adr), off))
