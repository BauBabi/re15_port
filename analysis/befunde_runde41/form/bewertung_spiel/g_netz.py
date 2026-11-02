# -*- coding: utf-8 -*-
"""Pruefer Spielmodell: Netzguete, UV-Inseln, Normalen, Knochenbau je Kandidat. Liest nur, schreibt nur in diesen Ordner."""
import sys, os, json, math
from collections import Counter
import numpy as np
import trimesh
import cv2

WT = 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form'
sys.path.insert(0, WT + '/re15_port/tools')
import form_emd as FE

FORM = WT + '/analysis/befunde_runde41/form'
K = {'A': (FORM + '/kandidat_a/FORM.EMD', FORM + '/kandidat_a/FORM.TIM'),
     'B': (FORM + '/kandidat_b/FORM.EMD', FORM + '/kandidat_b/FORM.TIM'),
     'C': (FORM + '/kandidat_c/FORM.EMD', FORM + '/kandidat_c/FORM.TIM'),
     'G4': (WT + '/re15_port/shared_assets/RE2/BAHNSTEIG_G4.EMD', WT + '/re15_port/shared_assets/RE2/BAHNSTEIG_G4.TIM')}


def tris_von(m):
    t = []
    for f in m['dreiecke']:
        t.append(tuple(f['e']))
    for f in m['vierecke']:
        a, b, c, d = f['e']
        t.append((a, b, d))
        t.append((a, d, c))
    return np.array(t, dtype=np.int64).reshape(-1, 3)


def netz(m):
    V = np.array(m['ecken'], float).reshape(-1, 3)
    F = tris_von(m)
    if len(F) == 0:
        return None
    u, inv = np.unique(np.round(V, 0), axis=0, return_inverse=True)
    inv = inv.reshape(-1)
    F2 = inv[F]
    F2 = F2[(F2[:, 0] != F2[:, 1]) & (F2[:, 1] != F2[:, 2]) & (F2[:, 0] != F2[:, 2])]
    return trimesh.Trimesh(u, F2, process=False)


def kanten_stat(me):
    e = np.sort(me.edges, axis=1)
    _, c = np.unique(e, axis=0, return_counts=True)
    return int((c == 1).sum()), int((c > 2).sum())


def uv_inseln(m):
    """Inseln = Flaechen, die eine Netzkante mit gleichen Texeln an beiden Enden teilen."""
    fl = []
    for f in m['dreiecke']:
        po = (f['page'] & 0xF) * 128
        fl.append([(e, (u + po, v)) for e, (u, v) in zip(f['e'], f['uv'])])
    for f in m['vierecke']:
        po = (f['page'] & 0xF) * 128
        q = [(e, (u + po, v)) for e, (u, v) in zip(f['e'], f['uv'])]
        fl.append([q[0], q[1], q[3], q[2]])      # Z-Form -> Ring
    V = np.round(np.array(m['ecken'], float), 0)

    def schl(e):
        return tuple(V[e])
    kant = {}
    for i, r in enumerate(fl):
        n = len(r)
        for j in range(n):
            a, ua = r[j]
            b, ub = r[(j + 1) % n]
            pa, pb = schl(a), schl(b)
            k = (pa, pb) if pa < pb else (pb, pa)
            uvk = (ua, ub) if pa < pb else (ub, ua)
            kant.setdefault(k, []).append((i, uvk))
    eltern = list(range(len(fl)))

    def such(x):
        while eltern[x] != x:
            eltern[x] = eltern[eltern[x]]
            x = eltern[x]
        return x
    naht = innen = 0
    for k, l in kant.items():
        if len(l) < 2:
            continue
        verbunden = False
        for x in range(len(l)):
            for y in range(x + 1, len(l)):
                if l[x][1] == l[y][1]:
                    eltern[such(l[x][0])] = such(l[y][0])
                    verbunden = True
        if verbunden:
            innen += 1
        else:
            naht += 1
    c = Counter(such(i) for i in range(len(fl)))
    return len(c), naht, innen, sorted(c.values(), reverse=True)


def dichte(m):
    """Texel je 1000 Einheiten (Wurzel Flaechenverhaeltnis) je Dreieck, mit 3D-Flaeche als Gewicht."""
    V = np.array(m['ecken'], float)
    aus = []

    def ein(ix, uv):
        p = V[list(ix)]
        a3 = 0.5 * np.linalg.norm(np.cross(p[1] - p[0], p[2] - p[0]))
        u = np.array(uv, float)
        a2 = 0.5 * abs((u[1, 0] - u[0, 0]) * (u[2, 1] - u[0, 1]) - (u[2, 0] - u[0, 0]) * (u[1, 1] - u[0, 1]))
        if a3 > 1 and a2 > 0:
            aus.append((math.sqrt(a2 / a3) * 1000, a3))
    for f in m['dreiecke']:
        ein(f['e'], f['uv'])
    for f in m['vierecke']:
        e, uv = f['e'], f['uv']
        ein((e[0], e[1], e[3]), (uv[0], uv[1], uv[3]))
        ein((e[0], e[3], e[2]), (uv[0], uv[3], uv[2]))
    return aus


def normalen(m):
    """Winkel gespeicherte Normale gegen geometrische Eckennormale. Windung ist gegen die Aussennormale (Kopf form_emd)."""
    V = np.array(m['ecken'], float)
    N = np.array(m['normalen'], float)
    if len(N) == 0:
        return None
    fl = []
    for f in m['dreiecke']:
        fl.append((tuple(f['e']), tuple(f['n'])))
    for f in m['vierecke']:
        e, n = f['e'], f['n']
        fl.append(((e[0], e[1], e[3]), (n[0], n[1], n[3])))
        fl.append(((e[0], e[3], e[2]), (n[0], n[3], n[2])))
    acc = np.zeros_like(V)
    for e, n in fl:
        p = V[list(e)]
        c = -np.cross(p[1] - p[0], p[2] - p[0])
        for ee in e:
            acc[ee] += c
    w = []
    for e, n in fl:
        for ee, nn in zip(e, n):
            a = acc[ee]
            b = N[nn]
            la, lb = np.linalg.norm(a), np.linalg.norm(b)
            if la > 1 and lb > 1:
                w.append(math.degrees(math.acos(np.clip(a.dot(b) / la / lb, -1, 1))))
    return np.array(w)


def bericht(name):
    emd = open(K[name][0], 'rb').read()
    tim = FE.tim_lesen(open(K[name][1], 'rb').read())
    form = FE.lese_emd(emd)
    n = len(form['knochen'])
    zeilen = []
    S = dict(name=name, knochen=n, clips=len(form['clips']), faces=0, tri=0, quad=0, verts=0, offen=0, nonman=0,
             teile_mehrfach=0, entartet=0, inseln=0, naht=0, innen=0, einzel=0)
    alle_d, alle_w, flaechen = [], [], []
    for i, m in enumerate(form['meshes']):
        if i >= n:
            break
        if not m['ecken']:
            zeilen.append((i, form['knochen'][i]['eltern'], 0, 0, 0, 0, 0, 0, 0))
            continue
        me = netz(m)
        if me is None:
            continue
        off, nm = kanten_stat(me)
        comps = trimesh.graph.connected_components(me.edges, nodes=np.arange(len(me.vertices)), min_len=1)
        comps_f = [c for c in comps if len(c) > 2]
        ent = int((me.area_faces < 1).sum())
        isl, naht, innen, gr = uv_inseln(m)
        nf = len(m['dreiecke']) + len(m['vierecke'])
        S['faces'] += nf
        S['tri'] += len(m['dreiecke'])
        S['quad'] += len(m['vierecke'])
        S['verts'] += len(m['ecken'])
        S['offen'] += off
        S['nonman'] += nm
        S['teile_mehrfach'] += max(len(comps_f) - 1, 0)
        S['entartet'] += ent
        S['inseln'] += isl
        S['naht'] += naht
        S['innen'] += innen
        e1 = sum(1 for g in gr if g == 1)
        S['einzel'] += e1
        zeilen.append((i, form['knochen'][i]['eltern'], nf, len(m['ecken']), off, nm, len(comps_f), isl, e1))
        alle_d += dichte(m)
        w = normalen(m)
        if w is not None:
            alle_w.append(w)
        flaechen.append(me.area_faces)
    d = np.array(alle_d)
    mw = np.average(d[:, 0], weights=d[:, 1])
    sd = math.sqrt(np.average((d[:, 0] - mw) ** 2, weights=d[:, 1]))
    S['dichte_mittel'] = round(float(mw), 1)
    S['dichte_cv'] = round(float(sd / mw), 3)
    S['dichte_p10'], S['dichte_p90'] = [round(float(x), 1) for x in np.percentile(d[:, 0], [10, 90])]
    w = np.concatenate(alle_w)
    S['norm_median'] = round(float(np.median(w)), 1)
    S['norm_p90'] = round(float(np.percentile(w, 90)), 1)
    S['norm_ueber45'] = round(float((w > 45).mean()), 3)
    S['norm_ueber90'] = round(float((w > 90).mean()), 3)
    fl = np.concatenate(flaechen)
    S['flaeche_cv'] = round(float(fl.std() / fl.mean()), 2)
    S['kante_mittel'] = round(float(np.sqrt(fl.mean() * 4 / math.sqrt(3))), 1)
    px = tim['pixel']
    H, W = px.shape
    ma = np.zeros((H, W), np.uint8)
    for m in form['meshes'][:n]:
        for f in m['dreiecke'] + m['vierecke']:
            po = (f['page'] & 0xF) * 128
            u = np.array([[a + po, b] for a, b in f['uv']], np.int32)
            if len(u) == 4:
                u = u[[0, 1, 3, 2]]
            cv2.fillConvexPoly(ma, u, 1)
    S['atlas_belegt'] = round(float(ma.mean()), 3)
    S['tex'] = '%dx%d' % (W, H)
    return S, zeilen


alle = {}
for k in K:
    S, z = bericht(k)
    alle[k] = S
    print(k, json.dumps(S, ensure_ascii=False))
    print('  b  elt faces verts offen nonman teile inseln einzel')
    for r in z:
        print('  %2d %3d %5d %5d %5d %5d %4d %5d %5d' % r)
json.dump(alle, open(FORM + '/bewertung_spiel/netz.json', 'w'), indent=1)
