# Nachbesserung 1 Spur C: fx.log/state.log der exe-Messung auswerten — je Blut-Effekt (id 0, frame=1, erste Zeile je
# Lage+Bild) Lage + Bild, dazu die HP-Wechsel von Platz 2 (Zombie (-1800,-19600)) aus state.log.
import re, sys, collections
d = sys.argv[1]
spawns = collections.OrderedDict()
pat = re.compile(r"id=0 sub=(\d+) eidx=(\d+) frame=1 w\((-?\d+),(-?\d+),(-?\d+)\) phys=\d .* F=(\d+)")
for l in open(d + "/fx.log", encoding="utf-8", errors="replace"):
    m = pat.search(l)
    if m:
        key = (m.group(6), m.group(3), m.group(4), m.group(5))
        if key not in spawns:
            spawns[key] = (int(m.group(6)), m.group(1), int(m.group(3)), int(m.group(4)), int(m.group(5)))
hp_prev = None; wechsel = []
for l in open(d + "/state.log", encoding="utf-8", errors="replace"):
    m = re.match(r"F(\d+) ", l)
    if not m: continue
    f = int(m.group(1))
    m2 = re.search(r"\[2 t=[0-9a-f]+ st=(\d+) ss1=(\d+) ss2=(\d+) .*?@\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)", l)
    if m2:
        hp = int(m2.group(7))
        if hp_prev is not None and hp != hp_prev:
            wechsel.append((f, hp_prev, hp, m2.group(1), m2.group(2), m2.group(3), m2.group(4), m2.group(5)))
        hp_prev = hp
for w in wechsel:
    print("HP Platz 2: F%d %d -> %d (st %s/%s/%s @(%s,%s))" % w)
for k, s in spawns.items():
    print("Blut-Spawn F%d sub=%s w(%d,%d,%d)" % s)
