"""leiste.py <video_alt> <video_neu> <aus.jpg> <titel> t1 t2 ... - Bildleiste "gestern NB5 | neu" (Normalhelligkeit), Zeilen = Videozeit in s.
Bilder per ffmpeg (-ss t -frames:v 1), halbe Fenstergroesse (320 x 240), Beschriftung oben und je Zeile."""
import sys, subprocess, os, tempfile
from PIL import Image, ImageDraw
alt, neu, aus, titel = sys.argv[1:5]
ts = [float(t) for t in sys.argv[5:]]
FF = "C:/ProgramData/chocolatey/bin/ffmpeg"
td = tempfile.mkdtemp()
def bild(v, t, n):
    p = os.path.join(td, "%s_%.2f.png" % (n, t))
    subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "%.2f" % t, "-i", v, "-frames:v", "1", "-vf", "scale=320:240", "-y", p], check=False)
    return Image.open(p).convert("RGB") if os.path.exists(p) else Image.new("RGB", (320, 240), (40, 0, 0))
W, H, KO, RA = 320, 240, 22, 60
img = Image.new("RGB", (RA + 2 * W + 6, KO + len(ts) * (H + 4)), (20, 20, 20))
d = ImageDraw.Draw(img)
d.text((RA, 4), "gestern NB5 (bc5fb7c9c, Abnahme 4)", fill=(255, 255, 0)); d.text((RA + W + 6, 4), "neu (G2-RE2)", fill=(0, 255, 0))
d.text((2, 4), titel[:9], fill=(200, 200, 200))
for i, t in enumerate(ts):
    y = KO + i * (H + 4)
    img.paste(bild(alt, t, "a"), (RA, y)); img.paste(bild(neu, t, "n"), (RA + W + 6, y))
    d.text((4, y + H // 2), "t %.1f s" % t, fill=(255, 255, 255))
img.save(aus, quality=88)
print("geschrieben", aus, img.size)
