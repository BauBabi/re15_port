# NB6: Leiste am Schnitt ausgerichtet: RE2 Videozeit 61,475 s + k/29,9 (Schnitt 9 sichtbar = k 0) | Port F1559 + k (Leon, Elza).
# Argumente: ausgabe, k-Liste, Ausschnitt x0,y0,x1,y1 (320x240), Skala, Laeufe (Komma)
import subprocess, numpy as np, sys
from PIL import Image, ImageDraw
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"
SRC = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5abn2/re2/re2_lauf1.mp4"
L = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5nb6/lauf"   # NB6-Abschluss 2026-11-06: Laeufe lauf6.sh (fd_leon, fd_elza, nativ, fenster)
out = sys.argv[1]; ks = [int(a) for a in sys.argv[2].split(",")]
x0, y0, x1, y1 = [int(a) for a in sys.argv[3].split(",")]; sk = float(sys.argv[4]); laeufe = sys.argv[5].split(",")
W, H = int((x1 - x0) * sk), int((y1 - y0) * sk)
img = Image.new("RGB", ((1 + len(laeufe)) * (W + 4), len(ks) * (H + 14))); dr = ImageDraw.Draw(img)
for r, k in enumerate(ks):
    t = 61.475 + k / 29.9
    raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "%.3f" % t, "-i", SRC, "-frames:v", "1",
                          "-vf", "scale=320:240", "-f", "rawvideo", "-pix_fmt", "rgb24", "-"], capture_output=True).stdout
    tiles = [Image.fromarray(np.frombuffer(raw[:320*240*3], np.uint8).reshape(240, 320, 3))]
    for la in laeufe:
        tiles.append(Image.open("%s/%s/fd/f%06d.ppm" % (L, la, 1559 + k)).convert("RGB").resize((320, 240), Image.BILINEAR))
    y = r * (H + 14)
    dr.text((2, y), "Bild %d nach dem Schnitt: RE2 echt %.3f s | %s (F%d)" % (k, t, " | ".join(laeufe), 1559 + k), fill=(255, 255, 0))
    for c, im in enumerate(tiles):
        img.paste(im.crop((x0, y0, x1, y1)).resize((W, H), Image.NEAREST), (c * (W + 4), y + 14))
img.save(out, quality=90); print(out, img.size)
