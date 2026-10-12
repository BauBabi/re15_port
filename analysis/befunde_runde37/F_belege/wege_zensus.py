# Wege-Zensus STAGE1 (Leon): Tuer-Graph aus ALLEN Door_aot_set-Saetzen (main + subs, sce != 0) der
# Raeume ROOM1xx0 — Annahme "schlimmster Fall": jede flag-gesperrte Tuer offen (maximale Verbindung).
# Inerte Saetze (sce 0, nie per Aot_on/Aot_reset scharf) zaehlen nicht. Selbst-Tueren (Raum -> derselbe
# Raum) aendern nur die Lage im Raum. Port-Sperren im Endspiel ((9,71)=1, (9,73)=0):
#   1060 -> 1040  (re15_tuer1060.h, Slot 2 @0x00D52)
#   1050 -> 10A0  (NEU, Slot 4 @0x00B5A)
import sys, re, glob, os
from collections import defaultdict, deque
dumps = sys.argv[1]
edges = defaultdict(set); detail = defaultdict(list)
for f in sorted(glob.glob(os.path.join(dumps, "ROOM1??0.txt"))):
    room = os.path.basename(f)[4:8]
    for line in open(f, encoding="utf-8", errors="replace"):
        m = re.search(r"(0x[0-9A-F]+) \(\+[0-9A-F]+\) Door_aot_set\s+((?:[0-9a-f]{2} )+)", line)
        if not m: continue
        b = [int(x, 16) for x in m.group(2).split()]
        tgt = "%X%02X0" % (b[22] + 1, b[23])
        if b[2] == 0 or tgt == room: continue
        edges[room].add(tgt); detail[(room, tgt)].append("%s Slot %d Band %d" % (m.group(1), b[1], b[4]))
def reach(start, sperren):
    seen = {start}; q = deque([start]); par = {start: None}
    while q:
        r = q.popleft()
        for t in sorted(edges[r]):
            if (r, t) in sperren or t in seen: continue
            seen.add(t); par[t] = r; q.append(t)
    return seen, par
def weg(par, z):
    w = []
    while z: w.append(z); z = par[z]
    return " -> ".join(reversed(w))
ziel = ["11B0", "11C0"]
for name, sp in [("ohne Sperren (Original)", set()),
                 ("nur Sperre 1060->1040 (Stand v0.8.24)", {("1060", "1040")}),
                 ("Sperren 1060->1040 + 1050->10A0 (neu)", {("1060", "1040"), ("1050", "10A0")})]:
    seen, par = reach("10F0", sp)
    print("== %s: von ROOM10F0 erreichbar %d Raeume" % (name, len(seen)))
    for z in ziel:
        print("   %s: %s" % (z, weg(par, z) if z in seen else "NICHT erreichbar"))
seen, _ = reach("10F0", {("1060", "1040"), ("1050", "10A0")})
print("== erreichbar mit beiden Sperren:", " ".join(sorted(seen)))
rest = sorted(set(edges) - seen)
print("== nicht erreichbar mit beiden Sperren:", " ".join(rest))
print("== Kanten aus dem erreichbaren Teil in den Rest (Schnitt):")
for r in sorted(seen):
    for t in sorted(edges[r]):
        if t not in seen:
            print("   %s -> %s  %s" % (r, t, "; ".join(detail[(r, t)])))
