"""Zahlen-Pruefer V4 r1: Nutzer-Linien unabhaengig nachmessen (Komponenten + Skelett-Enden + Belegung je Zuordnung)."""
import numpy as np, cv2, itertools, json
from PIL import Image
from skimage.morphology import skeletonize
V80 = r'C:/workspace/git/reAi_v2/Version_80%/'
for b in (1, 2):
    a = np.asarray(Image.open(V80 + 'Beschreibung%d.png' % b).convert('RGB')).astype(int)
    v = np.asarray(Image.open(V80 + 'Beschreibung%d_Vorlage.png' % b).convert('RGB')).astype(int)
    print('Bild', b, a.shape, v.shape, 'links gleich ausser Linien:', int((a[:, :640] != v).any(2).sum()))
    m = (np.abs(a - [237, 28, 36]).sum(2) <= 30)
    # Texte: nicht-weisse, nicht-rote Pixel rechts von 640
    n, lab, st, cen = cv2.connectedComponentsWithStats(m.astype(np.uint8), connectivity=8)
    comps = [i for i in range(1, n) if st[i, 4] > 30]
    sk = skeletonize(m)
    nb = cv2.filter2D(sk.astype(np.uint8), -1, np.ones((3, 3), np.float32)) - sk
    ends = np.argwhere(sk & (nb == 1))
    print(' Komponenten', len(comps), [tuple(st[i]) for i in comps])
    E = [(int(x), int(y), int(lab[y, x])) for y, x in ends]
    links = sorted([e for e in E if e[0] < 640], key=lambda e: e[2]); rechts = sorted([e for e in E if e[0] >= 640], key=lambda e: e[1])
    print(' Enden links (Bild):', links); print(' Enden rechts (Rand):', rechts)
    # Belegung je Start (rechts) x Ende (links) auf gerader Strecke
    def bel(p, q):
        L = max(1, int(np.hypot(q[0] - p[0], q[1] - p[1]) * 2))
        t = np.linspace(0, 1, L)
        xs = np.round(p[0] + t * (q[0] - p[0])).astype(int); ys = np.round(p[1] + t * (q[1] - p[1])).astype(int)
        mm = cv2.dilate(m.astype(np.uint8), np.ones((3, 3), np.uint8))
        return float(mm[ys, xs].mean())
    for s in rechts:
        print('  Start', s[:2], ' '.join('%s:%.3f%s' % (e[:2], bel(s, e), '*' if e[2] == s[2] else '') for e in links))
    # Text-Zeilenbloecke rechts: dunkle Pixel (nicht weiss, nicht rot) x>=650
    txt = (a[:, 645:].sum(2) < 300) & ~m[:, 645:]
    rows = np.nonzero(txt.any(1))[0]
    bl = []; 
    for r in rows:
        if bl and r - bl[-1][1] <= 3: bl[-1][1] = r
        else: bl.append([r, r])
    print(' Textzeilen y:', bl)
