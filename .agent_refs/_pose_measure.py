import numpy as np
from PIL import Image
import sys, os

BASE = r"C:\workspace\git\reAi\psx_dev\re15_reborn_pc"

def seg(path, crop):
    im = Image.open(path).convert("RGB").crop(crop)
    a = np.asarray(im).astype(np.int32)
    r,g,b = a[...,0],a[...,1],a[...,2]
    # zombie = grayish/pinkish clothing + skin. Carpet = green (g dominant, dark).
    # mask: NOT strongly-green carpet AND bright enough.
    bright = (r+g+b) > 150
    not_green = ~((g > r+12) & (g > b+12))   # carpet is green-dominant
    # also exclude very dark (shadow) and the wood table (orange r>>g) on far left handled by crop
    mask = bright & not_green & ((r+g+b) < 720)
    return im, mask

def measure(mask):
    ys, xs = np.where(mask)
    if len(xs) < 200:
        return None
    # principal axis via PCA of the silhouette pixels
    pts = np.stack([xs, ys], 1).astype(np.float64)
    c = pts.mean(0)
    cov = np.cov((pts-c).T)
    w, v = np.linalg.eigh(cov)
    major = v[:, np.argmax(w)]           # principal direction
    # angle of major axis from vertical (image y-down). vertical = (0,1)
    ang = np.degrees(np.arctan2(abs(major[0]), abs(major[1])))  # 0=vertical,90=horizontal
    h = ys.max()-ys.min(); wd = xs.max()-xs.min()
    aspect = h/max(wd,1)
    # arm/shoulder: fraction of mass in upper third width (arms-up spreads width up high)
    y0,y1 = ys.min(), ys.max()
    band = (ys < y0 + (y1-y0)*0.45)      # upper 45%
    upper_w = (xs[band].max()-xs[band].min()) if band.sum()>10 else 0
    lower = (ys >= y0 + (y1-y0)*0.55)
    lower_w = (xs[lower].max()-xs[lower].min()) if lower.sum()>10 else 0
    return dict(npix=len(xs), tilt_deg=round(ang,1), aspect_h_w=round(aspect,2),
                upper_w=int(upper_w), lower_w=int(lower_w),
                upper_lower_wratio=round(upper_w/max(lower_w,1),2))

cases = [
    ("clip5 UPRIGHT_ARMS_DOWN", "chanA_5/series_f00108.bmp", (820,360,1080,920)),
    ("clip0 FOLD_FORWARD",      "chanA_0/series_f00042.bmp", (840,460,1130,920)),
    ("clip2 ARMS_UP",           "chanA_2/series_f00072.bmp", (980,420,1280,920)),
    ("clip3 UPRIGHT(cluster)",  "chanA_3/series_f00084.bmp", (980,420,1180,920)),
    ("clip4 PRONE",             "chanA_4/series_f00042.bmp", (700,650,1180,920)),
]
for name, rel, crop in cases:
    p = os.path.join(BASE, rel)
    if not os.path.exists(p):
        print(f"{name}: MISSING"); continue
    im, m = seg(p, crop)
    r = measure(m)
    print(f"{name:32s} {r}")
