"""wa2_wechsel.py - Abnahme W2: listet jeden Bildwechsel mit Aenderung > 8 ausserhalb der Figur-Kaesten (wie wa2_blitz.py),
mit Kasten der geaenderten Bildpunkte (320x240), und misst ein Pruef-Fenster je Bild (Mittel, max).
Aufruf: wa2_wechsel.py <mkv> <ab_s> <befund_lauf.log> [x0 x1 y0 y1]"""
import re, subprocess, sys, numpy as np
FF = r"C:\ProgramData\chocolatey\bin\ffmpeg.exe"
mkv, ab, bef = sys.argv[1], float(sys.argv[2]), open(sys.argv[3], encoding="utf-8", errors="replace").read()
fen = tuple(map(int, sys.argv[4:8])) if len(sys.argv) > 7 else None
raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", str(ab), "-i", mkv, "-vf", "scale=320:240:flags=area",
                      "-f", "rawvideo", "-pix_fmt", "rgb24", "-"], capture_output=True).stdout
n = len(raw) // (320 * 240 * 3)
g = np.frombuffer(raw[: n * 320 * 240 * 3], np.uint8).reshape(n, 240, 320, 3).astype(np.int16).mean(axis=3)
maske = np.zeros((240, 320), bool)
for x0, x1, y0, y1 in [tuple(map(int, m.groups())) for m in re.finditer(r"kasten=x(-?\d+)\.\.(-?\d+),y(-?\d+)\.\.(-?\d+)", bef)]:
    maske[max(0, y0 - 45):max(0, min(240, y1 + 9)), max(0, x0 - 12):max(0, min(320, x1 + 13))] = True
d = np.abs(np.diff(g, axis=0)) > 8
for t in range(n - 1):
    m = d[t] & ~maske
    if m.any():
        ys, xs = np.nonzero(m)
        print("Bild %d->%d: %d Bildpunkte x%d..%d y%d..%d" % (t, t + 1, m.sum(), xs.min(), xs.max(), ys.min(), ys.max()))
if fen:
    x0, x1, y0, y1 = fen
    w = g[:, y0:y1 + 1, x0:x1 + 1]
    print("Fenster x%d..%d y%d..%d: Mittel min/max %.1f/%.1f, Punkt-max min/max %.0f/%.0f" % (x0, x1, y0, y1, w.mean(axis=(1, 2)).min(), w.mean(axis=(1, 2)).max(), w.max(axis=(1, 2)).min(), w.max(axis=(1, 2)).max()))
    s = w.max(axis=(1, 2)); ch = np.nonzero(np.abs(np.diff(s)) > 8)[0]
    print("  Spruenge > 8 im Fenster-Max: %d, bei Bild %s" % (len(ch), list(ch[:40])))
