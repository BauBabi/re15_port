#!/usr/bin/env python3
"""abstand5.py - Abnahme 5 Spur AN: Abstand Spieler <-> Akteur eines Typs je Bild im LETZTEN Raumabschnitt eines state.log
(F-Zaehler faellt beim Raumwechsel zurueck). Gibt die Bilder an, in denen sich der Spieler bewegt, Minimum und Maximum.
  python abstand5.py <state.log> [typ=4d] [ab_bild=0] [bis_bild=999999] [abschnitt=-1]"""
import sys, re, math
log = sys.argv[1]
typ = sys.argv[2] if len(sys.argv) > 2 else "4d"
ab = int(sys.argv[3]) if len(sys.argv) > 3 else 0
bis = int(sys.argv[4]) if len(sys.argv) > 4 else 999999
absch = int(sys.argv[5]) if len(sys.argv) > 5 else -1
pl = re.compile(r"PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\) pst=(\d+)")
ak = re.compile(r"\[(\d+) t=%s st=(\d+) [^\]]*mo=(\d+) af=(\d+)[^\]]*@\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)" % typ)
segs, cur, last = [], [], -1
for l in open(log, errors="replace"):
    if not l.startswith("F"): continue
    f = int(l.split(" ", 1)[0][1:])
    if f < last: segs.append(cur); cur = []
    last = f
    p, a = pl.search(l), ak.search(l)
    cur.append((f, p, a))
segs.append(cur)
seg = segs[absch]
rows = []
for f, p, a in seg:
    if f < ab or f > bis or not p or not a: continue
    px, pz = int(p.group(1)), int(p.group(2)); ax, az = int(a.group(5)), int(a.group(6))
    rows.append((f, px, pz, int(p.group(5)), ax, az, int(a.group(2)), int(a.group(8)), math.hypot(px - ax, pz - az)))
print("%s: %d Abschnitte, Abschnitt %d mit %d Bildern, davon %d mit Typ %s in F%d..F%d" % (log.split("/")[-2] if "/" in log else log,
      len(segs), absch, len(seg), len(rows), typ, ab, bis))
if not rows: sys.exit(0)
prev = None
for r in rows:
    key = (r[1], r[2], r[4], r[5], r[6], r[7])
    if key != prev:
        print("  F%-5d Spieler (%d,%d) pst %d | %s (%d,%d) st %d hp %d | Abstand %.0f" % (r[0], r[1], r[2], r[3], typ, r[4], r[5], r[6], r[7], r[8]))
        prev = key
m = min(rows, key=lambda r: r[8]); M = max(rows, key=lambda r: r[8])
print("Minimum %.0f bei F%d, Maximum %.0f bei F%d" % (m[8], m[0], M[8], M[0]))
