"""Zahlen-Pruefer V4 r4: eigene Messung der neuen Riegelzeilen TQ / TK / FO / KS (Definitionen Sicht-Pruefung V4 r3 1.1/1.3/1.5,
eigener Code: L* skimage (pz4_mess.lab), Videoraum pz4_mess.vraum, Sobel eigener Aufruf; Spielbilder = eigene FRAMEDUMPs sfv4/pz4/lauf,
registriert ueber nb2neu/c6.port wie die Lichtgegenprobe). Zusaetzlich robuste Gegenformen: Plateau mit Schwelle 0,5/1,0, Kruemmung
als Sehnenabstand, KS mit L*-Sobel und mit 9 verschobenen Kreisen, FO-Drittel."""
import os, sys, re, json
import numpy as np, cv2
from PIL import Image
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
from pz4_mess import vraum, lab, de, orig, SP
S = SP.rsplit('pz4', 1)[0]
os.environ['NB6_RUNS'] = S + 'pz4'
sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'nb2neu'))
import c6
import t2_mass_v4 as T2M
out = {}


def maske(f):
    d = np.load(f)
    return np.unpackbits(d['m'])[:int(np.prod(d['shape']))].reshape(tuple(d['shape'])).astype(bool)


def quer(L, m, x, ymax=262):
    ys = np.nonzero(m[:ymax, x])[0]
    if len(ys) < 4:
        return None
    p = np.array([L[y, x] for y in range(ys.min(), ys.max() + 1)])
    d = np.abs(np.diff(p))
    return dict(stufe3=round(float(np.abs(p[3:] - p[:-3]).max()), 1), plateau03=round(float((d < 0.3).mean()), 2),
                plateau05=round(float((d < 0.5).mean()), 2), plateau10=round(float((d < 1.0).mean()), 2), n=len(p),
                spanne=round(float(p.max() - p.min()), 1), y=(int(ys.min()), int(ys.max())))


def kontur(m, x0=284, x1=332, ymax=262):
    xs = np.array([x for x in range(x0, x1 + 1) if m[:ymax, x].any()], float)
    ys = np.array([np.nonzero(m[:ymax, int(x)])[0].min() for x in xs], float)
    k = np.polyfit(xs, ys, 1)
    r = ys - np.polyval(k, xs)
    sehne = ys - (ys[0] + (ys[-1] - ys[0]) * (xs - xs[0]) / (xs[-1] - xs[0]))
    kante = {x: (int(np.nonzero(m[:ymax, x])[0].min()) if m[:ymax, x].any() else None) for x in (276, 280, 284, 288, 292)}
    return dict(rms=round(float(np.sqrt((r ** 2).mean())), 2), sehne_max=round(float(np.abs(sehne).max()), 1), kante=kante)


Lo = lab(orig(2))[..., 0]
out['orig_TQ_TK'] = []
for f in T2M.MASKEN:
    m = maske(f)
    out['orig_TQ_TK'].append(dict(maske=os.path.basename(f), quer={x: quer(Lo, m, x) for x in (310, 320, 330, 340)}, kontur=kontur(m)))
for nm in ('V4', 'V4R3'):
    d = np.load(SP + 'r_%s_85.npz' % nm)
    sw = (d['ids'] >= 16) & (d['ids'] <= 22)
    L = lab(vraum(d['img']))[..., 0]
    out['%s_TQ_TK' % nm] = dict(quer={x: quer(L, sw, x) for x in (310, 320, 330, 340)}, kontur=kontur(sw))


def g_lum(V):
    Lm = (np.asarray(V, float) @ np.array([0.299, 0.587, 0.114])).astype(np.float32)
    return np.hypot(cv2.Sobel(Lm, cv2.CV_32F, 1, 0), cv2.Sobel(Lm, cv2.CV_32F, 0, 1))


def g_L(V):
    Lm = lab(V)[..., 0].astype(np.float32)
    return np.hypot(cv2.Sobel(Lm, cv2.CV_32F, 1, 0), cv2.Sobel(Lm, cv2.CV_32F, 0, 1))


O1 = orig(1)
ks = {}
for gn, gf in (('lum', g_lum), ('Lstern', g_L)):
    g = gf(O1)
    w, gl = [], []
    for dx in (-3, 0, 3):
        for dy in (-3, 0, 3):
            sub = g[148 + dy:183 + dy, 376 + dx:409 + dx]
            w.append(float(sub.mean())); gl.append(float((sub < 8).mean()))
    ks['orig_' + gn] = dict(mitte=dict(grad=round(w[4], 1), glatt=round(gl[4], 3)), grad=[round(min(w), 1), round(max(w), 1)],
                            glatt=[round(min(gl), 3), round(max(gl), 3)])
KOPF = {135: ((360, 191), 28), 118: ((358, 222), 26)}
txt = open(S + 'pz4/lauf/debug.log', encoding='latin-1').read()
spiel = {}
for h in (118, 135):
    F = int(re.search(r'\[hangar-takt\] h%d F(\d+) ' % h, txt).group(1))
    im = cv2.cvtColor(c6.port('lauf', h, F=F), cv2.COLOR_BGR2RGB).astype(float)
    spiel[h] = im
    Image.fromarray(np.clip(im, 0, 255).astype(np.uint8)).save(SP + 'spiel_V4r4_h%d.png' % h)
    V = vraum(im)
    for gn, gf in (('lum', g_lum), ('Lstern', g_L)):
        g = gf(V)
        (cx, cy), r = KOPF[h]
        w, gl = [], []
        for dx in (-3, 0, 3):
            for dy in (-3, 0, 3):
                m = np.zeros((480, 640), np.uint8)
                cv2.circle(m, (cx + dx, cy + dy), int(r * 0.6), 1, -1)
                m = m.astype(bool)
                w.append(float(g[m].mean())); gl.append(float((g[m] < 8).mean()))
        ks['h%d_%s' % (h, gn)] = dict(mitte=dict(grad=round(w[4], 1), glatt=round(gl[4], 3)), grad=[round(min(w), 1), round(max(w), 1)],
                                      glatt=[round(min(gl), 3), round(max(gl), 3)])
    p3 = S + 'sicht_r3/spiel_V4r3_h%d.png' % h
    if os.path.exists(p3):
        V3 = vraum(cv2.cvtColor(cv2.imread(p3), cv2.COLOR_BGR2RGB).astype(float)[:480])
        g = g_lum(V3)
        m = np.zeros((480, 640), np.uint8)
        cv2.circle(m, KOPF[h][0], int(KOPF[h][1] * 0.6), 1, -1)
        m = m.astype(bool)
        ks['h%d_V4r3_lum' % h] = dict(grad=round(float(g[m].mean()), 1), glatt=round(float((g[m] < 8).mean()), 3))
out['KS'] = ks
PV = {118: [(318, 190), (335, 170), (370, 160), (410, 162), (440, 175), (465, 195), (470, 230), (455, 255), (330, 255), (315, 225)],
      135: [(297, 175), (320, 150), (360, 140), (410, 138), (450, 150), (490, 165), (505, 195), (500, 235), (470, 250), (330, 250), (300, 230)]}
PS = {118: [(262, 225), (290, 195), (330, 170), (380, 162), (430, 170), (460, 195), (475, 230), (470, 265), (430, 280), (300, 280), (268, 265)],
      135: [(262, 205), (290, 170), (330, 150), (380, 148), (430, 160), (470, 180), (492, 215), (488, 255), (450, 268), (300, 268), (268, 250)]}


def pm(poly):
    m = np.zeros((480, 640), np.uint8)
    cv2.fillPoly(m, [np.array(poly, np.int32)], 1)
    return m.astype(bool)


def dritt(V, m):
    xs = np.nonzero(m.any(0))[0]
    x0, x1 = xs.min(), xs.max()
    xx = np.arange(640)[None, :]
    LAB = lab(V)
    rot, a12 = [], []
    for i in range(3):
        mm = m & (xx >= x0 + i * (x1 - x0) / 3) & (xx < x0 + (i + 1) * (x1 - x0) / 3)
        rot.append(float(((V[..., 0] - V[..., 1])[mm] >= 15).mean())); a12.append(float((LAB[..., 1][mm] > 12).mean()))
    return dict(rot=[round(v, 3) for v in rot], m_a=round(rot[1] / max(1e-3, (rot[0] + rot[2]) / 2), 2),
                schulter_a12=round(max(a12[0], a12[2]), 3), a12=[round(v, 3) for v in a12], a18=int((LAB[..., 1][m] > 18).sum()),
                a18_anteil=round(float((LAB[..., 1][m] > 18).mean()), 3), n=int(m.sum()))


fo = {}
vid = {118: cv2.cvtColor(cv2.imread(S + 'sicht_r3/video_h118.png'), cv2.COLOR_BGR2RGB).astype(float)[:480], 135: O1}
for h in (118, 135):
    fo['video_h%d' % h] = dritt(vid[h], pm(PV[h]))
    fo['spiel_V4r4_h%d' % h] = dritt(vraum(spiel[h]), pm(PS[h]))
    p3 = S + 'sicht_r3/spiel_V4r3_h%d.png' % h
    if os.path.exists(p3):
        fo['spiel_V4r3_h%d' % h] = dritt(vraum(cv2.cvtColor(cv2.imread(p3), cv2.COLOR_BGR2RGB).astype(float)[:480]), pm(PS[h]))
for nm in ('V4', 'V4R3'):
    d = np.load(SP + 'r_%s_135.npz' % nm)
    ids = d['ids']
    top = np.nonzero((ids >= 0).any(1))[0].min()
    yc = int(round(top + 0.37 * 364))
    m = np.isin(ids, (8, 9, 10, 13)) & (np.arange(480)[:, None] <= yc)
    m = cv2.erode(m.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
    fo['buehne_%s' % nm] = dritt(vraum(d['img']), m)
out['FO'] = fo
json.dump(out, open(SP + 'form_ks_fo.json', 'w'), indent=1, default=str)
print(json.dumps(out, indent=1, default=str))
