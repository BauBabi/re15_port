# NB5: exe-Lauf - gefahrene Bahn (gator_trag.log) gegen die geplante Bahn (gator_boss.log "TRAG BAHN an ... A (ax,az) B (bx,bz) R r Drehsinn k")
import re, sys, math
run = sys.argv[1]
rx = re.compile(r"TRAG F(\d+) ph=(\d+) mo=(\d+) af=(\d+) gier=(\d+) pos=\((-?\d+),(-?\d+)\)")
rows = [tuple(int(v) for v in m.groups()) for m in (rx.match(l) for l in open(run + "/gator_trag.log", errors="replace")) if m]
plan = None
for l in open(run + "/gator_boss.log", errors="replace"):
    m = re.search(r"TRAG BAHN an F(\d+).*-> A \((-?\d+),(-?\d+)\) B \((-?\d+),(-?\d+)\) R (\d+) Drehsinn (-?\d+)", l)
    if m: plan = tuple(int(v) for v in m.groups())
kk = sum(1 for l in open(run + "/gator_boss.log", errors="replace") if "KOPF-KLEMME an" in l)
if not plan: print(run, "keine Bahn"); sys.exit()
f0, ax, az, bx, bz, r, kd = plan
L = math.hypot(bx - ax, bz - az); U = 2 * L + 2 * math.pi * r
def proj(x, z):   # Abstand zur Bahn und Bogenlaenge (gegen den Uhrzeiger, Start A - R n)
    ux, uz = ((bx - ax) / L, (bz - az) / L) if L > 0 else (1.0, 0.0)
    nx, nz = -uz, ux
    t = ((x - ax) * ux + (z - az) * uz); tc = max(0.0, min(L, t))
    cx, cz = ax + ux * tc, az + uz * tc
    d = math.hypot(x - cx, z - cz); ab = abs(d - r)
    side = (x - cx) * nx + (z - cz) * nz
    if 0 < t < L: s = t if side < 0 else L + math.pi * r + (L - t)
    else:
        a = math.atan2(z - cz, x - cx)
        if t >= L: a0 = math.atan2(-nz, -nx); da = (a - a0) % (2 * math.pi); s = L + min(da, math.pi) * r if da <= math.pi else (L + math.pi * r + 0)
        else: a0 = math.atan2(nz, nx); da = (a - a0) % (2 * math.pi); s = 2 * L + math.pi * r + min(da, math.pi) * r
    return ab, s
rows = [r_ for r_ in rows if r_[0] >= f0]
on = []; started = False
for r_ in rows:
    ab, s = proj(r_[5], r_[6])
    if not started and ab < 200: started = True
    if started: on.append((r_[0], r_[5], r_[6], ab, s, r_[2]))
if len(on) < 30: print(run, "nicht angekommen"); sys.exit()
abs_ = sorted(o[3] for o in on)
prog = 0; steps = []
for i in range(1, len(on)):
    if on[i][0] != on[i-1][0] + 1: continue
    ds = on[i][4] - on[i-1][4]; ds = (ds + U * 1.5) % U - U / 2; prog += ds
    steps.append(math.hypot(on[i][1] - on[i-1][1], on[i][2] - on[i-1][2]))
steps.sort(); v = steps[len(steps) // 2]
runden = abs(prog) / U
sx = sum(o[1] for o in on) / len(on); sz = sum(o[2] for o in on) / len(on)
clips = {}
for o in on: clips[o[5]] = clips.get(o[5], 0) + 1
print("%s: Plan (F%d) A (%d,%d) B (%d,%d) R %d Drehsinn %d Umfang %.0f | ab F%d %d Bilder: Abstand Median %.0f p95 %.0f max %.0f | %.2f Runden, Umlauf %.0f Bilder (Plan %.0f bei %.0f je Bild) | Schwerpunkt (%.0f,%.0f) Plan-Mitte (%.0f,%.0f) | Clips %s | KOPF-KLEMME-an %d"
      % (run.split('/')[-1], f0, ax, az, bx, bz, r, kd, U, on[0][0], len(on), abs_[len(abs_) // 2], abs_[len(abs_) * 95 // 100], abs_[-1], runden, len(on) / runden if runden else 0, U / v if v else 0, v,
         sx, sz, (ax + bx) / 2, (az + bz) / 2, dict(sorted(clips.items())), kk))
