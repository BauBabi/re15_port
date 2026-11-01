#!/usr/bin/env python3
"""Sce_em_set-Zensus fuer Typ 0x30/0x36 (und weitere per --typen) ueber ALLE RDT eines Baums.
Laeuft die SCD-Regionen opcode-exakt (Tabelle re15_port/tools/scd_dump_room.py) UND sucht roh nach dem Muster,
damit Fundstellen in nicht erreichten Regionen ebenfalls auftauchen.
Aufruf: em_zensus.py <PSX-Wurzel> [--typen 0x30,0x36]
Ausgabe: Datei  Sektion  Offset  Bytes(+0..+19)  slot typ pose flag x y z"""
import sys, os, glob, struct, argparse
REPO = r'C:/workspace/git/reAi_v2'
TOOL = open(REPO + '/re15_port/tools/scd_dump_room.py', encoding='utf-8').read().replace('\nmain()\n', '\n')
ns = {}
exec(compile(TOOL, 'scd', 'exec'), ns)
op_size, rdt_section_end, section_regions, u16, u32, s16 = (ns[k] for k in
    ('op_size', 'rdt_section_end', 'section_regions', 'u16', 'u32', 's16'))

ap = argparse.ArgumentParser()
ap.add_argument('wurzel')
ap.add_argument('--typen', default='0x30,0x36')
a = ap.parse_args()
TYPEN = {int(t, 0) for t in a.typen.split(',')}
ges = 0
print('Datei; Abschnitt; Offset; slot; typ; +3; +4; +5..7; x; y; z; Rohbytes')
for s in range(1, 8):
    for p in sorted(glob.glob(f'{a.wurzel}/STAGE{s}/ROOM*.RDT')):
        d = open(p, 'rb').read()
        n = os.path.basename(p)[4:8]
        ms, ss = u32(d, 0x40), u32(d, 0x44)
        for sec, tag in ((ms, 'main'), (ss, 'sub')):
            if sec == 0 or sec >= len(d):
                continue
            se = rdt_section_end(d, sec)
            for (o, e, idx) in section_regions(d, sec, se):
                pc = sec + o
                end = sec + e
                while pc < end:
                    op = d[pc]
                    sz = op_size(d, pc)
                    if sz is None or pc + sz > end:
                        print(f'{n}; {tag}{idx:02d}; 0x{pc:X}; WALK-ABBRUCH op=0x{op:02X}')
                        break
                    if op == 0x44 and d[pc + 2] in TYPEN:
                        ges += 1
                        x, y, z = s16(d, pc + 8), s16(d, pc + 10), s16(d, pc + 12)
                        print(f'{n}; {tag}{idx:02d}; 0x{pc:X}; {d[pc+1]}; 0x{d[pc+2]:02X}; 0x{d[pc+3]:02X}; 0x{d[pc+4]:02X}; {d[pc+5:pc+8].hex()}; {x}; {y}; {z}; {d[pc:pc+20].hex(" ")}')
                    pc += sz
print('Summe', ges)
