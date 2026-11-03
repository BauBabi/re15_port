# NB5: Beben-Leiste am Schnitt ausgerichtet: RE2 Videozeit 61,467 s + k/29,9 (Schnitt sichtbar) | Port Spielbild F1559 + k (Leon, Elza).
import subprocess, numpy as np, sys
from PIL import Image, ImageDraw
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"
SRC = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5abn2/re2/re2_lauf1.mp4"
L = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5nb5/lauf"
W, H = 240, 180
ks = list(range(0, 16))
img = Image.new("RGB", (3 * (W + 4), len(ks) * (H + 14))); dr = ImageDraw.Draw(img)
for r, k in enumerate(ks):
    raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "%.3f" % (61.475 + k / 29.9), "-i", SRC, "-frames:v", "1",
                          "-vf", "scale=320:240", "-f", "rawvideo", "-pix_fmt", "rgb24", "-"], capture_output=True).stdout
    t = [Image.fromarray(np.frombuffer(raw[:320*240*3], np.uint8).reshape(240, 320, 3))]
    for sp in ("fd_leon", "fd_elza"):
        t.append(Image.open("%s/%s/fd/f%06d.ppm" % (L, sp, 1559 + k)).convert("RGB").resize((320, 240), Image.BILINEAR))
    y = r * (H + 14)
    dr.text((2, y), "Bild %d nach dem Schnitt: RE2 echt %.3f s | Port Leon F%d | Port Elza" % (k, 61.475 + k / 29.9, 1559 + k), fill=(255, 255, 0))
    for c, im in enumerate(t):
        img.paste(im.resize((W, H), Image.BILINEAR), (c * (W + 4), y + 14))
img.save(sys.argv[1], quality=90); print(sys.argv[1], img.size)
