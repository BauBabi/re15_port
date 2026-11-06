import subprocess, numpy as np
from PIL import Image
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"; VFPS = 59.817
SRC = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5abn2/re2/re2_lauf1.mp4"
t0 = 61.40; dur = 1.6
raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "%.3f" % t0, "-t", "%.3f" % dur, "-i", SRC,
                      "-vf", "format=gray", "-f", "rawvideo", "-"], capture_output=True).stdout
n = len(raw) // (640*480)
v = np.frombuffer(raw[:n*640*480], np.uint8).reshape(n, 480, 640).astype(float)
ref = v[-1]
# Spalten 250..600 (rechts, ohne Figur am Anfang?) Zeilenprofil
def prof(a): return a[:, 300:620].mean(axis=1)
pr = prof(ref)
# Balkenkanten
for k in range(n):
    p = prof(v[k])
    best = None
    for dy in range(-20, 21):
        a = p[60+dy:420+dy] if dy>=0 else p[60+dy:420+dy]
        e = np.abs(a - pr[60:420]).mean()
        if best is None or e < best[1]: best = (dy, e)
    # obere Balkenkante: erste Zeile mit Mittel > 8
    rows = np.where(v[k].mean(axis=1) > 6)[0]
    print("t %.3f Szenenbild %6.1f  Verschiebung %+d (Fehler %.1f)  hell-Zeilen %d..%d" % (t0 + k/VFPS, (t0 + k/VFPS - 63.47)*29.9, best[0], best[1], rows.min() if len(rows) else -1, rows.max() if len(rows) else -1))
