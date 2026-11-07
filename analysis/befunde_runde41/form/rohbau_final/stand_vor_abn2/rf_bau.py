#!/usr/bin/env python3
"""Rohbau final, Bau: Stand + Bauplan -> FORM.EMD, FORM.TIM (Streifen-Atlas), form.json, Pruefung.
Eine Normale je Ecke wie RE2 (0 harte Spruenge). python rf_bau.py STAND.json --tag NAME [--voll]"""
import argparse
import json
import math
import os
import subprocess
import sys

import cv2
import numpy as np

from rf_kern import E, FORM, HIER, TOOLS, lade_gerippe, namen, ringecken
import form_emd as FE
import rf_plan

TEX_W, TEX_H = 384, 256


def tiefste(ls):
    return ls


def wirksam(teile, lr, c):
    """Numpy-Fassung der Bindung: -> je Teil log-Radien, Mitten."""
    L, C = [None] * len(teile), [None] * len(teile)
    for i, p in enumerate(teile):
        if isinstance(p.bind, int):
            continue
        l_ = np.array(lr[str(i)], float) if str(i) in lr else np.log(np.maximum(p.r, 1.0))
        c_ = np.array(c[str(i)], float) if str(i) in c else p.c.copy()
        if l_.shape != p.r.shape:
            l_, c_ = np.log(np.maximum(p.r, 1.0)), p.c.copy()
        if p.bind == 'sym':
            sp = [(-j) % p.n for j in range(p.n)]
            l_ = .5 * (l_ + l_[:, sp])
            c_ = np.stack([c_[:, 0], 0 * c_[:, 1]], 1)
        L[i], C[i] = l_, c_
    for i, p in enumerate(teile):
        if isinstance(p.bind, int):
            L[i], C[i] = L[p.bind] + math.log(p.skala), C[p.bind] * p.skala
    return L, C


def teil_ecken(p, l, c):
    r = np.exp(np.asarray(l, float) + p.relief)
    cs, sn = ringecken(p.n)
    pos = p.o + p.t[:, None] * p.d
    mitte = pos + np.asarray(c, float)[:, :1] * p.ea + np.asarray(c, float)[:, 1:2] * p.eb
    ring = mitte[:, None, :] + (r * cs)[..., None] * p.ea + (r * sn)[..., None] * p.eb + p.dt[..., None] * p.d
    k0 = mitte[0] - p.spitze0 * p.d + p.apex0[0] * p.ea + p.apex0[1] * p.eb
    k1 = mitte[-1] + p.spitze1 * p.d + p.apex1[0] * p.ea + p.apex1[1] * p.eb
    return ring, k0, k1


def faces_teil(p):
    n, T = p.n, p.T
    id_ = lambda a, b: a * n + (b % n)  # noqa: E731
    q = [[id_(a, b), id_(a, b + 1), id_(a + 1, b + 1), id_(a + 1, b)] for a in range(T - 1) for b in range(n)]
    c0, c1 = T * n, T * n + 1
    tri = [[c0, id_(0, b + 1), id_(0, b)] for b in range(n)] + [[c1, id_(T - 1, b), id_(T - 1, b + 1)] for b in range(n)]
    return q, tri


# ═══════════════════════════════════════════════════ Atlas
DICHTE = dict(huefte=1.25, wirbel=1.0, brust=1.15, kopf=1.3, kiefer=.9, hals=.7, oberschenkel_r=1.25, unterschenkel_r=1.25, fuss_r=.9, oberschenkel_l=1.05, unterschenkel_l=1.05, fuss_l=.8,
              oberarm_r=.9, unterarm_r=1.35, hand_r=1.2, oberarm_l=.9, unterarm_l=1.1, hand_l=1.0)


def dichte_gewicht(n):
    if n.startswith('klaue') or n.startswith('kralle') or n.startswith('zeh') or n.startswith('fang'):
        return .55
    return DICHTE.get(n, 1.1 if n.startswith('schwanz') else 1.0)


def _regal(masse, d, rand=2):
    gr = [(max(8, int(math.ceil(u * d / 2)) * 2), max(8, int(math.ceil(l * d)))) for u, l in masse]
    folge = sorted(range(len(gr)), key=lambda i: -gr[i][1])
    x, y, h = 0, 0, 0
    lage = [None] * len(gr)
    for i in folge:
        w, hh = gr[i]
        if x + w + rand > TEX_W:
            x, y, h = 0, y + h + rand, 0
        lage[i] = (x, y, w, hh)
        x += w + rand
        h = max(h, hh)
    return lage, y + h <= TEX_H


def atlas(teile, L, C):
    masse, wer, feld = [], [], {}
    for i, p in enumerate(teile):
        ring, k0, k1 = teil_ecken(p, L[i], C[i])
        um = np.linalg.norm(np.diff(np.concatenate([ring, ring[:, :1]], 1), axis=1), axis=2).sum(1)
        g = dichte_gewicht(p.name)
        wert = (float(np.max(um)) * g, float(p.t[-1] - p.t[0] + .5 * (p.spitze0 + p.spitze1) + 40) * g)
        if p.feld is not None:
            if p.feld in feld:
                j = feld[p.feld]
                masse[j] = (max(masse[j][0], wert[0]), max(masse[j][1], wert[1]))
                wer[j].append(i)
                continue
            feld[p.feld] = len(masse)
        masse.append(wert)
        wer.append([i])
    d = .2
    for _ in range(300):
        if _regal(masse, d)[1]:
            break
        d *= .985
    lage, ok = _regal(masse, d)
    aus = [None] * len(teile)
    for j, l in enumerate(wer):
        for i in l:
            aus[i] = lage[j]
    return aus, d


def teil_uv(p, rect, ring):
    x, y, w, h = rect
    u = np.round(x + w * np.arange(p.n + 1) / p.n).astype(int)
    l = np.concatenate([[0], np.cumsum(np.linalg.norm(np.diff(ring.mean(1), axis=0), axis=1))])
    rand0, rand1 = min(3, h // 6), min(3, h // 6)
    v = np.round(y + rand0 + (h - rand0 - rand1 - 1) * l / max(l[-1], 1)).astype(int)
    return u, v, y, y + h - 1


# ═══════════════════════════════════════════════════ Normalen: je Ecke genau eine (Summe der angrenzenden Flaechen, Flaechengewicht)
def eck_normalen(pts, fl):
    n = np.zeros_like(pts)
    for f in fl:
        a = pts[f[0]]
        s = np.zeros(3)
        for i in range(1, len(f) - 1):
            s += np.cross(pts[f[i]] - a, pts[f[i + 1]] - a)
        for v in f:
            n[v] += s
    return n


def teil_mesh(form, p, l, c, rect):
    ring, k0, k1 = teil_ecken(p, l, c)
    T, n = p.T, p.n
    pts = np.round(np.concatenate([ring.reshape(-1, 3), k0[None], k1[None]]))
    q, tri = faces_teil(p)
    vz = 1.0 if np.cross(p.ea, p.eb) @ p.d > 0 else -1.0
    fl = [list(f) if vz > 0 else list(f[::-1]) for f in q + tri]
    nn = eck_normalen(pts, fl)
    m = form['meshes'][p.knochen]
    v0 = len(m['ecken'])
    n0 = len(m['normalen'])
    for x in pts:
        m['ecken'].append([int(c_) for c_ in x])
    for x in nn:
        x = x / (np.linalg.norm(x) or 1.0) * 4096.0
        m['normalen'].append([int(round(c_)) for c_ in x])
    u, v, vtop, vbot = teil_uv(p, rect, ring)

    def tex_of(a, b):
        return [int(u[b]), int(v[a])]
    fi = 0
    for a in range(T - 1):
        for b in range(n):
            ring_f = fl[fi]
            fi += 1
            orig = q[a * n + b]
            texs = {orig[0]: tex_of(a, b), orig[1]: tex_of(a, b + 1), orig[2]: tex_of(a + 1, b + 1), orig[3]: tex_of(a + 1, b)}
            z = (ring_f[0], ring_f[1], ring_f[3], ring_f[2])
            FE.viereck(m, [v0 + i for i in z], [n0 + i for i in z], [texs[i] for i in z], 0)
    for ti in range(2 * n):
        ring_f = fl[fi]
        fi += 1
        b = ti % n
        anf = ti < n
        a = 0 if anf else T - 1
        tip = T * n + (0 if anf else 1)
        um = (u[b] + u[b + 1]) // 2
        texs = {tip: [int(um), int(vtop if anf else vbot)], a * n + b: tex_of(a, b), a * n + ((b + 1) % n): [int(u[b + 1]), int(v[a])]}
        if b == n - 1:
            texs[a * n + 0] = [int(u[n]), int(v[a])]
        FE.dreieck(m, [v0 + i for i in ring_f], [n0 + i for i in ring_f], [texs[i] for i in ring_f], 0)


def ruhe_winkel(g, st):
    """Clip 2: neutrale, symmetrische Ruhepose. Kommt aus rf_ruhe.py, wenn vorhanden."""
    p = os.path.join(HIER, 'zw', 'ruhe.json')
    if os.path.exists(p):
        return json.load(open(p))
    return dict(winkel=st['posen']['1']['winkel'], wurzel=st['posen']['1']['wurzel'])


def baue_form(g, teile, stand):
    form = FE.neu()
    for k in g['knochen']:
        FE.knochen_dazu(form, k['name'], k['eltern'], k['versatz'])
    L, C = wirksam(teile, stand['lr'], stand['c'])
    lage, dichte = atlas(teile, L, C)
    for i, p in enumerate(teile):
        teil_mesh(form, p, L[i], C[i], lage[i])
    FE.orientiere(form)
    for n, nm in ((1, 'pose1'), (2, 'pose2')):
        p = stand['posen'][str(n)]
        w = np.round(np.array(p['winkel'], float)).astype(int)
        wz = list(p['wurzel'])
        wz[1] += p['lage'][1]
        FE.clip_dazu(form, nm, [FE.bild(form, wurzel=[int(round(x)) for x in wz], winkel=w.tolist())])
    r = ruhe_winkel(g, stand)
    FE.clip_dazu(form, 'ruhe', [FE.bild(form, wurzel=[int(round(x)) for x in r['wurzel']], winkel=np.round(np.array(r['winkel'], float)).astype(int).tolist())])
    FE.boden_nachfuehren(form)
    return form, lage, dichte, L, C


def tim_genau(rgb):
    q = np.clip((np.round(rgb.astype(np.float32) / 8.0)).astype(np.int32), 0, 31)
    code = q[..., 0] | (q[..., 1] << 5) | (q[..., 2] << 10)
    u, inv, n = np.unique(code, return_inverse=True, return_counts=True)
    inv = inv.reshape(code.shape)
    if len(u) > 255:
        rang = np.argsort(-n)
        halten = rang[:255]
        fb = np.stack([u & 31, (u >> 5) & 31, (u >> 10) & 31], 1).astype(float)
        ziel_ = np.arange(len(u))
        for i in rang[255:]:
            ziel_[i] = halten[int(((fb[halten] - fb[i]) ** 2).sum(1).argmin())]
        neu_ = {int(a): i for i, a in enumerate(halten)}
        inv2 = np.array([neu_[int(ziel_[v])] for v in range(len(u))])[inv]
        cl = [fb[a] for a in halten]
    else:
        inv2 = inv
        cl = [np.array([v & 31, (v >> 5) & 31, (v >> 10) & 31], float) for v in u]
    px = (inv2 + 1).astype(np.uint8)
    farben = [(0, 0, 0)] + [tuple(int(c) << 3 for c in f) for f in cl]
    return px, FE.clut_aus_rgb(farben, transparent0=True), len(cl)


def teile_json(g):
    ix = namen(g)
    z = g['zuordnung']
    t = {}
    for bild in ('bild1', 'bild2'):
        for nm, ks in z[bild].items():
            t[nm] = [ix[k] for k in ks]
    t['right_arm'] = [ix[k] for k in ('oberarm_r', 'unterarm_r', 'hand_r', 'klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4')]
    t['right_arm_front'] = [ix[k] for k in ('unterarm_r', 'hand_r', 'klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4')]
    t['left_arm_front'] = [ix[k] for k in ('unterarm_l', 'hand_l', 'klaue_l1', 'klaue_l2', 'klaue_l3', 'klaue_l4')]
    # streng: nur Rumpf, Hals, Kopf, Kiefer, linker Oberarm
    t['face_and_upper_body'] = [ix[k] for k in ('huefte', 'wirbel', 'brust', 'oberarm_l', 'hals', 'kopf', 'kiefer')]
    return t


def schreibe(form, rgb, aus, g, stand, name='FORM'):
    emd = FE.schreibe_emd(form)
    px, clut, nf = tim_genau(rgb)
    tim = FE.tim_schreiben(px, [clut])
    open(os.path.join(aus, name + '.EMD'), 'wb').write(emd)
    open(os.path.join(aus, name + '.TIM'), 'wb').write(tim)
    j = dict(name='F', emd=name + '.EMD', tim=name + '.TIM', licht='mess', ruhe=dict(clip=2, bild=0), teile=teile_json(g))
    for n in (1, 2):
        p = stand['posen'][str(n)]
        j['bild%d' % n] = dict(lage=[int(round(p['lage'][0])), 0, int(round(p['lage'][2]))], gier=int(round(p['gier'])) % 4096, clip=n - 1, bild=0)
    json.dump(j, open(os.path.join(aus, 'form.json'), 'w', encoding='utf-8'), indent=1)
    return nf


def pruefe(aus, tag, nur_teile=True):
    d = os.path.join(aus, 'lauf', tag)
    os.makedirs(d, exist_ok=True)
    env = dict(os.environ, PYTHONIOENCODING='utf-8')
    r = subprocess.run([sys.executable, os.path.join(TOOLS, 'form_pruefung.py'), 'teile' if nur_teile else 'pruefe', os.path.join(aus, 'form.json'), '--aus', d], capture_output=True, text=True, env=env, encoding='utf-8', errors='replace')
    if r.returncode:
        print(r.stdout[-3000:], r.stderr[-3000:])
    return d


def vergleich(aus, tag, s=3):
    import form_pruefung as P
    import form_video as FV
    k = P.Kandidat(os.path.join(aus, 'form.json'))
    for n, box in ((1, (185, 170, 445, 480)), (2, (135, 135, 480, 480))):
        sauber, alb, m, _ = k.rendern(n)
        gef = FV.video_filter(sauber, bild=n, entzerren=False)
        o = P.original(n)
        x0, y0, x1, y1 = box

        def gr(a, st=cv2.INTER_CUBIC):
            return cv2.resize(a[y0:y1, x0:x1], None, fx=s, fy=s, interpolation=st)
        c = np.hstack([gr(o), gr(gef), gr(sauber, cv2.INTER_NEAREST)])
        cv2.imwrite(os.path.join(aus, 'lauf', tag, 'vergleich_bild%d.png' % n), c[:, :, ::-1])


if __name__ == '__main__':
    import rf_textur
    a = argparse.ArgumentParser()
    a.add_argument('stand')
    a.add_argument('--tag', default='t')
    a.add_argument('--voll', action='store_true')
    a = a.parse_args()
    g, teile = rf_plan.plan()
    stand = json.load(open(os.path.join(HIER, a.stand)))
    form, lage, dichte, L, C = baue_form(g, teile, stand)
    rgb, bk = rf_textur.backen(form, g, teile, lage, stand, L=L, C=C)
    cv2.imwrite(os.path.join(HIER, 'zw', 'textur.png'), rgb[:, :, ::-1])
    print('Dichte', round(dichte, 4), 'Flaechen', sum(len(m['dreiecke']) + len(m['vierecke']) for m in form['meshes']))
    print('Farben', schreibe(form, rgb, HIER, g, stand))
    json.dump([dict(name=p.name, knochen=p.knochen, feld=p.feld, rechteck=list(r)) for p, r in zip(teile, lage)], open(os.path.join(HIER, 'atlas.json'), 'w'), indent=0)
    d = pruefe(HIER, a.tag, not a.voll)
    vergleich(HIER, a.tag)
    print(d)
