# Leser A V3: gemeinsame Helfer (Lab D65 ueber OpenCV, Masken, Erosion)
import numpy as np, cv2
from PIL import Image
SRC = r'C:/workspace/git/reAi_v2/Version_80%/'
SCR = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv3/'
BEL = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v3_lesung/'
# Zuordnung birkinNN -> (n, h, Versatz im 1280x960-Bild) aus SF_LESER_B.md Z.39-62
MAP = {1: (1351, 1, 620, 332), 2: (1354, 4, 617, 326), 3: (1370, 20, 471, 218), 4: (1374, 24, 391, 208),
       5: (1411, 61, 538, 273), 6: (1419, 69, 512, 325), 9: (1420, 70, 539, 334), 7: (1421, 71, 567, 338),
       10: (1422, 72, 582, 341), 8: (1423, 73, 490, 348), 11: (1433, 83, 548, 368), 12: (1435, 85, 547, 366),
       13: (1442, 92, 545, 365), 14: (1447, 97, 514, 362), 15: (1451, 101, 596, 351), 16: (1455, 105, 598, 334),
       17: (1459, 109, 595, 320), 18: (1462, 112, 597, 315), 19: (1468, 118, 606, 315), 20: (1475, 125, 584, 293),
       22: (1476, 126, 581, 292), 23: (1478, 128, 571, 295), 21: (1480, 130, 567, 293), 24: (1482, 132, 564, 284)}

def load(nn):
    im = np.array(Image.open(SRC + 'birkin%02d.png' % nn).convert('RGBA'))
    return im[..., :3].copy(), im[..., 3] > 128

def lab(rgb):
    return cv2.cvtColor(rgb.astype(np.float32) / 255.0, cv2.COLOR_RGB2Lab)

def erode(mask, px=2):
    k = np.ones((2 * px + 1, 2 * px + 1), np.uint8)
    return cv2.erode(mask.astype(np.uint8), k) > 0

def hbox(alpha):
    ys, xs = np.nonzero(alpha)
    return xs.min(), xs.max(), ys.min(), ys.max(), ys.max() - ys.min() + 1

def stats(rgb, labim, m):
    if m.sum() == 0:
        return None
    return dict(n=int(m.sum()), rgb=[round(float(v), 1) for v in rgb[m].mean(0)],
                lab=[round(float(v), 1) for v in labim[m].mean(0)])

def de76(a, b):
    return float(np.sqrt(sum((x - y) ** 2 for x, y in zip(a, b))))

def de2000(l1, l2):
    a = np.array([[l1]], np.float32); b = np.array([[l2]], np.float32)
    # CIEDE2000 nach Sharma 2005
    L1, a1, b1 = l1; L2, a2, b2 = l2
    import math
    C1 = math.hypot(a1, b1); C2 = math.hypot(a2, b2); Cm = (C1 + C2) / 2
    G = 0.5 * (1 - math.sqrt(Cm ** 7 / (Cm ** 7 + 25 ** 7)))
    a1p, a2p = a1 * (1 + G), a2 * (1 + G)
    C1p, C2p = math.hypot(a1p, b1), math.hypot(a2p, b2)
    h1p = math.degrees(math.atan2(b1, a1p)) % 360; h2p = math.degrees(math.atan2(b2, a2p)) % 360
    dLp = L2 - L1; dCp = C2p - C1p
    dh = h2p - h1p
    if C1p * C2p == 0: dh = 0
    elif dh > 180: dh -= 360
    elif dh < -180: dh += 360
    dHp = 2 * math.sqrt(C1p * C2p) * math.sin(math.radians(dh / 2))
    Lm = (L1 + L2) / 2; Cmp = (C1p + C2p) / 2
    if C1p * C2p == 0: hm = h1p + h2p
    elif abs(h1p - h2p) <= 180: hm = (h1p + h2p) / 2
    elif h1p + h2p < 360: hm = (h1p + h2p + 360) / 2
    else: hm = (h1p + h2p - 360) / 2
    T = 1 - 0.17 * math.cos(math.radians(hm - 30)) + 0.24 * math.cos(math.radians(2 * hm)) + 0.32 * math.cos(math.radians(3 * hm + 6)) - 0.20 * math.cos(math.radians(4 * hm - 63))
    dth = 30 * math.exp(-((hm - 275) / 25) ** 2)
    Rc = 2 * math.sqrt(Cmp ** 7 / (Cmp ** 7 + 25 ** 7))
    Sl = 1 + 0.015 * (Lm - 50) ** 2 / math.sqrt(20 + (Lm - 50) ** 2); Sc = 1 + 0.045 * Cmp; Sh = 1 + 0.015 * Cmp * T
    Rt = -math.sin(math.radians(2 * dth)) * Rc
    return math.sqrt((dLp / Sl) ** 2 + (dCp / Sc) ** 2 + (dHp / Sh) ** 2 + Rt * (dCp / Sc) * (dHp / Sh))
