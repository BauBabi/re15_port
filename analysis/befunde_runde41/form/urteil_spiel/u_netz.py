# -*- coding: utf-8 -*-
"""Pruefer Spielmodell: Netzguete je Modell, Teile, offene Kanten, Inseln, Normalen, Flaechen. Schreibt netz.json."""
from collections import Counter
import trimesh, cv2
from u_basis import *


def netz(m):
    V = np.array(m['ecken'], float).reshape(-1, 3)
    F = dreiecke(m)
    if len(F) == 0:
        return None
    u, inv = np.unique(np.round(V, 0), axis=0, return_inverse=True)
    inv = inv.reshape(-1)
    F2 = inv[F]
    F2 = F2[(F2[:, 0] != F2[:, 1]) & (F2[:, 1] != F2[:, 2]) & (F2[:, 0] != F2[:, 2])]
    return trimesh.Trimesh(u, F2, process=False)


def kanten(me):
    e = np.sort(me.edges, axis=1)
    _, c = np.unique(e, axis=0, return_counts=True)
    return int((c == 1).sum()), int((c > 2).sum())


def uv_inseln(m):
    fl = []
    for f in m['dreiecke']:
        po = (f['page'] & 0xF) * 128
        fl.append([(e, (u + po, v)) for e, (u, v) in zip(f['e'], f['uv'])])
    for f in m['vierecke']:
        po = (f['page'] & 0xF) * 128
        q = [(e, (u + po, v)) for e, (u, v) in zip(f['e'], f['uv'])]
        fl.append([q[0], q[1], q[3], q[2]])
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
    for k, l in kant.items():
        for x in range(len(l)):
            for y in range(x + 1, len(l)):
                if l[x][1] == l[y][1]:
                    eltern[such(l[x][0])] = such(l[y][0])
    c = Counter(such(i) for i in range(len(fl)))
    return len(c), sorted(c.values(), reverse=True)


def normalen_abw(m):
    V = np.array(m['ecken'], float)
    N = np.array(m['normalen'], float)
    if len(N) == 0:
        return None
    fl = [(tuple(f['e']), tuple(f['n'])) for f in m['dreiecke']]
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
            a, b = acc[ee], N[nn]
            la, lb = np.linalg.norm(a), np.linalg.norm(b)
            if la > 1 and lb > 1:
                w.append(math.degrees(math.acos(np.clip(a.dot(b) / la / lb, -1, 1))))
    return np.array(w)


erg = {}
for k in ('A', 'B', 'C', 'K34', 'K33', 'K28', 'K23', 'K22', 'K20'):
    form = lade(k)
    n = len(form['knochen'])
    S = dict(knochen=n, flaechen=0, drei=0, vier=0, ecken=0, offen=0, nonman=0, inseln=0, einzel=0, teile_mehr=0, entartet=0, split=0)
    zeilen, ww, fl = [], [], []
    for i, m in enumerate(form['meshes'][:n]):
        if not m['ecken']:
            continue
        me = netz(m)
        if me is None:
            continue
        off, nm = kanten(me)
        comps = trimesh.graph.connected_components(me.edges, nodes=np.arange(len(me.vertices)), min_len=1)
        cf = [c for c in comps if len(c) > 2]
        ent = int((me.area_faces < 1).sum())
        isl, gr = uv_inseln(m)
        nf = len(m['dreiecke']) + len(m['vierecke'])
        S['flaechen'] += nf
        S['drei'] += len(m['dreiecke'])
        S['vier'] += len(m['vierecke'])
        S['ecken'] += len(m['ecken'])
        S['offen'] += off
        S['nonman'] += nm
        S['inseln'] += isl
        S['einzel'] += sum(1 for g in gr if g == 1)
        S['teile_mehr'] += max(len(cf) - 1, 0)
        S['entartet'] += ent
        a = me.vertices[me.faces]
        ls = np.stack([np.linalg.norm(a[:, 1] - a[:, 0], axis=1), np.linalg.norm(a[:, 2] - a[:, 1], axis=1), np.linalg.norm(a[:, 0] - a[:, 2], axis=1)], 1)
        h = 2 * me.area_faces / np.maximum(ls.max(1), 1e-6)
        S['split'] += int(((ls.max(1) / np.maximum(h, 1e-6)) > 8).sum())
        fl.append(me.area_faces)
        w = normalen_abw(m)
        if w is not None:
            ww.append(w)
        zeilen.append(dict(b=i, name=NAMEN[i] if k in ROHBAU else 'b%d' % i, eltern=form['knochen'][i]['eltern'], flaechen=nf, offen=off, nonman=nm, teile=len(cf), inseln=isl))
    w = np.concatenate(ww)
    f = np.concatenate(fl)
    S['norm_med'] = round(float(np.median(w)), 1)
    S['norm_p90'] = round(float(np.percentile(w, 90)), 1)
    S['flaeche_cv'] = round(float(f.std() / f.mean()), 2)
    S['kante'] = round(float(np.sqrt(f.mean() * 4 / math.sqrt(3))), 1)
    S['split_ant'] = round(S['split'] / len(f), 3)
    erg[k] = dict(S=S, zeilen=zeilen)
    print(k, S)
    if k in ROHBAU:
        for z in zeilen:
            print('   %2d %-16s elt %2d  fl %3d  offen %3d  nonman %2d  teile %2d  inseln %2d' % (z['b'], z['name'], z['eltern'], z['flaechen'], z['offen'], z['nonman'], z['teile'], z['inseln']))
json.dump(erg, open(AUS + '/netz.json', 'w'), indent=1)
