# NB5: Beben = senkrechte Verschiebung des Szenenbilds gegen ein ruhiges Bezugsbild (Zeilenprofil x 150..310), RE2 echt und Port.
import subprocess, numpy as np, os
from PIL import Image
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"; VFPS = 59.817
SRC = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5abn2/re2/re2_lauf1.mp4"
L = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5nb5/lauf"
def prof(a): return a[:, 150:310].astype(float).mean(axis=1)
def schub(a, ref):
    pa, pr = prof(a), prof(ref); best = None
    for dy in range(-12, 13):
        e = np.abs(pa[40 + dy:200 + dy] - pr[40:200]).mean()
        if best is None or e < best[1]: best = (dy, e)
    return best
# RE2 (Spielbild = 2 Videobilder; Bezug 62,97 s)
raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "61.40", "-t", "1.6", "-i", SRC, "-vf", "scale=320:240,format=gray",
                      "-f", "rawvideo", "-"], capture_output=True).stdout
v = np.frombuffer(raw, np.uint8).reshape(-1, 240, 320)
print("RE2 echt (320x240): Zeit, Verschiebung in Zeilen (+ = nach unten)")
r2 = []
for k in range(0, len(v), 2):
    dy, e = schub(v[k], v[-1]); r2.append(dy); print("  %.3f s  %+d" % (61.40 + k / VFPS, dy))
print("Port (Spielbild, Bild = F - 1613):")
for sp in ("fd_leon", "fd_elza"):
    ref = np.asarray(Image.open(os.path.join(L, sp, "fd", "f%06d.ppm" % 1590)).convert("L").resize((320, 240), Image.BILINEAR))
    s = []
    for f in range(1556, 1576):
        a = np.asarray(Image.open(os.path.join(L, sp, "fd", "f%06d.ppm" % f)).convert("L").resize((320, 240), Image.BILINEAR))
        s.append("%d:%+d" % (f, schub(a, ref)[0]))
    print("  %s %s" % (sp, " ".join(s)))
