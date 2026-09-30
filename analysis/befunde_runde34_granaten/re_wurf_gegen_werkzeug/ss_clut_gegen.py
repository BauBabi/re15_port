#!/usr/bin/env python3
"""Gegenpruefung R34: liest aus Savestates die Laufzeit-Effektkoepfe (0x800b2248[id] -> CLUT/TPAGE @+4/+6) und die
VRAM-CLUT-Zeilen (272,481/483/491/492) und vergleicht mit CORE00.ESP bzw. DATA/TEX.TIM (Datei-Offset aus dem Dossier)."""
import sys, os, struct, glob
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', '..', '.claude', 'skills', 're15-savestate-ghidra', 'scripts'))
from re15_ss import Ram
REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
tex = open(os.path.join(REPO, 're15_port/shared_assets/PSX/DATA/TEX.TIM'), 'rb').read()
FILEROWS = {481: 0x74, 483: 0xF4, 491: 0x2F4, 492: 0x334}
for n in ('room1140_entry.sav', 'equip_test.sav', 'room1090_orig.sav', 'doorA_square.sav'):
    p = os.path.join(REPO, 'stage_saves', n)
    if not os.path.exists(p): continue
    r = Ram(p)
    def rd(a, k):
        o = r.base + (a - 0x80000000); return r.blob[o:o + k]
    out = [n]
    for eid in (3, 4):
        h = struct.unpack('<I', rd(0x800b2248 + 4 * eid, 4))[0]
        sub = struct.unpack('<I', rd(0x800b22d4 + 4 * eid, 4))[0]
        ca, cb, clut, tp = struct.unpack('<HHHH', rd(h, 8))
        out.append('eff%d hdr=%08x sub=%08x ca=%d cb=%d CLUT=%04x TPAGE=%04x' % (eid, h, sub, ca, cb, clut, tp))
    print(' | '.join(out))
    vb = r.vram_base
    for y, fo in FILEROWS.items():
        o = vb + (y * 1024 + 272) * 2
        v = r.blob[o:o + 32]
        f = tex[fo:fo + 32]
        print('   VRAM(272,%d) %s  TEX.TIM@%#x %s  %s' % (y, v.hex(' '), fo, f.hex(' '), 'GLEICH' if v == f else 'ABWEICHEND'))
