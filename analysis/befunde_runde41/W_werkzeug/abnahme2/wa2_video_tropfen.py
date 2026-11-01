"""wa2_video_tropfen.py - Abnahme W2: sucht im 80%-Video (Complete Disc Video 2, 320x240, 30 Bilder/s) kurzlebige helle Punkte
in den Fallstreifen der Tropfpunkte 3/5/7 (PSX-Bildlage aus Dossier W 2.1 (c); Video ~ PSX-Lage, gemessen am Bildvergleich).
Je Bild: Graustufe minus gleitender Median ueber +-15 Bilder; Treffer = Fleck >= 1 Bildpunkt mit +SCHWELLE in einem Streifen.
Aufruf: wa2_video_tropfen.py <mp4> <von> <bis> [schwelle]"""
import subprocess, sys, numpy as np
from scipy import ndimage
FF = r"C:\ProgramData\chocolatey\bin\ffmpeg.exe"
mp4, von, bis = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]); thr = int(sys.argv[4]) if len(sys.argv) > 4 else 30
raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-i", mp4, "-vf", "select='between(n,%d,%d)'" % (von, bis), "-vsync", "0",
                      "-f", "rawvideo", "-pix_fmt", "gray", "-"], capture_output=True).stdout
n = len(raw) // (320 * 240)
g = np.frombuffer(raw[: n * 320 * 240], np.uint8).reshape(n, 240, 320).astype(np.int16)
streifen = {"P3": (185, 225, 22, 175), "P5": (108, 135, 22, 125), "P7": (76, 102, 22, 108)}
for t in range(n):
    lo, hi = max(0, t - 15), min(n, t + 16)
    med = np.median(g[lo:hi], axis=0)
    d = g[t] - med
    for k, (x0, x1, y0, y1) in streifen.items():
        m = d[y0:y1, x0:x1] > thr
        if m.any():
            lab, c = ndimage.label(m)
            for j, sl in enumerate(ndimage.find_objects(lab), 1):
                a = int((lab[sl] == j).sum())
                print("Bild %d %s: %d Bildpunkte x%d..%d y%d..%d +%d" % (von + t, k, a, x0 + sl[1].start, x0 + sl[1].stop - 1, y0 + sl[0].start, y0 + sl[0].stop - 1, int(d[y0:y1, x0:x1][sl][lab[sl] == j].max())))
