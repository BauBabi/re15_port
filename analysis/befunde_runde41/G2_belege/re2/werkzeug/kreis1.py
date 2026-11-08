# Abnahme 4: Verfolgungs-Kreis (kreis 1) - gefahrene Bahn gegen den Mittelpunkt aus "TRAG KREISEN an", Abstand Wurzel/Kopf zur Figur, Umlauf
import re, sys, math
run, f0, f1 = sys.argv[1], int(sys.argv[2]), int(sys.argv[3])
cx, cz = float(sys.argv[4]), float(sys.argv[5])
rx = re.compile(r"TRAG F(\d+) ph=(\d+) mo=(\d+) af=(\d+) gier=(\d+) pos=\((-?\d+),(-?\d+)\).*spieler=\((-?\d+),(-?\d+)\)")
rows = [tuple(int(v) for v in m.groups()) for m in (rx.search(l) for l in open(run + "/gator_trag.log")) if m]
rows = [r for r in rows if f0 <= r[0] <= f1]
xs = [r[5] for r in rows]; zs = [r[6] for r in rows]
rad = sorted(math.hypot(r[5] - cx, r[6] - cz) for r in rows)
wsum = 0; prev = None; netto = 0
for r in rows:
    a = math.atan2(r[6] - cz, r[5] - cx)
    if prev is not None:
        d = (a - prev + math.pi) % (2 * math.pi) - math.pi; wsum += abs(d); netto += d
    prev = a
dw = sorted(math.hypot(r[5] - r[7], r[6] - r[8]) for r in rows)
dk = sorted(math.hypot(r[5] + 3236 * math.cos(r[4] * 2 * math.pi / 4096) - r[7], r[6] - 3236 * math.sin(r[4] * 2 * math.pi / 4096) - r[8]) for r in rows)
clips = {}
for r in rows: clips[r[2]] = clips.get(r[2], 0) + 1
print("%s F%d-F%d (%d Bilder): Wurzel x %d..%d z %d..%d; Abstand zum Mittelpunkt (%d,%d) Median %.0f (p5 %.0f p95 %.0f); Winkel um den Mittelpunkt |Summe| %.1f Runden, netto %.1f; Wurzel-Figur Median %.0f min %.0f; Kopf-Figur Median %.0f min %.0f; Clips %s"
      % (run.split('/')[-1], f0, f1, len(rows), min(xs), max(xs), min(zs), max(zs), cx, cz, rad[len(rad)//2], rad[len(rad)//20], rad[len(rad)*95//100], wsum / 2 / math.pi, netto / 2 / math.pi, dw[len(dw)//2], dw[0], dk[len(dk)//2], dk[0], dict(sorted(clips.items()))))
