# -*- coding: utf-8 -*-
"""Runde 35 Spur A (Nachbesserung 1) — Zustandsspur EINES Gegnerplatzes aus RE15_STATE_LOG.

    python zustand_spur.py <state.log> <slot> [von] [bis] [schritt]

Wertet nur die Zeilen NACH dem Raumsprung aus (der Bildzaehler faellt beim Raumwechsel zurueck) und gibt
eine Zeile aus, sobald sich st/ss1/ss2/g/mo/hp aendern, sowie alle <schritt> Bilder (Lage, af).
"""
import re, sys

def main():
    pfad, slot = sys.argv[1], int(sys.argv[2])
    von = int(sys.argv[3]) if len(sys.argv) > 3 else 0
    bis = int(sys.argv[4]) if len(sys.argv) > 4 else 10 ** 9
    schritt = int(sys.argv[5]) if len(sys.argv) > 5 else 20
    ls = open(pfad, encoding='utf-8', errors='replace').read().splitlines()
    idx, prev = 0, -1
    for i, l in enumerate(ls):
        m = re.match(r'F(\d+) ', l)
        if m:
            f = int(m.group(1))
            if f < prev: idx = i
            prev = f
    rx = re.compile(r'\[%d t=([0-9a-f]+) st=(\d+) ss1=(\d+) ss2=(\d+) ss3=\d+ g=([0-9a-f]+) mo=(\d+) af=(\d+) '
                    r'stun=-?\d+ d=\d+ @\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)' % slot)
    last = None
    for l in ls[idx:]:
        m = re.match(r'F(\d+) .*?PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\) pst=(\d+)', l)
        g = rx.search(l)
        if not m or not g: continue
        f = int(m.group(1))
        if f < von or f > bis: continue
        t, st, ss1, ss2, grid, mo, af, x, z, r, hp = g.groups()
        key = (st, ss1, ss2, grid, mo, hp)
        if key != last or f % schritt == 0:
            print('F%d pst=%s [%d t=%s st=%s ss1=%s ss2=%s g=%s mo=%s af=%s @(%s,%s,r%s)] hp=%s' % (
                f, m.group(6), slot, t, st, ss1, ss2, grid, mo, af, x, z, r, hp))
            last = key

if __name__ == '__main__':
    main()
