# NB5: Bezug - mittlere Helligkeit ruhiger Hintergrundflaechen (RE2-BSS, in beiden dasselbe Bild) RE2-Aufnahme vs Port, Bild -30.
import subprocess, numpy as np
from PIL import Image
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"
SRC = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5abn2/re2/re2_lauf1.mp4"
L = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5nb5/lauf"
def re2_t(n): return 63.47 + n / 29.9
n = -30
raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "%.3f" % re2_t(n - 2.0), "-i", SRC, "-frames:v", "1",
                      "-vf", "scale=320:240", "-f", "rawvideo", "-pix_fmt", "rgb24", "-"], capture_output=True).stdout
a = np.frombuffer(raw[:320*240*3], np.uint8).reshape(240, 320, 3).astype(float)
p = np.asarray(Image.open("%s/fd_leon/fd/f%06d.ppm" % (L, 1613 + n)).convert("RGB").resize((320, 240), Image.BILINEAR)).astype(float)
for name, (x0, y0, x1, y1) in {"Wand rechts": (150, 130, 300, 200), "Lichtleiste Mitte": (195, 85, 225, 100), "Decke": (200, 30, 300, 60),
                               "Pfosten links": (5, 60, 40, 200)}.items():
    ra, pa = a[y0:y1, x0:x1].mean(), p[y0:y1, x0:x1].mean()
    print("%-18s RE2 %6.1f  Port %6.1f  Port/RE2 %.2f" % (name, ra, pa, pa / ra))
