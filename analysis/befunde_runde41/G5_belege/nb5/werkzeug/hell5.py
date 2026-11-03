# NB5: integrierte Helligkeit des Lampenflecks (Summe Grauwert - Median des Fensters, in 320x240-Flaecheneinheiten) RE2 640x480 vs Port 960x720.
import subprocess, numpy as np
from PIL import Image
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"
SRC = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5abn2/re2/re2_lauf1.mp4"
L = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5nb5/lauf"
def re2_t(n): return 63.47 + n / 29.9
def summe(img, s, cx, cy):
    g = img.astype(float).mean(axis=2)
    w = g[int((cy - 8) * s):int((cy + 8) * s), int((cx - 16) * s):int((cx + 24) * s)]
    return float(np.clip(w - np.median(w), 0, None).sum()) / (s * s)
print("Bild  Lampe-x  Fenstermitte  RE2 (640x480)  Port Leon (960x720)  Port/RE2")
for n, cx in ((-44, 108), (-43, 95), (-42, 81), (-41, 67), (-34, 108), (-33, 95), (-32, 81), (6, 108), (7, 95), (8, 81)):
    raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "%.3f" % re2_t(n - 2.0), "-i", SRC, "-frames:v", "1",
                          "-f", "rawvideo", "-pix_fmt", "rgb24", "-"], capture_output=True).stdout
    a = np.frombuffer(raw[:640*480*3], np.uint8).reshape(480, 640, 3)
    p = np.asarray(Image.open("%s/fd_leon/fd/f%06d.ppm" % (L, 1613 + n)).convert("RGB"))
    r2, pp = summe(a, 2, cx, 121), summe(p, 3, cx, 121)
    print("%4d  %6d  %5d  %10.0f  %10.0f  %6.2f" % (n, cx, cx, r2, pp, pp / r2 if r2 else 0))
