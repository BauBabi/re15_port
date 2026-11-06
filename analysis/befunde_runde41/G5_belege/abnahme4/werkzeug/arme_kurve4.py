# Abnahme 4: Arm-Signal (Rotanteil R-G>25, R>50 im Feld ueber der Masse x 180..560, y 40..170 bei 640x480) je Spielbild in Schnitt 17
# (Bild 440..806): RE2 (jedes 2. Videobild) und Port Leon/Elza (Framedump). Kreuzkorrelation -> Versatz der RE2-Zeitachse; dann Kante [T7].
import subprocess, sys, os, numpy as np
from PIL import Image
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from re2zeit4 import re2_t
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"; VFPS = 59.817
src = sys.argv[1]
def rot(a):
    a = a[40:170, 180:560].astype(np.int16); return float(((a[..., 0] - a[..., 1] > 25) & (a[..., 0] > 50)).mean() * 100)
t0 = re2_t(440) - 30 / 29.9; t1 = re2_t(806)
raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "%.3f" % t0, "-t", "%.3f" % (t1 - t0), "-i", src,
                      "-vf", "scale=640:480,format=rgb24", "-f", "rawvideo", "-"], capture_output=True).stdout
v = np.frombuffer(raw, np.uint8).reshape(-1, 480, 640, 3)
re2 = np.array([rot(v[k]) for k in range(len(v))])
tv = t0 + np.arange(len(v)) / VFPS
def port(f): return np.array([rot(np.asarray(Image.open(f % (1613 + n)).convert("RGB").resize((640, 480), Image.BILINEAR))) for n in range(440, 807)])
pl = port(sys.argv[2]); pe = port(sys.argv[3])
ns = np.arange(440, 807)
# RE2-Signal an re2_t(n) + s/29.9 abtasten, Versatz s in Spielbildern
best = None
for s10 in range(-150, 151, 5):
    s = s10 / 10
    r = np.interp([re2_t(n) + s / 29.9 for n in ns], tv, re2)
    c = np.corrcoef(r, pl)[0, 1]
    if best is None or c > best[1]: best = (s, c)
print("Kreuzkorrelation Bild 440..806: bester Versatz %+.1f Spielbilder (RE2-Zeit = re2_t(n) %+.1f/29.9), r = %.3f" % (best[0], best[0], best[1]))
s = best[0]
r = np.interp([re2_t(n) + s / 29.9 for n in ns], tv, re2)
r0 = np.interp([re2_t(n) for n in ns], tv, re2)
print("Bild  RE2(re2_t)  RE2(+Versatz)  Leon   Elza")
for n in list(range(630, 690, 2)) + list(range(700, 807, 20)):
    i = n - 440
    print("%4d   %5.2f        %5.2f        %5.2f  %5.2f" % (n, r0[i], r[i], pl[i], pe[i]))
np.save("arme_kurve.npy", np.vstack([ns, r0, r, pl, pe]))
