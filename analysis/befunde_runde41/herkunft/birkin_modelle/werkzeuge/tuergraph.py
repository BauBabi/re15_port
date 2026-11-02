#!/usr/bin/env python3
"""Tuerziele (Door_aot_set 0x3B, pc[22] = Stage 0-basiert, pc[23] = Raum; Feldlage re15_port/engine/src/scd_vm.c:4045,
aot_common.c:688) je RDT eines Baums, opcode-exakt gelaufen. Aufruf: tuergraph.py <PSX-Wurzel> <Raum-Praefix...>"""
import sys, os, glob, struct
REPO = r'C:/workspace/git/reAi_v2'
TOOL = open(REPO + '/re15_port/tools/scd_dump_room.py', encoding='utf-8').read().replace('\nmain()\n', '\n')
ns = {}; exec(compile(TOOL, 'scd', 'exec'), ns)
op_size, rdt_section_end, section_regions = ns['op_size'], ns['rdt_section_end'], ns['section_regions']
u32 = lambda b, o: struct.unpack_from('<I', b, o)[0]
w = sys.argv[1]; pre = sys.argv[2:]
for p in sorted(glob.glob(f'{w}/STAGE*/ROOM*.RDT')):
    n = os.path.basename(p)[4:8]
    if pre and not any(n.startswith(x) for x in pre): continue
    d = open(p, 'rb').read()
    if len(d) < 0x48: continue
    ziele = []
    for sec in (u32(d, 0x40), u32(d, 0x44)):
        if sec == 0 or sec >= len(d): continue
        se = rdt_section_end(d, sec)
        for (o, e, idx) in section_regions(d, sec, se):
            pc, end = sec + o, sec + e
            while pc < end:
                sz = op_size(d, pc)
                if sz is None or pc + sz > end: break
                if d[pc] == 0x3B:
                    ziele.append(f'{d[pc+22]+1}{d[pc+23]:02X}@0x{pc:X}')
                pc += sz
    print(n, ' '.join(ziele))
