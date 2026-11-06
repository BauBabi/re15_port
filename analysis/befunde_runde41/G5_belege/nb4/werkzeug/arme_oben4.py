# Nachbesserung 4 G5A (Messung aus Abnahme 3 arme_oben.py uebernommen): Rotanteil (R-G > 25 und R > 50) im Feld x 180..560, y 40..170
# (640x480) je Szenenbild - RE2 echt | Port vorher (8c45dbd14, Framedump Abnahme 3) | Leon nachher | Elza nachher. Port-Bild n = F1613 + n.
import numpy as np, subprocess, sys
from PIL import Image
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"; FPS = 29.9
def re2_t(n):
    if n <= 132: return 63.47 + n / FPS
    if n <= 437: return 68.261 + (n - 133) / FPS
    if n <= 807: return 79.649 + (n - 438) / FPS
    return 92.776 + (n - 807) / FPS
src, A3 = sys.argv[1], sys.argv[2]
raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "%.3f" % re2_t(560), "-t", "%.3f" % (re2_t(806) - re2_t(560)), "-i", src,
                      "-vf", "scale=640:480,format=rgb24", "-f", "rawvideo", "-"], capture_output=True).stdout
v = np.frombuffer(raw, np.uint8).reshape(-1, 480, 640, 3).astype(np.int16)
def rot(a):
    a = a[40:170, 180:560]; return float(((a[..., 0] - a[..., 1] > 25) & (a[..., 0] > 50)).mean() * 100)
def lade(p): return np.asarray(Image.open(p).convert("RGB").resize((640, 480)), dtype=np.int16)
t0 = re2_t(560)
for n in range(600, 806, 10):
    k = min(int(round((re2_t(n) - t0) * 59.8)), len(v) - 1)
    print("Bild %d  Rotanteil %%: RE2 %5.2f  vorher %5.2f  Leon %5.2f  Elza %5.2f" % (n, rot(v[k]), rot(lade(A3 + "/fd_leon/fd/f%06d.ppm" % (1613 + n))),
          rot(lade("lauf/fd_leon/fd/f%06d.ppm" % (1613 + n))), rot(lade("lauf/fd_elza/fd/f%06d.ppm" % (1613 + n)))))
