#!/usr/bin/env python3
"""Rohbau C, Bau: Passungsstand + Bauplan -> FORM.EMD, FORM.TIM (Streifen-Atlas), form.json, Pruefung.
python rc_bau.py STAND.json --tag NAME [--pruefe]"""
import argparse
import json
import math
import os
import subprocess
import sys

import cv2
import numpy as np

from rc_kern import HIER, FORM, TOOLS, TEILE, ringecken, lade_gerippe, namen, E
import form_emd as FE
import rc_aufsatz

TEX_W, TEX_H = 384, 256


# ═══════════════════════════════════════════════════ Geometrie je Teil
def teil_ecken(p, lr, c):
    """-> ring[T,n,3] im Knochenraum, k0, k1."""
    r = np.exp(np.asarray(lr, float) + p.relief)
    cs, sn = ringecken(p.n)
    pos = p.o + p.t[:, None] * p.d
    mitte = pos + np.asarray(c, float)[:, :1] * p.ea + np.asarray(c, float)[:, 1:2] * p.eb
    ring = mitte[:, None, :] + (r * cs)[..., None] * p.ea + (r * sn)[..., None] * p.eb
    return ring, mitte[0] - p.spitze0 * p.d, mitte[-1] + p.spitze1 * p.d


def faces_teil(p):
    """Vierecke als Ring (T,n Indizes) und Kappen-Dreiecke. Aussen = (q1-q0) x (q3-q0) * vz."""
    n, T = p.n, p.T
    id_ = lambda a, b: a * n + (b % n)  # noqa: E731
    q = [[id_(a, b), id_(a, b + 1), id_(a + 1, b + 1), id_(a + 1, b)] for a in range(T - 1) for b in range(n)]
    c0, c1 = T * n, T * n + 1
    tri = [] if p.offen0 else [[c0, id_(0, b + 1), id_(0, b)] for b in range(n)]
    tri = tri + [[c1, id_(T - 1, b), id_(T - 1, b + 1)] for b in range(n)]
    return q, tri


# ═══════════════════════════════════════════════════ Atlas
def atlas(teile, stand, dichte=None):
    """Streifen je Teil: Breite = Umfang, Hoehe = Laenge, Regalfuellung. dichte = Texel je Einheit. Teile mit gleichem feld teilen einen Streifen."""
    masse, wer = [], []
    feld = {}
    for i, p in enumerate(teile):
        ring, k0, k1 = teil_ecken(p, stand['lr'][i], stand['c'][i])
        um = np.linalg.norm(np.diff(np.concatenate([ring, ring[:, :1]], 1), axis=1), axis=2).sum(1)
        lang = p.t[-1] - p.t[0]
        g = dichte_gewicht(p.name)
        wert = (float(np.max(um)) * g, float(lang + 0.5 * (p.spitze0 + p.spitze1)) * g)
        if p.feld is not None:
            if p.feld in feld:
                j = feld[p.feld]
                masse[j] = (max(masse[j][0], wert[0]), max(masse[j][1], wert[1]))
                wer[j].append(i)
                continue
            feld[p.feld] = len(masse)
        masse.append(wert)
        wer.append([i])
    if dichte is None:
        dichte = 0.2
        for _ in range(200):
            if not _regal(masse, dichte)[1]:
                dichte *= 0.97
            else:
                break
    lage, ok = _regal(masse, dichte)
    aus = [None] * len(teile)
    for j, l in enumerate(wer):
        for i in l:
            aus[i] = lage[j]
    return aus, dichte


DICHTE = dict(huefte=1.25, wirbel=1.0, brust=1.1, kopf=1.25, kiefer=0.8, hals=0.6, oberschenkel_r=1.3, unterschenkel_r=1.3, fuss_r=1.1, oberschenkel_l=1.1, unterschenkel_l=1.1, fuss_l=1.0,
              oberarm_r=1.0, unterarm_r=1.3, hand_r=1.0, oberarm_l=0.8, unterarm_l=0.9, hand_l=0.8)


def dichte_gewicht(n):
    if n.startswith('klaue') or n.startswith('zahn') or n.startswith('zeh'):
        return 0.6
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


def teil_uv(p, rect, ring):
    """(u[T,n+1], v[T]) in Texeln, Zeile 0 und letzte tragen die Kappen im Rand."""
    x, y, w, h = rect
    u = np.round(x + w * np.arange(p.n + 1) / p.n).astype(int)
    l = np.concatenate([[0], np.cumsum(np.linalg.norm(np.diff(ring.mean(1), axis=0), axis=1))])
    rand0, rand1 = min(3, h // 6), min(3, h // 6)
    v = np.round(y + rand0 + (h - rand0 - rand1 - 1) * l / max(l[-1], 1)).astype(int)
    return u, v, y, y + h - 1


# ═══════════════════════════════════════════════════ Normalen mit Kante
def normalen(pts, faces, knick_grad=38.0):
    """faces = Listen von Eckindizes (Aussen-Windung nach der Regel (b-a) x (d-a) nach aussen). Je Flaechenecke Mittel der Nachbarn innerhalb des Knickwinkels. -> Liste je Flaeche von Normalen."""
    fn = []
    for f in faces:
        a = np.asarray(pts[f[0]], float)
        s = np.zeros(3)
        for i in range(1, len(f) - 1):
            s += np.cross(pts[f[i]] - a, pts[f[i + 1]] - a)
        fn.append(s)
    fn = np.array(fn)
    fl = np.linalg.norm(fn, axis=1) + 1e-9
    fe = fn / fl[:, None]
    an = {}
    for i, f in enumerate(faces):
        for v in f:
            an.setdefault(v, []).append(i)
    ck = math.cos(math.radians(knick_grad))
    aus = []
    for i, f in enumerate(faces):
        z = []
        for v in f:
            s = np.zeros(3)
            for j in an[v]:
                if fe[i] @ fe[j] >= ck:
                    s += fn[j]
            z.append(s if np.linalg.norm(s) > 0 else fe[i])
        aus.append(z)
    return aus


class Mesher:
    def __init__(self, m):
        self.m = m
        self.nmap = {}

    def norm(self, v):
        v = np.asarray(v, float)
        v = v / (np.linalg.norm(v) or 1.0) * 4096.0
        k = tuple(int(round(c)) for c in v)
        if k not in self.nmap:
            self.m['normalen'].append(list(k))
            self.nmap[k] = len(self.m['normalen']) - 1
        return self.nmap[k]


def teil_mesh(form, p, lr, c, rect, knick=38.0):
    ring, k0, k1 = teil_ecken(p, lr, c)
    T, n = p.T, p.n
    pts = np.concatenate([ring.reshape(-1, 3), k0[None], k1[None]])
    pts = np.round(pts)
    q, tri = faces_teil(p)
    vz = 1.0 if np.cross(p.ea, p.eb) @ p.d > 0 else -1.0
    # Windung fuer normalen(): (b-a) x (c-a) nach aussen. q laeuft fuer vz>0 schon richtig, Kappen sind so gebaut.
    fl = [list(f) if vz > 0 else list(f[::-1]) for f in q + tri]
    nor = normalen(pts, fl, knick)
    m = form['meshes'][p.knochen]
    mh = m.get('_mh')
    if mh is None:
        mh = m['_mh'] = Mesher(m)
    v0 = len(m['ecken'])
    for x in pts:
        m['ecken'].append([int(c_) for c_ in x])
    u, v, vtop, vbot = teil_uv(p, rect, ring)

    def tex_of(a, b):
        return [int(u[b]), int(v[a])]
    fi = 0
    for a in range(T - 1):
        for b in range(n):
            ring_f = fl[fi]
            normal_f = nor[fi]
            fi += 1
            orig = q[a * n + b]
            texs = {orig[0]: tex_of(a, b), orig[1]: tex_of(a, b + 1), orig[2]: tex_of(a + 1, b + 1), orig[3]: tex_of(a + 1, b)}
            # Z-Reihenfolge: ring (r0,r1,r2,r3) -> (r0,r1,r3,r2)
            z = (ring_f[0], ring_f[1], ring_f[3], ring_f[2])
            nz = (normal_f[0], normal_f[1], normal_f[3], normal_f[2])
            FE.viereck(m, [v0 + i for i in z], [mh.norm(x) for x in nz], [texs[i] for i in z], 0)
    for ti, t_ in enumerate(tri):
        ring_f, normal_f = fl[fi], nor[fi]
        fi += 1
        b = ti % n
        kopf = (not p.offen0) and ti < n
        a = 0 if kopf else T - 1
        tip = T * n + (0 if kopf else 1)
        um = (u[b] + u[b + 1]) // 2
        texs = {tip: [int(um), int(vtop if kopf else vbot)], a * n + b: tex_of(a, b), a * n + ((b + 1) % n): [int(u[b + 1]), int(v[a])]}
        if b == n - 1:
            texs[a * n + 0] = [int(u[n]), int(v[a])]
        FE.dreieck(m, [v0 + i for i in ring_f], [mh.norm(x) for x in normal_f], [texs[i] for i in ring_f], 0)


def _hauer_ziel(form, g, stand):
    """Spitze des linken Hauers im Kopfraum: Punkt auf dem Sehstrahl durch Stelle P (366,304) in Bild 2, nahe der Wurzel. Pose 2 aus dem Stand."""
    import form_kamera as FK
    n = 2
    p = g['posen']['bild2']
    w = np.round(np.array(p['winkel'], float) + np.array(stand['dw']['2'], float)).astype(int)
    b = FE.bild(form, wurzel=p['wurzel'], winkel=w.tolist())
    R, T = FE.pose_aus_bild(form, b)
    kam = FK.lade_kamera(n)
    a = p['gier'] * E
    Y = np.array([[math.cos(a), 0, math.sin(a)], [0, 1, 0], [-math.sin(a), 0, math.cos(a)]])
    ix = namen(g)

    def ziel(kn, wurzel):
        Rw = Y @ (R[kn].astype(float) / 4096.0)
        Tw = Y @ T[kn].astype(float) + np.array(p['lage'], float)
        W = Rw @ wurzel + Tw
        auge, d = FK.strahl(kam, np.array([372.0, 314.0]))
        t = float((W - auge) @ d)
        C = auge + t * d
        return Rw.T @ (C - Tw)
    return ziel


def baue_form(g, teile, stand, knick=38.0, ruhe=None):
    form = FE.neu()
    for k in g['knochen']:
        FE.knochen_dazu(form, k['name'], k['eltern'], k['versatz'])
    teile_alle, stand = rc_aufsatz.mit_aufsaetzen(g, teile, stand, hauer=_hauer_ziel(form, g, stand))
    lage, dichte = atlas(teile_alle, stand)
    for i, p in enumerate(teile_alle):
        teil_mesh(form, p, stand['lr'][i], stand['c'][i], lage[i], knick)
    for m in form['meshes']:
        m.pop('_mh', None)
    FE.orientiere(form)
    for n, nm in ((1, 'pose1'), (2, 'pose2')):
        p = g['posen']['bild%d' % n]
        dw = np.array(stand['dw'][str(n)], float) if 'dw' in stand else 0.0
        w = np.round(np.array(p['winkel'], float) + dw).astype(int)
        FE.clip_dazu(form, nm, [FE.bild(form, wurzel=p['wurzel'], winkel=w.tolist())])
    w = ruhe if ruhe is not None else np.round(np.array(g['posen']['bild1']['winkel'], float) + (np.array(stand['dw']['1'], float) if 'dw' in stand else 0.0)).astype(int)
    w = np.array(w)
    ix = namen(g)
    # Ruhepose: Pose 1 mit gehobenem Kopf und geschlossenem Maul. Alle Viere bleiben, wo sie in Pose 1 stehen. [f]
    w[ix['hals']] = (0, 0, 120)
    w[ix['kopf']] = (0, 0, 60)
    w[ix['kiefer']] = (0, 0, 30)
    FE.clip_dazu(form, 'ruhe', [FE.bild(form, wurzel=g['posen']['bild1']['wurzel'], winkel=w.tolist())])
    FE.boden_nachfuehren(form)
    return form, lage, dichte, teile_alle


def tim_genau(rgb):
    q = np.clip((np.round(rgb.astype(np.float32) / 8.0)).astype(np.int32), 0, 31)
    code = q[..., 0] | (q[..., 1] << 5) | (q[..., 2] << 10)
    u, inv, n = np.unique(code, return_inverse=True, return_counts=True)
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
    px = (inv2.reshape(code.shape) + 1).astype(np.uint8)
    farben = [(0, 0, 0)] + [tuple(int(c) << 3 for c in f) for f in cl]
    return px, FE.clut_aus_rgb(farben, transparent0=True), len(cl)


def teile_json(g, beine_im_rumpf=True):
    ix = namen(g)
    z = g['zuordnung']
    t = {}
    for bild in ('bild1', 'bild2'):
        for nm, ks in z[bild].items():
            t[nm] = [ix[k] for k in ks]
    t['right_arm'] = [ix[k] for k in ('oberarm_r', 'unterarm_r', 'hand_r', 'klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4')]
    t['right_arm_front'] = [ix[k] for k in ('unterarm_r', 'hand_r', 'klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4')]
    t['left_arm_front'] = [ix[k] for k in ('unterarm_l', 'hand_l', 'klaue_l1', 'klaue_l2', 'klaue_l3', 'klaue_l4')]
    # Bild 2: In der dunklen Masse des Nutzer-Umrisses steht unten das rechte Bein in der Hocke (FORMBLATT2 Abschnitt 5, 7), dazu der rechte Oberarm.
    if beine_im_rumpf:
        t['face_and_upper_body'] = t['face_and_upper_body'] + [ix[k] for k in ('oberarm_r', 'oberschenkel_r', 'unterschenkel_r', 'fuss_r')]
    return t


def schreibe(form, rgb, aus, g, name='FORM', j_zusatz=None):
    emd = FE.schreibe_emd(form)
    px, clut, nf = tim_genau(rgb)
    tim = FE.tim_schreiben(px, [clut])
    open(os.path.join(aus, name + '.EMD'), 'wb').write(emd)
    open(os.path.join(aus, name + '.TIM'), 'wb').write(tim)
    j = dict(name='C', emd=name + '.EMD', tim=name + '.TIM', licht='mess', ruhe=dict(clip=2, bild=0), teile=teile_json(g))
    for n in (1, 2):
        p = g['posen']['bild%d' % n]
        j['bild%d' % n] = dict(lage=p['lage'], gier=p['gier'], clip=n - 1, bild=0)
    if j_zusatz:
        j.update(j_zusatz)
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
    """Original | gefiltert | sauber, je Bild, Ausschnitt um die Figur."""
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
    import rc_plan
    import rc_textur
    a = argparse.ArgumentParser()
    a.add_argument('stand')
    a.add_argument('--tag', default='t')
    a.add_argument('--voll', action='store_true')
    a = a.parse_args()
    g, teile = rc_plan.plan()
    stand = json.load(open(os.path.join(HIER, a.stand)))
    form, lage, dichte, teile_alle = baue_form(g, teile, stand)
    rgb, bk = rc_textur.backen(form, g, teile_alle, lage)
    cv2.imwrite(os.path.join(HIER, 'zw', 'textur.png'), rgb[:, :, ::-1])
    print('Dichte', dichte, 'Flaechen', sum(len(m['dreiecke']) + len(m['vierecke']) for m in form['meshes']))
    print('Farben', schreibe(form, rgb, HIER, g))
    json.dump([dict(name=p.name, knochen=p.knochen, feld=p.feld, rechteck=list(r)) for p, r in zip(teile_alle, lage)], open(os.path.join(HIER, 'atlas.json'), 'w'), indent=0)
    d = pruefe(HIER, a.tag, not a.voll)
    vergleich(HIER, a.tag)
    print(d)
