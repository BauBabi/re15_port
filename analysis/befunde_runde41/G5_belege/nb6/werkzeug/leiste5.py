# NB5: Vergleichsleiste RE2 echt | Port Leon | Port Elza je Szenenbild (Bild 0 = Armierung = Spielbild F1613), optional Ausschnitt
import subprocess, numpy as np, sys, os
from PIL import Image, ImageDraw
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"; VFPS = 59.817
SRC = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5abn2/re2/re2_lauf1.mp4"
L = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5nb6/lauf"   # NB6-Abschluss 2026-11-06: Laeufe lauf6.sh (fd_leon, fd_elza, nativ, fenster)
def re2_t(n):
    if n <= 132: return 63.47 + n / 29.9
    if n <= 437: return 68.261 + (n - 133) / 29.9
    if n <= 807: return 79.649 + (n - 438) / 29.9
    return 92.776 + (n - 807) / 29.9
out = sys.argv[1]; ns = [int(a) for a in sys.argv[2].split(",")]
box = tuple(int(a) for a in sys.argv[3].split(",")) if len(sys.argv) > 3 and sys.argv[3] != "-" else (0, 0, 320, 240)
spalten = sys.argv[4].split(",") if len(sys.argv) > 4 else ["fd_leon", "fd_elza"]
skal = float(sys.argv[5]) if len(sys.argv) > 5 else 1.0
OFF = float(sys.argv[6]) if len(sys.argv) > 6 else 2.0   # RE2-Zeitachse: RE2-Bild n zeigt Szenenbild n + 2,0 (Abnahme 4)
t0 = re2_t(min(ns) - OFF) - 0.3; t1 = re2_t(max(ns)) + 0.3
raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "%.3f" % t0, "-t", "%.3f" % (t1 - t0), "-i", SRC,
                      "-vf", "scale=320:240,format=rgb24", "-f", "rawvideo", "-"], capture_output=True).stdout
v = np.frombuffer(raw, np.uint8).reshape(-1, 240, 320, 3)
x0, y0, x1, y1 = box; W, H = int((x1 - x0) * skal), int((y1 - y0) * skal)
n_sp = 1 + len(spalten)
img = Image.new("RGB", (W * n_sp + 4 * n_sp, (H + 14) * len(ns))); dr = ImageDraw.Draw(img)
for r, n in enumerate(ns):
    k = int(round((re2_t(n - OFF) - t0) * VFPS)); k = max(0, min(k, len(v) - 1))
    tiles = [Image.fromarray(v[k])]
    for sp in spalten:
        p = os.path.join(L, sp, "fd", "f%06d.ppm" % (1613 + n))
        tiles.append(Image.open(p).convert("RGB").resize((320, 240), Image.BILINEAR) if os.path.exists(p) else Image.new("RGB", (320, 240)))
    y = r * (H + 14)
    dr.text((2, y), "Bild %d:  RE2 echt (Achse -%.1f) | %s" % (n, OFF, " | ".join(spalten)), fill=(255, 255, 0))
    for c, t in enumerate(tiles):
        img.paste(t.crop(box).resize((W, H), Image.NEAREST), (c * (W + 4), y + 14))
img.save(out, quality=90); print(out, img.size)
