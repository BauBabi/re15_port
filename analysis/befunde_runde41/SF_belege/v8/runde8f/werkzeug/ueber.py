"""ueber.py <basis> [F0 F1]: Schwanzpunkte ueber der Video-Austrittszeile je h (Taille: h16-h24 und h58-h72; Beine h26-h34 zur Info).
Austrittszeile = tabelle_v8.json 'austritt' y (Originalvideo 640x480) -> Port y = (vy - 1,3) / 2,079 (Registrierung bf_video_welt), linear je h.
Skaliert auf die Framedump-Hoehe. Ausgabe JSON <basis>_ueber.json."""
import numpy as np, sys, json
T8 = json.load(open('C:/workspace/git/reAi_v2/.claude/worktrees/r41_gruenblut/analysis/befunde_runde41/SF_belege/v8/bewegung/tabelle_v8.json'))
def ppm(p):
    b = open(p, 'rb').read(); parts = b.split(b'\n', 3); w, h = map(int, parts[1].split())
    return np.frombuffer(parts[3], np.uint8)[:w * h * 3].reshape(h, w, 3)
def yaus(h):
    ks = sorted(int(k) for k in T8)
    if h <= 18: h = 18
    lo = max(k for k in ks if k <= h); hi = min([k for k in ks if k >= h] or [lo])
    y0, y1 = T8[str(lo)]['austritt'][1], T8[str(hi)]['austritt'][1]
    vy = y0 if hi == lo else y0 + (y1 - y0) * (h - lo) / (hi - lo)
    return (vy - 1.3) / 2.079
def phase(h):
    if 16 <= h <= 24 or 58 <= h <= 72: return 'taille'
    if 25 <= h <= 35: return 'beine'
    return None
if __name__ == '__main__':
    B = sys.argv[1]; F0 = int(sys.argv[2]) if len(sys.argv) > 2 else 255; F1 = int(sys.argv[3]) if len(sys.argv) > 3 else 350
    out = []
    for F in range(F0, F1 + 1):
        h = F - 243; ph = phase(h)
        if not ph: continue
        try: n, s = ppm('%s_n/fd/f%06d.ppm' % (B, F)), ppm('%s_s/fd/f%06d.ppm' % (B, F))
        except FileNotFoundError: continue
        T = (n != s).any(2); sc = T.shape[0] / 240.0
        ya = yaus(h) * sc
        ys = np.where(T)[0]
        ueb = int((ys < ya).sum()); top = float(ys.min() / sc) if len(ys) else None
        out.append(dict(F=F, h=h, phase=ph, yaus=round(yaus(h), 1), T=int(T.sum()), ueber=ueb, ueber_px320=round(ueb / sc / sc, 1),
                        oben=None if top is None else round(top, 1), hoehe=None if top is None else round(yaus(h) - top, 1)))
    json.dump(out, open(B + '_ueber.json', 'w'))
    for e in out: print('h%(h)3d F%(F)d %(phase)-6s yaus %(yaus)5.1f T %(T)6d ueber %(ueber)5d  oben %(oben)s (%(hoehe)s ueber)' % e)
