# Leser B (Runde 41, SF V4): Beschriftungslinien in Beschreibung1/2.png messen.
# Linienfarbe exakt (237,28,36) (gemessen: alle 3798/6712 Differenzpixel gegen die Vorlage haben genau diese Farbe).
# Maske: Chebyshev-Abstand zur Linienfarbe <= TOL. Endpunkte = Skelett-Enden; Belegung = Anteil der Abtastpunkte
# (Schritt 0.25 px) auf der Strecke Start->Ende, deren naechstes Pixel (Radius R_BEL) in der Maske liegt.
import sys, json, numpy as np, cv2
from PIL import Image, ImageDraw, ImageFont
from skimage.morphology import skeletonize
from scipy import ndimage as ndi

V = sys.argv[1]; OUT = sys.argv[2]
LC = np.array([237, 28, 36]); TOL = 40; R_BEL = 1
res = {}

def belegung(M, p, q, r=R_BEL):
    p = np.array(p, float); q = np.array(q, float); L = np.linalg.norm(q - p)
    n = max(2, int(L / 0.25)); hit = 0
    H, W = M.shape
    for t in np.linspace(0, 1, n):
        x, y = p + t * (q - p); xi, yi = int(round(x)), int(round(y))
        y0, y1, x0, x1 = max(0, yi - r), min(H, yi + r + 1), max(0, xi - r), min(W, xi + r + 1)
        hit += M[y0:y1, x0:x1].any()
    return hit / n

def seg_intersect(a, b, c, d):
    a, b, c, d = [np.array(v, float) for v in (a, b, c, d)]
    r = b - a; s = d - c; den = r[0] * s[1] - r[1] * s[0]
    if abs(den) < 1e-9: return None
    t = ((c - a)[0] * s[1] - (c - a)[1] * s[0]) / den; u = ((c - a)[0] * r[1] - (c - a)[1] * r[0]) / den
    if 0 < t < 1 and 0 < u < 1: return (a + t * r).round(1).tolist()
    return None

for bi in (1, 2):
    img = np.asarray(Image.open(f'{V}/Beschreibung{bi}.png').convert('RGB')).astype(int)
    tpl = np.asarray(Image.open(f'{V}/Beschreibung{bi}_Vorlage.png').convert('RGB')).astype(int)
    H, W, _ = img.shape
    M = (np.abs(img - LC).max(2) <= TOL)
    # Gegenprobe Bildteil: Maske == Differenz zur Vorlage
    D = np.zeros_like(M); D[:, :640] = np.abs(img[:, :640] - tpl).max(2) > 0
    agree = int((M[:, :640] == D[:, :640]).sum()); tot = 640 * H
    lab, nl = ndi.label(M, structure=np.ones((3, 3)))
    comps = []
    for k in range(1, nl + 1):
        ys, xs = np.nonzero(lab == k)
        comps.append(dict(k=k, n=len(xs), x0=int(xs.min()), x1=int(xs.max()), y0=int(ys.min()), y1=int(ys.max())))
    linek = [c['k'] for c in comps if c['x0'] < 640]          # Linien reichen in den Bildteil
    LM = np.isin(lab, linek)
    # Text: roetliche Pixel im weissen Rand, nicht Linie
    rd = (img[..., 0] > 150) & (img[..., 0] - img[..., 1] > 50); rd[:, :640] = False
    TX = rd & ~ndi.binary_dilation(LM, iterations=2)
    TB = ndi.binary_dilation(TX, structure=np.ones((9, 17)))
    tl, nt = ndi.label(TB)
    texts = []
    for k in range(1, nt + 1):
        ys, xs = np.nonzero((tl == k) & TX)
        if len(xs) < 30: continue
        texts.append(dict(x0=int(xs.min()), x1=int(xs.max()), y0=int(ys.min()), y1=int(ys.max()), n=int(len(xs))))
    texts.sort(key=lambda t: t['y0'])
    # Skelett und Enden
    sk = skeletonize(LM)
    nb = ndi.convolve(sk.astype(int), np.ones((3, 3), int), mode='constant') - 1
    ends = np.argwhere(sk & (nb == 1))
    # Sporn-Filter: vom Ende aus entlang des Skeletts laufen; trifft man in < 8 Schritten eine Verzweigung -> Sporn
    def spur(p):
        seen = {tuple(p)}; cur = tuple(p)
        for step in range(8):
            y, x = cur; nxt = []
            for dy in (-1, 0, 1):
                for dx in (-1, 0, 1):
                    q = (y + dy, x + dx)
                    if q != cur and 0 <= q[0] < H and 0 <= q[1] < W and sk[q] and q not in seen: nxt.append(q)
            if len(nxt) != 1: return len(nxt) > 1
            seen.add(nxt[0]); cur = nxt[0]
        return False
    ends = [tuple(int(v) for v in e) for e in ends if not spur(e)]
    # Enden clustern (<4 px)
    cl = []
    for (y, x) in ends:
        for c in cl:
            if abs(c[0] - y) <= 4 and abs(c[1] - x) <= 4: break
        else: cl.append((y, x))
    S = [(x, y) for (y, x) in cl if x >= 640]   # Starts im weissen Rand
    E = [(x, y) for (y, x) in cl if x < 640]    # Endpunkte im Bild
    # Start -> Text (naechste Textbox)
    def dbox(p, t):
        dx = max(t['x0'] - p[0], 0, p[0] - t['x1']); dy = max(t['y0'] - p[1], 0, p[1] - t['y1'])
        return float(np.hypot(dx, dy))
    res[bi] = dict(groesse=[W, H], maske=f'Chebyshev |rgb-(237,28,36)|<=%d' % TOL, maske_vs_differenz_bildteil=f'{agree}/{tot}',
                   komponenten=comps, linienkomponenten=linek, texte=texts, starts=S, enden=E)
    print(f'Bild {bi}: {W}x{H}, Maske==Differenz im Bildteil {agree}/{tot}; Komponenten {[(c["k"],c["n"],c["x0"],c["x1"],c["y0"],c["y1"]) for c in comps]}')
    print(' Texte', [(t['x0'], t['y0'], t['x1'], t['y1']) for t in texts])
    print(' Starts (weiss)', S); print(' Enden (Bild)', E)
    np.save(f'{OUT}/B_linienmaske_{bi}.npy', LM)
    Image.fromarray((LM * 255).astype(np.uint8)).save(f'{OUT}/B_linienmaske_{bi}.png')
    Image.fromarray((sk * 255).astype(np.uint8)).save(f'{OUT}/B_skelett_{bi}.png')
json.dump(res, open(f'{OUT}/B_linien_roh.json', 'w'), indent=1)
