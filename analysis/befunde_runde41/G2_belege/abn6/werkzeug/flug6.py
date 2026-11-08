"""flug6.py <laufordner> [...]: Weg der Figur nach jedem Treffer (hp-Flanke) bis zum Stillstand (Lage aendert sich 10 Bilder lang nicht), getrennt oben (y < -900) / Wasser;
letzter Besuch von 2090/2091. Zeigt, wie weit der RE1.5-Niederwurf (s_kd_speed 1000 @0x8003615c) die Figur traegt."""
import re, sys, math, os
for run in sys.argv[1:]:
    sl = [l for l in open(run + "/state.log", errors="replace") if l.startswith("F")]
    cl = [l for l in open(run + "/climb.log", errors="replace") if l.startswith("T")]
    start = 0; prev = None
    for i, l in enumerate(sl):
        f = int(re.match(r"F(\d+)", l).group(1))
        if prev is not None and f < prev: start = i
        prev = f
    st = []
    for i in range(start, len(sl)):
        m = re.match(r"F(\d+) pad=\w+ PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\)", sl[i])
        c = re.search(r"pos=\((-?\d+),(-?\d+),(-?\d+)\)", cl[i]) if i < len(cl) else None
        if m and c: st.append((int(m.group(1)), int(m.group(2)), int(m.group(3)), int(m.group(5)), int(c.group(2))))
    wege = {"oben": [], "wasser": []}
    for k in range(1, len(st)):
        if st[k][3] < 100 and st[k - 1][3] >= 100:
            x0, z0 = st[k][1], st[k][2]; ort = "oben" if st[k][4] < -900 else "wasser"
            j = k; ruhe = 0; weg = 0.0
            while j + 1 < len(st) and ruhe < 10:
                d = math.hypot(st[j + 1][1] - st[j][1], st[j + 1][2] - st[j][2]); weg += d
                ruhe = ruhe + 1 if d == 0 else 0; j += 1
            wege[ort].append((st[k][0], int(weg), int(math.hypot(st[j][1] - x0, st[j][2] - z0)), (st[j][1], st[j][2])))
    name = run.replace("\\", "/").rstrip("/").split("/")[-1]
    for ort, w in wege.items():
        if not w: continue
        mx = max(w, key=lambda t: t[2])
        print("%-40s %-6s Treffer %3d: Weg bis Stillstand max %5d (Luftlinie max %5d, F%d -> %s), Mittel %5d" % (name, ort, len(w), max(t[1] for t in w), mx[2], mx[0], mx[3], sum(t[2] for t in w) // len(w)))
