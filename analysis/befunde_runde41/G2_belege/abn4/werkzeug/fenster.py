"""fenster.py <lauf> <F_von> <F_bis> [schritt_s]: Kennzahlen eines Bildfensters aus gator_trag.log (wie mass_v4.py, nur fuer F_von..F_bis), dazu je <schritt_s> Sekunden
eine Zeile: F, Phase, Clip, Wurzel, Kopf-Abstand zur Figur (Kopfpunkt 3236 vor der Wurzel, Clip 0 NB5 M2a), Gier-Summe im Abschnitt, Weg der Wurzel im Abschnitt."""
import re, sys, math
run, f0, f1 = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]); step = float(sys.argv[4]) if len(sys.argv) > 4 else 2.0
rx = re.compile(r"TRAG F(\d+) ph=(\d+) mo=(\d+) af=(\d+) gier=(\d+) pos=\((-?\d+),(-?\d+)\) takt=\((-?\d+),(-?\d+)\).*spieler=\((-?\d+),(-?\d+)\)")
rows = [tuple(int(v) for v in m.groups()) for m in (rx.search(l) for l in open(run + "/gator_trag.log", errors="replace")) if m]
rows = [r for r in rows if f0 <= r[0] <= f1]
n = len(rows)
def kopf(r):
    a = r[4] * 2 * math.pi / 4096; return (r[5] + math.cos(a) * 3236, r[6] - math.sin(a) * 3236)
kd = [math.hypot(kopf(r)[0] - r[9], kopf(r)[1] - r[10]) for r in rows]
# laengste Zeit ohne Fortschritt (wie mass_v4.py)
best = None; lauf = 0; ohne_max = 0; p0 = None
for i, r in enumerate(rows):
    px, pz = r[9], r[10]
    if p0 is None or kd[i] <= 4500 or math.hypot(px - p0[0], pz - p0[1]) > 1500 or kd[i] < best - 300: lauf = 0; best = kd[i]; p0 = (px, pz)
    else: lauf += 1; ohne_max = max(ohne_max, lauf)
dreh_f = set(); dreh = set()
for i in range(120, n):
    w = rows[i - 120:i + 1]
    tot = sum(abs(((w[k + 1][4] - w[k][4] + 2048) % 4096) - 2048) for k in range(len(w) - 1))
    if tot >= 4096:
        xs = [r[5] for r in w]; zs = [r[6] for r in w]
        if max(xs) - min(xs) < 3000 and max(zs) - min(zs) < 3000:
            if kd[i] > 4500: dreh.update(range(i - 120, i + 1))
            if min(kd[i - 120:i + 1]) > 4500: dreh_f.update(range(i - 120, i + 1))
# 2-s-Fenster ohne Fortschritt fern der Figur (wie traglog: Wurzel netto < 300 in 30 Bildern = Stillstand; 60-Bilder-Fenster ohne Bild "am Spieler" (Kopf <= 4500))
still = [0] * n
for i in range(n - 30):
    w = max(math.hypot(rows[k][5] - rows[i][5], rows[k][6] - rows[i][6]) for k in range(i + 1, i + 31)); still[i] = w < 300
f2 = 0
for i in range(n - 60):
    if all(still[i:i + 61]) and all(k > 4500 for k in kd[i:i + 61]): f2 += 1
nah = sum(1 for k in kd if k <= 4500)
print("%s F%d-F%d: %d Bilder (%.1f s) | ohne Fortschritt zur Figur max %.1f s | Drehen auf der Stelle %.1f %% (ganz fern %.1f %%) | 2-s-Fenster still+fern %d | Kopf <= 4500 an der Figur %.1f %% der Bilder | Kopf-Abstand min %d mittel %d max %d"
      % (run.replace("\\", "/").split("/")[-1], f0, f1, n, n / 30.0, ohne_max / 30.0, 100.0 * len(dreh) / max(n, 1), 100.0 * len(dreh_f) / max(n, 1), f2, 100.0 * nah / max(n, 1), min(kd) if kd else -1, sum(kd) / max(n, 1), max(kd) if kd else -1))
s = int(step * 30)
for i in range(0, n, s):
    w = rows[i:i + s]; r = w[0]
    tot = sum(abs(((w[k + 1][4] - w[k][4] + 2048) % 4096) - 2048) for k in range(len(w) - 1))
    weg = sum(math.hypot(w[k + 1][5] - w[k][5], w[k + 1][6] - w[k][6]) for k in range(len(w) - 1))
    print("  F%-5d ph=%d mo=%2d Wurzel (%6d,%6d) gier %4d Figur (%6d,%6d) Kopf-Abst %5d | Gier-Summe %5d Weg %5d %s" % (r[0], r[1], r[2], r[5], r[6], r[4], r[9], r[10], kd[i], tot, weg, "STILL" if weg < 300 * len(w) / 30 else ""))
