#!/usr/bin/env python3
"""Runde 41 Spur K: RE15_ANIM_TRACE (anim_select_common.c) je Aktor zu einer Clip-Folge verdichten.
Spalten: bild aktor typ motion clip fc cur slot cliplen loco rev legacy. Raumwechsel = Bildzaehler faellt.
    python anim_folge.py <anim.txt> [raum_index=Nummer des Abschnitts, Standard letzter vor Ende] [von] [bis]
"""
import sys
rows = [l.split() for l in open(sys.argv[1]) if l.strip()]
abschnitte, cur, last = [], [], -1
for r in rows:
    f = int(r[0])
    if f < last - 5: abschnitte.append(cur); cur = []
    last = f; cur.append(r)
abschnitte.append(cur)
idx = int(sys.argv[2]) if len(sys.argv) > 2 else len(abschnitte) - 1
von = int(sys.argv[3]) if len(sys.argv) > 3 else 0
bis = int(sys.argv[4]) if len(sys.argv) > 4 else 10**9
print('Abschnitte: %d (Bilder je Abschnitt: %s)' % (len(abschnitte), [ (a[0][0], a[-1][0]) for a in abschnitte if a]))
folge = {}
for r in abschnitte[idx]:
    f = int(r[0])
    if f < von or f > bis: continue
    key = (r[1], r[2])
    clip, fc, slot, rev = int(r[4]), int(r[5]), int(r[7]), int(r[10])
    st = folge.setdefault(key, [])
    if not st or st[-1][1] != clip or st[-1][2] != fc or st[-1][5] != rev:
        st.append([f, clip, fc, slot, slot, rev, f])
    else:
        st[-1][4] = slot; st[-1][6] = f
for (a, t), st in sorted(folge.items(), key=lambda kv: kv[0]):
    print('Aktor %s Typ 0x%02x: %d Abschnitte' % (a, int(t), len(st)))
    for f, clip, fc, s0, s1, rev, fe in st:
        print('   F%5d-%5d clip %2d fc %3d slot %3d..%3d%s' % (f, fe, clip, fc, s0, s1, ' rev' if rev else ''))
