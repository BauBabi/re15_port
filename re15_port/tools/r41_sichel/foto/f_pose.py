"""SF Messblatt Fotovorlage, Schritt 2: Pose + Gier + Lage der Variante V5r4 (sfv4/emd/EM30_V4.EMD), die die Foto-Trimap
(f_maske.py) am besten deckt. Kamera = Spielkamera 50A0 Cut 12 (b2.raum).

IoU = |M & F| / |(M | F) & ~U|  (F = Figur sicher, U = unbekannt; Modellpixel in U zaehlen nicht).
Gesamtwert = 0,5 IoU + 0,5 Mittel der Ankerwerte (anker(): Kopf und Klauenhand = max(0, 1 - Schwerpunktabstand/40 px),
Schwanz = IoU), weil der Umriss allein vorn/hinten
nicht trennt.
Grob: alle Keyframes aller 21 Clips (je Keyframe-Index einmal), Gier 0..355 Grad in 5 Grad, Lage der Startpose; je Kandidat
beste 2D-Verschiebung (+-32 px, Schritt 4 px) auf 1/2-Raster als Vorauswahl. Fein: die besten 25 Kandidaten mit
Bergsteigen ueber Gier (64/32/16/8/4/2/1 Einheiten), Bild +-1, x/z (Welt, 512..8). Startlage = posen_v4 ("135" bzw. "85").
Aufruf: python f_pose.py <foto 1|2> [prozesse]
"""
import os, sys, json, time, numpy as np, cv2
from multiprocessing import Pool
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'v4'))
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import v4_gemein as G, b2
import birkin_buehne as BB

S = os.path.join(G.SFV4, 'foto')
_mo = None; _T = None; B_ = None
SOHLE_AN = os.environ.get('SF_SOHLE', '1') == '1'   # Lauf 2 (13:50): Sohlen-Anker an; Lauf 1 (13:09) ohne


def _init(b):
    global _mo, _T, B_
    B_ = b
    _mo = b2.modell(G.EMD_V4)
    _T = np.load(os.path.join(S, 'trimap_%d.npy' % b))


def umriss_ids(mo, pose):
    """Wie b2.umriss_schnell, aber mit Knochen je Pixel (Maler-Reihenfolge wie b2.rastern_ids, ohne Texturtransparenz)."""
    r, kam, M, li = b2.raum()
    kf = BB.kf_von_clip(mo, pose['clip'], pose['bild'])
    tris = b2.dreiecke_ids(mo, kf, (pose['x'], pose.get('y', 0), pose['z']), int(pose['gier']), kam, None)
    ids = np.full((480 + G.UNTEN, 640), -1, np.int16)
    for _t, p, _uv, _z, _f, bn in sorted(tris, key=lambda t: -t[0]):
        x, y = BB.abbilden(np.array([q[0] + BB.PHASE for q in p], float), np.array([q[1] + BB.PHASE for q in p], float), M)
        cv2.fillConvexPoly(ids, np.round(np.stack([x, y], 1) * 4).astype(np.int32), int(bn), lineType=cv2.LINE_8, shift=2)
    return ids


# Anker je Foto (Teil-Lage, damit die Silhouette vorn/hinten nicht verwechselt): Kopf = Ellipse in der Kopfbox des
# Messblatts V4 (Abschnitt 2/3), Foto 1 Klauenhand (Knochen 12) = rote Hand (B)-rechts, Foto 2 Schwanz (16-22) = Schwanzmaske.
def anker(b):
    A = {}
    k = np.zeros((480, 640), np.uint8)
    x0, y0, x1, y1 = G.KB_ORIG[b]
    cv2.ellipse(k, ((x0 + x1) // 2, (y0 + y1) // 2), ((x1 - x0) // 2, (y1 - y0) // 2), 0, 0, 360, 1, -1)
    A['kopf'] = ((9,), k > 0)
    if SOHLE_AN:
        A['sohle'] = SOHLE[b]
    if b == 1:
        h = np.zeros((480, 640), np.uint8); cv2.rectangle(h, (476, 332), (510, 374), 1, -1)
        A['klauenhand'] = ((12,), h > 0)
    else:
        S_ = np.load(os.path.join(G.SFV4, 'schwanz_maske.npy')) | np.load(os.path.join(G.SFV4, 'B_L3_masken.npz'))['Mc']
        S_[:, :270] = False; S_[346:] = False
        A['schwanz'] = (G.SCHWANZ, S_)
    return A


SOHLE = {1: 448, 2: 436}     # Foto 2: Messblatt V4 Abschnitt 1 (435-437 aus n1439..1445). Foto 1: 448 statt 442, weil die Figur
# naeher kommt (Fuesse n1470 ~437, n1475 ~442, n1479 ~445, ab n1482 unter der Letterbox y 452; fuesse1.png); Lauf mit 442 = pose_1_sohle442.json


def bewerte(ids_voll, T, A):
    ids = ids_voll[:480]
    m = ids >= 0
    s = iou(m, T)
    teile = {}
    for k, (kn, ziel) in ((k, v) for k, v in A.items() if k != 'sohle'):
        mk = np.isin(ids, kn)
        if k == 'schwanz':
            i = (mk & ziel).sum(); u = (mk | ziel).sum()
            teile[k] = i / u if u else 0.0
        else:                       # Schwerpunktabstand d (px): Wert max(0, 1 - d / 40)
            ys, xs = np.nonzero(mk); yz, xz = np.nonzero(ziel)
            d = np.hypot(xs.mean() - xz.mean(), ys.mean() - yz.mean()) if len(xs) else 999
            teile[k] = max(0.0, 1 - d / 40.0)
    if 'sohle' in A:             # Sohle (unterste Zeile ohne Schwanz 16-22) gegen Messblatt-Sohle: max(0, 1 - |d| / 120)
        rows = np.nonzero(((ids_voll >= 0) & ~np.isin(ids_voll, G.SCHWANZ)).any(1))[0]
        d = abs(rows.max() - A['sohle']) if len(rows) else 999
        teile['sohle'] = max(0.0, 1 - d / 120.0)
    return s, teile, 0.5 * s + 0.5 * float(np.mean(list(teile.values())))


def iou(m, T):
    F = T == 1; K = T != 2
    i = (m & F).sum(); u = ((m | F) & K).sum()
    return i / u if u else 0.0


def iou_versatz(m, T, r=32, st=4):
    """beste Verschiebung (dx,dy) des Modellumrisses, auf halbem Raster."""
    m2 = m[::2, ::2]; F = (T == 1)[::2, ::2]; K = (T != 2)[::2, ::2]
    best = (-1, 0, 0)
    H, W = m2.shape
    for dy in range(-r // 2, r // 2 + 1, st // 2):
        for dx in range(-r // 2, r // 2 + 1, st // 2):
            s = np.roll(np.roll(m2, dy, 0), dx, 1)
            i = (s & F).sum(); u = ((s | F) & K).sum()
            v = i / u if u else 0
            if v > best[0]: best = (v, 2 * dx, 2 * dy)
    return best


_A = None
def grob(arg):
    global _A
    if _A is None: _A = anker(B_)
    clip, bild, gier, x, z = arg
    p = dict(clip=clip, bild=bild, gier=gier, x=x, z=z)
    ids = umriss_ids(_mo, p)
    v0, teile, ges = bewerte(ids, _T, _A)
    v, dx, dy = iou_versatz(ids[:480] >= 0, _T)
    return (ges, v0, dx, dy, clip, bild, gier, v)


def fein(arg):
    p0 = arg
    mo, T = _mo, _T
    A = anker(B_)
    f = lambda q: bewerte(umriss_ids(mo, q), T, A)[2]
    p = dict(p0); s = f(p)
    for schritt in (64, 32, 16, 8, 4, 2, 1):
        besser = True
        while besser:
            besser = False
            zs = [('gier', schritt), ('gier', -schritt), ('x', 8 * schritt), ('x', -8 * schritt),
                  ('z', 8 * schritt), ('z', -8 * schritt)]
            if schritt >= 16: zs += [('bild', 1), ('bild', -1)]
            for k, d in zs:
                q = dict(p); q[k] = q[k] + d
                if k == 'bild': q['bild'] %= mo.clips[q['clip']][1]
                if k == 'gier': q['gier'] %= 4096
                t = f(q)
                if t > s + 1e-5: s, p, besser = t, q, True
    return s, p


if __name__ == '__main__':
    b = int(sys.argv[1]); npr = int(sys.argv[2]) if len(sys.argv) > 2 else 12
    P = G.posen(); start = P['135' if b == 1 else '85']
    _init(b)
    mo = _mo
    alt = iou(b2.umriss_schnell(mo, start), _T)
    # Keyframes eindeutig
    seen = {}
    for c, (f0, n) in enumerate(mo.clips):
        for i in range(n):
            kf = BB.kf_von_clip(mo, c, i)
            seen.setdefault(kf, (c, i))
    kfs = sorted(seen.values())
    giere = [int(round(g * 4096 / 360)) % 4096 for g in range(0, 360, 5)]
    args = [(c, i, g, start['x'], start['z']) for (c, i) in kfs for g in giere]
    print('Keyframes', len(kfs), 'Kandidaten', len(args), 'IoU alt', round(alt, 4), flush=True)
    t0 = time.time()
    if len(sys.argv) > 3 and sys.argv[3] == 'nurfein':
        # Grobliste aus Lauf 1 (ohne Sohlen-Anker): beste 30 nach Gesamtwert + beste 10 nach Silhouetten-IoU
        Rg = np.load(os.path.join(S, 'grob_%d.npy' % b))
        sel = list(range(30)) + [int(i) for i in np.argsort(-Rg[:, 1])[:10]]
        R = [tuple(Rg[i]) for i in dict.fromkeys(sel)]
    else:
        with Pool(npr, _init, (b,)) as pool:
            R = pool.map(grob, args, chunksize=64)
        print('grob fertig', round(time.time() - t0), 's', flush=True)
        R.sort(key=lambda r: -r[0])
        np.save(os.path.join(S, 'grob_%d.npy' % b), np.array(R, float))
    # Fein: Startliste (verschiedene Keyframe/Gier-Paare), Startlage = posen_v4 -> Bergsteigen
    top = R[:40]
    starts = [dict(clip=int(r[4]), bild=int(r[5]), gier=int(r[6]), x=start['x'], z=start['z']) for r in top]
    with Pool(min(npr, len(starts)), _init, (b,)) as pool:
        F = pool.map(fein, starts)
    F.sort(key=lambda t: -t[0])
    A = anker(b)
    F = [(s_, p, bewerte(umriss_ids(mo, p), _T, A)) for s_, p in F]
    ba = bewerte(umriss_ids(mo, start), _T, A)
    out = dict(foto=b, iou_alt=round(alt, 4), pose_alt=start,
               grob_top=[dict(gesamt=round(r[0], 4), iou_versatz=round(r[7], 4), iou=round(r[1], 4), dx=r[2], dy=r[3], clip=int(r[4]),
                              bild=int(r[5]), gier=int(r[6]), grad=round(r[6] * 360 / 4096, 1)) for r in R[:40]],
               alt=dict(iou=round(ba[0], 4), teile={k: round(v, 4) for k, v in ba[1].items()}, gesamt=round(ba[2], 4)),
               fein=[dict(gesamt=round(s_, 4), iou=round(bw[0], 4), teile={k: round(v, 4) for k, v in bw[1].items()},
                          **p, grad=round(p['gier'] * 360 / 4096, 1)) for s_, p, bw in F])
    out['sohlen_anker'] = SOHLE_AN
    json.dump(out, open(os.path.join(S, 'pose_%d.json' % b), 'w'), indent=1)
    print(json.dumps(out['fein'][:8]))
