# Leser B (SF V4): gemeinsame Helfer. Bildnummer n = 1-basiert wie SF_belege/roh/alle (Formblatt, h = n - 1350);
# allf/f_XXXX.png im Laufordner ist 0-basiert ab Videobild-Index 1200 -> Datei = n - 1 - 1200.
import numpy as np
from PIL import Image
from skimage.color import rgb2lab, deltaE_ciede2000
SCHWARZ = 17.0
def bild(S, n): return np.asarray(Image.open(f'{S}/allf/f_{n - 1 - 1200:04d}.png').convert('RGB')).astype(float)
def lab(rgb): return rgb2lab(np.clip(np.asarray(rgb, float), 0, 255).reshape(-1, 1, 3) / 255.0).reshape(-1, 3)
def stats(img, m):
    px = img[m]
    if len(px) == 0: return None
    L = lab(px)
    return dict(n=int(len(px)), rgb=[round(float(v), 1) for v in px.mean(0)], rgb_std=[round(float(v), 1) for v in px.std(0)],
                lab=[round(float(v), 1) for v in lab(px.mean(0))[0]], lab_std=[round(float(v), 1) for v in L.std(0)])
def schwarz_ab(img): return np.clip(img - SCHWARZ, 0, 255)
def de00(l1, l2): return round(float(deltaE_ciede2000(np.array([l1], float), np.array([l2], float))[0]), 1)
def fenster(x, y, w, shape=(480, 640)):
    m = np.zeros(shape, bool); r = w // 2; m[max(0, y - r):y + r + 1, max(0, x - r):x + r + 1] = True; return m
