#!/usr/bin/env python3
"""Tiefenpruefung: Geschlossenheit, Durchstossung, Normalen, Sichtbarkeit je Knochen, Dicke, Transparenz.
Laeuft mit torch auf der GPU. Schreibt geo2_<k>.json und druckt eine Kurzfassung."""
import json, math, os, sys
import numpy as np
import torch
import cv2
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import geo as G
sys.path.insert(0, G.TOOLS)
import form_emd as FE
import form_kamera as FK
import emd_ansichtsblatt as EA

DEV = 'cuda' if torch.cuda.is_available() else 'cpu'
torch.set_grad_enabled(False)

GRUPPEN = {
 'a': dict(TORSO=[0, 9, 10, 1, 2], KOPF=[20, 21, 22], FR=[3, 4, 5], FL=[6, 7, 8], NL=[11, 12, 13, 14, 15], NR=[16, 17, 18, 19]),
 'b': dict(TORSO=[0, 9, 10, 15, 20], FR=[5, 6, 7, 8], FL=[1, 2, 3, 4], NL=[11, 12, 13, 14], NR=[16, 17, 18, 19]),
 'c': dict(TORSO=[0, 9, 10], KOPF=[14, 15, 20], FR=[1, 2, 3, 4], FL=[5, 6, 7, 8], NL=[16, 17, 18, 19], NR=[11, 12, 13]),
}


def welle(v, t):
    """Ecken nach Koordinate verschweissen."""
    sch, inv = np.unique(v.round().astype(np.int64), axis=0, return_inverse=True)
    return sch, inv.reshape(-1)[t]


def geschlossenheit(f, b):
    v, n, t, nt = G.mesh_arrays(f, b)
    if not len(t):
        return dict(rand=0, nicht_mf=0, komponenten=0, flaechen=0)
    vw, tw = welle(v, t)
    e = np.vstack([tw[:, [0, 1]], tw[:, [1, 2]], tw[:, [2, 0]]])
    es = np.sort(e, axis=1)
    u, c = np.unique(es, axis=0, return_counts=True)
    rand = int((c == 1).sum())
    nmf = int((c > 2).sum())
    # Komponenten
    par = list(range(len(vw)))
    def fi(x):
        while par[x] != x:
            par[x] = par[par[x]]
            x = par[x]
        return x
    for a, b_ in u:
        par[fi(a)] = fi(b_)
    kom = len({fi(i) for i in np.unique(tw)})
    return dict(rand=rand, nicht_mf=nmf, komponenten=kom, flaechen=len(t))


def dreiecke_welt(f, ci, bs):
    R, T = G.pose(f, ci)
    V, Fa, Bi = [], [], []
    o = 0
    for b in bs:
        v, n, t, nt = G.mesh_arrays(f, b)
        if not len(t):
            continue
        V.append(v @ R[b].T + T[b])
        Fa.append(t + o)
        Bi.append(np.full(len(t), b))
        o += len(v)
    return np.vstack(V), np.vstack(Fa), np.concatenate(Bi)


def windung(q, V, Fa, block=512):
    """Verallgemeinerte Windungszahl fuer Punkte q (N,3) gegen Dreiecke (M), torch."""
    Q = torch.tensor(q, dtype=torch.float32, device=DEV)
    A = torch.tensor(V[Fa[:, 0]], dtype=torch.float32, device=DEV)
    B = torch.tensor(V[Fa[:, 1]], dtype=torch.float32, device=DEV)
    C = torch.tensor(V[Fa[:, 2]], dtype=torch.float32, device=DEV)
    out = []
    for i in range(0, len(Q), block):
        q_ = Q[i:i + block][:, None, :]
        a, b, c = A[None] - q_, B[None] - q_, C[None] - q_
        la, lb, lc = a.norm(dim=-1), b.norm(dim=-1), c.norm(dim=-1)
        det = (a * torch.linalg.cross(b, c)).sum(-1)
        nen = la * lb * lc + (a * b).sum(-1) * lc + (b * c).sum(-1) * la + (c * a).sum(-1) * lb
        om = 2 * torch.atan2(det, nen)
        out.append(om.sum(-1) / (4 * math.pi))
    return torch.cat(out).cpu().numpy()


def flaechen_proben(V, Fa, dichte=1 / 3600.0, rng=np.random.default_rng(1)):
    a, b, c = V[Fa[:, 0]], V[Fa[:, 1]], V[Fa[:, 2]]
    fl = 0.5 * np.linalg.norm(np.cross(b - a, c - a), axis=1)
    n = np.maximum(1, (fl * dichte).astype(int))
    idx = np.repeat(np.arange(len(Fa)), n)
    r1, r2 = rng.random(len(idx)), rng.random(len(idx))
    s = np.sqrt(r1)
    p = a[idx] * (1 - s)[:, None] + b[idx] * (s * (1 - r2))[:, None] + c[idx] * (s * r2)[:, None]
    return p, idx


def naechster(q, p, block=2048):
    Q = torch.tensor(q, dtype=torch.float32, device=DEV)
    P = torch.tensor(p, dtype=torch.float32, device=DEV)
    aus = []
    for i in range(0, len(Q), block):
        aus.append(torch.cdist(Q[i:i + block], P).min(1)[0])
    return torch.cat(aus).cpu().numpy()


def durchstoss(f, k):
    """Je Pose: Anteil der Oberflaeche einer Gruppe, der in einer anderen Gruppe steckt, und tiefster Punkt. Ansatzglieder zaehlen mit, darum Tiefe nur grob lesen."""
    gr = GRUPPEN[k]
    nm = list(gr)
    aus = {}
    for ci in (0, 1, 2):
        z = {}
        for a in nm:
            Va, Fa, Ba = dreiecke_welt(f, ci, gr[a])
            pa, ia = flaechen_proben(Va, Fa)
            for b in nm:
                if a == b:
                    continue
                Vb, Fb, _ = dreiecke_welt(f, ci, gr[b])
                w = windung(pa, Vb, Fb)
                d = np.abs(w) > 0.5
                if d.mean() < 0.002:
                    continue
                pb, _ = flaechen_proben(Vb, Fb, dichte=1 / 1600.0)
                t = naechster(pa[d], pb) if d.any() else np.zeros(1)
                z['%s in %s' % (a, b)] = (round(float(d.mean()), 3), round(float(t.max()), 0))
        aus[ci] = z
    return aus


def einzel_durchstoss(f, k):
    """Je Knochen: Anteil seiner Oberflaeche, der im Rest der Figur (ohne Eltern, Kinder, sich selbst) steckt, je Pose. Findet versenkte Teile."""
    kn = f['knochen']
    aus = {}
    for ci in (0, 1, 2):
        r = {}
        for b in range(len(kn)):
            v, n, t, nt = G.mesh_arrays(f, b)
            if not len(t):
                continue
            nah = {b, kn[b]['eltern']} | {i for i, x in enumerate(kn) if x['eltern'] == b}
            V, Fa, Bi = dreiecke_welt(f, ci, [i for i in range(len(kn)) if i not in nah])
            Vb, Fb, _ = dreiecke_welt(f, ci, [b])
            p, _ = flaechen_proben(Vb, Fb, dichte=1 / 2500.0)
            w = windung(p, V, Fa)
            r[b] = round(float((np.abs(w) > 0.5).mean()), 3)
        aus[ci] = r
    return aus


def normalen(f, k):
    """Winkel zwischen gespeicherter Eckennormale und geometrischer Flaechennormale (Windung gegen die Aussennormale)."""
    W = []
    Oben = []
    for b in range(len(f['knochen'])):
        v, n, t, nt = G.mesh_arrays(f, b)
        if not len(t):
            continue
        a, b_, c = v[t[:, 0]], v[t[:, 1]], v[t[:, 2]]
        g = np.cross(b_ - a, c - a)
        lg = np.linalg.norm(g, axis=1)
        ok = lg > 1
        g = g[ok] / lg[ok][:, None]
        nn = n[nt[ok]] / 4096.0
        nm = nn.mean(1)
        nm /= np.maximum(np.linalg.norm(nm, axis=1, keepdims=True), 1e-6)
        # orientiere: cross zeigt GEGEN die Aussennormale
        cw = np.clip((-g * nm).sum(1), -1, 1)
        W.append(np.degrees(np.arccos(cw)))
    W = np.concatenate(W)
    return dict(median=round(float(np.median(W)), 1), mittel=round(float(W.mean()), 1), p90=round(float(np.percentile(W, 90)), 1), ueber30=round(float((W > 30).mean()), 3), ueber60=round(float((W > 60).mean()), 3), ueber90=round(float((W > 90).mean()), 3))


def transparent(k):
    d = os.path.join(G.FORM, 'kandidat_' + k)
    t = FE.tim_lesen(open(d + '/FORM.TIM', 'rb').read())
    p = t['pixel']
    c = np.array(t['cluts'][0]) if len(t['cluts']) else None
    u = np.unique(p)
    loch = [int(i) for i in u if int(c[i]) == 0]
    return dict(farben=len(u), loch_indizes=loch, anteil=round(float(np.isin(p, loch).mean()), 4))


def welt_kamera(k, n):
    j = G.lage(k)
    s = j['bild%d' % n]
    lage = np.array(s['lage'], float)
    g = int(s['gier']) * 2 * math.pi / 4096.0
    c, sn = math.cos(g), math.sin(g)
    Y = np.array([[c, 0, sn], [0, 1, 0], [-sn, 0, c]])
    return Y, lage


def sichtbarkeit(f, k, n, ci):
    """Z-Puffer je Dreieck, Bild 640x480. Rueckgabe: Pixel je Knochen, Dreieck-Ids."""
    kam = FK.lade_kamera(n)
    Y, lage = welt_kamera(k, n)
    R, T = G.pose(f, ci)
    H, W = 480, 640
    zb = np.full((H, W), 1e12)
    ids = np.full((H, W), -1, np.int32)
    V, Fa, Bi = dreiecke_welt(f, ci, range(len(f['knochen'])))
    Wv = V @ Y.T + lage
    pr = FK.projiziere(kam, Wv)
    tief = (Wv - np.asarray(kam['auge'], float)) @ kam['R'][2]
    # Rueckseiten weg
    a, b, c = pr[Fa[:, 0]], pr[Fa[:, 1]], pr[Fa[:, 2]]
    ar = (b[:, 0] - a[:, 0]) * (c[:, 1] - a[:, 1]) - (c[:, 0] - a[:, 0]) * (b[:, 1] - a[:, 1])
    # Vorzeichen: Windung cross(b-a,c-a) gegen die Aussennormale; Aussenflaeche zeigt zur Kamera, wenn Schirmflaeche > 0 im y-unten-Bild
    for fi in np.nonzero(ar > 0)[0]:
        i0, i1, i2 = Fa[fi]
        x = np.array([pr[i0, 0], pr[i1, 0], pr[i2, 0]])
        y = np.array([pr[i0, 1], pr[i1, 1], pr[i2, 1]])
        z = np.array([tief[i0], tief[i1], tief[i2]])
        x0, x1 = max(int(x.min()), 0), min(int(x.max()) + 2, W)
        y0, y1 = max(int(y.min()), 0), min(int(y.max()) + 2, H)
        if x0 >= x1 or y0 >= y1:
            continue
        PX, PY = np.meshgrid(np.arange(x0, x1), np.arange(y0, y1))
        d = (x[1] - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (y[1] - y[0])
        if abs(d) < 1e-9:
            continue
        w1 = ((PX - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (PY - y[0])) / d
        w2 = ((x[1] - x[0]) * (PY - y[0]) - (PX - x[0]) * (y[1] - y[0])) / d
        w0 = 1 - w1 - w2
        m = (w0 >= 0) & (w1 >= 0) & (w2 >= 0)
        if not m.any():
            continue
        zz = w0 * z[0] + w1 * z[1] + w2 * z[2]
        s = zb[y0:y1, x0:x1]
        q = m & (zz < s)
        s[q] = zz[q]
        ids[y0:y1, x0:x1][q] = fi
    return ids, Bi, zb


def dicke(f, k, n, ci):
    """Je Knochen: Ausdehnung quer zum Sehstrahl gegen entlang des Sehstrahls, in dieser Kamera. Papp-Teile haben kleine Tiefe."""
    kam = FK.lade_kamera(n)
    Y, lage = welt_kamera(k, n)
    R, T = G.pose(f, ci)
    aus = {}
    for b in range(len(f['knochen'])):
        v, n_, t, nt = G.mesh_arrays(f, b)
        if not len(t):
            continue
        p = (v @ R[b].T + T[b]) @ Y.T + lage
        q = (p - np.asarray(kam['auge'], float)) @ kam['R'].T
        e = np.ptp(q, axis=0)
        # Hauptachsen
        c = p - p.mean(0)
        s = np.linalg.svd(c, compute_uv=False)
        s = s / math.sqrt(len(p)) * 4       # rund Ausdehnung
        aus[b] = dict(quer=[round(float(e[0])), round(float(e[1]))], tief=round(float(e[2])), pca=[round(float(x)) for x in s])
    return aus


def lauf(k):
    f, emd = G.lade(k)
    nm = G.NAMEN[k]
    print('\n================== Kandidat', k.upper())
    kn = f['knochen']
    gs = [geschlossenheit(f, b) for b in range(len(kn))]
    print('Geschlossenheit (offene Kanten, nicht-mannigfaltig, Komponenten, Dreiecke):')
    for b, g in enumerate(gs):
        print(' %2d %-11s rand %3d  nm %3d  komp %2d  tri %3d' % (b, nm[b], g['rand'], g['nicht_mf'], g['komponenten'], g['flaechen']))
    print(' Summe offene Kanten', sum(g['rand'] for g in gs), 'Meshes offen', sum(g['rand'] > 0 for g in gs), 'Komponenten', sum(g['komponenten'] for g in gs))
    print('Normalen', normalen(f, k))
    print('Transparenz', transparent(k))
    e = einzel_durchstoss(f, k)
    print('Versenkt je Knochen (Anteil der Oberflaeche im Rest, ohne Eltern und Kinder): Clip 0 / 1 / 2')
    for b in range(len(kn)):
        if b in e[0]:
            print(' %2d %-11s %.2f %.2f %.2f' % (b, nm[b], e[0][b], e[1][b], e[2][b]))
    d = durchstoss(f, k)
    print('Gruppen-Durchstossung (Anteil Oberflaeche, tiefster Punkt):')
    for ci in (0, 1, 2):
        print(' Clip', ci, d[ci])
    vis = {}
    for n, ci in ((1, 0), (2, 1)):
        ids, Bi, zb = sichtbarkeit(f, k, n, ci)
        px = {}
        for b in range(len(kn)):
            px[b] = int(np.isin(ids, np.nonzero(Bi == b)[0]).sum())
        vis[n] = px
        np.save(os.path.join(G.FORM, 'bewertung_mass_zw', 'ids_%s_%d.npy' % (k, n)), ids)
    print('Sichtbare Pixel je Knochen (Z-Puffer): Bild 1, Bild 2, verdeckt in beiden = beide 0')
    for b in range(len(kn)):
        print(' %2d %-11s %6d %6d' % (b, nm[b], vis[1][b], vis[2][b]))
    # Flaechenanteil, der in keinem Bild sichtbar ist
    d1 = dicke(f, k, 1, 0)
    d2 = dicke(f, k, 2, 1)
    print('Dicke: quer, Tiefe (Bild1) | quer, Tiefe (Bild2) | Hauptachsen')
    for b in range(len(kn)):
        if b in d1:
            print(' %2d %-11s %s %5d | %s %5d | %s' % (b, nm[b], d1[b]['quer'], d1[b]['tief'], d2[b]['quer'], d2[b]['tief'], d1[b]['pca']))
    json.dump(dict(gs=gs, einzel=e, durch=d, vis=vis), open(os.path.join(G.FORM, 'bewertung_mass_zw', 'geo2_%s.json' % k), 'w'), indent=0)


if __name__ == '__main__':
    for k in (sys.argv[1:] or 'abc'):
        lauf(k)
