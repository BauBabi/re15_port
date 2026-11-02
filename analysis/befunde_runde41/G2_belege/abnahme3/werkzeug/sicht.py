# Sichtbarkeit bewegter Inhalte bei Normalhelligkeit: je Sekunde Bildpaar (t, t+0.2 s), Pixel mit |Luma-Diff| > SCHWELLE
# (ohne Kino-Balken), dazu mittlere Luma der bewegten Pixel. Stehende Figur, bewegter Gator -> Diff = Gator (+ Wasser-Rauschen).
import sys, subprocess, numpy as np
vid, t0, t1 = sys.argv[1], float(sys.argv[2]), float(sys.argv[3]); TH = int(sys.argv[4]) if len(sys.argv) > 4 else 12
W, H = 320, 240
p = subprocess.run(["C:/ProgramData/chocolatey/bin/ffmpeg", "-v", "error", "-ss", str(t0), "-t", str(t1 - t0), "-i", vid, "-vf", "fps=30,scale=%d:%d,format=gray" % (W, H), "-f", "rawvideo", "-"], capture_output=True)
v = np.frombuffer(p.stdout, np.uint8).reshape(-1, H, W).astype(np.int16)[:, 12:228, :]
res = []
for i in range(0, len(v) - 6, 30):
    d = np.abs(v[i + 6] - v[i]); m = d > TH
    res.append((t0 + i / 30, int(m.sum()), float(v[i + 6][m].mean()) if m.any() else 0, int(d.max())))
for t, n, l, mx in res: print("t=%6.1f bewegte Pixel %5d (von %d) Luma %5.1f max-Diff %3d" % (t, n, W * 216, l, mx))
a = np.array([r[1] for r in res]); print("Median %d, Anteil Sekunden mit >= 150 bewegten Pixeln: %d/%d" % (np.median(a), (a >= 150).sum(), len(a)))
