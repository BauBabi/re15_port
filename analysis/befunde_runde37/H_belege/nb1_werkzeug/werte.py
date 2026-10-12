# werte.py <state.log> : letzter Besuch (ROOM1130), je Zombie Zustandsfolge (st/ss1), Abstand aus den LAGEN Leon<->Zombie
import re, sys, math, collections
RX_F = re.compile(r'^F(\d+) .*?PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\)')
RX_E = re.compile(r'\[(\d+) t=([0-9a-f]+) st=(\d+) ss1=(\d+) ss2=(\d+) ss3=(\d+) g=([0-9a-f]+) mo=(\d+) af=(\d+) stun=(-?\d+) d=(\d+) @\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)')
Z = {0x10, 0x11, 0x12, 0x13, 0x16, 0x18}
besuche = []; cur = None; last = -1
for line in open(sys.argv[1], errors='replace'):
    m = RX_F.match(line)
    if not m: continue
    f = int(m.group(1))
    if cur is None or f < last: cur = []; besuche.append(cur)
    last = f
    pl = (int(m.group(2)), int(m.group(3)), int(m.group(5)))
    ens = {}
    for e in RX_E.finditer(line):
        t = int(e.group(2), 16)
        if t in Z:
            ens[int(e.group(1))] = (t, int(e.group(3)), int(e.group(4)), int(e.group(7), 16), int(e.group(12)), int(e.group(13)))
    cur.append((f, pl, ens))
b = besuche[-1]
marken = [int(x) for x in (sys.argv[2] if len(sys.argv) > 2 else '1,100,200,300,400').split(',')]
print(f"Besuch {len(besuche)-1} (letzter): F{b[0][0]}..F{b[-1][0]}, Leon {b[0][1][:2]} -> {b[-1][1][:2]}, HP {b[0][1][2]} -> {b[-1][1][2]}")
slots = sorted({s for (_, _, ens) in b for s in ens})
for s in slots:
    rows = [(f, pl, ens[s]) for (f, pl, ens) in b if s in ens]
    folge = []; prev = None
    for (f, pl, e) in rows:
        k = (e[1], e[2])
        if k != prev: folge.append(f"{k[0]}/{k[1]}@{f}"); prev = k
    def d(r): return round(math.hypot(r[2][4] - r[1][0], r[2][5] - r[1][1]))
    ab = {f: d((f, pl, e)) for (f, pl, e) in rows}
    weg = sum(math.hypot(rows[i][2][4] - rows[i-1][2][4], rows[i][2][5] - rows[i-1][2][5]) for i in range(1, len(rows)))
    werte = ' '.join(f"F{mk}:{ab[mk]}" for mk in marken if mk in ab)
    e0 = rows[0][2]
    print(f"  slot {s} t={e0[0]:02x} g={e0[3]:02x} Folge {' '.join(folge[:8])}{' ...' if len(folge) > 8 else ''} | Abstand {werte} | Weg {weg:.0f}")
