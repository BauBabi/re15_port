import numpy as np
from skimage.draw import polygon
from skimage.color import rgb2lab
def faces(E, b):
    m = E.meshes[b]
    return [('d%d' % i, u, 3) for i, u in enumerate(m['tuv'])] + [('v%d' % i, u, 4) for i, u in enumerate(m['quv'])]


def texel_maske(uv, n, shape):
    po = (uv[5] & 0xF) * 128
    us = [uv[3 * k] + po for k in range(n)]; vs = [uv[3 * k + 1] for k in range(n)]
    if n == 4:  # Viereck a b d c
        us = [us[0], us[1], us[3], us[2]]; vs = [vs[0], vs[1], vs[3], vs[2]]
    m = np.zeros(shape, bool)
    rr, cc = polygon(np.array(vs) + 0.0, np.array(us) + 0.0, shape)
    m[rr, cc] = True
    for u, v in zip(us, vs):  # Ecken mitnehmen
        if 0 <= v < shape[0] and 0 <= u < shape[1]:
            m[v, u] = True
    return m


def zeile(uv): return (uv[2] >> 6) & 0x1FF


def lab15(c):
    c = np.asarray(c, np.int64)
    rgb = np.stack([(c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3], -1) / 255.0
    return rgb2lab(rgb.reshape(-1, 1, 3)).reshape(-1, 3)


