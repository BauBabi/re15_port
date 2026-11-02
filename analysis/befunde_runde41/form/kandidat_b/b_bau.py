#!/usr/bin/env python3
"""Kandidat B: Quad-Netze + Pose + Textur -> FORM.EMD, FORM.TIM, Kandidat-JSON.
    python b_bau.py --quad zw/quad0 --posen zw/posen0.json --aus zw/v0 [--pruefe]
"""
import argparse, json, math, os, sys, subprocess
import numpy as np, cv2
from b_geom import *
import b_teile as T
sys.path.insert(0, BAUM + '/re15_port/tools')
import form_emd as FE


# ═══════════════════════════════════════════════════ OBJ
def lese_obj(pfad):
    v, vt, f = [], [], []
    for z in open(pfad):
        if z.startswith('v '):
            v.append([float(c) for c in z.split()[1:4]])
        elif z.startswith('vt '):
            vt.append([float(c) for c in z.split()[1:3]])
        elif z.startswith('f '):
            ecken = []
            for t in z.split()[1:]:
                q = t.split('/')
                ecken.append((int(q[0]) - 1, int(q[1]) - 1 if len(q) > 1 and q[1] else -1))
            f.append(ecken)
    return np.array(v, float), np.array(vt, float), f


def vertexnormalen(v, f):
    n = np.zeros_like(v)
    vol = 0.0
    for e in f:
        idx = [a for a, _ in e]
        for i in range(1, len(idx) - 1):
            a, b, c = v[idx[0]], v[idx[i]], v[idx[i + 1]]
            k = np.cross(b - a, c - a)
            for j in (idx[0], idx[i], idx[i + 1]):
                n[j] += k
            vol += a @ np.cross(b, c)
    if vol < 0:
        n = -n
    return n / np.maximum(np.linalg.norm(n, axis=1, keepdims=True), 1e-9), vol < 0


# ═══════════════════════════════════════════════════ UV-Inseln und Atlas
def inseln(netz):
    """Inseln ueber gemeinsame Kanten mit gleichen (v, vt). Rueckgabe: Liste von Flaechenlisten."""
    f = netz['f']
    kante = {}
    p = list(range(len(f)))

    def wurzel(i):
        while p[i] != i:
            p[i] = p[p[i]]
            i = p[i]
        return i
    for i, e in enumerate(f):
        n = len(e)
        for j in range(n):
            a, b = e[j], e[(j + 1) % n]
            s = tuple(sorted((a, b)))
            if s in kante:
                x, y = wurzel(i), wurzel(kante[s])
                if x != y:
                    p[x] = y
            else:
                kante[s] = i
    g = {}
    for i in range(len(f)):
        g.setdefault(wurzel(i), []).append(i)
    return list(g.values())


def flaeche3d(v, e):
    idx = [a for a, _ in e]
    return sum(0.5 * np.linalg.norm(np.cross(v[idx[i]] - v[idx[0]], v[idx[i + 1]] - v[idx[0]])) for i in range(1, len(idx) - 1))


def flaeche2d(q):
    a = 0.0
    for i in range(len(q)):
        x0, y0 = q[i]
        x1, y1 = q[(i + 1) % len(q)]
        a += x0 * y1 - x1 * y0
    return abs(a) * 0.5


def packe(netze, B=384, H=256, rand=2, dichte_je=None, drehen=True):
    """Alle Inseln in den Atlas. Dichte = Texel je Einheit. Regal-Packung, Dichte per Bisektion. Rueckgabe: je Netz je Flaeche Liste Texel (x,y) je Ecke, Dichte."""
    roh = []
    for ni, nz in enumerate(netze):
        v, vt, f = nz['v'], nz['vt'], nz['f']
        for fl in inseln(nz):
            ids = sorted({c for i in fl for _, c in f[i]})
            uv = vt[ids]
            a3 = sum(flaeche3d(v, f[i]) for i in fl)
            a2 = sum(flaeche2d([vt[c] for _, c in f[i]]) for i in fl)
            mass = math.sqrt(a3 / max(a2, 1e-12))    # Einheiten je UV-Einheit
            # Kleinste umschliessende Box
            pts = (uv - uv.mean(0)) * mass
            best = None
            if len(pts) >= 3:
                hull = cv2.convexHull(pts.astype(np.float32))[:, 0, :]
                for w in np.arange(0, 90, 7.5):
                    c, s = math.cos(math.radians(w)), math.sin(math.radians(w))
                    r = hull @ np.array([[c, s], [-s, c]]).T
                    ext = r.max(0) - r.min(0)
                    if best is None or ext[0] * ext[1] < best[0]:
                        best = (ext[0] * ext[1], w)
            c, s = math.cos(math.radians(best[1])), math.sin(math.radians(best[1]))
            r = pts @ np.array([[c, s], [-s, c]]).T
            r -= r.min(0)
            roh.append(dict(netz=ni, flaechen=fl, ids=ids, p=r, ext=r.max(0), gew=(dichte_je or {}).get(nz['name'], 1.0)))

    def lege(d):
        o = sorted(roh, key=lambda q: -(q['ext'][1] * d * q['gew']))
        x = y = zeile = 0
        aus = {}
        for q in o:
            w, h = q['ext'] * d * q['gew']
            dreh = False
            if drehen and h > w * 1.2 and h <= zeile + 1e-9 and zeile > 0:
                w, h = h, w
                dreh = True
            w, h = int(math.ceil(w)) + 1 + rand, int(math.ceil(h)) + 1 + rand
            if x + w > B:
                x = 0
                y += zeile
                zeile = 0
            if y + h > H or w > B:
                return None
            aus[id(q)] = (x + rand / 2.0, y + rand / 2.0, dreh)
            x += w
            zeile = max(zeile, h)
        return aus
    lo, hi = 1e-4, 1.0
    for _ in range(40):
        m = (lo + hi) / 2
        if lege(m) is None:
            hi = m
        else:
            lo = m
    d = lo
    lage = lege(d)
    tex = [[None] * len(nz['f']) for nz in netze]
    for q in roh:
        x0, y0, dreh = lage[id(q)]
        s = d * q['gew']
        pp = q['p'] * s
        if dreh:
            pp = pp[:, ::-1]
        t = {c: (x0 + a, y0 + b) for c, (a, b) in zip(q['ids'], pp)}
        nz = netze[q['netz']]
        for i in q['flaechen']:
            tex[q['netz']][i] = [(int(round(t[c][0])), int(round(t[c][1]))) for _, c in nz['f'][i]]
    return tex, d


# ═══════════════════════════════════════════════════ Texel-Raster
def tri_raster(p, q, r, ziel):
    """Dreieck in Texelraum: Rueckgabe xs, ys, w0, w1, w2 fuer Texelmitten innerhalb (auch knapp ausserhalb, damit keine Spalte bleiben)."""
    x = np.array([p[0], q[0], r[0]], float)
    y = np.array([p[1], q[1], r[1]], float)
    fl = (x[1] - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (y[1] - y[0])
    if abs(fl) < 1e-6:
        return None
    x0, x1 = max(int(x.min()) - 1, 0), min(int(x.max()) + 2, ziel[1])
    y0, y1 = max(int(y.min()) - 1, 0), min(int(y.max()) + 2, ziel[0])
    if x0 >= x1 or y0 >= y1:
        return None
    PX, PY = np.meshgrid(np.arange(x0, x1) + 0.0, np.arange(y0, y1) + 0.0)
    w1 = ((PX - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (PY - y[0])) / fl
    w2 = ((x[1] - x[0]) * (PY - y[0]) - (PX - x[0]) * (y[1] - y[0])) / fl
    w0 = 1 - w1 - w2
    m = (w0 >= -0.02) & (w1 >= -0.02) & (w2 >= -0.02)
    return PX[m].astype(int), PY[m].astype(int), w0[m], w1[m], w2[m]


def texel_karten(netze, tex, B, H):
    pos = np.zeros((H, B, 3), np.float32)
    nrm = np.zeros((H, B, 3), np.float32)
    bone = np.full((H, B), -1, np.int16)
    for ni, nz in enumerate(netze):
        v, n, f = nz['v'], nz['n'], nz['f']
        for i, e in enumerate(f):
            t = tex[ni][i]
            idx = [a for a, _ in e]
            for j in range(1, len(idx) - 1):
                a, b, c = 0, j, j + 1
                g = tri_raster(t[a], t[b], t[c], (H, B))
                if g is None:
                    continue
                xs, ys, w0, w1, w2 = g
                pos[ys, xs] = (w0[:, None] * v[idx[a]] + w1[:, None] * v[idx[b]] + w2[:, None] * v[idx[c]])
                nrm[ys, xs] = (w0[:, None] * n[idx[a]] + w1[:, None] * n[idx[b]] + w2[:, None] * n[idx[c]])
                bone[ys, xs] = nz['bone']
    return pos, nrm, bone


def rand_fuellen(a, belegt, schritte=4):
    """Texel ausserhalb der Inseln von den Nachbarn fuellen (Naehte)."""
    a = a.copy()
    m = belegt.copy()
    k = np.ones((3, 3), np.uint8)
    for _ in range(schritte):
        neu = (cv2.dilate(m.astype(np.uint8), k) > 0) & ~m
        ys, xs = np.nonzero(neu)
        for y, x in zip(ys, xs):
            y0, y1, x0, x1 = max(y - 1, 0), y + 2, max(x - 1, 0), x + 2
            q = m[y0:y1, x0:x1]
            a[y, x] = a[y0:y1, x0:x1][q].mean(0)
        m |= neu
    return a


# ═══════════════════════════════════════════════════ Posen
def eul_R(w):
    cx, sx = math.cos(w[0]), math.sin(w[0])
    cy, sy = math.cos(w[1]), math.sin(w[1])
    cz, sz = math.cos(w[2]), math.sin(w[2])
    Rx = np.array([[1, 0, 0], [0, cx, -sx], [0, sx, cx]])
    Ry = np.array([[cy, 0, sy], [0, 1, 0], [-sy, 0, cy]])
    Rz = np.array([[cz, -sz, 0], [sz, cz, 0], [0, 0, 1]])
    return Rx @ Ry @ Rz


def fk(w, J, eltern, dy=0.0):
    R, Tt = [None] * len(eltern), [None] * len(eltern)
    for i, p in enumerate(eltern):
        L = eul_R(w[i])
        if p < 0:
            R[i], Tt[i] = L, J[i] + np.array([0, dy, 0])
        else:
            R[i], Tt[i] = R[p] @ L, R[p] @ (J[i] - J[p]) + Tt[p]
    return R, Tt


def form_bauen(netze, tex, J, posen, M=None):
    """J: Gelenke im Bindraum. M: dict Knochen -> 3x3, Netzrahmen des Knochens (Ruhe-Ausrichtung). posen: [(Name, [dict(R=Liste 3x3, T=Liste)])]."""
    f = FE.neu()
    n = len(T.KNOCHEN)
    Mm = [np.eye(3) if M is None or b not in M else M[b] for b in range(n)]
    Jr = np.round(J)
    for i, (nm, e) in enumerate(T.KNOCHEN):
        ver = (0, 0, 0) if e < 0 else tuple(np.round(Mm[e].T @ (Jr[i] - Jr[e])))
        FE.knochen_dazu(f, nm, e, ver)
    for ni, nz in enumerate(netze):
        b = nz['bone']
        m = f['meshes'][b]
        v0 = len(m['ecken'])
        n0 = len(m['normalen'])
        for q in (nz['v'] - Jr[b]) @ Mm[b]:
            FE.ecke(m, q)
        for q in nz['n'] @ Mm[b]:
            FE.normale(m, q)
        for i, e in enumerate(nz['f']):
            idx = [a for a, _ in e]
            t = tex[ni][i]
            if nz['umdrehen']:
                idx = idx[::-1]
                t = t[::-1]
            if len(idx) == 3:
                FE.dreieck(m, [v0 + k for k in idx], [n0 + k for k in idx], t)
            else:
                z = (0, 1, 3, 2)
                FE.viereck(m, [v0 + idx[k] for k in z], [n0 + idx[k] for k in z], [t[k] for k in z])
    for name, p in posen:
        bilder = []
        for b in p:
            R, Tt = b['R'], b['T']
            L = []
            for i, e in enumerate(T.ELTERN):
                l = R[i] @ Mm[i] if e < 0 else (R[e] @ Mm[e]).T @ (R[i] @ Mm[i])
                L.append(FE.winkel_aus_matrix(l))
            bilder.append(FE.bild(f, wurzel=tuple(int(round(c)) for c in Tt[0]), winkel=L))
        FE.clip_dazu(f, name, bilder)
    FE.orientiere(f)
    FE.boden_nachfuehren(f)
    return f


def lade_netze(ordner, kuppel_teilen=None, snap=None):
    """Netze je Knochen. kuppel.obj wird von kuppel_teilen(mitte je Flaeche) -> Liste (Knochen, Maske) aufgeteilt."""
    netze = []
    p = os.path.join(ordner, 'kuppel.obj')
    if os.path.exists(p):
        v, vt, f = lese_obj(p)
        if snap and 'kuppel' in snap:
            v = snap['kuppel']
        n, um = vertexnormalen(v, f)
        mi = np.array([v[[a for a, _ in e]].mean(0) for e in f])
        for b, m in kuppel_teilen(mi):
            ff = [e for e, q in zip(f, m) if q]
            netze.append(dict(name=T.KNOCHEN[b][0], bone=b, v=v, vt=vt, f=ff, n=n, umdrehen=um))
    for i, (nm, _) in enumerate(T.KNOCHEN):
        p = os.path.join(ordner, 'k%02d.obj' % i)
        if not os.path.exists(p):
            continue
        v, vt, f = lese_obj(p)
        k = 'k%02d' % i
        if snap and k in snap:
            v = snap[k]
        n, um = vertexnormalen(v, f)
        netze.append(dict(name=nm, bone=i, v=v, vt=vt, f=f, n=n, umdrehen=um))
    return netze


def pruefe(json_pfad, aus):
    r = subprocess.run([sys.executable, BAUM + '/re15_port/tools/form_pruefung.py', 'pruefe', json_pfad, '--aus', aus], capture_output=True, text=True, encoding='utf-8', env=dict(os.environ, PYTHONIOENCODING='utf-8'))
    if r.returncode:
        print(r.stdout[-2000:], r.stderr[-3000:])
    return r.returncode == 0
