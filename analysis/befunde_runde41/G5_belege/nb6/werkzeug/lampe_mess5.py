# NB5 M4: Lampenfleck im Tuerloch je Szenenbild, RE2 echt (Zeitachse um VERSATZ Bilder korrigiert, Abnahme 4: -2,0) gegen Port Leon/Elza.
# Fleck = Pixel mit Grauwert > SCHWELLE (Vorgabe 150) und Buntheit < 60 im Feld x 15..140, y 60..200 (320x240); Schwerpunkt + Kasten.
import subprocess, numpy as np, sys, os
from PIL import Image
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"; VFPS = 59.817
SRC = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5abn2/re2/re2_lauf1.mp4"
L = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5nb5/lauf"
VERSATZ = float(sys.argv[1]) if len(sys.argv) > 1 else 2.0
SCHWELLE = float(sys.argv[2]) if len(sys.argv) > 2 else 150
N0, N1 = -54, 110
def re2_t(n): return 63.47 + n / 29.9
X0, X1, Y0, Y1 = 15, 140, 60, 200
def fleck(a):
    a = a.astype(int)[Y0:Y1, X0:X1]
    g = a.mean(axis=2); bunt = a.max(axis=2) - a.min(axis=2)
    m = (g > SCHWELLE) & (bunt < 60)
    n = int(m.sum())
    if n < 3: return None
    ys, xs = np.nonzero(m)
    return (n, xs.mean() + X0, ys.mean() + Y0, xs.min() + X0, xs.max() + X0, ys.min() + Y0, ys.max() + Y0)
t0 = re2_t(N0 - VERSATZ) - 0.2; t1 = re2_t(N1) + 0.2
raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "%.3f" % t0, "-t", "%.3f" % (t1 - t0), "-i", SRC,
                      "-vf", "scale=320:240,format=rgb24", "-f", "rawvideo", "-"], capture_output=True).stdout
v = np.frombuffer(raw, np.uint8).reshape(-1, 240, 320, 3)
rows = []
for n in range(N0, N1 + 1):
    k = int(round((re2_t(n - VERSATZ) - t0) * VFPS)); k = max(0, min(k, len(v) - 1))
    r = fleck(v[k])
    ps = []
    for sp in ("fd_leon", "fd_elza"):
        p = os.path.join(L, sp, "fd", "f%06d.ppm" % (1613 + n))
        ps.append(fleck(np.asarray(Image.open(p).convert("RGB").resize((320, 240), Image.BILINEAR))) if os.path.exists(p) else "fehlt")
    rows.append((n, r, ps[0], ps[1]))
def f(x):
    if x is None: return "-" * 26
    if x == "fehlt": return "fehlt".ljust(26)
    return "%3d px (%5.1f,%5.1f) %3d..%3d" % (x[0], x[1], x[2], x[3], x[4])
print("Szenenbild | RE2 echt (Zeitachse -%.1f) | Port Leon 5090 | Port Elza 5091" % VERSATZ)
both = []; nur_re2 = nur_port = gleich = 0
for n, r, pl, pe in rows:
    print("%5d | %s | %s | %s" % (n, f(r), f(pl), f(pe)))
    if r and pl and pl != "fehlt": both.append((n, r[1] - pl[1], r[2] - pl[2], r[0], pl[0]))
    if (r is None) == (pl is None): gleich += 1
    elif r: nur_re2 += 1
    else: nur_port += 1
print("Bilder %d: Lampe in beiden oder in keinem %d, nur RE2 %d, nur Port %d" % (len(rows), gleich, nur_re2, nur_port))
if both:
    dx = np.array([b[1] for b in both]); dy = np.array([b[2] for b in both])
    print("beide sichtbar %d Bilder: dx RE2-Port Mittel %+.1f (|dx| Median %.1f), dy Mittel %+.1f (|dy| Median %.1f); Pixelzahl RE2 Median %d, Port %d"
          % (len(both), dx.mean(), np.median(abs(dx)), dy.mean(), np.median(abs(dy)), np.median([b[3] for b in both]), np.median([b[4] for b in both])))
leon_elza = sum(1 for n, r, pl, pe in rows if (pl is None) == (pe is None) and (pl is None or abs(pl[1] - pe[1]) < 1.5))
print("Leon = Elza (Fleck gleich, Schwerpunkt < 1,5 px): %d von %d" % (leon_elza, len(rows)))
