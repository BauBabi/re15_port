"""Pruefung V4 r3 Sicht: rote Kopf-Oberseite im SPIELBILD h85/h90/h100 (Figur vorgebeugt, Kopfoberseite sichtbar). Videoraum des
Spielbilds gegen das Video (roh) - Anzahl und Farbe der kraeftig roten Pixel (a* > 15 / > 22) im Oberkoerper-Fenster."""
import sys, json, numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4')
import v4_gemein as G
R = sys.argv[1]
def S(p): return cv2.cvtColor(cv2.imread(p), cv2.COLOR_BGR2RGB).astype(float)[:480]
FEN = {85: (300, 150, 480, 380), 90: (300, 150, 480, 380), 100: (300, 150, 480, 330)}   # Oberkoerper-Fenster (Spiel und Video, von Hand)
out = {}
def st(img, b):
    x0, y0, x1, y1 = b; w = img[y0:y1, x0:x1].reshape(-1, 3); lab = G.lab(w); r = {}
    for t in (15, 22):
        m = lab[:, 1] > t; r['a>%d' % t] = dict(n=int(m.sum()), lab=G.r1(lab[m].mean(0)) if m.any() else None)
    r['a_p99'] = round(float(np.percentile(lab[:, 1], 99)), 1); return r
for h in (85, 90, 100):
    d = {}
    try: d['video'] = st(S(R + '/video_h%d.png' % h), FEN[h])
    except Exception: d['video'] = None
    for k in ('V4r3', 'V4r2'): d[k + '_videoraum'] = st(G.videoraum(S(R + '/spiel_%s_h%d.png' % (k, h))), FEN[h])
    out['h%d' % h] = d
json.dump(out, open(sys.argv[2], 'w'), indent=1); print(json.dumps(out, indent=1))
