# Abnahme 3: Arme ueber der Masse in Schnitt 17 - Rotanteil (R-G > 25 und R > 50) im Feld x 180..560, y 40..170 (640x480) je Szenenbild,
# RE2 echt (re2_t) gegen Port-Framedump. Arme sind dunkelrote Stangen vor grauem Wagen; die Masse liegt in diesem Feld erst unterhalb y 170.
import numpy as np, subprocess, sys
from PIL import Image
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"; FPS = 29.9
def re2_t(n):
    if n <= 132: return 63.47 + n / FPS
    if n <= 437: return 68.261 + (n - 133) / FPS
    if n <= 807: return 79.649 + (n - 438) / FPS
    return 92.776 + (n - 807) / FPS
src = sys.argv[1]
raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "%.3f" % re2_t(560), "-t", "%.3f" % (re2_t(806) - re2_t(560)), "-i", src,
                      "-vf", "scale=640:480,format=rgb24", "-f", "rawvideo", "-"], capture_output=True).stdout
v = np.frombuffer(raw, np.uint8).reshape(-1, 480, 640, 3).astype(np.int16)
def rot(a):
    a = a[40:170, 180:560]; return float(((a[..., 0] - a[..., 1] > 25) & (a[..., 0] > 50)).mean() * 100)
t0 = re2_t(560)
for n in range(560, 806, 10):
    k = int(round((re2_t(n) - t0) * 59.8)); k = min(k, len(v) - 1)
    p = np.asarray(Image.open("lauf/fd_leon/fd/f%06d.ppm" % (1613 + n)).convert("RGB").resize((640, 480)), dtype=np.int16)
    q = np.asarray(Image.open("lauf/fd_elza/fd/f%06d.ppm" % (1613 + n)).convert("RGB").resize((640, 480)), dtype=np.int16)
    print("Bild %d  Rotanteil %%: RE2 %5.2f  Leon %5.2f  Elza %5.2f" % (n, rot(v[k]), rot(p), rot(q)))
