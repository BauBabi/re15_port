# NB5: gefahrene Bahn gegen geplante Bahn (Harness G2_BAHN, Spalten: name cam t x z gier ph clip trag kk kreis kx kz bo ax az bx bz r kdir U s ab)
import sys, math, collections
import numpy as np
runs = collections.OrderedDict(); cur = None; last_t = None
for l in open(sys.argv[1]):
    f = l.split()
    if len(f) < 23: continue
    name, cam, t = f[0], int(f[1]), int(f[2])
    if last_t is None or t <= last_t or (cur and (cur[0] != name or cur[1] != cam)):
        key = (name, cam, len(runs)); runs[key] = []; cur = (name, cam)
    last_t = t
    runs[key].append([int(v) for v in f[2:]])
for (name, cam, i), rows in runs.items():
    a = np.array(rows, dtype=float)
    h = a[len(a)//2:]
    on = h[h[:, 11] == 1]
    if len(on) < 10:
        print("%-11s cam %d: keine Bahn in der zweiten Haelfte" % (name, cam)); continue
    ax, az, bx, bz, r, kd, U = on[-1, 12:19]
    x, z, s, ab = on[:, 1], on[:, 2], on[:, 19], on[:, 20]
    ds = np.diff(s); ds = (ds + U / 2) % U - U / 2          # Bogenlaenge entwickeln
    prog = ds.sum(); runden = prog / U
    step = np.hypot(np.diff(x), np.diff(z)); v = np.median(step)
    plan_t = U / v if v > 0 else float('nan')
    ist_t = len(on) / runden if runden > 0 else float('inf')
    mx, mz = (ax + bx) / 2, (az + bz) / 2
    print("%-11s cam %d: Plan A (%d,%d) B (%d,%d) R %d Umfang %d | Abstand zur Bahn Median %4.0f p95 %4.0f max %4.0f | Schwerpunkt (%6.0f,%6.0f) gegen Plan (%6.0f,%6.0f) "
          "Ausdehnung x %5.0f..%5.0f z %6.0f..%6.0f (Plan x %5.0f..%5.0f z %6.0f..%6.0f) | %5.2f Runden in %d Bildern, Umlauf %4.0f Bilder (Plan %4.0f bei %2.0f je Bild) | Klemme %3.0f %% | Bahn %3.0f %% der Bilder"
          % (name, cam, ax, az, bx, bz, r, U, np.median(ab), np.percentile(ab, 95), ab.max(), x.mean(), z.mean(), mx, mz, x.min(), x.max(), z.min(), z.max(),
             min(ax, bx) - r, max(ax, bx) + r, min(az, bz) - r, max(az, bz) + r, runden, len(on), ist_t, plan_t, v, 100 * on[:, 7].mean(), 100 * len(on) / len(h)))
