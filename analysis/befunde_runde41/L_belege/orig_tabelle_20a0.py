#!/usr/bin/env python3
"""Runde 41 Spur L: erzeugt re15_port/tests/unit/test_r41_spinnenanim_20a0.inc aus der Runde-38-Original-Messung
analysis/befunde_runde38/R_belege/orig_20a0_schacht.txt (DuckStation-GDB, MZD-Disc, ROOM20A0, Spinne e0 floor 3,
Spieler (21500,-3600,-24000), Cut 4 gesetzt; Haltepunkt 0x8001a50c je Takt).

Zeile nK = Stand NACH Takt K-1. Ausgabe je Zeile: rel = Takt seit dem ersten Takt mit Phase 1 (Zeile n42 -> rel 0),
Phase +0x7, rot x/y/z, Clip +0x94, Bild +0x95 nach dem Takt, y, +0x1ba; bis zur Landung (Zustand != 4) + 1.
Aufruf: python orig_tabelle_20a0.py <orig_20a0_schacht.txt> <ausgabe.inc>
"""
import re
import sys

src, dst = sys.argv[1], sys.argv[2]
out = ['/* ERZEUGT von analysis/befunde_runde41/L_belege/orig_tabelle_20a0.py aus',
       ' * analysis/befunde_runde38/R_belege/orig_20a0_schacht.txt (Original RE1.5, ROOM20A0, Spinne e0, Runde 38).',
       ' * Je Zeile: rel Takt seit Phase 1, Zustand, Phase, rot x/y/z, Clip/Bild nach dem Takt, y, +0x1ba. Nicht von Hand aendern. */']
erst = None
anz = 0
for l in open(src):
    m = re.match(r'(\d+) n(\d+) cut(-?\d+)', l)
    if not m:
        continue
    n = int(m.group(2))
    e = re.search(r'\| e0 t25 s(\d+)/(\d+)/(\d+)/(\d+) g(\w+) f(\d+) @(-?\d+),(-?\d+),(-?\d+) r(-?\d+),(-?\d+),(-?\d+) '
                  r'c(\d+)/(\d+) gy(-?\d+)', l)
    if not e:
        continue
    g = e.groups()
    if erst is None:
        if g[3] == '1':
            erst = n
        else:
            continue
    rel = n - erst
    out.append('    { %3d, %d, %d, {%s,%s,%s}, %2s, %2s, %s, %s },' % (rel, int(g[0]), int(g[3]), g[9], g[10], g[11],
                                                                      g[12], g[13], g[7], g[14]))
    anz += 1
    if g[0] != '4':
        break
open(dst, 'w', newline='\n').write('\n'.join(out) + '\n')
print('%d Zeilen -> %s (erste Phase-1-Zeile n%d)' % (anz, dst, erst))
