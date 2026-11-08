"""Pruefung V4 r5 Sicht: Koernung (Salz-Pfeffer) der Koerperoberflaeche. Hochpass = L* - Gauss(L*, sigma 1,5) im Schirmbild;
Kennzahlen: Std des Hochpasses, Anteil |Hochpass| > 8 (Einzelpixel, die um > 8 L* aus ihrer Umgebung springen), Anteil heller
Einzelpixel (Hochpass > 8 und L* > 45). Spiel: Boxen auf dem Rumpf/Buckel (von Hand, in beiden Laeufen Figur), Buehne: Ids 7-13."""
import sys, json, numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4')
import v4_gemein as G
R = sys.argv[1]; out = {}
def S(p): return cv2.cvtColor(cv2.imread(p), cv2.COLOR_BGR2RGB).astype(float)[:480]
def kenn(img, m):
    L = G.lab(img)[..., 0].astype(np.float32); hp = L - cv2.GaussianBlur(L, (0, 0), 1.5); v = hp[m]; l = L[m]
    return dict(n=int(m.sum()), hp_std=round(float(v.std()), 2), anteil_gt8=round(float((np.abs(v) > 8).mean()), 3),
                hell_einzel=round(float(((v > 8) & (l > 45)).mean()), 4), L_p50_p95=[round(float(x), 1) for x in np.percentile(l, [50, 95])])
BOX = {85: [(300, 230, 440, 330)], 90: [(300, 230, 440, 330)], 100: [(330, 200, 470, 290)], 118: [(300, 180, 460, 260)], 135: [(300, 170, 480, 250)]}
for h, bs in BOX.items():
    for i, (x0, y0, x1, y1) in enumerate(bs):
        m = np.zeros((480, 640), bool); m[y0:y1, x0:x1] = True; d = {}
        for k in ('V4r4', 'V4r5'): d[k] = kenn(S(R + '/spiel_%s_h%d.png' % (k, h)), m)
        try: d['video'] = kenn(S(R + '/video_h%d.png' % h if h != 90 and h != 135 else (R + '/video_h85.png' if h == 90 else R + '/vorlage1_entbl.png')), m)
        except Exception as e: d['video'] = str(e)
        out['spiel_h%d' % h] = d
for p in ('135', '85'):
    for nm in ('REF2', 'V4r4', 'V4'):
        ids = np.load(R + '/%s_%s_ids.npy' % (nm, p))[:480]; m = cv2.erode(np.isin(ids, range(7, 14)).astype(np.uint8), np.ones((5, 5), np.uint8)).astype(bool)
        out['buehne_%s_%s' % (p, nm)] = kenn(S(R + '/%s_%s.png' % (nm, p)), m)
        out['buehne_neutral_%s_%s' % (p, nm)] = kenn(S(R + '/%s_%s_neutral.png' % (nm, p)), m)
json.dump(out, open(sys.argv[2], 'w'), indent=1); print(json.dumps(out, indent=1))
