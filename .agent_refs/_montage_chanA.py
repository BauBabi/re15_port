import os, glob
from PIL import Image, ImageDraw, ImageFont

BASE = r"C:\workspace\git\reAi\psx_dev\re15_reborn_pc"
OUT  = r"C:\workspace\git\reAi\.agent_refs"

# Crop region for the zombie cluster (1280x960 frame). Right portion where zombies stand.
# We'll crop a wide right band and scale.
CROP = (760, 300, 1280, 960)  # left, top, right, bottom

def load_clip(clipdir, want_frames):
    files = sorted(glob.glob(os.path.join(BASE, clipdir, "series_*.bmp")))
    out = []
    for f in files:
        n = int(os.path.basename(f).split('_f')[1].split('.')[0])
        out.append((n, f))
    return out

clips = [0,1,2,3,4,5]
# pick up to 3 frames spread across each clip
rows = []
for c in clips:
    fr = load_clip(f"chanA_{c}", None)
    if not fr:
        rows.append((c, []))
        continue
    # spread: first, middle, last
    idxs = sorted(set([0, len(fr)//2, len(fr)-1]))
    sel = [fr[i] for i in idxs]
    rows.append((c, sel))

# tile dims
tw, th = 260, 330
pad = 6
labelh = 22
ncols = 3
nrows = len(rows)
W = pad + ncols*(tw+pad)
H = pad + nrows*(th+labelh+pad)
canvas = Image.new("RGB", (W, H), (30,30,30))
d = ImageDraw.Draw(canvas)
try:
    font = ImageFont.truetype("arial.ttf", 16)
except:
    font = ImageFont.load_default()

for ri,(c,sel) in enumerate(rows):
    y0 = pad + ri*(th+labelh+pad)
    d.text((pad, y0), f"chanA clip {c}", fill=(255,255,0), font=font)
    for ci,(n,f) in enumerate(sel):
        im = Image.open(f).convert("RGB").crop(CROP)
        im = im.resize((tw,th))
        x0 = pad + ci*(tw+pad)
        canvas.paste(im, (x0, y0+labelh))
        d.text((x0+2, y0+labelh+2), f"f{n}", fill=(0,255,0), font=font)

canvas.save(os.path.join(OUT, "chanA_montage.png"))
print("saved chanA_montage.png", canvas.size)
