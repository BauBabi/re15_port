#!/usr/bin/env python3
"""feld_xref.py <bin> <von_hex> <bis_hex> <off_dez_oder_hex> [...] — listet alle Lade-/Speicher-
Instruktionen mit Offset <off> (z.B. 477 = +0x1dd) im Bereich [von,bis) eines Binaries (RE1.5 via
re15_disasm.load; 're2:<BIN>' via re2_disasm.load). Nur Offset-Match, Basisregister wird mit ausgegeben."""
import sys, os, struct, importlib.util
here = os.path.dirname(os.path.abspath(__file__))
root = os.path.abspath(os.path.join(here, '..', '..', '..'))
def modload(name):
    p = os.path.join(root, '.claude', 'skills', 're15-psx-disasm', 'scripts', name)
    spec = importlib.util.spec_from_file_location(name[:-3], p)
    m = importlib.util.module_from_spec(spec); spec.loader.exec_module(m); return m
a = sys.argv[1:]
binn, lo, hi = a[0], int(a[1], 16), int(a[2], 16)
offs = [int(x, 0) for x in a[3:]]
if binn.startswith('re2:'): m = modload('re2_disasm.py'); b = binn[4:]
else: m = modload('re15_disasm.py'); b = None if binn == 'PSX.EXE' else binn
data, fo, path = m.load(lo, b)
OPS = {0x20:'lb',0x21:'lh',0x23:'lw',0x24:'lbu',0x25:'lhu',0x28:'sb',0x29:'sh',0x2b:'sw'}
R = m.REGS
for ad in range(lo, hi, 4):
    try: w = struct.unpack_from('<I', data, fo(ad))[0]
    except struct.error: break
    op = w >> 26
    if op not in OPS: continue
    imm = w & 0xffff; simm = imm - 0x10000 if imm & 0x8000 else imm
    if simm in offs:
        rs = (w >> 21) & 31; rt = (w >> 16) & 31
        print(f'  {ad:08x}: {OPS[op]} {R[rt]},{simm}({R[rs]})')
