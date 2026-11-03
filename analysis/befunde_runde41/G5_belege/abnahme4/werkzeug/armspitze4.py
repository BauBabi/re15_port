# Abnahme 4: Ausfahren Arm 0 (Bild 10..26, Schnitt 16) - Armflaeche = Pixel, die sich gegen das armlose Bezugsbild (Bild 6) um > 40
# (Summe |dR|+|dG|+|dB|) unterscheiden, im Feld x 62..300, y 250..430 (640x480, Spieler ausserhalb); RE2 jedes Videobild (halbe Spielbilder).
import subprocess, sys, os, numpy as np
from PIL import Image
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from re2zeit4 import re2_t
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"; VFPS = 59.817
src = sys.argv[1]
A3 = r"C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5abn3/lauf"
X0, X1, Y0, Y1 = 62, 300, 250, 430
def flaeche(a, ref):
    d = np.abs(a[Y0:Y1, X0:X1].astype(int) - ref[Y0:Y1, X0:X1].astype(int)).sum(axis=2) > 40
    return int(d.sum()), (int(np.where(d.any(axis=0))[0].max()) + X0 if d.any() else -1)
t0 = re2_t(4); t1 = re2_t(30)
raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "%.3f" % t0, "-t", "%.3f" % (t1 - t0), "-i", src,
                      "-vf", "scale=640:480,format=rgb24", "-f", "rawvideo", "-"], capture_output=True).stdout
v = np.frombuffer(raw, np.uint8).reshape(-1, 480, 640, 3)
ref = v[int((re2_t(6) - t0) * VFPS)]
print("RE2 (Videobild -> Szenenbild nach re2_t):")
re2 = []
for k in range(len(v)):
    n = 4 + (t0 + k / VFPS - re2_t(4)) * 29.9
    f, x = flaeche(v[k], ref); re2.append((n, f, x))
    if 14 <= n <= 27: print("  RE2 n=%5.1f Flaeche %5d rechtester Armpixel x %d" % (n, f, x))
def port(pfad):
    ref = np.asarray(Image.open(pfad % (1613 + 6)).convert("RGB").resize((640, 480), Image.BILINEAR))
    out = []
    for n in range(10, 28):
        a = np.asarray(Image.open(pfad % (1613 + n)).convert("RGB").resize((640, 480), Image.BILINEAR)); out.append((n,) + flaeche(a, ref))
    return out
for nm, p in (("vorher", A3 + "/fd_leon/fd/f%06d.ppm"), ("Leon NB4", "lauf/fd_leon/fd/f%06d.ppm"), ("Elza NB4", "lauf/fd_elza/fd/f%06d.ppm")):
    print(nm, " ".join("%d:%d/%d" % t for t in port(p)))
# Versatz: fuer jede Port-Flaeche das RE2-n mit gleicher Flaeche (lineare Interpolation, steigender Teil) -> n_RE2 - n_Port
import bisect
r = [(n, f) for n, f, x in re2 if 10 <= n <= 27]
def n_bei(fl):
    for (n1, f1), (n2, f2) in zip(r, r[1:]):
        if f1 <= fl <= f2 and f2 > f1: return n1 + (fl - f1) / (f2 - f1) * (n2 - n1)
    return None
for nm, p in (("vorher", A3 + "/fd_leon/fd/f%06d.ppm"), ("Leon NB4", "lauf/fd_leon/fd/f%06d.ppm")):
    vs = []
    for n, f, x in port(p):
        if 300 < f < max(ff for _, ff in r) * 0.9:
            m = n_bei(f)
            if m is not None: vs.append(m - n)
    print("%s: gleiche Armflaeche wie RE2 bei RE2-n - Port-n = %s (Mittel %+.1f)" % (nm, ["%+.1f" % x for x in vs], sum(vs) / len(vs) if vs else 0))
