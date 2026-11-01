#!/usr/bin/env python3
"""Rohscan: jede Stelle 44 ss TT in den SCD-Abschnitten einer RDT (main+sub, RDT-Kopf +0x40/+0x44), ohne Walker.
Filter: TT in Typen, ss<=0x20, Koordinaten-Feld +8/+12 belegt, +7 bekannt. Liefert auch Fundstellen in Regionen, an
denen der lineare Walker abbricht. Aufruf: em_roh.py <PSX-Wurzel> [--typen 0x30,0x36]"""
import sys, os, glob, struct, argparse
ap = argparse.ArgumentParser()
ap.add_argument('wurzel')
ap.add_argument('--typen', default='0x30,0x36')
ap.add_argument('--locker', action='store_true', help='ohne Pruefung auf +7 == 0xFF')
a = ap.parse_args()
TY = {int(t, 0) for t in a.typen.split(',')}
u32 = lambda b, o: struct.unpack_from('<I', b, o)[0]
s16 = lambda b, o: struct.unpack_from('<h', b, o)[0]
n_ges = 0
print('Datei; Offset; Bereich; slot; typ; +3; +4; +7; x; y; z; roh20')
for s in range(1, 8):
    for p in sorted(glob.glob(f'{a.wurzel}/STAGE{s}/ROOM*.RDT')):
        d = open(p, 'rb').read()
        n = os.path.basename(p)[4:8]
        if len(d) < 0x60:
            print(f'{n}; ZU KLEIN ({len(d)} Byte)')
            continue
        ms, ss = u32(d, 0x40), u32(d, 0x44)
        off = sorted(set(u32(d, o) for o in range(0x08, 0x60, 4) if 0 < u32(d, o) <= len(d)))
        def ende(sec):
            g = [x for x in off if x > sec]
            return g[0] if g else len(d)
        for sec, tag in ((ms, 'main'), (ss, 'sub')):
            if not sec or sec >= len(d):
                continue
            e = ende(sec)
            for o in range(sec, e - 20):
                if d[o] == 0x44 and d[o + 2] in TY and d[o + 1] <= 0x20 and (a.locker or d[o+7] == 0xff):
                    n_ges += 1
                    print(f'{n}; 0x{o:X}; {tag}[0x{sec:X}..0x{e:X}]; {d[o+1]}; 0x{d[o+2]:02X}; 0x{d[o+3]:02X}; 0x{d[o+4]:02X}; 0x{d[o+7]:02X}; {s16(d,o+8)}; {s16(d,o+10)}; {s16(d,o+12)}; {d[o:o+20].hex(" ")}')
print('Summe', n_ges)
