#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""r30_bau_marken_herkunft.py - Runde 30, Thema F, BAU: Herkunft der Marken nachlesen.

Liest die Herkunftsliste, die der Ermittler mit r30_karten_gen_diag.py erzeugt hat
(marken_herkunft.json), und gibt die Marken aus, deren zid der Generator NICHT kannte
(Rueckfall 0) - dazu auf Wunsch einzelne Lagen.

Aufruf: python r30_bau_marken_herkunft.py <marken_herkunft.json> [blatt,x,y ...]
"""
import json, sys

def main():
    d = json.load(open(sys.argv[1]))
    extra = set()
    for a in sys.argv[2:]:
        p = a.split(',')
        extra.add((int(p[0]), int(p[1]), int(p[2])))
    print("%d Marken im Generatorlauf" % len(d))
    for m in d:
        if not (m.get('zid_fehlt') or (m['pg'], m['mx'], m['my']) in extra):
            continue
        dd = m.get('d', {})
        print("ROOM%s z%d #%d Blatt %d rect %d (%d,%d) seite %d zid %d zid_fehlt %s | "
              "dest %s band %s Trigger-Mitte (%s,%s)"
              % (m['room'], m['zi'], m['idx'], m['pg'], m['r'], m['mx'], m['my'], m['seite'],
                 m['zid'], m.get('zid_fehlt'), dd.get('dest'), dd.get('band'),
                 dd.get('lx'), dd.get('lz')))

if __name__ == '__main__':
    main()
