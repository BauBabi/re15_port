# Abnahme 4: Arm-Signal in Schnitt 16 Segment 1 (Bild 0..130): Anteil "Arm-Pixel" (R-G>25, R>40, nicht das rote Deckenlicht: y > 150)
# im Tuer-/Raumfeld x 0..520, y 150..470 bei 640x480; RE2 an re2_t(n)+s, Port Framedump; Kreuzkorrelation -> Versatz, Kante erster Arm.
import subprocess, sys, os, numpy as np
from PIL import Image
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from re2zeit4 import re2_t
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"; VFPS = 59.817
src = sys.argv[1]
def sig(a):
    a = a[150:470, 0:520].astype(np.int16); return float(((a[..., 0] - a[..., 1] > 25) & (a[..., 0] > 40)).mean() * 100)
t0 = re2_t(0) - 30 / 29.9; t1 = re2_t(131)
raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "%.3f" % t0, "-t", "%.3f" % (t1 - t0), "-i", src,
                      "-vf", "scale=640:480,format=rgb24", "-f", "rawvideo", "-"], capture_output=True).stdout
v = np.frombuffer(raw, np.uint8).reshape(-1, 480, 640, 3)
re2 = np.array([sig(v[k]) for k in range(len(v))]); tv = t0 + np.arange(len(v)) / VFPS
ns = np.arange(0, 131)
pl = np.array([sig(np.asarray(Image.open(sys.argv[2] % (1613 + n)).convert("RGB").resize((640, 480), Image.BILINEAR))) for n in ns])
best = None
for s10 in range(-150, 151, 5):
    s = s10 / 10
    r = np.interp([re2_t(n) + s / 29.9 for n in ns], tv, re2)
    c = np.corrcoef(r, pl)[0, 1]
    if best is None or c > best[1]: best = (s, c)
print("Segment 1 (Bild 0..130): bester Versatz %+.1f Spielbilder, r = %.3f" % best)
r0 = np.interp([re2_t(n) for n in ns], tv, re2); r = np.interp([re2_t(n) + best[0] / 29.9 for n in ns], tv, re2)
for n in list(range(0, 40, 2)) + list(range(40, 131, 10)): print("%4d RE2 %5.2f RE2+V %5.2f Port %5.2f" % (n, r0[n], r[n], pl[n]))
