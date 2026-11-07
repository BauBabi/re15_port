#!/usr/bin/env python3
"""logvergleich.py - Abnahme 4 Spur AN: state.log eines Abnahme-3-Laufs (exe b646c72a6, NB4) gegen denselben Lauf mit der
exe des Kopfs cefd57402 (NB5). Zeilen werden in Reihenfolge verglichen (Bildnummern beginnen je Raum neu).
Ausgabe: Zeilenzahlen, erste abweichende Zeile, Zahl abweichender Zeilen, je Abschnitt (Raumwechsel = Bildnummer faellt)
die Spannen mit Abweichung, und fuer Spieler PL(...) und Annette [.. t=4d ..] die groesste Lageabweichung.
  python logvergleich.py <alt/state.log> <neu/state.log> [max_beispiele]"""
import sys, re
a = [l.rstrip("\n") for l in open(sys.argv[1], errors="replace") if l.startswith("F")]
b = [l.rstrip("\n") for l in open(sys.argv[2], errors="replace") if l.startswith("F")]
nmax = int(sys.argv[3]) if len(sys.argv) > 3 else 6
print("Zeilen alt %d neu %d" % (len(a), len(b)))
def abschnitt(lines):
    seg, last, out = 0, -1, []
    for l in lines:
        f = int(l.split(" ", 1)[0][1:])
        if f < last: seg += 1
        last = f; out.append(seg)
    return out
sa, sb = abschnitt(a), abschnitt(b)
pl = re.compile(r"PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\)")
an = re.compile(r"\[(\d+) t=4d st=(\d+) ss1=(\d+) ss2=(\d+) ss3=(\d+) g=(\w+) mo=(\d+) af=(\d+) [^\]]*@\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)")
n = min(len(a), len(b)); diff = [i for i in range(n) if a[i] != b[i]]
print("abweichende Zeilen: %d von %d" % (len(diff), n))
if diff:
    i = diff[0]
    print("erste Abweichung Zeile %d (Abschnitt %d):\n  alt %s\n  neu %s" % (i, sa[i], a[i][:700], b[i][:700]))
spannen = {}
for i in diff: spannen.setdefault(sa[i], []).append(a[i].split(" ", 1)[0])
for s, v in sorted(spannen.items()): print("Abschnitt %d: %d Zeilen anders, %s .. %s" % (s, len(v), v[0], v[-1]))
dpl = dan = 0; bpl = ban = None; anz = 0
for i in range(n):
    ma, mb = pl.search(a[i]), pl.search(b[i])
    if ma and mb:
        d = max(abs(int(ma.group(1)) - int(mb.group(1))), abs(int(ma.group(2)) - int(mb.group(2))))
        if d > dpl: dpl, bpl = d, (a[i].split(" ", 1)[0], sa[i], ma.groups(), mb.groups())
    xa, xb = an.search(a[i]), an.search(b[i])
    if xa and xb:
        d = max(abs(int(xa.group(9)) - int(xb.group(9))), abs(int(xa.group(10)) - int(xb.group(10))))
        if xa.groups() != xb.groups(): anz += 1
        if d > dan: dan, ban = d, (a[i].split(" ", 1)[0], sa[i], xa.groups(), xb.groups())
    elif bool(xa) != bool(xb): anz += 1
print("Spieler: groesste Lageabweichung %d %s" % (dpl, bpl))
print("Annette: Zeilen mit anderem Zustand/Lage %d, groesste Lageabweichung %d %s" % (anz, dan, ban))
for i in diff[:nmax]:
    print("  [%d] alt %s\n       neu %s" % (i, a[i][:400], b[i][:400]))
