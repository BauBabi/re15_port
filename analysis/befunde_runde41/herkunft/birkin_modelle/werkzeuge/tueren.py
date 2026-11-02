#!/usr/bin/env python3
"""Door_aot_set je Raum eines Baums (Opcode 0x3B, 32/40 B): Ziel Stage/Raum/Variante + Bedingungen als Kontext. Aufruf: tueren.py <PSX-Wurzel> <Stages>"""
import sys, os, glob, struct
REPO = r'C:/workspace/git/reAi_v2'
TOOL = open(REPO + '/re15_port/tools/scd_dump_room.py', encoding='utf-8').read().replace('\nmain()\n', '\n')
ns = {}; exec(compile(TOOL, 'scd', 'exec'), ns)
op_size, rdt_section_end, section_regions, u16, u32, s16 = (ns[k] for k in ('op_size', 'rdt_section_end', 'section_regions', 'u16', 'u32', 's16'))
w, st = sys.argv[1], sys.argv[2]
for s in st:
    for p in sorted(glob.glob(f'{w}/STAGE{s}/ROOM*.RDT')):
        d = open(p, 'rb').read()
        n = os.path.basename(p)[4:8]
        if len(d) < 0x60: continue
        ms, ss = u32(d, 0x40), u32(d, 0x44)
        out = []
        for sec, tag in ((ms, 'main'), (ss, 'sub')):
            if not sec or sec >= len(d): continue
            se = rdt_section_end(d, sec)
            for (o, e, idx) in section_regions(d, sec, se):
                pc = sec + o; end = sec + e
                while pc < end:
                    op = d[pc]; sz = op_size(d, pc)
                    if sz is None or pc + sz > end: break
                    if op == 0x3B:
                        r = d[pc:pc+sz]
                        # +22 = stage/room Byte-Paar laut Beispiel 3b .. 10 f5 00 00 9a c5 00 08 [st][room][cut][..]
                        out.append(f'{tag}{idx:02d}@0x{pc:X}:' + r[22:32].hex())
                    pc += sz
        print(n, ' '.join(out))
