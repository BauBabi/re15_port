"""licht_mass.py : mittlere Farbe der G5-Masse (Region unten links, Schnitt 16) - RE2 echt / Port NB1 / Port NB2, Bild 180/240."""
import json, subprocess
from PIL import Image
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"
SP = r"C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad"
def bild(video, t):
    subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-y", "-ss", "%.3f" % t, "-i", video, "-frames:v", "1", SP + "/g5nb2/_lm.png"], check=True)
    return Image.open(SP + "/g5nb2/_lm.png").convert("RGB").resize((400, 300))
def mittel(im):
    px = [im.getpixel((x, y)) for x in range(60, 170) for y in range(185, 290)]
    n = len(px)
    r = sum(p[0] for p in px) / n; g = sum(p[1] for p in px) / n; b = sum(p[2] for p in px) / n
    hell = sum(1 for p in px if p[0] > 150 and p[1] > 90) / n
    return r, g, b, hell
quellen = [("RE2 echt", SP + "/re2/re2_g5_run1.mp4", None)]
for lauf in ("g5nb1", "g5nb2"):
    z = json.load(open(SP + "/%s/lauf/leon/zeit.json" % lauf))
    quellen.append(("Port %s Leon" % lauf, SP + "/%s/lauf/leon/fenster.mp4" % lauf, z))
for b in (180, 240):
    for name, vid, z in quellen:
        t = 63.48 + b / 30.0 if z is None else z["a"] + z["b"] * (z["F_arm"] + b)
        r, g, bl, hell = mittel(bild(vid, t))
        print("Bild %d %-16s R %.0f G %.0f B %.0f  Anteil orange-hell (R>150,G>90) %.1f %%" % (b, name, r, g, bl, 100 * hell))
