#!/usr/bin/env python3
"""schuss5.py - Abnahme 5 Spur AN: Schuesse (Abfall des Magazins mg) und hp-Wechsel je Akteur im letzten Raumabschnitt eines
state.log; dazu Zustand/Lage der 0x4D zu Beginn und am Ende.
  python schuss5.py <state.log> [abschnitt=-1]"""
import sys, re
absch = int(sys.argv[2]) if len(sys.argv) > 2 else -1
pl = re.compile(r"PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\).* mg=(-?\d+)")
ak = re.compile(r"\[(\d+) t=([0-9a-f]+) st=(\d+) [^\]]*@\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)")
segs, cur, last = [], [], -1
for l in open(sys.argv[1], errors="replace"):
    if not l.startswith("F"): continue
    f = int(l.split(" ", 1)[0][1:])
    if f < last: segs.append(cur); cur = []
    last = f; cur.append((f, l))
segs.append(cur)
seg = segs[absch]
print("Abschnitte %d, gewaehlt %d: %d Bilder F%d..F%d" % (len(segs), absch, len(seg), seg[0][0], seg[-1][0]))
mg_prev = None; hp_prev = {}; schuesse = []; wechsel = []; an_first = an_last = None
for f, l in seg:
    p = pl.search(l)
    if p:
        mg = int(p.group(5))
        if mg_prev is not None and mg < mg_prev: schuesse.append((f, mg_prev, mg, int(p.group(1)), int(p.group(2))))
        mg_prev = mg
    for m in ak.finditer(l):
        k = (m.group(1), m.group(2)); hp = int(m.group(7))
        if k in hp_prev and hp != hp_prev[k]: wechsel.append((f, k, hp_prev[k], hp, m.group(3)))
        hp_prev[k] = hp
        if m.group(2) == "4d":
            t = (f, int(m.group(4)), int(m.group(5)), m.group(3), hp)
            if an_first is None: an_first = t
            an_last = t
print("Schuesse (mg-Abfall): %d" % len(schuesse))
for s in schuesse[:12]: print("  F%d mg %d -> %d, Spieler (%d,%d)" % s)
if len(schuesse) > 12: print("  ... letzter F%d" % schuesse[-1][0])
print("hp-Wechsel: %d" % len(wechsel))
for w in wechsel[:60]: print("  F%d Slot %s t=%s hp %d -> %d (st %s)" % (w[0], w[1][0], w[1][1], w[2], w[3], w[4]))
if an_first: print("0x4D erstes Bild F%d (%d,%d) st %s hp %d; letztes F%d (%d,%d) st %s hp %d" % (an_first + an_last))
else: print("keine 0x4D im Abschnitt")
