#!/usr/bin/env python3
"""Sce_em_set-Zensus Typ 0x30..0x37: (a) opcode-exakter Lauf (scd_dump_room.py-Tabelle), (b) Rohsuche in den SCD-Abschnitten (main/sub bis Abschnittsende)
nach 44 ss tt mit tt in TYPEN und Byte +7 == 0xFF (Form aller Opcode-Funde). Rohfunde ausserhalb (a) werden markiert.
Aufruf: em_zensus2.py <PSX-Wurzel>"""
import sys, os, glob, struct
REPO = r'C:/workspace/git/reAi_v2'
TOOL = open(REPO + '/re15_port/tools/scd_dump_room.py', encoding='utf-8').read().replace('\nmain()\n', '\n')
ns = {}
exec(compile(TOOL, 'scd', 'exec'), ns)
op_size, rdt_section_end, section_regions = ns['op_size'], ns['rdt_section_end'], ns['section_regions']
u32 = lambda b, o: struct.unpack_from('<I', b, o)[0]
s16 = lambda b, o: struct.unpack_from('<h', b, o)[0]
TYPEN = set(range(0x30, 0x38))
wurzel = sys.argv[1]
ges = 0; abbr = 0
for s in range(1, 8):
    for p in sorted(glob.glob(f'{wurzel}/STAGE{s}/ROOM*.RDT')):
        d = open(p, 'rb').read()
        n = os.path.basename(p)[4:8]
        if len(d) < 0x48:
            print(f'{n}; Datei {len(d)} B, uebersprungen'); continue
        exakt = {}
        bereiche = []
        for sec, tag in ((u32(d, 0x40), 'main'), (u32(d, 0x44), 'sub')):
            if sec == 0 or sec >= len(d): continue
            se = rdt_section_end(d, sec)
            bereiche.append((sec, se, tag))
            for (o, e, idx) in section_regions(d, sec, se):
                pc, end = sec + o, sec + e
                while pc < end:
                    sz = op_size(d, pc)
                    if sz is None or pc + sz > end:
                        abbr += 1; break
                    if d[pc] == 0x44: exakt[pc] = f'{tag}{idx:02d}'
                    pc += sz
        for pc in range(len(d) - 22):
            if d[pc] == 0x44 and d[pc+2] in TYPEN and d[pc+7] == 0xFF:
                imscd = [t for (a_, e_, t) in bereiche if a_ <= pc < e_]
                if pc not in exakt and not imscd:
                    continue
                ges += 1
                wo = exakt.get(pc, 'ROH-im-SCD-%s-nicht-im-Lauf' % imscd[0])
                print(f'{n}; {wo}; 0x{pc:X}; slot {d[pc+1]}; typ 0x{d[pc+2]:02X}; pose 0x{d[pc+3]:02X}; '
                      f'xyz {s16(d,pc+8)},{s16(d,pc+10)},{s16(d,pc+12)}; {d[pc:pc+22].hex(" ")}')
print('Summe', ges, 'Lauf-Abbrueche', abbr)
