# Abnahme 4: Belagerungsbahn Cut 2 - Abstand von Kopf/Kanister zur Kamera (Cut 2 (2484,-3132,-25956), Blick Nord = +z; Dossier NB5) und zur Suedwand z -27000
import re, sys, math
run = sys.argv[1]; f0 = int(sys.argv[2]); CZ = -25956
rx = re.compile(r"TRAG F(\d+) ph=(\d+) mo=(\d+) af=(\d+) gier=(\d+) pos=\((-?\d+),(-?\d+)\)")
rows = [tuple(int(v) for v in m.groups()) for m in (rx.search(l) for l in open(run + "/gator_trag.log", errors="replace")) if m]
n = hinter = nah1000 = sued = 0; mins = []
for (F, ph, mo, af, g, x, z) in rows:
    if F < f0: continue
    n += 1
    a = g * 2 * math.pi / 4096; c, s = math.cos(a), math.sin(a)
    pts = [(x + c * l0 + s * l1, z - s * l0 + c * l1) for (l0, l1) in ((0, 0), (1600, 0), (3236, 0), (4630, -237), (4630, 1200), (4630, 2694))]
    d = min(p[1] - CZ for p in pts)
    mins.append(d)
    if d < 0: hinter += 1
    if d < 1000: nah1000 += 1
    if min(p[1] for p in pts) < -27000: sued += 1
mins.sort()
print("%s ab F%d: %d Bilder; Kopf/Kanister HINTER der Kameraebene %d (%.0f %%), naeher als 1000 vor der Kamera %d (%.0f %%), jenseits der Suedwand z<-27000 %d (%.0f %%); kleinster Abstand %d, Median %d"
      % (run.split('/')[-1], f0, n, hinter, 100.0 * hinter / n, nah1000, 100.0 * nah1000 / n, sued, 100.0 * sued / n, mins[0], mins[len(mins) // 2]))
