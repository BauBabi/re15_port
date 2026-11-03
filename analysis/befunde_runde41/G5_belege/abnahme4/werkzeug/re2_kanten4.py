# Abnahme 4: Schnittkanten und Standbilder (CD-Lesepausen) im RE2-Mitschnitt selbst messen, 80x60 Graustufen, alle Videobilder
import subprocess, numpy as np, sys
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"
src, t0, dauer = sys.argv[1], 55.0, 55.0
raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", str(t0), "-t", str(dauer), "-i", src, "-vf", "scale=80:60,format=gray",
                      "-f", "rawvideo", "-"], capture_output=True).stdout
v = np.frombuffer(raw, np.uint8).reshape(-1, 60, 80).astype(np.int16)
fps = float(sys.argv[2]) if len(sys.argv) > 2 else 59.8
d = np.abs(np.diff(v, axis=0)).mean(axis=(1, 2))
print("Videobilder", len(v))
for i in np.where(d > 25)[0]: print("Kante %.3f s (diff %.1f)" % (t0 + (i + 1) / fps, d[i]))
lauf = 0
for i, x in enumerate(list(d) + [99]):
    if x < 0.05: lauf += 1
    else:
        if lauf >= 15: print("Standbild %.3f .. %.3f s (%d Videobilder)" % (t0 + (i + 1 - lauf) / fps, t0 + (i + 1) / fps, lauf))
        lauf = 0
