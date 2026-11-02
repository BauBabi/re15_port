"""farbtest.py: Farbmuster des Oberkoerpers, Video gegen Modelle (D0 Disc, M1 Mod) in derselben Spielkamera.
Kennzahl Q = mittlere Helligkeit der roetesten 20 % / mittlere Helligkeit der uebrigen Pixel.
D0 (helle Haut, dunkelrote Adern) -> Q < 1 erwartet; M1 (dunkelgraue Haut, leuchtend rot) -> Q >= 1 erwartet.
Q haengt nicht von einer gleichmaessigen Lichtstaerke ab (Verhaeltnis). Blaue Pixel (Jeans) fallen raus.
Aufruf: python farbtest.py   (Faelle unten fest eingetragen). Nur Analyse."""
import sys, json
exec(open(__file__.replace('farbtest.py', 'buehne.py'), encoding='utf-8').read())
SP = r"C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad"
BG = SP + '/s40/pruefung/va/bgmod/'


def kennz(px):
    px = px.astype(np.float32)
    s = px.sum(1) + 1e-3
    L = s/3
    rr = px[:, 0]/s
    blau = px[:, 2] > px[:, 0] + 4
    ok = (~blau) & (L > 12)
    L, rr = L[ok], rr[ok]
    if len(L) < 30:
        return None
    k = np.quantile(rr, 0.8)
    rot = rr >= k
    Q = L[rot].mean()/max(L[~rot].mean(), 1e-3)
    c = np.corrcoef(rr, L)[0, 1]
    return Q, c, len(L), rr.mean()


def video_px(cam, bgp, frp, roi, ybelt, thr=26, linie=None, cc=True):
    f_ = cv2.cvtColor(cv2.imread(frp), cv2.COLOR_BGR2RGB).astype(np.float32)
    H, W = f_.shape[:2]
    bg = cv2.cvtColor(cv2.imread(bgp), cv2.COLOR_BGR2RGB)
    bgv = cv2.warpPerspective(bg, cam.B, (W, H), flags=cv2.INTER_LINEAR).astype(np.float32)
    valid = cv2.warpPerspective(np.ones(bg.shape[:2], np.uint8), cam.B, (W, H), flags=cv2.INTER_NEAREST)
    rx, ry, rw, rh = roi
    r = np.zeros((H, W), np.uint8)
    r[ry:ry+rh, rx:rx+rw] = 1
    aus = (valid > 0) & (r == 0)
    g = np.ones(3)
    for k in range(3):
        a, b = bgv[..., k][aus], f_[..., k][aus]
        g[k] = (a*b).sum()/max((a*a).sum(), 1)
        bgv[..., k] *= g[k]
    d = np.abs(cv2.GaussianBlur(f_, (0, 0), 1.2) - cv2.GaussianBlur(bgv, (0, 0), 1.2)).max(axis=2)
    m = ((d > thr) & (r > 0)).astype(np.uint8)
    m = cv2.morphologyEx(m, cv2.MORPH_OPEN, np.ones((3, 3), np.uint8))
    n, lab, st, _ = cv2.connectedComponentsWithStats(m)
    if n > 1 and cc:
        m = (lab == 1 + np.argmax(st[1:, cv2.CC_STAT_AREA])).astype(np.uint8)
    m[ybelt:, :] = 0
    if linie:
        (x0, y0), (x1, y1), bw = linie
        cv2.line(m, (x0, y0), (x1, y1), 0, bw)
    if EROD:
        m = cv2.erode(m, np.ones((EROD, EROD), np.uint8))
    return f_[m > 0]/g, m


def modell_px(name, cam, pos, clip, bild_i, yaw, W=320, H=240, ss=2):
    mo = lade(name)
    first, cnt = mo.clips[clip]
    kf = mo.frames[first + bild_i % cnt] & 0xfff
    V, N, F = mo.geometrie(kf)
    F = [f for f in F if f['bone'] not in (1, 2, 3, 4, 5, 6)]   # ohne Beine
    Vw = dreh_y(V - np.array([V[:, 0].mean(), 0, V[:, 2].mean()]), yaw) + np.array([pos[0], 0, pos[2]])
    Nw = dreh_y(N, yaw)
    col, alp = bild(mo, cam, Vw, Nw, F, W, H, ss=ss, boost=1.0)
    if not DEGRAD:
        return col[alp > 0]*255
    bgv = cv2.warpPerspective(cv2.cvtColor(cv2.imread(BGP), cv2.COLOR_BGR2RGB), cam.B, (W, H)).astype(np.float32)/255
    a = cv2.resize(alp, (W, H), interpolation=cv2.INTER_AREA)
    c = cv2.resize(col, (W, H), interpolation=cv2.INTER_AREA)
    mix = bgv*(1-a[..., None]) + c*a[..., None]
    y = cv2.cvtColor((np.clip(mix, 0, 1)*255).astype(np.uint8), cv2.COLOR_RGB2YCrCb).astype(np.float32)
    for k in (1, 2):
        y[..., k] = cv2.resize(cv2.resize(y[..., k], (W//2, H//2), interpolation=cv2.INTER_AREA), (W, H))
    im = cv2.cvtColor(np.clip(y, 0, 255).astype(np.uint8), cv2.COLOR_YCrCb2RGB)
    im = cv2.GaussianBlur(im, (0, 0), 0.8)
    ok, enc = cv2.imencode('.jpg', im, [cv2.IMWRITE_JPEG_QUALITY, 35])
    im = cv2.imdecode(enc, 1).astype(np.float32)
    m = cv2.erode((a > 0.5).astype(np.uint8), np.ones((2, 2), np.uint8))
    return im[m > 0]


FAELLE = [
    # Name, cam, hjson, bg, Bilder, roi, Guertel-y, Fuss (x,y) im Video
    ('307 Cut 5 hinten (B 13:50-13:56)', '307c5', 'h307c5.json', BG+'ROOM30705.ppm',
     [SP+'/s41video/fb/%05d.jpg' % n for n in (24915, 24945, 24975, 25005, 25035, 25065)], (100, 60, 170, 180), 160, (190, 250)),
    ('50A Cut 12 vorn (CD4 0:55.2-0:56.0)', '50A', 'h50a.json', BG+'ROOM50A12.ppm',
     [SP+'/s41video/fc4/%05d.jpg' % n for n in (1656, 1662, 1668, 1674, 1680)], (110, 50, 150, 175), 130, (195, 212)),
    ('307 Cut 8 vorn (CD3 0:46-0:49, Querstange ausgeblendet)', '307c8', 'h307c8_cd3.json', BG+'ROOM30708.ppm',
     [SP+'/s41video/fc3/%05d.jpg' % n for n in (1380, 1395, 1410, 1425, 1440, 1455, 1470)], (135, 70, 75, 54), 124, (170, 200), ((55, 175), (205, 79), 14)),
]
FAELLE = [f if len(f) == 9 else f + (None,) for f in FAELLE]
erg = {}
DEGRAD = '--roh' not in sys.argv
EROD = int(sys.argv[sys.argv.index('--erod')+1]) if '--erod' in sys.argv else 0
for name, ck, hj, bgp, frs, roi, yb, fuss, lin in FAELLE:
    cam = Cam(ck, hj)
    BGP = bgp
    print('==', name, 'DEGRAD', DEGRAD)
    alle = []
    for fp in frs:
        px, m = video_px(cam, bgp, fp, roi, yb, linie=lin, cc=lin is None)
        if fp.endswith('01410.jpg'): cv2.imwrite('d/maske_307c8.png', m*255)
        kz = kennz(px)
        alle.append(px)
        print('  Video %s: Pixel %d  Q %s' % (fp[-9:], len(px), 'n/a' if kz is None else '%.2f corr %+.2f rr %.3f' % (kz[0], kz[1], kz[3])))
    kz = kennz(np.concatenate(alle))
    print('  Video gesamt: Q %.2f corr %+.2f n %d rr %.3f' % (kz[0], kz[1], kz[2], kz[3]))
    pos = cam.vid2boden(*fuss)
    for mod in ('D0', 'M1'):
        qs = []
        for clip, bi in ((0, 0), (1, 0), (11, 54), (12, 22), (16, 0)):
            for yaw in range(0, 4096, 512):
                try:
                    px = modell_px(mod, cam, pos, clip, bi, yaw)
                except Exception as e:
                    continue
                kz = kennz(px)
                if kz:
                    qs.append(kz)
        q = np.array([k[0] for k in qs])
        c = np.array([k[1] for k in qs])
        rr = np.array([k[3] for k in qs])
        print('  %s: %d Ansichten  Q min %.2f median %.2f max %.2f | corr min %+.2f median %+.2f max %+.2f | rr min %.3f median %.3f max %.3f' % (
            mod, len(q), q.min(), np.median(q), q.max(), c.min(), np.median(c), c.max(), rr.min(), np.median(rr), rr.max()))
