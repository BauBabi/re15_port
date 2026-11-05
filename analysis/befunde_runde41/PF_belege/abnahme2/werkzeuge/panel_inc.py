import re, numpy as np
def grund(pfad):
    t = open(pfad).read()
    a = t.index('re15_r41n_panel_grund[76800]')
    b = t.index('};', a)
    v = np.array([int(x) for x in re.findall(r'\d+', t[t.index('{', a):b])], np.uint32)
    v = v[-76800:] if len(v) > 76800 else v
    r, g, bl = v & 31, (v >> 5) & 31, (v >> 10) & 31
    f = lambda c: ((c << 3) | (c >> 2)).astype(np.uint8)
    return np.stack([f(r), f(g), f(bl)], -1).reshape(240, 320, 3)
def glyphen(pfad):
    t = open(pfad).read()
    blk = t[t.index('re15_r41n_panel_glyphen[42]'):]
    blk = blk[:blk.index('};')]
    return [(int(a, 16), int(b), int(c), int(d)) for a, b, c, d in re.findall(r'\{ (0x[0-9a-f]+), (\d+), (\d+), (\d+) \}', blk)]
if __name__ == '__main__':
    gv, gn = grund('panel_vor.inc'), grund('panel_nach.inc')
    d = np.abs(gv.astype(int) - gn.astype(int)).max(-1)
    ys, xs = np.nonzero(d)
    print('grund vor/nach verschieden:', len(ys), 'Texel; bbox x %d..%d y %d..%d' % (xs.min(), xs.max(), ys.min(), ys.max()))
    gl = glyphen('panel_nach.inc')
    m = np.zeros((240, 320), bool)
    for c, x, y, k in gl: m[y:y + 9, x:x + 9] = True
    print('davon in Glyphenzellen (8x8 + Schatten 1):', int((d[m] > 0).sum()))
    import scipy.ndimage as nd
    lab, n = nd.label(d > 0, np.ones((3, 3)))
    for s in nd.find_objects(lab):
        if (s[0].stop - s[0].start) * (s[1].stop - s[1].start) > 4:
            print('  Bereich x %d..%d y %d..%d, %d Texel' % (s[1].start, s[1].stop - 1, s[0].start, s[0].stop - 1, int((d[s] > 0).sum())))
