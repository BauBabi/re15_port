"""Eigene Luecken-Messung Ruecken/Kopf (Pruefer r5). Eigener Leser/Raster pl.py, eigene Huelle (skimage), eigenes Schliessen (scipy),
eigene Tiefe (EDT). Aufruf: python luecke_eigen.py NAME.EMD [nv] [jobs] -> luecke_NAME.json"""
import sys, re, json, os
import numpy as np
from scipy import ndimage
from skimage.morphology import convex_hull_image
import pl
SZ = os.environ.get('SZLOG', r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/p5/pruef_zahlen-r3/laeufe/V7/debug.log')
VAR = ((0.375, 0, 0), (0.375, 0.5, 0), (0.375, -0.5, 0), (0.375, 0, 0.5), (0.375, 0, -0.5), (0.0, 0, 0), (0.75, 0, 0))
P1 = dict(clip=20, bild=139, gier=1422, x=-8420, z=-21908)
P2 = dict(clip=12, bild=48, gier=861, x=-8182, z=-21330)
NAH = 12; YR = -971
S8 = np.ones((3, 3), bool)


def szene(log=SZ):
    out = {}
    for z in open(log, errors='replace'):
        m = re.search(r'\[hangar-takt\] h(-?\d+) F(\d+) B clip (\d+) bild (\d+) gier (\d+) \((-?\d+),(-?\d+)\)', z)
        if m:
            f = int(m.group(2))
            out[f] = dict(clip=int(m.group(3)), bild=int(m.group(4)), gier=int(m.group(5)), x=int(m.group(6)), z=int(m.group(7)))
    return out


def ansichten():
    sp = szene()
    out = [('PV', pl.PV), ('P1', P1), ('P2', P2)] + [('F%d' % k, sp[k]) for k in sorted(sp)]
    for nm, p in (('PV', pl.PV), ('P2', P2)):
        for k in range(1, 16):
            q = dict(p); q['gier'] = (p['gier'] + 256 * k) & 0xFFF
            out.append(('%s+%d' % (nm, 256 * k), q))
    return out


def setvar(v):
    off, dx, dy = v

    def tv(sx, sy, M):
        x = np.asarray(sx, float) + off; y = np.asarray(sy, float) + off
        w = M[2, 0] * x + M[2, 1] * y + M[2, 2]
        return (M[0, 0] * x + M[0, 1] * y + M[0, 2]) / w + dx, (M[1, 0] * x + M[1, 1] * y + M[1, 2]) / w + dy
    pl.to_video = tv


def mass(bone, lok):
    """-> dict(summe, kerbe, spalt, tief, maxtiefe, ort, G)"""
    S = bone >= 0; H = bone == 9
    leer = dict(summe=0, kerbe=0, spalt=0, tief=0, maxtiefe=0.0, ort=None)
    if not H.any():
        return leer, np.zeros_like(S)
    m8 = bone == 8
    R = np.zeros_like(S); R[m8] = lok[m8][:, 1] < YR
    dH = ndimage.distance_transform_cdt(~H, metric='chessboard')      # Quadrat-Umkreis wie cv2.dilate
    Hn = dH <= NAH; Hd = dH <= 1
    basis = H | (m8 & Hn)
    hull = convex_hull_image(basis, offset_coordinates=False)
    G = np.zeros_like(S); kerbe = 0; orte = []
    lab, n = ndimage.label(hull & ~S, S8)
    for i in range(1, n + 1):
        c = lab == i; cd = ndimage.binary_dilation(c, S8)
        if (cd & Hd).any() and (cd & R).any():
            G |= c; kerbe += int(c.sum()); ys, xs = np.nonzero(c); orte.append(('kerbe', int(c.sum()), int(xs.mean()), int(ys.mean())))
    pad = 4
    Sp = np.pad(S, pad)
    zu = ndimage.binary_erosion(ndimage.binary_dilation(Sp, np.ones((5, 5), bool)), np.ones((5, 5), bool))[pad:-pad, pad:-pad]
    lab, n = ndimage.label(zu & ~S & ~G, S8); spalt = 0
    for i in range(1, n + 1):
        c = lab == i; cd = ndimage.binary_dilation(c, S8)
        if (cd & R).any() and (c & Hn).any():
            G |= c; spalt += int(c.sum()); ys, xs = np.nonzero(c); orte.append(('spalt', int(c.sum()), int(xs.mean()), int(ys.mean())))
    if not G.any():
        return leer, G
    frei = (~S) & ~G
    d = ndimage.distance_transform_edt(~frei)[G]
    return dict(summe=kerbe + spalt, kerbe=kerbe, spalt=spalt, tief=int((d >= 1.5).sum()), maxtiefe=round(float(d.max()), 1), ort=orte), G


def arbeit(a):
    name, i0, i1, nv = a
    E = pl.EMD(open(name, 'rb').read())
    A = ansichten()[i0:i1]; out = []
    for nm, pose in A:
        zl = []
        for v in VAR[:nv]:
            setvar(v)
            r = (pl.raster_maler if os.environ.get('MALER') == '1' else pl.raster)(E, pl.world_tris(E, pose))
            m, _G = mass(r['bone'], r['lok'])
            zl.append(m)
        out.append((nm, zl))
    return out


def soll(m):
    return m['summe'] <= 21 and m['tief'] <= 7 and m['maxtiefe'] <= 3.2


if __name__ == '__main__':
    from multiprocessing import Pool
    name = sys.argv[1]; nv = int(sys.argv[2]) if len(sys.argv) > 2 else 7; jobs = int(sys.argv[3]) if len(sys.argv) > 3 else 8
    n = len(ansichten()); k = (n + jobs - 1) // jobs
    with Pool(jobs) as p:
        res = [x for t in p.map(arbeit, [(name, i, min(i + k, n), nv) for i in range(0, n, k)]) for x in t]
    json.dump(res, open('luecke_%s%s.json' % (os.path.basename(name).split('.')[0], '_maler' if os.environ.get('MALER') == '1' else ''), 'w'))
    ueber = []
    for nm, zl in res:
        if not all(soll(m) for m in zl):
            ueber.append((nm, max(m['summe'] for m in zl), max(m['tief'] for m in zl), max(m['maxtiefe'] for m in zl), [m['summe'] for m in zl]))
    mit = [(nm, max(m['summe'] for m in zl), max(m['tief'] for m in zl), max(m['maxtiefe'] for m in zl)) for nm, zl in res if any(m['summe'] for m in zl)]
    print(name, 'Ansichten', len(res), 'Varianten', nv, '| mit Luecke', len(mit), '| Summe der Maxima', sum(x[1] for x in mit), '| ueber Soll', len(ueber))
    for u in ueber: print('   UEBER', u)
    print('   alle mit Luecke:', mit)
