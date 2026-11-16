# -*- coding: utf-8 -*-
"""K5g3 R9-Einbau: N52 BEIDSEITIG + Boden ueber ALLE 771 Keys einer EMD (Ganzzahl-Pose wie das Spiel: FE.modell -> kf_k5g2_geo.posen,
erstes Bild, das den Key nutzt). Herkunft: scratchpad k5g2/kr4/r9_einbau/scan2.py (Bauer R9-Einbau), Strahl-Paritaet wie kr4 r9 n52lib
(3 feste schiefe Richtungen, Mehrheit; keine Normalen -> keine Fehltreffer am offenen Kiefernetz), Tiefe = Abstand zur Huelle.
  vor:   Teil g des rechten Arms (16, 17, 18, 19, 20, 29, 31) in jedes fremde Teil h (nicht Eltern/Kind/selbst)
  rueck: jedes fremde Teil h ausserhalb des Arms in Teil g des Arms (Kiefer-in-Hand findet nur diese Richtung; Pruefer R9 Key 546)
  boden: groesstes y (y = 0 Boden, + = darunter; die Fuesse 27/25 setzen bei y = 0..-2 auf) von 17, 18, 19, 20, 29, 31 (16 zur Info)
Grenze N52 = max(Bezuege im selben Key) + 25 (Krallenpaare rechts zusaetzlich 1,3167 x linke Gegenpaarung, wie kr4 r9 n52scan);
Grenze Boden = max(25, Bezug) (E8/K12: nichts mehr als 25 unter dem Boden; Clip 12 Bild 49-55 sinkt die Form selbst ein).
  python kf_k5g3_n52.py ref <name> <emd> <aus.json>
  python kf_k5g3_n52.py kand <emd> <aus.json> <ref.json> [<ref2.json> ...]"""
import sys, os, json, time, hashlib
import numpy as np
from multiprocessing import Pool
GEN = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, GEN); sys.dont_write_bytecode = True
import kf_k5g2_geo as G
FE = G.FE
TIM0 = open(os.path.join(G.K5, 'ruhe', 'K5e_f1', 'BAHNSTEIG_FORM.TIM'), 'rb').read()
ARM = (16, 17, 18, 19, 20, 29, 31); KR_R = (19, 20, 29, 31); KR_L = (14, 15, 28, 30); LV = {19: 14, 20: 15, 29: 28, 31: 30}
BODEN = (17, 18) + KR_R
S_ = 1.3167
DIRS = [np.array(d, float) / np.linalg.norm(d) for d in ((1, .13, .07), (-.11, 1, .05), (.06, -.09, 1))]


def paritaet(P, Tri, d):
    A, B, C = Tri[:, 0], Tri[:, 1], Tri[:, 2]; e1 = B - A; e2 = C - A; h = np.cross(d, e2); a = (e1 * h).sum(1); ok = np.abs(a) > 1e-9
    inv = np.where(ok, 1.0 / np.where(ok, a, 1.0), 0.0)
    s = P[:, None, :] - A[None]; u = (s * h[None]).sum(2) * inv
    q = np.cross(s, e1[None]); v = (q * d).sum(2) * inv; t = (q * e2[None]).sum(2) * inv
    tr = ok[None] & (u >= 0) & (v >= 0) & (u + v <= 1) & (t > 1e-6)
    return (tr.sum(1) % 2) == 1


def tiefe52(P, Tri):
    lo, hi = Tri.reshape(-1, 3).min(0), Tri.reshape(-1, 3).max(0)
    sel = ((P >= lo) & (P <= hi)).all(1)
    if not sel.any():
        return 0.0
    Pi = P[sel]
    inn = np.sum([paritaet(Pi, Tri, d) for d in DIRS], 0) >= 2
    if not inn.any():
        return 0.0
    d2, _X, _i = G.naechster_punkt(Pi[inn], Tri[:, 0], Tri[:, 1], Tri[:, 2])
    return float(np.sqrt(d2.min(1)).max())


def nachbarn(form, b):
    n = {b, form['knochen'][b]['eltern']}
    for q in range(len(form['knochen'])):
        if form['knochen'][q]['eltern'] == b:
            n.add(q)
    return n


_ST = {}


def _init(emd):
    form = FE.lese_emd(emd); mo = FE.modell(emd, TIM0); cb, R, T = G.posen(mo); first = {}
    for i, (c, b, k) in enumerate(cb):
        first.setdefault(int(k), (i, c, b))
    V = {t: np.array(form['meshes'][t]['ecken'], np.int64)[:, :3] for t in range(32)}
    IXd = {t: G.dreiecke_idx(form['meshes'][t]) for t in range(32) if len(V[t])}
    _ST.update(form=form, R=R, T=T, first=first, V=V, IX=IXd, NB={b: nachbarn(form, b) for b in range(32)})


def messe_key(k):
    S = _ST; i, c, b = S['first'][k]; R, T = S['R'][i], S['T'][i]
    W = {t: G.welt(R, T, t, S['V'][t]) for t in range(32) if len(S['V'][t])}
    box = {t: (W[t].min(0), W[t].max(0)) for t in W}
    vor, rueck = {}, {}
    for g in ARM:
        lo, hi = box[g]
        for h in W:
            if h in S['NB'][g]:
                continue
            lo2, hi2 = box[h]
            if (lo2 > hi).any() or (hi2 < lo).any():
                continue
            d = tiefe52(W[g], W[h][S['IX'][h]])
            if d > 0:
                vor['%d>%d' % (g, h)] = round(d, 2)
            if h not in ARM:
                d = tiefe52(W[h], W[g][S['IX'][g]])
                if d > 0:
                    rueck['%d>%d' % (h, g)] = round(d, 2)
    links = {}
    for g in KR_L:
        for h in KR_L:
            if h == g or h in S['NB'][g]:
                continue
            d = tiefe52(W[g], W[h][S['IX'][h]])
            if d > 0:
                links['%d>%d' % (g, h)] = round(d, 2)
    boden = {str(t): round(float(W[t][:, 1].max()), 1) for t in (16,) + BODEN}
    return k, dict(clip_bild=[c, b], vor=vor, rueck=rueck, links=links, boden=boden)


def lauf(emd, keys, n=4):
    with Pool(n, initializer=_init, initargs=(emd,)) as p:
        return dict(p.map(messe_key, keys, chunksize=4))


def alle_keys(emd):
    _init(emd)
    return sorted(_ST['first'])


def grenze(refs, k, art, key):
    r = max(rf[str(k)][art].get(key, 0.0) for rf in refs)
    if art == 'vor':
        g, h = (int(x) for x in key.split('>'))
        if g in KR_R and h in KR_R:
            r = max(r, max(S_ * rf[str(k)]['links'].get('%d>%d' % (LV[g], LV[h]), 0.0) for rf in refs))
    return r + 25.0


def bewerte(m, refs):
    faelle = []; boden = []; tief = (-1e9, None)
    for k, e in m.items():
        for art in ('vor', 'rueck'):
            for key, v in e[art].items():
                u = v - grenze(refs, k, art, key)
                if u > 0:
                    faelle.append((round(u, 2), int(k), art, key, v, round(grenze(refs, k, art, key) - 25, 2)) + tuple(e['clip_bild']))
        yb = max(e['boden'][str(t)] for t in BODEN)
        yr = max(max(rf[str(k)]['boden'][str(t)] for t in BODEN) for rf in refs)
        if yb > tief[0]:
            tief = (yb, int(k))
        if yb > max(25.0, yr):
            boden.append((yb, int(k), yr) + tuple(e['clip_bild']))
    faelle.sort(reverse=True); boden.sort(reverse=True)
    return faelle, boden, tief


if __name__ == '__main__':
    a = sys.argv[1:]
    if a[0] == 'ref':
        name, emdp, aus = a[1:4]; emd = open(emdp, 'rb').read(); t0 = time.time()
        m = lauf(emd, alle_keys(emd))
        json.dump(dict(name=name, emd=emdp, sha256=hashlib.sha256(emd).hexdigest(), keys={str(k): v for k, v in sorted(m.items())}), open(aus, 'w'))
        print('ref %s: %d Keys (%.0f s)' % (name, len(m), time.time() - t0))
    else:
        keys_sel = None
        if '--keys' in a:
            i = a.index('--keys'); spec = a[i + 1]; del a[i:i + 2]; keys_sel = set()
            for part in spec.split(','):
                lo, hi = (int(x) for x in part.split('-')); keys_sel |= set(range(lo, hi + 1))
        emdp, aus = a[1:3]; refs = [json.load(open(p))['keys'] for p in a[3:]]
        emd = open(emdp, 'rb').read(); t0 = time.time()
        ks = [k for k in alle_keys(emd) if keys_sel is None or k in keys_sel]
        m = lauf(emd, ks); m = {str(k): v for k, v in sorted(m.items())}
        faelle, boden, tief = bewerte(m, refs)
        json.dump(dict(emd=emdp, sha256=hashlib.sha256(emd).hexdigest(), refs=a[3:], faelle=faelle, boden=boden, tiefster=tief, keys=m), open(aus, 'w'))
        print('Kandidat %s (sha %s): %d Keys, N52 beidseitig %d Faelle ueber max(Bezug) + 25 (vor %d, rueck %d); Boden > 25: %d Keys, tiefster y %.1f Key %s (%.0f s)'
              % (os.path.basename(emdp), hashlib.sha256(emd).hexdigest()[:12], len(m), len(faelle), sum(1 for f in faelle if f[2] == 'vor'),
                 sum(1 for f in faelle if f[2] == 'rueck'), len(boden), tief[0], tief[1], time.time() - t0))
        for f in faelle[:30]:
            print('  +%.1f Key %d %s %s Tiefe %.1f Bezug %.1f (Clip %d Bild %d)' % f)
        for b in boden[:30]:
            print('  Boden y %.1f Key %d (Bezug %.1f) Clip %d Bild %d' % b)
