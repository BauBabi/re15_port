#!/usr/bin/env python3
"""ST80-A: geprüfte Disc-Zeilen (40%-Disc) direkt aus den RDT-Bytes, plus Schreiber der Bedingungs-Flags.

Aufruf aus der Repo-Wurzel des Hauptbaums (dort liegt Version_40%/):
    python <worktree>/analysis/story80/abgleich/werkzeuge/disc_zeilen.py <worktree>
Dekoder: analysis/story_begegnungen/werkzeuge/jp.py. Opcode-Längen: re15_port/tools/scd_walk_lib.py.
Nachrichtenblock: Zeiger u32 @RDT+0x3C, Tabelle u16 (erste Zahl/2 = Anzahl).
"""
import sys, struct, os, glob
WT = sys.argv[1]
sys.path.insert(0, os.path.join(WT, 'analysis/story_begegnungen/werkzeuge'))
sys.path.insert(0, os.path.join(WT, 're15_port/tools'))
import jp, scd_walk_lib as L

def msgs(p):
    d = open(p, 'rb').read(); ms = struct.unpack_from('<I', d, 0x3C)[0]
    n = (d[ms] | d[ms+1] << 8) // 2
    tbl = [d[ms+2*i] | d[ms+2*i+1] << 8 for i in range(n)]
    for i, o in enumerate(tbl):
        st = ms + o; en = ms + tbl[i+1] if i + 1 < n else len(d)
        yield i, st, jp.decode(d, st, min(en, st+600)), d[st:min(en, st+48)].hex(' ')

PRUEF = [('STAGE3/ROOM30E1.RDT', [3, 5, 7, 12, 15, 19, 20, 21, 22]),
         ('STAGE3/ROOM3071.RDT', [0, 1, 2, 3, 4, 9, 11]),
         ('STAGE4/ROOM4001.RDT', [12, 15, 16]),
         ('STAGE5/ROOM5011.RDT', [0, 23, 24, 26, 29, 30, 33, 35, 37]),
         ('STAGE4/ROOM4010.RDT', [11, 16, 21, 22]),
         ('STAGE3/ROOM3091.RDT', [9])]
for room, ids in PRUEF:
    for i, st, t, h in msgs('Version_40%/PSX/' + room):
        if i in ids:
            print(f'{room} #{i:2d} @0x{st:05X}  {t}')
            print(f'      {h}')

print('\nSchreiber Set(3,x)=v fuer x in 0..5 (Opcode 0x22 Bank Index Wert):')
for p in sorted(glob.glob('Version_40%/PSX/STAGE*/ROOM*.RDT')):
    d = open(p, 'rb').read()
    if len(d) < 0x60: continue
    for sec, tag in ((L.u32(d, 0x40), 'main'), (L.u32(d, 0x44), 'sub')):
        if sec == 0 or sec >= len(d): continue
        se = L.rdt_section_end(d, sec)
        for (o, e, idx) in L.section_regions(d, sec, se):
            for ev in L.walk_ops(d, sec+o, sec+e):
                pc, op = ev[0], ev[1]
                if op == 0x22 and d[pc+1] == 3 and d[pc+2] <= 5:
                    print(f'  {os.path.basename(p)} {tag}{idx:02d} @0x{pc:04X}  {d[pc:pc+4].hex(" ")}  Set(3,{d[pc+2]})={d[pc+3]}')
