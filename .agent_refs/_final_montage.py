from PIL import Image, ImageEnhance, ImageDraw, ImageFont
import os
BASE = r"C:\workspace\git\reAi\psx_dev\re15_reborn_pc"
OUT  = r"C:\workspace\git\reAi\.agent_refs"

# representative frame + classification per channel-A clip
items = [
    (0, "series_f00042.bmp", "FOLD_FORWARD (feed/bent)", (640,300,1280,960)),
    (1, "series_f00042.bmp", "FOLD_FORWARD",              (640,300,1280,960)),
    (2, "series_f00072.bmp", "ARMS_UP (fwd reach)",       (640,300,1280,960)),
    (3, "series_f00084.bmp", "UPRIGHT_ARMS_DOWN",         (640,300,1280,960)),
    (4, "series_f00042.bmp", "PRONE (on floor)",          (640,300,1280,960)),
    (5, "series_f00108.bmp", "UPRIGHT_ARMS_DOWN (walk)",  (640,360,1280,960)),
]
tw, th = 300, 360
pad=8; labelh=24
ncols=3; nrows=2
W = pad+ncols*(tw+pad); H = pad+nrows*(th+labelh+pad)
canvas = Image.new("RGB",(W,H),(25,25,25))
d = ImageDraw.Draw(canvas)
try: font = ImageFont.truetype("arial.ttf", 15)
except: font = ImageFont.load_default()
for idx,(c,fname,label,crop) in enumerate(items):
    p = os.path.join(BASE, f"chanA_{c}", fname)
    ri, ci = idx//ncols, idx%ncols
    x0 = pad+ci*(tw+pad); y0 = pad+ri*(th+labelh+pad)
    if os.path.exists(p):
        im = Image.open(p).convert("RGB").crop(crop)
        im = ImageEnhance.Brightness(im).enhance(2.0)
        im = im.resize((tw,th))
        canvas.paste(im,(x0,y0+labelh))
    col = (0,255,0) if "UPRIGHT_ARMS_DOWN" in label else (255,120,120)
    d.text((x0+2,y0+4), f"clip {c}: {label}", fill=col, font=font)
canvas.save(os.path.join(OUT,"chanA_classified.png"))
print("ok")
