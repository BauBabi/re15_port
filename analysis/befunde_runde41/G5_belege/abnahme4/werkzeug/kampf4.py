# Abnahme 4: Kampf nach Szenenende (F2775) aus state.log: Spieler-HP/Lage und G5-HP/Lage bei jeder Aenderung
import re, sys
END = 2775
abschnitte = [[]]; last = -1
for z in open(sys.argv[1], encoding="latin-1"):
    m = re.match(r"F(\d+) .*?PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\)", z)
    if not m: continue
    F = int(m.group(1))
    if F < last: abschnitte.append([])
    last = F; abschnitte[-1].append((F, z, m))
sz = [a for a in abschnitte if any(F == END for F, _, _ in a)][-1]
php = ghp = None
for F, z, m in sz:
    if F < END: continue
    g = re.search(r"\[\d+ t=36 [^]]*@\((-?\d+),(-?\d+),r-?\d+\)\] hp=(-?\d+)", z)
    hp = int(m.group(5)); gh = int(g.group(3)) if g else None; gx = int(g.group(1)) if g else None
    if hp != php or (gh is not None and gh != ghp):
        print("F%d (+%.1f s) Spieler (%s,%s) HP %d | G5 x %s HP %s" % (F, (F - END) / 30.0, m.group(2), m.group(3), hp, gx, gh))
    php, ghp = hp, gh
print("letztes Bild F%d" % sz[-1][0])
