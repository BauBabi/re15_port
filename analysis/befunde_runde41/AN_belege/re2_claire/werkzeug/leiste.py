"""leiste.py VIDEO OUT.jpg T0 T1 N [COLS] [BREITE] [TITEL] - Bildleiste: N Bilder gleichmaessig in [T0, T1] (Videozeit),
je Bild mit Videozeit und PAL-Bildnummer (49,74682 Bilder/s, DuckStation-Log) beschriftet."""
import sys, subprocess, io
from PIL import Image, ImageDraw
FF = r"C:\ProgramData\chocolatey\bin\ffmpeg.exe"
FPS = 49.74682
v, out, t0, t1, n = sys.argv[1], sys.argv[2], float(sys.argv[3]), float(sys.argv[4]), int(sys.argv[5])
cols = int(sys.argv[6]) if len(sys.argv) > 6 else min(n, 4)
bw = int(sys.argv[7]) if len(sys.argv) > 7 else 320
titel = sys.argv[8] if len(sys.argv) > 8 else ""
ts = [t0 + (t1 - t0) * i / max(1, n - 1) for i in range(n)]
ims = []
for t in ts:
    p = subprocess.run([FF, "-v", "error", "-ss", "%.3f" % t, "-i", v, "-frames:v", "1", "-vf", "scale=%d:-2" % bw,
                        "-f", "image2pipe", "-vcodec", "png", "-"], capture_output=True)
    ims.append(Image.open(io.BytesIO(p.stdout)).convert("RGB"))
w, h = ims[0].size
top = 16 if titel else 0
rows = (n + cols - 1) // cols
W = Image.new("RGB", (cols * w, top + rows * (h + 14)), (16, 16, 16))
d = ImageDraw.Draw(W)
if titel:
    d.text((4, 2), titel, fill=(255, 255, 255))
for i, (t, im) in enumerate(zip(ts, ims)):
    x, y = (i % cols) * w, top + (i // cols) * (h + 14)
    W.paste(im, (x, y + 14))
    d.text((x + 3, y + 1), "%.2f s  (Bild %d)" % (t, round(t * FPS)), fill=(255, 230, 0))
W.save(out, quality=90)
print(out, W.size)
