#!/usr/bin/env python3
"""tab2d.py <bin> <addr_hex> <rows> <cols> [--rowbytes N] [--rows-from K]
Gibt eine 2D-Zeigertabelle aus (Zeile = +0x5-Waffen-Id, Spalte = +0x6). Liest ueber load() der
Skill-Skripte (nie Offsets selbst rechnen). <bin> = PSX.EXE | STAGEn.BIN | DEBUG.BIN (RE1.5, Default-
Verzeichnis info/Re1.5/PSX/BIN) oder re2:<OVERLAY.BIN> fuer info/re2leon (re2_disasm.load)."""
import sys, os, struct, importlib.util
here = os.path.dirname(os.path.abspath(__file__))
root = os.path.abspath(os.path.join(here, '..', '..', '..'))
def modload(name):
    p = os.path.join(root, '.claude', 'skills', 're15-psx-disasm', 'scripts', name)
    spec = importlib.util.spec_from_file_location(name[:-3], p)
    m = importlib.util.module_from_spec(spec); spec.loader.exec_module(m); return m
a = sys.argv[1:]
binn, addr, rows, cols = a[0], int(a[1], 16), int(a[2]), int(a[3])
rowbytes = int(a[a.index('--rowbytes') + 1], 0) if '--rowbytes' in a else cols * 4
start = int(a[a.index('--rows-from') + 1], 0) if '--rows-from' in a else 0
if binn.startswith('re2:'):
    m = modload('re2_disasm.py'); b = binn[4:]
else:
    m = modload('re15_disasm.py'); b = None if binn == 'PSX.EXE' else binn
data, fo, path = m.load(addr, b)
print(f'# {path} @0x{addr:08x}  Zeilen {start}..{start+rows-1} x {cols} Spalten (Zeile = {rowbytes} B)')
for r in range(start, start + rows):
    ws = [struct.unpack_from('<I', data, fo(addr + r*rowbytes + c*4))[0] for c in range(cols)]
    print(f'  [{r:2d}] @0x{addr + r*rowbytes:08x}: ' + ' '.join(f'{w:08x}' for w in ws))
