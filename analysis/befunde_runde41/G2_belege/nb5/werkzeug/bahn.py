# NB5: tatsaechliche Bahn je Lauf (Harness G2_BAHN): Kreis-Fit (Kasa) ueber die zweite Haelfte, Radius, Umlauf um Fit- und Plan-Mittelpunkt
import sys, math, collections
import numpy as np
runs = collections.OrderedDict(); cur = None; last_t = None
for l in open(sys.argv[1]):
    f = l.split()
    name, cam, t = f[0], int(f[1]), int(f[2])
    if last_t is None or t <= last_t or (cur and cur[0] != name) or (cur and cur[1] != cam):
        key = (name, cam, len(runs)); runs[key] = []; cur = (name, cam)
    last_t = t
    runs[key].append([int(v) for v in f[2:]])
for (name, cam, i), rows in runs.items():
    a = np.array(rows, dtype=float)
    h = a[len(a)//2:]
    x, z = h[:,1], h[:,2]
    A = np.c_[2*x, 2*z, np.ones(len(x))]; b = x*x + z*z
    (cx, cz, c0), *_ = np.linalg.lstsq(A, b, rcond=None); r = math.sqrt(max(c0 + cx*cx + cz*cz, 0))
    rr = np.hypot(x - cx, z - cz)
    kr = h[:,8]; px, pz = h[-1,9], h[-1,10]
    def wind(mx, mz):
        ang = np.unwrap(np.arctan2(z - mz, x - mx)); return (ang[-1] - ang[0]) / (2*math.pi)
    wf, wp = wind(cx, cz), wind(px, pz) if kr[-1] else float('nan')
    dp = np.hypot(x - px, z - pz) if kr[-1] else None
    kk = h[:,7].mean()
    print("%-11s cam %d: Fit (%6.0f,%6.0f) R %5.0f (+-%4.0f) Runden um Fit %5.2f | Plan (%d,%d) Abstand %s Runden %5.2f | Klemme %3.0f %%" % (
        name, cam, cx, cz, r, rr.std(), wf, px, pz, ("%5.0f..%5.0f" % (dp.min(), dp.max())) if dp is not None else "-", wp, 100*kk))
