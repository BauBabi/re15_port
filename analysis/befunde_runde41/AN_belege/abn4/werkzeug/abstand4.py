#!/usr/bin/env python3
"""abstand4.py - Abnahme 4: kleinster XZ-Abstand Spieler <-> Annette (t=4d) je Raumabschnitt eines state.log, mit Bild und Lagen,
dazu die Bilder, in denen der Abstand unter 1000 liegt (Koerperschub Spieler r 450 + 0x4D r 350 = 800, vorher 450 = 900).
  python abstand4.py <state.log>"""
import sys, re, math
pl = re.compile(r"PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\)")
an = re.compile(r"\[(\d+) t=4d st=(\d+) ss1=(\d+) ss2=(\d+) [^\]]*mo=(\d+) af=(\d+) [^\]]*@\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)")
seg, last, best, nah = 0, -1, {}, {}
for l in open(sys.argv[1], errors="replace"):
    if not l.startswith("F"): continue
    f = int(l.split(" ", 1)[0][1:])
    if f < last: seg += 1
    last = f
    p, a = pl.search(l), an.search(l)
    if not (p and a): continue
    px, pz = int(p.group(1)), int(p.group(2)); ax, az = int(a.group(7)), int(a.group(8))
    d = math.hypot(px - ax, pz - az)
    if seg not in best or d < best[seg][0]: best[seg] = (d, f, (px, pz), (ax, az), a.group(2), a.group(5))
    if d < 1000: nah.setdefault(seg, []).append((f, int(d)))
for s in sorted(best):
    d, f, P, A, st, mo = best[s]
    print("Abschnitt %d: kleinster Abstand %.0f bei F%d, Spieler %s, Annette %s (st %s mo %s)" % (s, d, f, P, A, st, mo))
    if s in nah:
        v = nah[s]; print("   unter 1000: %d Bilder, F%d..F%d, Minimum %d" % (len(v), v[0][0], v[-1][0], min(x[1] for x in v)))
