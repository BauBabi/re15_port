# Abnahme 3: Schnittkanten + Standbilder im RE2-Mitschnitt selbst messen (80x60, alle Bilder)
import subprocess, numpy as np, sys
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"
src = sys.argv[1]
raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "55", "-t", "55", "-i", src, "-vf", "scale=80:60,format=gray", "-f", "rawvideo", "-"], capture_output=True).stdout
v = np.frombuffer(raw, np.uint8).reshape(-1, 60, 80).astype(np.int16)
fps = 59.8
d = np.abs(np.diff(v, axis=0)).mean(axis=(1, 2))
np.save("re2_diff3.npy", d)
print("Bilder", len(v))
for i in np.where(d > 25)[0]: print("Kante %.3f s diff %.1f" % (55 + (i + 1) / fps, d[i]))
# Standbilder: >= 20 Videobilder Differenz 0
run = 0
for i, x in enumerate(d):
    if x < 0.05: run += 1
    else:
        if run >= 15: print("Standbild %.3f..%.3f s (%d Videobilder)" % (55 + (i + 1 - run) / fps, 55 + (i + 1) / fps, run))
        run = 0
