# Pruefer V3: eigene Farbrechnung (sRGB -> CIE-Lab D65, CIEDE2000), unabhaengig von OpenCV/Leser-Skripten.
import numpy as np

def srgb_lin(c):
    c = np.asarray(c, float) / 255.0
    return np.where(c <= 0.04045, c / 12.92, ((c + 0.055) / 1.055) ** 2.4)

M = np.array([[0.4124564, 0.3575761, 0.1804375],
              [0.2126729, 0.7151522, 0.0721750],
              [0.0193339, 0.1191920, 0.9503041]])
WP = np.array([0.95047, 1.0, 1.08883])

def lab(rgb):
    """rgb (..., 3) 0..255 -> Lab (..., 3), D65/2 Grad."""
    x = srgb_lin(rgb) @ M.T / WP
    d = 6.0 / 29.0
    f = np.where(x > d ** 3, np.cbrt(x), x / (3 * d * d) + 4.0 / 29.0)
    L = 116 * f[..., 1] - 16
    a = 500 * (f[..., 0] - f[..., 1])
    b = 200 * (f[..., 1] - f[..., 2])
    return np.stack([L, a, b], -1)

def de76(p, q):
    return float(np.linalg.norm(np.asarray(p, float) - np.asarray(q, float)))

def de00(lab1, lab2):
    L1, a1, b1 = map(float, lab1); L2, a2, b2 = map(float, lab2)
    C1, C2 = np.hypot(a1, b1), np.hypot(a2, b2); Cm = (C1 + C2) / 2
    G = 0.5 * (1 - np.sqrt(Cm ** 7 / (Cm ** 7 + 25.0 ** 7)))
    a1p, a2p = (1 + G) * a1, (1 + G) * a2
    C1p, C2p = np.hypot(a1p, b1), np.hypot(a2p, b2)
    h1p = np.degrees(np.arctan2(b1, a1p)) % 360; h2p = np.degrees(np.arctan2(b2, a2p)) % 360
    dLp, dCp = L2 - L1, C2p - C1p
    dh = h2p - h1p
    if C1p * C2p == 0: dh = 0
    elif dh > 180: dh -= 360
    elif dh < -180: dh += 360
    dHp = 2 * np.sqrt(C1p * C2p) * np.sin(np.radians(dh / 2))
    Lm, Cmp = (L1 + L2) / 2, (C1p + C2p) / 2
    if C1p * C2p == 0: hm = h1p + h2p
    elif abs(h1p - h2p) <= 180: hm = (h1p + h2p) / 2
    elif h1p + h2p < 360: hm = (h1p + h2p + 360) / 2
    else: hm = (h1p + h2p - 360) / 2
    T = 1 - 0.17 * np.cos(np.radians(hm - 30)) + 0.24 * np.cos(np.radians(2 * hm)) + 0.32 * np.cos(np.radians(3 * hm + 6)) - 0.20 * np.cos(np.radians(4 * hm - 63))
    dth = 30 * np.exp(-((hm - 275) / 25) ** 2)
    RC = 2 * np.sqrt(Cmp ** 7 / (Cmp ** 7 + 25.0 ** 7))
    SL = 1 + 0.015 * (Lm - 50) ** 2 / np.sqrt(20 + (Lm - 50) ** 2)
    SC, SH = 1 + 0.045 * Cmp, 1 + 0.015 * Cmp * T
    RT = -np.sin(np.radians(2 * dth)) * RC
    return float(np.sqrt((dLp / SL) ** 2 + (dCp / SC) ** 2 + (dHp / SH) ** 2 + RT * (dCp / SC) * (dHp / SH)))

def ch(l):
    return float(np.hypot(l[1], l[2])), float(np.degrees(np.arctan2(l[2], l[1])))

def r1(v, n=1):
    return [round(float(x), n) for x in v]

if __name__ == '__main__':
    # Selbsttest: Sharma et al. 2005, Paar 1 -> 2,0425; Weiss -> L 100
    print(round(de00([50, 2.6772, -79.7751], [50, 0, -82.7485]), 4), r1(lab([255, 255, 255]), 3), r1(lab([255, 0, 0]), 2))
