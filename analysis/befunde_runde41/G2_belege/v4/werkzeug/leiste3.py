"""G2-RE2 NB2 (Vorschlag 4): leiste3.py <video_ohne> <video_f942> <video_neu> <aus.jpg> <titel> t1 t2 ... (Zeit je Spalte: t oder t_ohne/t_f942/t_neu)
Bildleiste "NB5 ohne Kanister | f942f56c0 | neu" (Normalhelligkeit, gdigrab-Videos der eigenen exe-Kopien), Zeilen = Videozeit in s.
Bilder per ffmpeg (-ss t -frames:v 1), halbe Fenstergroesse (320 x 240). Kopie/Erweiterung von G2_belege/re2/werkzeug/leiste.py."""
import sys, subprocess, os, tempfile
from PIL import Image, ImageDraw
v = sys.argv[1:4]; aus, titel = sys.argv[4:6]
zeilen = []
for a in sys.argv[6:]:
    p = [float(t) for t in a.split("/")]
    zeilen.append(p if len(p) == 3 else [p[0]] * 3)
FF = "C:/ProgramData/chocolatey/bin/ffmpeg"
td = tempfile.mkdtemp()
def bild(vid, t, n):
    p = os.path.join(td, "%s_%.2f.png" % (n, t))
    subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "%.2f" % t, "-i", vid, "-frames:v", "1", "-vf", "scale=320:240", "-y", p], check=False)
    return Image.open(p).convert("RGB") if os.path.exists(p) else Image.new("RGB", (320, 240), (40, 0, 0))
W, H, KO, RA = 320, 240, 22, 66
img = Image.new("RGB", (RA + 3 * W + 12, KO + len(zeilen) * (H + 4)), (20, 20, 20))
d = ImageDraw.Draw(img)
kopf = ["NB5 ohne Kanister (Messhaken)", "f942f56c0 (Kanister zaehlt)", "neu (Vorschlag 4)"]
farbe = [(255, 255, 0), (255, 140, 0), (0, 255, 0)]
for k in range(3): d.text((RA + k * (W + 6), 4), kopf[k], fill=farbe[k])
d.text((2, 4), titel[:10], fill=(200, 200, 200))
for i, ts in enumerate(zeilen):
    y = KO + i * (H + 4)
    for k in range(3): img.paste(bild(v[k], ts[k], "v%d" % k), (RA + k * (W + 6), y))
    d.text((4, y + H // 2), "t %.1f s" % ts[2], fill=(255, 255, 255))
img.save(aus, quality=88)
print("geschrieben", aus, img.size)
