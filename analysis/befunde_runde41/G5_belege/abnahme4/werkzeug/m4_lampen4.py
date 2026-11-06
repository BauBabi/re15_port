# Abnahme 4 (M4): Tuerloch (x 40..210, y 60..330 bei 640x480) RE2 je Videobild vs Port Leon - Helligkeitsspitzen (Tunnellampen) zaehlen
# und Kontaktbogen RE2 / Port Bild -55..120 (jedes 3. Spielbild).
import subprocess, sys, os, numpy as np
from PIL import Image, ImageDraw
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from re2zeit4 import re2_t
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"; VFPS = 59.817
src = sys.argv[1]; X0, X1, Y0, Y1 = 40, 210, 60, 330
t0 = re2_t(-60); t1 = re2_t(130)
raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "%.3f" % t0, "-t", "%.3f" % (t1 - t0), "-i", src,
                      "-vf", "scale=640:480,format=rgb24", "-f", "rawvideo", "-"], capture_output=True).stdout
v = np.frombuffer(raw, np.uint8).reshape(-1, 480, 640, 3)
def hell(a): g = a[Y0:Y1, X0:X1].astype(float).mean(axis=2); return float((g > 60).mean() * 100), float(g.mean())
print("RE2 Tuerloch: Anteil Pixel > 60 (Lampe/Truemmer) je Spielbild")
re2 = []
for k in range(0, len(v), 2):
    n = -60 + (t0 + k / VFPS - re2_t(-60)) * 29.9
    re2.append((n, hell(v[k])))
for n, (p, m) in re2:
    if int(round(n)) % 3 == 0: print("  RE2 n=%6.1f hell %5.2f %% Mittel %5.1f" % (n, p, m))
port = []
for n in range(-55, 131):
    a = np.asarray(Image.open("lauf/fd_leon/fd/f%06d.ppm" % (1613 + n)).convert("RGB").resize((640, 480), Image.BILINEAR)); port.append((n, hell(a)))
print("Port Leon Tuerloch: max hell %.2f %%, Mittel %.1f..%.1f" % (max(p for _, (p, m) in port), min(m for _, (p, m) in port), max(m for _, (p, m) in port)))
spitzen = [n for (n, (p, m)), (_, (p0, _m)) in zip(re2[1:], re2) if p > 0.8 and p0 <= 0.8]
print("RE2 Lampen-Einsaetze (Anteil > 0,8 %%): %s" % ["%.0f" % s for s in spitzen])
# Kontaktbogen
ns = list(range(-54, 121, 6))
W, H = X1 - X0, Y1 - Y0
img = Image.new("RGB", (W * 2 * 2 + 8, (H * 2 + 12) * ((len(ns) + 1) // 2))); dr = ImageDraw.Draw(img)
for i, n in enumerate(ns):
    r, c = i // 2, i % 2
    k = int(round((re2_t(n) - 2 / 29.9 - t0) * VFPS)); k = max(0, min(k, len(v) - 1))
    a = Image.fromarray(v[k]).crop((X0, Y0, X1, Y1)); b = Image.open("lauf/fd_leon/fd/f%06d.ppm" % (1613 + n)).convert("RGB").resize((640, 480)).crop((X0, Y0, X1, Y1))
    x = c * (W * 2 + 8); y = r * (H * 2 + 12)
    img.paste(a.resize((W, H * 2)), (x, y + 12)); img.paste(b.resize((W, H * 2)), (x + W, y + 12))
    dr.text((x + 2, y), "RE2 %d | Port %d" % (n, n), fill=(255, 255, 0))
img.save("bilder/a4_m4_tuerloch_re2_port.jpg", quality=88); print("bilder/a4_m4_tuerloch_re2_port.jpg", img.size)
