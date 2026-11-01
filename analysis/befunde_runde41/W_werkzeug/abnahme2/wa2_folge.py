"""wa2_folge.py - Abnahme W2: Bildfolge zweier Aufnahmen nebeneinander (Zeile je Aufnahme), Ausschnitt im 960er-Fenster.
Aufruf: wa2_folge.py <out.jpg> <ab_s> <bild0> <anzahl> <x0> <y0> <w> <h> <zoom> <titel1>=<mkv1> [<titel2>=<mkv2> ...]"""
import subprocess, sys, numpy as np
from PIL import Image, ImageDraw
FF = r"C:\ProgramData\chocolatey\bin\ffmpeg.exe"
out, ab, b0, cnt, x0, y0, w, h, zoom = sys.argv[1], float(sys.argv[2]), *map(int, sys.argv[3:5]), *map(int, sys.argv[5:9]), float(sys.argv[9])
runs = [a.split("=", 1) for a in sys.argv[10:]]
tw, th = int(w * zoom), int(h * zoom)
cols = min(cnt, 8); rows_per = (cnt + cols - 1) // cols
sheet = Image.new("RGB", (cols * (tw + 2), len(runs) * rows_per * (th + 14) + 4), (30, 30, 30))
dr = ImageDraw.Draw(sheet)
for ri, (tit, mkv) in enumerate(runs):
    raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "%.3f" % (ab + b0 / 30.0), "-i", mkv, "-frames:v", str(cnt),
                          "-vf", "crop=%d:%d:%d:%d" % (w, h, x0, y0), "-f", "rawvideo", "-pix_fmt", "rgb24", "-"], capture_output=True).stdout
    n = len(raw) // (w * h * 3)
    a = np.frombuffer(raw[: n * w * h * 3], np.uint8).reshape(n, h, w, 3)
    for i in range(n):
        r, c = divmod(i, cols)
        X = c * (tw + 2); Y = (ri * rows_per + r) * (th + 14) + 14
        sheet.paste(Image.fromarray(a[i]).resize((tw, th), Image.NEAREST), (X, Y))
        dr.text((X + 2, Y - 13), "%s B%d" % (tit, b0 + i), fill=(255, 255, 0))
sheet.save(out, quality=90)
print(out, sheet.size)
