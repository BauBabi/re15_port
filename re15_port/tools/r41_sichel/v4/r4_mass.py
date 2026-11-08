"""SF V4 Runde 4 (Bauer): neue Masse fuer die Befunde der Pruefung V4 r3 - Bauer und Pruefer benutzen dieselben Funktionen.

Quellen der Verfahren (jeweils die Definition des Pruefers, hier nachgebaut, damit Riegel und Pruefung dasselbe messen):
  t2_lage      Zahlen-Pruefer V4 r3 4.1: Lage der Biegung = Mitte des Fensters mit der groessten Drehung nach t2_mass_v4 (Entscheid O2),
               relativ zum T1-Beginn (riegel_v4.schwanz_mass_h 'beginn'), in H. Am Original (3 Leser-Masken, Beginn = Nutzerpunkt
               (357,243), H2 253): -0,268 / +0,056 (Pruefer), hier nachgerechnet in t2_lage_original().
  t5_quer      Zahlen-Pruefer V4 r3 4.2 (pz3_t5orig.py): Breite quer zur t2_mass_v4-Mittellinie (Strahl je 0,5 px in der Schwanzmaske),
               Achse Knick -> Beginn, f 0,1/0,3/0,5/0,7 und Abfall = max(Maximum bis f - Breite bei f) ueber f 0,05-0,75 (15 Stellen).
  querprofil   Sicht-Pruefer V4 r3 1.3 (pruefung_sicht_r3/schwanz_brett.py): L* quer (Spalte) durch den waagerechten Teil bei
               x 310/320/330/340, nur Schwanzpixel oberhalb y 262; groesste Stufe ueber 3 px und Plateau-Anteil (|dL/dy| < 0,3).
               Original: Vorlage 2 roh in der Leser-Maske; Port: Videoraum (Vergleichsregel) in den Ids 16-22.
  oberkontur   Sicht-Pruefer V4 r3 1.3: oberster Schwanzpixel je Spalte x 284-332 (y < 262), RMS zur Ausgleichsgeraden (Kruemmung),
               dazu die Oberkante je Spalte x 276-292 gegen das Original.
  dorn         Sicht-Pruefer V4 r3 1.3 (schwanz_silhouette_r3.ecken): Douglas-Peucker (eps 2 px) der Schwanz-Aussenkontur, kleinster
               Eckwinkel im Bereich der Biegung (x <= 300, y <= 270).
Keine RE-Werte: die Sichel-Form gibt es auf der Disc nicht; alle Ziele sind Messungen am 80%-Video (Vorlage 2 = n1435).
"""
import os, sys
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v4_gemein as G
import t2_mass_v4 as T2M

XS_QUER = (310, 320, 330, 340)        # Sicht-Pruefer V4 r3 1.3 (schwanz_brett.py)
Y_MAX = 262                           # dito: nur der waagerechte Teil (oberhalb y 262)
X_KONTUR = (284, 332)                 # dito: ab x 336 ist die Leser-Maske oben bei y 196 abgeschnitten
X_KANTE = (276, 280, 284, 288, 292)   # Sicht-Pruefer V4 r3 1.3: Oberkante x 276-292 (Original y 251/250/243/236/234)


def masken_original():
    out = []
    for f in T2M.MASKEN:
        d = np.load(f)
        out.append(np.unpackbits(d['m'])[:int(np.prod(d['shape']))].reshape(tuple(d['shape'])).astype(bool))
    return out


def t2_lage_port(ids, H, beginn):
    r = T2M.port(ids, H)
    if r is None:
        return None
    m = np.asarray(r['mitte'], float)
    return dict(drehung=r['drehung'], mitte=m, dx=(m[0] - beginn[0]) / H, dy=(m[1] - beginn[1]) / H, profil=r['profil'])


def t2_lage_original():
    out = []
    for t in masken_original():
        ys, xs = np.nonzero(t); seed = (int(xs.max()), int(ys[np.argmax(xs)]))
        P, s = T2M.linie(t, seed)
        w, m, _p = T2M.drehung(P, s, T2M.H2, T2M.BEGINN_X_ORIG - 0.10 * T2M.H2)
        out.append(dict(drehung=w, dx=(m[0] - 357) / T2M.H2, dy=(m[1] - 243) / T2M.H2))
    return out


def _breiten(t, P):
    W = []
    for j in range(len(P)):
        a = P[min(j + 2, len(P) - 1)] - P[max(j - 2, 0)]; a = a / (np.linalg.norm(a) + 1e-9); n = np.array([-a[1], a[0]]); w = 0.0
        for sg in (1, -1):
            for st in np.arange(0.5, 80, 0.5):
                x, y = P[j] + sg * st * n; xi, yi = int(round(x)), int(round(y))
                if not (0 <= xi < t.shape[1] and 0 <= yi < t.shape[0]) or not t[yi, xi]:
                    break
                w += 0.5
        W.append(w)
    return np.array(W)


def t5_quer(t, P, H, beginn, knick):
    """Breitenprofil quer zur Mittellinie P (Pruefer pz3_t5orig.profil)."""
    s = np.r_[0, np.cumsum(np.linalg.norm(np.diff(P, axis=0), axis=1))]
    W = _breiten(t, P)
    a = int(np.argmin(np.linalg.norm(P - np.asarray(beginn, float), axis=1)))
    k = int(np.argmin(np.linalg.norm(P - np.asarray(knick, float), axis=1)))
    L = s[k] - s[a]
    wf = lambda f: float(W[int(np.argmin(np.abs(s - (s[k] - f * L))))] / H)
    seq = [wf(f) for f in np.linspace(0.05, 0.75, 15)]
    abfall = max(max(seq[:i + 1]) - seq[i] for i in range(len(seq)))
    return dict(f={0.1: wf(0.1), 0.3: wf(0.3), 0.5: wf(0.5), 0.7: wf(0.7)}, profil=[round(wf(f), 3) for f in np.arange(0, 1.0001, 0.1)],
                abfall=float(abfall))


def t5_quer_port(ids, H, beginn, knick):
    t = (ids >= 16) & (ids <= 22)
    r = T2M.port(ids, H)
    return t5_quer(t, r['P'], H, beginn, knick)


def t5_quer_original():
    out = []
    for t in masken_original():
        ys, xs = np.nonzero(t); P, _s = T2M.linie(t, (int(xs.max()), int(ys[np.argmax(xs)])))
        out.append(t5_quer(t, P, T2M.H2, (357, 243), (290, 308)))
    return out


def _kenn(L):
    L = np.asarray(L, float); d = np.diff(L)
    st3 = float(np.abs(L[3:] - L[:-3]).max()) if len(L) > 3 else 0.0
    return dict(plateau=float((np.abs(d) < 0.3).mean()) if len(d) else 1.0, stufe3px=st3)


def querprofil(Lstern, maske):
    out = {}
    for x in XS_QUER:
        ys = np.nonzero(maske[:Y_MAX, x])[0]
        if len(ys) < 4:
            out[x] = None; continue
        out[x] = _kenn([Lstern[y, x] for y in range(ys.min(), ys.max() + 1)])
    return out


def oberkontur(maske):
    xs, ys = [], []
    for x in range(X_KONTUR[0], X_KONTUR[1] + 1):
        yy = np.nonzero(maske[:Y_MAX, x])[0]
        if len(yy):
            xs.append(x); ys.append(yy.min())
    xs = np.array(xs, float); ys = np.array(ys, float)
    kante = {}
    for x in X_KANTE:
        yy = np.nonzero(maske[:Y_MAX, x])[0]
        kante[x] = int(yy.min()) if len(yy) else None
    if len(xs) < 5:
        return dict(rms=0.0, max_abw=0.0, n=len(xs), kante=kante)
    k = np.polyfit(xs, ys, 1); r = ys - np.polyval(k, xs)
    return dict(rms=float(np.sqrt((r ** 2).mean())), max_abw=float(np.abs(r).max()), n=len(xs), kante=kante)


def dorn(maske, box=(0, 0, 300, 270)):
    cs, _ = cv2.findContours(maske.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    if not cs:
        return dict(min_winkel=180.0, bei=None, ecken=0)
    c = max(cs, key=cv2.contourArea)
    p = cv2.approxPolyDP(c, 2.0, True)[:, 0, :]; n = len(p); best = (180.0, None)
    for i in range(n):
        a, b, cc = p[i - 1], p[i], p[(i + 1) % n]
        if not (box[0] <= b[0] <= box[2] and box[1] <= b[1] <= box[3]):
            continue
        v1, v2 = a - b, cc - b
        w = float(np.degrees(np.arccos(np.clip(np.dot(v1, v2) / (np.linalg.norm(v1) * np.linalg.norm(v2) + 1e-9), -1, 1))))
        if w < best[0]:
            best = (w, (int(b[0]), int(b[1])))
    return dict(min_winkel=best[0], bei=best[1], ecken=n)


def form_original():
    """Querprofil, Oberkontur und Dorn am Original: Vorlage 2 roh (G.vorlage(2)), Leser-Masken A / hi / lo."""
    Lo = G.lab(G.vorlage(2))[..., 0]
    out = []
    for t in masken_original():
        out.append(dict(quer=querprofil(Lo, t), kontur=oberkontur(t), dorn=dorn(t)))
    return out


def form_port(img, ids):
    sw = (ids >= 16) & (ids <= 22)
    L = G.lab(G.videoraum(img))[..., 0]
    return dict(quer=querprofil(L, sw), kontur=oberkontur(sw), dorn=dorn(sw))


def _rund(o):
    if isinstance(o, float):
        return round(o, 3)
    if isinstance(o, dict):
        return {str(k): _rund(v) for k, v in o.items()}
    if isinstance(o, (list, tuple)):
        return [_rund(v) for v in o]
    return o


if __name__ == '__main__':
    import json, b2
    import riegel_v4 as RV
    print('T2 Lage Original', _rund(t2_lage_original()))
    print('T5 quer Original', _rund([(d['f'], d['abfall'], d['profil']) for d in t5_quer_original()]))
    for d in form_original():
        print('Form Original', json.dumps(_rund(d)))
    P = G.posen()
    for e in sys.argv[1:]:
        mo = b2.modell(e)
        img, ids, iv = G.zeichne(mo, P['85'], voll=True)
        ok, so, H = G.h_port(iv)
        s = RV.schwanz_mass_h(img, ids, ok, so)
        tl = t2_lage_port(ids, H, s['beginn'])
        t5 = t5_quer_port(ids, H, s['beginn'], s['knick'])
        print(os.path.basename(e), 'T2 %.1f Grad dx %.3f dy %.3f' % (tl['drehung'], tl['dx'], tl['dy']), 'T5q', _rund(t5['f']),
              'Abfall %.3f' % t5['abfall'], 'Profil', t5['profil'])
        print('   Form', json.dumps(_rund(form_port(img, ids))))


# ---------------------------------------------------------------------------------------------------- Oberkoerper vorn (Sicht V4 r3 1.5)
# Sicht-Pruefer V4 r3 (pruefung_sicht_r3/oberkoerper_vorn.py, oberkoerper_spiel.py): Original = Vorlage 1 entblendet im Polygon POLY
# (Buckel/Schultern/Brust); Port-Buehne = Ids 8/9/10/13 oberhalb 0,37 H_port ab Figur-Oberkante (3x3 erodiert), Videoraum; Drittel der
# Breite (B)-links / Mitte / (B)-rechts. Rotanteil = Anteil R - G >= 15; Rot Mitte / aussen = Mitte / Mittel der Aussendrittel;
# Schultern = Anteil a* > 12 in den Aussendritteln; kraeftig rot = Anzahl a* > 18.
POLY_135 = [(297, 175), (320, 150), (360, 140), (410, 138), (450, 150), (490, 165), (505, 195), (500, 235), (470, 250), (330, 250), (300, 230)]
# Spielbilder (registriert ueber nb2neu/c6.port): Polygone des Sicht-Pruefers V4 r3 (oberkoerper_spiel.py PV/PS, von Hand auf Gittern)
PV_SPIEL = {118: [(318, 190), (335, 170), (370, 160), (410, 162), (440, 175), (465, 195), (470, 230), (455, 255), (330, 255), (315, 225)],
            135: POLY_135}
PS_SPIEL = {118: [(262, 225), (290, 195), (330, 170), (380, 162), (430, 170), (460, 195), (475, 230), (470, 265), (430, 280), (300, 280),
                  (268, 265)],
            135: [(262, 205), (290, 170), (330, 150), (380, 148), (430, 160), (470, 180), (492, 215), (488, 255), (450, 268), (300, 268),
                  (268, 250)]}


def drittel(img_v, maske, xspanne=None):
    ys, xs = np.nonzero(maske)
    x0, x1 = (xs.min(), xs.max()) if xspanne is None else xspanne
    xx = np.arange(maske.shape[1])[None, :]
    res = {}
    for i, nm in enumerate(('B_links', 'Mitte', 'B_rechts')):
        mm = maske & (xx >= x0 + i * (x1 - x0) / 3) & (xx < x0 + (i + 1) * (x1 - x0) / 3)
        px = img_v[mm]; lab = G.lab(px)
        res[nm] = dict(rot=float(((px[:, 0] - px[:, 1]) >= 15).mean()), a12=float((lab[:, 1] > 12).mean()), n=int(mm.sum()))
    res['rot_m_a'] = res['Mitte']['rot'] / max(1e-3, 0.5 * (res['B_links']['rot'] + res['B_rechts']['rot']))
    res['schulter_a12'] = max(res['B_links']['a12'], res['B_rechts']['a12'])
    lab = G.lab(img_v[maske])
    res['a18'] = int((lab[:, 1] > 18).sum()); res['n'] = int(maske.sum()); res['lab'] = lab.mean(0).tolist()
    res['L_p10'] = float(np.percentile(lab[:, 0], 10))
    return res


def poly_maske(poly, shape=(480, 640)):
    m = np.zeros(shape, np.uint8); cv2.fillPoly(m, [np.array(poly, np.int32)], 1)
    return m.astype(bool)


def oberkoerper_original():
    return drittel(G.vorlage(1), poly_maske(POLY_135))


def oberkoerper_buehne(img, ids, H=364.0):
    top = np.nonzero((ids >= 0).any(1))[0].min(); ycut = int(round(top + 0.37 * H))
    m = np.isin(ids, (8, 9, 10, 13)) & (np.arange(ids.shape[0])[:, None] <= ycut)
    m = cv2.erode(m.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
    return drittel(G.videoraum(img), m)


# ---------------------------------------------------------------------------------------------------- Kopf-Struktur im Spielbild (O4)
# Sicht-Pruefer V4 r3 1.1 (pruefung_sicht_r3/kopf_struktur_spiel.py): Lum-Gradient (Sobel) im Videoraum; Original = Vorlage 1 entblendet,
# Innenflaeche Box 376-408 x 148-182 (Kopfbox ohne Umriss), Ziel = Spanne der um +-3 px verschobenen Boxen (29,0-40,4), glatte Pixel
# (Gradient < 8) 0,074; Spielbild = Kreis um die Kuppelmitte x 0,6 (h135 (360,191) r 28, h118 (358,222) r 26, von Hand gesetzt).
KOPF_SPIEL = {135: ((360, 191), 28), 118: ((358, 222), 26)}
KOPF_INNEN_ORIG = (376, 148, 408, 182)


def _grad_lum(img):
    Lm = G.lum(img).astype(np.float32)
    return np.hypot(cv2.Sobel(Lm, cv2.CV_32F, 1, 0), cv2.Sobel(Lm, cv2.CV_32F, 0, 1))


def kopf_innen_original():
    g = _grad_lum(G.vorlage(1)); x0, y0, x1, y1 = KOPF_INNEN_ORIG
    werte = [float(g[y0 + dy:y1 + dy + 1, x0 + dx:x1 + dx + 1].mean()) for dx in (-3, 0, 3) for dy in (-3, 0, 3)]
    sub = g[y0:y1 + 1, x0:x1 + 1]
    return dict(grad=float(sub.mean()), spanne=(min(werte), max(werte)), glatt=float((sub < 8).mean()))


def kopf_innen_spiel(bild_rgb, h):
    """bild_rgb = registriertes Spielbild 640 x 480 (c6.port, RGB)."""
    g = _grad_lum(G.videoraum(bild_rgb)); (cx, cy), r = KOPF_SPIEL[h]
    m = np.zeros(g.shape, np.uint8); cv2.circle(m, (cx, cy), int(r * 0.6), 1, -1); m = m.astype(bool)
    return dict(grad=float(g[m].mean()), glatt=float((g[m] < 8).mean()))


def kopf_innen_buehne(img, ids, spielaufloesung=True):
    """Naeherung an der Buehne (Pose 135): optional ueber 320 x 240 (INTER_AREA hin und zurueck wie die FRAMEDUMPs in c6.port),
    Kreis um den Schwerpunkt von Id 9 mit 0,6 x Radius gleicher Flaeche."""
    im = img
    if spielaufloesung:
        im = cv2.resize(cv2.resize(img, (320, 240), interpolation=cv2.INTER_AREA), (640, 480), interpolation=cv2.INTER_AREA)
    g = _grad_lum(G.videoraum(im))
    ys, xs = np.nonzero(ids == 9); r = np.sqrt(len(xs) / np.pi)
    m = np.zeros(g.shape, np.uint8); cv2.circle(m, (int(xs.mean()), int(ys.mean())), int(0.6 * r), 1, -1); m = m.astype(bool)
    return dict(grad=float(g[m].mean()), glatt=float((g[m] < 8).mean()))
