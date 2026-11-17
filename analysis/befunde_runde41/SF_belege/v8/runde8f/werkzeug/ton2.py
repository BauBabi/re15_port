"""ton2.py - Tonunterschied Mitte gegen Rand quer zum Schwanz (Delta E 1976, Lab-Mittel), Querprofile entlang der Mittellinie.
Port (960x720): Mittellinie = Gelenke aus schwanz.log (320er x 3), Glieder mit Bildlaenge >= 15 px; je Glied Profile bei t 0,2..0,8 (Schritt 0,1)
  senkrecht zur Gliedrichtung. Halbbreite w+/w- = Abstand bis zum Rand der Schwanzmaske (Lauf normal != ohne Schwanz) je Seite (beide >= 4).
  Mitte = Versatz |s| <= 0,2 w, Rand+ = 0,6 w+ .. 0,9 w+, Rand- ebenso; Seite + = links der Gliedrichtung (Wurzel -> Spitze) im Bild.
  Je Bild: dE(Mitte, Rand+), dE(Mitte, Rand-), dazu dEab ohne L* (Schattierung).
Video 640x480 (nur Richter, Original n(1349+h)): Mittellinie = bogen_v8.json Ablesung austritt -> links -> knolle (Bogenmitte), nur die zweite
  Haelfte austritt->links und links->knolle (weg vom Koerper); feste Halbbreite W_VID = Median der Port-Halbbreite im Bogen (auf 640 umgerechnet).
ton2.py port <basis_n> <basis_s> F0 F1 | ton2.py video h0 h1 W"""
import sys, json, re, numpy as np
from skimage.color import rgb2lab
from ueber import ppm
SP = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad'
def de(a, b, ab=False):
    d = (a - b)[1:] if ab else (a - b)
    return float(np.sqrt((d ** 2).sum()))
import os
LMIN, WMIN = int(os.environ.get("LMIN", 15)), int(os.environ.get("WMIN", 4))
def proben(lab, T, P, Q, w_fest=None):
    d = Q - P; L = np.hypot(*d)
    if L < LMIN: return None
    t_ = d / L; n = np.array([-t_[1], t_[0]])
    H, W = lab.shape[:2]
    out = {'m': [], 'p': [], 'n': [], 'w': []}
    for t in np.arange(0.2, 0.81, 0.1):
        c = P + d * t
        ws = []
        for sg in (1, -1):
            if w_fest: ws.append(w_fest); continue
            k = 0
            while k < 60:
                x, y = int(round(c[0] + sg * n[0] * (k + 1))), int(round(c[1] + sg * n[1] * (k + 1)))
                if not (0 <= x < W and 0 <= y < H) or not T[y, x]: break
                k += 1
            ws.append(k)
        if min(ws) < WMIN: continue
        if T is not None:
            x0, y0 = int(round(c[0])), int(round(c[1]))
            if not (0 <= x0 < W and 0 <= y0 < H and T[y0, x0]): continue
        out['w'].append((ws[0] + ws[1]) / 2)
        for sg, wk, key in ((1, ws[0], 'p'), (-1, ws[1], 'n')):
            for s in np.linspace(0.6 * wk, 0.9 * wk, 4):
                x, y = int(round(c[0] + sg * n[0] * s)), int(round(c[1] + sg * n[1] * s))
                if 0 <= x < W and 0 <= y < H: out[key].append(lab[y, x])
        wm = min(ws)
        for s in np.linspace(-0.2 * wm, 0.2 * wm, 5):
            x, y = int(round(c[0] + n[0] * s)), int(round(c[1] + n[1] * s))
            if 0 <= x < W and 0 <= y < H: out['m'].append(lab[y, x])
    return out
def werte(acc):
    if min(len(acc['m']), len(acc['p']), len(acc['n'])) < 10: return None
    m, p, q = (np.mean(acc[k], 0) for k in 'mpn')
    return dict(dE_p=round(de(m, p), 1), dE_n=round(de(m, q), 1), dE=round(max(de(m, p), de(m, q)), 1),
                dEab=round(max(de(m, p, 1), de(m, q, 1)), 1), mitte=[round(float(x), 1) for x in m], rand_p=[round(float(x), 1) for x in p],
                rand_n=[round(float(x), 1) for x in q], w=round(float(np.median(acc['w'])), 1), n=len(acc['m']))
if sys.argv[1] == 'port':
    bn, bs, F0, F1 = sys.argv[2], sys.argv[3], int(sys.argv[4]), int(sys.argv[5]); SK = float(sys.argv[6]) if len(sys.argv) > 6 else 3.0
    log = {}
    for z in open(bn + '/schwanz.log'):
        m = re.match(r'\[schwanz-bild\] F(\d+) verdeckt (\d+)(.*)', z)
        if m: log[int(m.group(1))] = [np.array((float(x) * SK, float(y) * SK)) for j, x, y, dd in re.findall(r' (\d)\(([-\d.]+),([-\d.]+),(-?\d+)\)', m.group(3))]
    out = {}
    for F in range(F0, F1 + 1):
        n, s = ppm('%s/fd/f%06d.ppm' % (bn, F)), ppm('%s/fd/f%06d.ppm' % (bs, F))
        T = (n != s).any(2); lab = rgb2lab(n.astype(float) / 255.0)
        acc = {'m': [], 'p': [], 'n': [], 'w': []}
        J = log.get(F, [])
        for a, b in zip(J[:-1], J[1:]):
            r = proben(lab, T, a, b)
            if r:
                for k in acc: acc[k] += r[k]
        out[F] = werte(acc)
    json.dump(out, open(bn + '_ton2.json', 'w'))
    for F, e in out.items(): print('F%d h%d' % (F, F - 243), e)
    v = [e['dE'] for e in out.values() if e]; va = [e['dEab'] for e in out.values() if e]; w = [e['w'] for e in out.values() if e]
    print('ZUSAMMEN dE Mittel %.1f Median %.1f max %.1f | dEab Mittel %.1f max %.1f | Halbbreite Median %.1f (n %d)' % (
        np.mean(v), np.median(v), np.max(v), np.mean(va), np.max(va), np.median(w), len(v)))
else:
    import cv2
    B = json.load(open('C:/workspace/git/reAi_v2/.claude/worktrees/r41_gruenblut/analysis/befunde_runde41/SF_belege/v8/bewegung/bogen_v8.json'))['je_h']
    h0, h1, W = int(sys.argv[2]), int(sys.argv[3]), float(sys.argv[4]); out = {}
    for h in range(h0, h1 + 1):
        e = B.get(str(h))
        if not e or 'links' not in e['ablesung'] or 'knolle' not in e['ablesung']: continue
        a = e['ablesung']; A, Lk, K = np.array(e['austritt'], float), np.array(a['links'], float), np.array(a['knolle'], float)
        im = cv2.cvtColor(cv2.imread(SP + '/gruenblut/video/n%04d.png' % (1349 + h)), cv2.COLOR_BGR2RGB)
        lab = rgb2lab(im.astype(float) / 255.0)
        acc = {'m': [], 'p': [], 'n': [], 'w': []}
        for P, Q in (((A + Lk) / 2, Lk), (Lk, K)):
            r = proben(lab, None, P, Q, W)
            if r:
                for k in acc: acc[k] += r[k]
        out[h] = werte(acc)
    json.dump(out, open('video_ton2.json', 'w'))
    for h, e in out.items(): print('h%d' % h, e)
    v = [e['dE'] for e in out.values() if e]; va = [e['dEab'] for e in out.values() if e]
    print('ZUSAMMEN dE Mittel %.1f Median %.1f max %.1f | dEab Mittel %.1f max %.1f (n %d)' % (np.mean(v), np.median(v), np.max(v), np.mean(va), np.max(va), len(v)))
