# Pruefer V3: (1) Freisteller == KI-Videobild (ffmpeg-Export, eigener) ; (2) Original-Video: Schwanz gegen Haut mit den
# eigenen KI-Masken (P_schwanz.py), halbiert (1280 -> 640), 2 px erodiert, Schwarzwert 17 abgezogen (x 255/238).
import os, sys, json
import numpy as np, cv2
from scipy import ndimage as nd
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from P_farbe import lab, de00, de76, ch, r1

P = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv3/P/'
V = 'C:/workspace/git/reAi_v2/Version_80%/'
ZU = {'birkin11': (1433, (548, 368)), 'birkin12': (1435, (547, 366)), 'birkin13': (1442, (545, 365)),
      'birkin19': (1468, (606, 315)), 'birkin20': (1475, (584, 293))}
erg = {}
for name, (n, (ox, oy)) in ZU.items():
    fr = cv2.imread(V + name + '.png', cv2.IMREAD_UNCHANGED)
    sl = cv2.imread(P + 'slp/n%d.png' % n)
    a = fr[:, :, 3] == 255
    sub = sl[oy:oy + fr.shape[0], ox:ox + fr.shape[1]].astype(float)
    rms = float(np.sqrt(((sub[a] - fr[:, :, :3][a].astype(float)) ** 2).mean()))
    nb = []
    for dn in (-1, 1):
        pass
    erg[name] = dict(n=n, rms_freisteller_video=round(rms, 2))
    if name in ('birkin11', 'birkin12', 'birkin13'):
        ms = np.load(P + 'P_maske_%s.npy' % name); mh = np.load(P + 'P_haut_%s.npy' % name)
        og = cv2.imread(P + 'orig/n%d.png' % n)[:, :, ::-1].astype(float)
        og = np.clip((og - 17) * 255.0 / 238.0, 0, 255)
        def halb(m):
            big = np.zeros((960, 1280), bool); big[oy:oy + m.shape[0], ox:ox + m.shape[1]] = m
            sm = cv2.resize(big.astype(np.uint8), (640, 480), interpolation=cv2.INTER_AREA) > 0
            return nd.binary_erosion(sm, iterations=2)
        s2, h2 = halb(ms), halb(mh)
        A = lab(og)
        ls, lh = A[s2].mean(0), A[h2].mean(0)
        erg[name].update(orig_lab_schwanz=r1(ls), orig_lab_haut=r1(lh), orig_de00=round(de00(ls, lh), 1), orig_de76=round(de76(ls, lh), 1),
                         orig_L_verh=round(float(ls[0] / lh[0]), 3), orig_C_verh=round(ch(ls)[0] / ch(lh)[0], 2),
                         orig_h=[round(ch(ls)[1], 1), round(ch(lh)[1], 1)], n_px=[int(s2.sum()), int(h2.sum())])
        # Hintergrund neben dem Schwanz (Ring 3-8 px ausserhalb der halbierten Figur, nur links des Schwanzes)
        big = np.zeros((960, 1280), bool); big[oy:oy + fr.shape[0], ox:ox + fr.shape[1]] = fr[:, :, 3] > 128
        fgs = cv2.resize(big.astype(np.uint8), (640, 480), interpolation=cv2.INTER_AREA) > 0
        ring = nd.binary_dilation(s2, iterations=8) & ~nd.binary_dilation(fgs, iterations=3)
        erg[name]['orig_lab_hintergrund_neben'] = r1(A[ring].mean(0))
        # KI mit derselben (halbierten) Maske, zum Vergleich
        ki = cv2.resize(sl, (640, 480), interpolation=cv2.INTER_AREA)[:, :, ::-1].astype(float)
        K = lab(ki); ks, kh = K[s2].mean(0), K[h2].mean(0)
        erg[name].update(ki_halb_de00=round(de00(ks, kh), 1), ki_halb_L_verh=round(float(ks[0] / kh[0]), 3),
                         ki_gegen_orig_schwanz_de00=round(de00(ks, ls), 1))
    print(name, erg[name])
json.dump(erg, open(P + 'P_video.json', 'w'), indent=1)
