# Abnahme 2: Kampfverlauf ab Szenenende aus state.log (Abschnitt mit dem G5, Typ 0x36)
import re, sys
d = sys.argv[1]
secs = [[]]; last = -1
for z in open(d + "/state.log", encoding="latin-1"):
    m = re.match(r"F(\d+) pad=(\w+) PL\((-?\d+),(-?\d+),rot=(\d+),hp=(-?\d+)\) pst=(\d+) ps1=(\d+) ps2=(\d+) mo=(\d+).* cam=(\d+).*pm=(\d+)", z)
    if not m: continue
    F = int(m.group(1))
    if F < last: secs.append([])
    last = F
    g = re.search(r"t=36 st=(\d+) ss1=(\d+) ss2=(\d+) ss3=(\d+) g=(\w+) mo=(\d+) af=(\d+) stun=\d+ d=(\d+) @\((-?\d+),(-?\d+),r(\d+)\)\] hp=(-?\d+)", z)
    secs[-1].append((F, m.groups(), g.groups() if g else None))
for i, s in enumerate(secs):
    print("Abschnitt", i, "F", s[0][0], "..", s[-1][0], "G5-Zeilen", sum(1 for r in s if r[2]))
s = max(secs, key=lambda s: sum(1 for r in s if r[2]))
F0 = int(sys.argv[2]) if len(sys.argv) > 2 else 2775
prev = None
for F, p, g in s:
    if F < F0 - 1: continue
    key = (p[2], p[3], p[5], p[6], p[7], p[8], p[9], g and g[0], g and g[1], g and g[2], g and g[5])
    if key != prev:
        print("F%d (+%.1fs) PL %s,%s hp %s pst %s/%s/%s mo %s cam %s | G5 st %s/%s/%s mo %s x %s hp %s" % (
            F, (F - F0) / 30.0, p[2], p[3], p[5], p[6], p[7], p[8], p[9], p[10], g and g[0], g and g[1], g and g[2], g and g[5], g and g[8], g and g[11]))
    prev = key
