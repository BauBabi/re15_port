"""wa_folge.py - Abnahme W1: Bildfolge-Blatt (vorher | nachher) aus zwei Aufnahmen, gleiche Bildnummern ab <ab_s>.
Aufruf: wa_folge.py <mkv_oben> <mkv_unten> <ab_s> <von> <bis> <schritt> <out.jpg> [titel_oben] [titel_unten] [--box x0 y0 x1 y1] [--skala k]"""
import subprocess, sys, numpy as np
from PIL import Image, ImageDraw
FF = r"C:\ProgramData\chocolatey\bin\ffmpeg.exe"
a = sys.argv[1:]
m1, m2, ab, von, bis, st, out = a[0], a[1], float(a[2]), int(a[3]), int(a[4]), int(a[5]), a[6]
t1 = a[7] if len(a) > 7 and not a[7].startswith("--") else "oben"; t2 = a[8] if len(a) > 8 and not a[8].startswith("--") else "unten"
box = (0, 0, 320, 240); sk = 2
if "--box" in a: k = a.index("--box"); box = tuple(int(v) for v in a[k + 1:k + 5])
if "--skala" in a: sk = int(a[a.index("--skala") + 1])
def lade(m):
    raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", str(ab), "-i", m, "-f", "rawvideo", "-pix_fmt", "rgb24", "-"], capture_output=True).stdout
    n = len(raw) // (320 * 240 * 3)
    return np.frombuffer(raw[: n * 320 * 240 * 3], np.uint8).reshape(n, 240, 320, 3)
A, B = lade(m1), lade(m2)
fr = list(range(von, bis + 1, st))
w = (box[2] - box[0]) * sk; h = (box[3] - box[1]) * sk
sp = int(a[a.index("--spalten") + 1]) if "--spalten" in a else 4
zeilen = (len(fr) + sp - 1) // sp
S = Image.new("RGB", (sp * (w + 2), zeilen * 2 * (h + 16) + zeilen * 6), (25, 25, 25)); dr = ImageDraw.Draw(S)
for k, i in enumerate(fr):
    zk, sk2 = divmod(k, sp)
    for row, (X, t) in enumerate(((A, t1), (B, t2))):
        if i >= len(X): continue
        im = Image.fromarray(X[i]).crop(box).resize((w, h), Image.NEAREST)
        x = sk2 * (w + 2); y = zk * (2 * (h + 16) + 6) + row * (h + 16)
        S.paste(im, (x, y + 14)); dr.text((x + 2, y + 1), "%s Bild %d (+%.2fs)" % (t, i, i / 30.0), fill=(255, 255, 0) if row == 0 else (120, 255, 120))
S.save(out, quality=88)
print(out, S.size)
