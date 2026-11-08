"""SF Messblatt Fotovorlage, Schritt 2b (Foto 1): Posensuche MIT fester Sohle.

Grund: In Foto 1 sind Beine/Fuesse kontrastlos bzw. im Schatten (Trimap unbekannt); die freie Suche (f_pose.py) setzt die
Figur darum naeher an die Kamera (Sohle y 513 statt 441-443, H 359 statt 302) und deckt den Oberkoerper mit einem zu grossen
Modell. Hier wird die Lage je Kandidat so bestimmt, dass die Sohle (unterste Zeile ohne Schwanz) auf SOHLE[b] liegt
(Sekantenverfahren entlang der Blickrichtung der Kamera auf dem Boden) und die Querlage die Silhouetten-IoU maximiert
(Schritte 256/128/64/32/16 quer zur Blickrichtung). Bewertung wie f_pose.bewerte (Gesamt = 0,5 IoU + 0,5 Anker-Mittel).
Kandidaten: alle Keyframes x Gier in 10 Grad, dann die besten 30 fein (Gier +-1..32 Einheiten, Bild +-1, Lage neu geloest).
Aufruf: python f_pose_sohle.py <foto> [prozesse]
"""
import os, sys, json, time, numpy as np
from multiprocessing import Pool
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'v4'))
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import v4_gemein as G, b2
import birkin_buehne as BB
import f_pose as FP

S = os.path.join(G.SFV4, 'foto')
_A = None


def _init(b):
    global _A
    FP._init(b)
    _A = FP.anker(b)


def richtungen():
    r, kam, M, li = b2.raum()
    V = kam.V.astype(float) / 4096.0
    f = V[2].copy(); f[1] = 0; f /= np.linalg.norm(f)          # Blickrichtung auf dem Boden (Welt x/z)
    q = np.array([f[2], 0, -f[0]])                              # quer
    return f, q


def sohle(ids):
    rows = np.nonzero(((ids >= 0) & ~np.isin(ids, G.SCHWANZ)).any(1))[0]
    return rows.max() if len(rows) else None


def loese(p, ziel, f, q, quer=True):
    """Lage entlang f so, dass die Sohle auf ziel liegt; dann quer nach IoU."""
    mo, T = FP._mo, FP._T
    def ids_bei(w, u):
        r = dict(p); r['x'] = int(round(p['x'] + w * f[0] + u * q[0])); r['z'] = int(round(p['z'] + w * f[2] + u * q[2]))
        return r, FP.umriss_ids(mo, r)
    w0, w1 = 0.0, 600.0
    r0, i0 = ids_bei(w0, 0); s0 = sohle(i0)
    r1_, i1 = ids_bei(w1, 0); s1 = sohle(i1)
    if s0 is None or s1 is None or s0 == s1:
        return None
    for _ in range(6):
        w2 = w1 + (ziel - s1) * (w1 - w0) / (s1 - s0)
        w2 = float(np.clip(w2, -6000, 6000))
        r2, i2 = ids_bei(w2, 0); s2 = sohle(i2)
        if s2 is None: return None
        w0, s0, w1, s1 = w1, s1, w2, s2
        if abs(s2 - ziel) <= 1: break
    w = w1; u = 0.0
    if not quer:
        return i2, r2
    best = FP.bewerte(i2, T, _A)[2]; br = r2
    for st in (256, 128, 64, 32, 16):
        besser = True
        while besser:
            besser = False
            for d in (st, -st):
                r, i = ids_bei(w, u + d)
                v = FP.bewerte(i, T, _A)[2]
                if v > best + 1e-5: best, br, u, besser = v, r, u + d, True
    return best, br


def grob(arg):
    """Grob: Sohle loesen, Querlage als Bildverschiebung dx (+-40 px, Schritt 2) auf den Ids (Naeherung)."""
    clip, bild, gier, x, z, ziel = arg
    f, q = richtungen()
    r = loese(dict(clip=clip, bild=bild, gier=gier, x=x, z=z), ziel, f, q, quer=False)
    if not r:
        return (-1, None)
    ids, p = r
    best = (-1, 0)
    for dx in range(-40, 41, 2):
        sh = np.roll(ids, dx, 1)
        v = FP.bewerte(sh, FP._T, _A)[2]
        if v > best[0]: best = (v, dx)
    p = dict(p); p['dx_bild'] = best[1]
    return (best[0], p)


def fein(arg):
    p, ziel = arg
    f, q = richtungen()
    mo = FP._mo
    s, bp = loese(p, ziel, f, q) or (-1, p)
    for st in (32, 16, 8, 4, 2, 1):
        besser = True
        while besser:
            besser = False
            zs = [('gier', st), ('gier', -st)] + ([('bild', 1), ('bild', -1)] if st >= 8 else [])
            for k, d in zs:
                c = dict(bp); c[k] = c[k] + d
                if k == 'bild': c['bild'] %= mo.clips[c['clip']][1]
                if k == 'gier': c['gier'] %= 4096
                r = loese(c, ziel, f, q)
                if r and r[0] > s + 1e-5: s, bp, besser = r[0], r[1], True
    return s, bp


if __name__ == '__main__':
    b = int(sys.argv[1]); npr = int(sys.argv[2]) if len(sys.argv) > 2 else 14
    ziel = FP.SOHLE[b]
    # Lauf 2 (14:35): Foto 1 Sohle 448 (n1470 ~435-440, n1475 ~442, n1479 ~445, ab n1482 unter der Letterbox-Kante y 452;
    # Schatten n1470 -> n1485 +10 px tiefer; fuesse1.png) und Gier nur frontal: posen_v4 "135" 1737 +-45 Grad
    # (Foto 1 symmetrisch von vorn, Sichel Bild-links, Klauenhand Bild-rechts)
    if len(sys.argv) > 3: ziel = int(sys.argv[3])
    gb = (int(sys.argv[4]), int(sys.argv[5])) if len(sys.argv) > 5 else None
    _init(b)
    mo = FP._mo
    st = G.posen()['135' if b == 1 else '85']
    seen = {}
    for c, (f0, n) in enumerate(mo.clips):
        for i in range(n):
            seen.setdefault(BB.kf_von_clip(mo, c, i), (c, i))
    kfs = sorted(seen.values())
    giere = [int(round(g * 4096 / 360)) % 4096 for g in range(0, 360, 10)]
    if gb:
        giere = list(range(gb[0], gb[1] + 1, 57))
    args = [(c, i, g, st['x'], st['z'], ziel) for (c, i) in kfs for g in giere]
    t0 = time.time()
    print('Kandidaten', len(args), flush=True)
    with Pool(npr, _init, (b,)) as pool:
        R = pool.map(grob, args, chunksize=32)
    print('grob', round(time.time() - t0), 's', flush=True)
    R = [r for r in R if r[1] is not None]
    R.sort(key=lambda t: -t[0])
    json.dump([dict(gesamt=round(s, 4), **p) for s, p in R[:200]], open(os.path.join(S, 'sohle_grob_%d.json' % b), 'w'))
    with Pool(npr, _init, (b,)) as pool:
        Fn = pool.map(fein, [({k: p[k] for k in ('clip', 'bild', 'gier', 'x', 'z')}, ziel) for s, p in R[:30]])
    Fn.sort(key=lambda t: -t[0])
    A = FP.anker(b)
    out = []
    for s, p in Fn:
        ids = FP.umriss_ids(mo, p)
        bw = FP.bewerte(ids, FP._T, A)
        out.append(dict(gesamt=round(s, 4), iou=round(bw[0], 4), teile={k: round(v, 4) for k, v in bw[1].items()}, sohle=int(sohle(ids)),
                        **{k: int(p[k]) for k in ('clip', 'bild', 'gier', 'x', 'z')}, grad=round(p['gier'] * 360 / 4096, 1)))
    P = json.load(open(os.path.join(S, 'pose_%d.json' % b)))
    P.setdefault('fein_frei', P['fein'])
    P['fein'] = out
    P['verfahren'] = 'f_pose_sohle.py: Sohle fest auf y %d, Querlage nach Gesamtwert, Gier %s' % (ziel, gb or 'alle 10 Grad')
    json.dump(P, open(os.path.join(S, 'pose_%d.json' % b), 'w'), indent=1)
    print(json.dumps(out[:6]))
