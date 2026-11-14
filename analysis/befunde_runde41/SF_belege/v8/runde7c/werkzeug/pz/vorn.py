"""Pruefer P5 (C)+(D) offline, eigene Maler-Rasterung (pl.raster_maler = Port-Reihenfolge, kein Tiefenpuffer).
Vorderansicht = Kopf-Vorwaertsachse (Knochen 9 lok +x; Gesicht bei x >= 150) zur Kamera cos >= 0,5 (Wortlaut riegel_r6.s18 / Bank).
Je Vorderansicht gegen V7 (Variante 0):
  kragen  Mesh-8-Bildpunkte mit lok y8 < -1270 (Bank K-VORN a; hoechster Rumpfpunkt von vorn V7 -1259 / r4 -1263) + hoechster y8
  umriss  Bildpunkte von (8|9) Kand ausserhalb (8|9) V7 im Kopffenster (Kopf-Rahmen V7 +-40 px), groesster Abstand (px)
  geaend  Bildpunkte NEUER/GEAENDERTER Flaechen (Mesh 8/9, Schluessel Ecken-Orte+UV+CLUT gegen den Eingang r6) ausserhalb (8|9) V7 (+1 px)
  gesicht Gesichts-Bildpunkte (Mesh 9 V7-Flaeche, Schwerpunkt x >= 150) des Bezugs, die im Kand nicht Gesicht sind; Bezug = Kand ohne Anhang
          (A0) und V7
Aufruf: python vorn.py KAND [OHNE]  -> vorn_KAND.json"""
import sys, json, os
import numpy as np
from scipy import ndimage
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import pl, luecke_eigen as L
M = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/m/'


def lade(n): return pl.EMD(open(M + n + '.EMD', 'rb').read())


def schluessel(E, mi):
    m = E.meshes[mi]; out = []
    for kind, V, F, U in (('t', m['tv'], m['tf'], m['tuv']), ('q', m['qv'], m['qf'], m['quv'])):
        for f, uv in zip(F, U):
            out.append((kind, tuple(map(tuple, V[list(f[1::2])].tolist())), tuple(uv)))
    return out


def geaendert(E, Ein):
    """-> set((mesh, kind, fi)) neue/geaenderte Flaechen gegen den Eingang"""
    s = set()
    for mi in (8, 9):
        ref = set(schluessel(Ein, mi)); m = E.meshes[mi]; k = 0
        for kind, n in (('t', len(m['tf'])), ('q', len(m['qf']))):
            for fi in range(n):
                if schluessel(E, mi)[k] not in ref: s.add((mi, kind, fi))
                k += 1
    return s


def gesicht_flaechen(E, n7t, n7q):
    m = E.meshes[9]; g = set()
    for kind, V, F, n0 in (('t', m['tv'], m['tf'], n7t), ('q', m['qv'], m['qf'], n7q)):
        for fi, f in enumerate(F[:n0]):
            if V[list(f[1::2])].mean(0)[0] >= 150: g.add((9, kind, fi))
    return g


def cosvorn(E, pose):
    V, t, h, Mm, cp = pl.cam()
    R, T = E.pose(E.kf_of(pose['clip'], pose['bild']))
    g = pose['gier']; s, c = pl.q12s(g) / 4096, pl.q12c(g) / 4096
    Y = np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])
    fw = Y @ (R[9] / 4096.0) @ np.array([1.0, 0, 0])
    hp = Y @ T[9] + np.array([pose['x'], pose.get('y', 0), pose['z']], float)
    d = np.array(cp, float) - hp
    return float(fw @ d / np.linalg.norm(fw) / np.linalg.norm(d))


def render(E, pose, gs=None):
    L.setvar(L.VAR[0]); tris = pl.world_tris(E, pose); r = pl.raster_maler(E, tris)
    keyt = [(t[0], t[1], t[2]) for t in tris] + [None]
    r['key'] = keyt
    return r


def arbeit(a):
    kn, ohne, i0, i1 = a
    K, V7, Ein = lade(kn), lade('V7'), lade('R6')
    O = lade(ohne) if ohne else None
    n7t, n7q = len(V7.meshes[9]['tf']), len(V7.meshes[9]['qf'])
    ch = geaendert(K, Ein); gK = gesicht_flaechen(K, n7t, n7q); g7 = gesicht_flaechen(V7, n7t, n7q)
    out = []
    for nm, pose in L.ansichten()[i0:i1]:
        cv = cosvorn(V7, pose)
        if cv < 0.5:
            out.append(dict(nm=nm, cos=round(cv, 3), vorn=False)); continue
        rk, r7 = render(K, pose), render(V7, pose)
        def maske(r, menge):
            f = r['face']; lut = np.array([k in menge for k in r['key'][:-1]] + [False]); return lut[f]
        Sk = (rk['bone'] == 8) | (rk['bone'] == 9); S7 = (r7['bone'] == 8) | (r7['bone'] == 9)
        H7 = r7['bone'] == 9; ys, xs = np.nonzero(H7)
        W = np.zeros_like(S7)
        if len(ys): W[max(ys.min() - 40, 0):ys.max() + 41, max(xs.min() - 40, 0):xs.max() + 41] = True
        m8 = rk['bone'] == 8; y8 = rk['lok'][..., 1]
        kr = m8 & (y8 < -1270)
        ext = Sk & ~S7 & W
        dist = ndimage.distance_transform_edt(~S7)
        chm = maske(rk, ch)
        ga = chm & ~ndimage.binary_dilation(S7, np.ones((3, 3), bool))
        gk = maske(rk, gK)
        r6 = render(Ein, pose); S6 = (r6['bone'] == 8) | (r6['bone'] == 9); d6 = ndimage.distance_transform_edt(~S6)
        e6 = Sk & ~S6 & W; e6v7 = e6 & ~S7
        d = dict(nm=nm, cos=round(cv, 3), vorn=True, kragen=int(kr.sum()), y8min=round(float(np.nanmin(y8[m8])), 0) if m8.any() else None,
                 umriss_px=int(ext.sum()), umriss_max=round(float(dist[ext].max()), 1) if ext.any() else 0.0,
                 geaend_sicht=int(chm.sum()), geaend_aussen=int(ga.sum()), geaend_aussen_max=round(float(dist[ga].max()), 1) if ga.any() else 0.0,
                 umriss_r6_px=int(e6.sum()), umriss_r6_max=round(float(d6[e6].max()), 1) if e6.any() else 0.0,
                 neu_aussen_r6_v7=int(e6v7.sum()), neu_aussen_r6_v7_max=round(float(np.minimum(d6, dist)[e6v7].max()), 1) if e6v7.any() else 0.0,
                 gesicht_V7=int((maske(r7, g7) & ~gk).sum()), gesicht_V7_px=int(maske(r7, g7).sum()))
        if O is not None:
            ro = render(O, pose); go = maske(ro, gesicht_flaechen(O, n7t, n7q))
            d['gesicht_ohne'] = int((go & ~gk).sum()); d['gesicht_ohne_px'] = int(go.sum())
            nmask = np.array([k is not None and k[0] == 9 and ((k[1] == 't' and k[2] >= n7t) or (k[1] == 'q' and k[2] >= n7q)) for k in rk['key'][:-1]] + [False])[rk['face']]
            So = (ro['bone'] == 8) | (ro['bone'] == 9); Ho = ro['bone'] == 9
            d['anhang_sicht'] = int(nmask.sum()); d['anhang_ausserhalb_kopf'] = int((nmask & ~ndimage.binary_dilation(Ho, np.ones((3, 3), bool))).sum())
            d['anhang_ausserhalb_8u9'] = int((nmask & ~ndimage.binary_dilation(So, np.ones((3, 3), bool))).sum())
        out.append(d)
    return out


if __name__ == '__main__':
    from multiprocessing import Pool
    kn = sys.argv[1]; ohne = sys.argv[2] if len(sys.argv) > 2 else None
    n = len(L.ansichten()); j = 8; k = (n + j - 1) // j
    with Pool(j) as p:
        res = [x for t in p.map(arbeit, [(kn, ohne, i, min(i + k, n)) for i in range(0, n, k)]) for x in t]
    json.dump(res, open('vorn_%s.json' % kn, 'w'))
    v = [r for r in res if r['vorn']]
    s = lambda key: sum(r.get(key, 0) for r in v)
    nz = lambda key: [(r['nm'], r[key]) for r in v if r.get(key, 0) > 0]
    print(kn, 'Vorderansichten', len(v), '| kragen px', s('kragen'), 'in', len(nz('kragen')), 'hoechster y8', min(r['y8min'] for r in v if r['y8min'] is not None),
          '| umriss px', s('umriss_px'), 'max', max(r['umriss_max'] for r in v), '| geaendert sichtbar', s('geaend_sicht'), 'ausserhalb V7', s('geaend_aussen'),
          'in', len(nz('geaend_aussen')), 'max', max(r['geaend_aussen_max'] for r in v), '| ausserhalb Eingang r6', s('umriss_r6_px'), 'in', len(nz('umriss_r6_px')), 'max', max(r['umriss_r6_max'] for r in v), '| ausserhalb r6 UND V7', s('neu_aussen_r6_v7'), 'in', len(nz('neu_aussen_r6_v7')), 'max', max(r['neu_aussen_r6_v7_max'] for r in v), '| Gesicht gegen V7', s('gesicht_V7'), 'in', len(nz('gesicht_V7')))
    if ohne:
        print('   Gesicht gegen ohne Anhang', s('gesicht_ohne'), 'in', len(nz('gesicht_ohne')), nz('gesicht_ohne')[:10], '| Anhang sichtbar', s('anhang_sicht'), 'in', len(nz('anhang_sicht')),
              '| Anhang ausserhalb Kopf', s('anhang_ausserhalb_kopf'), 'in', len(nz('anhang_ausserhalb_kopf')), '| ausserhalb 8+9 (ohne Anhang)', s('anhang_ausserhalb_8u9'), 'in', len(nz('anhang_ausserhalb_8u9')))
    print('   ausserhalb r6 UND V7 >0:', [(r['nm'], r['neu_aussen_r6_v7'], r['neu_aussen_r6_v7_max']) for r in v if r['neu_aussen_r6_v7'] > 0][:30])
    print('   kragen>0:', nz('kragen')[:12]); print('   geaendert ausserhalb>0:', nz('geaend_aussen')[:20]); print('   Gesicht gegen V7>0:', nz('gesicht_V7')[:12])
