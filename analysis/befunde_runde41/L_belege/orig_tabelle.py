#!/usr/bin/env python3
"""Runde 41 Spur L: erzeugt re15_port/tests/unit/test_r41_spinnenanim_orig.inc aus der Original-GDB-Messung
L_belege/orig_2060_gdb.log (gdb_shots.py, MZD-Disc, ROOM2060, Spinne e0).

Zeile nK des Logs = Stand NACH Takt K-1. Je Takt K = 62..137 (Zeilen n63..n138) eine Tabellenzeile:
  Lage x/y/z, rot x/y/z, Clip +0x94 / Bild +0x95 / Ueberblendung +0x8f NACH dem Takt, Figur-Matrix +0x20 (9),
  Teil-0-Weltmatrix (9), Teil-0-Weltlage (3), Wurzel-Versatz Teil 0 +0x38 (3).
Aufruf: python orig_tabelle.py <orig_2060_gdb.log> <ausgabe.inc>
"""
import re
import sys

src, dst = sys.argv[1], sys.argv[2]
zeilen = {}
for l in open(src):
    m = re.match(r'(\d+) n(\d+) cut(-?\d+)', l)
    if not m:
        continue
    n = int(m.group(2))
    e = re.search(r'\| e0 t25 s(\d+)/(\d+)/(\d+)/(\d+) g([0-9a-f]+) @(-?\d+),(-?\d+),(-?\d+) r(-?\d+),(-?\d+),(-?\d+) '
                  r'c(\d+)/(\d+) f8f=(\d+) gy(-?\d+) M\[([^\]]*)\] P0W\[([^\]]*)\] P0T\(([^)]*)\) P0root\(([^)]*)\)', l)
    if not e:
        continue
    g = e.groups()
    zeilen[n] = dict(st=int(g[0]), s1=int(g[1]), s2=int(g[2]), s3=int(g[3]), grid=int(g[4], 16),
                     pos=[int(g[5]), int(g[6]), int(g[7])], rot=[int(g[8]), int(g[9]), int(g[10])],
                     clip=int(g[11]), bild=int(g[12]), f8f=int(g[13]), gy=int(g[14]),
                     M=[int(v) for v in g[15].split(',')], P0W=[int(v) for v in g[16].split(',')],
                     P0T=[int(v) for v in g[17].split(',')], root=[int(v) for v in g[18].split(',')])

out = []
out.append('/* ERZEUGT von analysis/befunde_runde41/L_belege/orig_tabelle.py aus L_belege/orig_2060_gdb.log')
out.append(' * (Original RE1.5, MZD-Disc, DuckStation-GDB, ROOM2060 Spinne e0, Haltepunkt 0x8001a50c je Takt).')
out.append(' * Je Zeile: Takt K (Stand NACH Takt K = Logzeile n(K+1)); Phase +0x7; Lage; rot; Clip +0x94, Bild +0x95,')
out.append(' * Ueberblendung +0x8f NACH dem Takt; Boden-Referenz +0x1ba; Figur-Matrix +0x20; Teil-0-Weltmatrix +0x40;')
out.append(' * Teil-0-Weltlage +0x54;')
out.append(' * Wurzel-Versatz Teil 0 +0x38. Nicht von Hand aendern. */')
anz = 0
for K in range(61, 138):
    z = zeilen.get(K + 1)
    if not z:
        continue
    anz += 1
    out.append('    { %3d, %d, %d, {%d,%d,%d}, {%d,%d,%d}, %2d, %2d, %d, %d, {%s}, {%s}, {%s}, {%s} },' % (
        K, z['st'], z['s3'], *z['pos'], *z['rot'], z['clip'], z['bild'], z['f8f'], z['gy'],
        ','.join(str(v) for v in z['M']), ','.join(str(v) for v in z['P0W']),
        ','.join(str(v) for v in z['P0T']), ','.join(str(v) for v in z['root'])))
open(dst, 'w', newline='\n').write('\n'.join(out) + '\n')
print('%d Zeilen -> %s' % (anz, dst))
