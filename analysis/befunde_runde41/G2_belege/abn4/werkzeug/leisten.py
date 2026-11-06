"""G2 Abnahme 4: leisten.py <aus.jpg> <titel> <kopf1>=<video1> [<kopf2>=<video2> ...] -- t1 t2 ... (Zeit je Zeile: t oder t1/t2/.. je Spalte)
Bildleiste mit N Spalten (gdigrab-Videos der eigenen exe-Kopien, Normalhelligkeit), Zeilen = Videozeit in s; Bilder per ffmpeg (-ss t -frames:v 1), 320 x 240.
Abgeleitet von G2_belege/v4/werkzeug/leiste3.py (Bauer); Spalten frei waehlbar (z. B. NB5 gelobt | neu ohne Kanister | neu mit Kanister)."""
import sys, subprocess, os, tempfile
from PIL import Image, ImageDraw
aus, titel = sys.argv[1], sys.argv[2]
i = sys.argv.index("--")
spalten = [a.split("=", 1) for a in sys.argv[3:i]]
zeilen = []
for a in sys.argv[i + 1:]:
    p = [float(t) for t in a.split("/")]
    zeilen.append(p if len(p) == len(spalten) else [p[0]] * len(spalten))
FF = "C:/ProgramData/chocolatey/bin/ffmpeg"
td = tempfile.mkdtemp()
def bild(vid, t, n):
    p = os.path.join(td, "%s_%.2f.png" % (n, t))
    subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "%.2f" % t, "-i", vid, "-frames:v", "1", "-vf", "scale=320:240", "-y", p], check=False)
    return Image.open(p).convert("RGB") if os.path.exists(p) else Image.new("RGB", (320, 240), (40, 0, 0))
W, H, KO, RA = 320, 240, 22, 66
N = len(spalten)
img = Image.new("RGB", (RA + N * W + 6 * (N - 1) + 6, KO + len(zeilen) * (H + 4)), (20, 20, 20))
d = ImageDraw.Draw(img)
farbe = [(255, 255, 0), (0, 200, 255), (0, 255, 0), (255, 140, 0)]
for k, (kopf, _) in enumerate(spalten): d.text((RA + k * (W + 6), 4), kopf, fill=farbe[k % 4])
d.text((2, 4), titel[:10], fill=(200, 200, 200))
for r, ts in enumerate(zeilen):
    y = KO + r * (H + 4)
    for k, (_, vid) in enumerate(spalten): img.paste(bild(vid, ts[k], "v%d" % k), (RA + k * (W + 6), y))
    d.text((4, y + H // 2), "t %.1f s" % ts[-1], fill=(255, 255, 255))
img.save(aus, quality=88)
print("geschrieben", aus, img.size)
