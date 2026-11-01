"""wa_fenster_zahl.py - Abnahme W1: Bildwechsel mit Aenderung > 8 Graustufen in einem Fenster (320x240-Koordinaten), ab <ab_s>.
Aufruf: wa_fenster_zahl.py <mkv> <ab_s> x0 x1 y0 y1"""
import subprocess, sys, numpy as np
FF = r"C:\ProgramData\chocolatey\bin\ffmpeg.exe"
mkv, ab = sys.argv[1], float(sys.argv[2]); x0, x1, y0, y1 = map(int, sys.argv[3:7])
raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", str(ab), "-i", mkv, "-f", "rawvideo", "-pix_fmt", "rgb24", "-"], capture_output=True).stdout
n = len(raw) // (320 * 240 * 3)
g = np.frombuffer(raw[: n * 320 * 240 * 3], np.uint8).reshape(n, 240, 320, 3).astype(np.int16).mean(axis=3)[:, y0:y1 + 1, x0:x1 + 1]
d = np.abs(np.diff(g, axis=0))
w = (d > 8).reshape(n - 1, -1).any(axis=1)
print("%s: %d Bilder (%.1f s), Fenster x%d..%d y%d..%d: Wechsel>8 in %d von %d, groesster Sprung %.0f, Mittel min/max %.1f/%.1f" % (
    mkv.split("runs/")[-1], n, n / 30.0, x0, x1, y0, y1, int(w.sum()), n - 1, d.max(), g.mean(axis=(1, 2)).min(), g.mean(axis=(1, 2)).max()))
