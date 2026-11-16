"""Gemeinte Stelle NEBEN dem Linienende per Pixelbelegung der Grundlage (Memory reai-v2-beschriftungslinien-messen)."""
import numpy as np, cv2
from scipy import ndimage as ndi
from g_basis import grund_lab, figur, VS

KLASSEN = {
    'weiss': 'L* >= 70 und C* <= 30 (Grundlage)',
    'auge': 'L* >= Median(21 px) + 12 (lokal hell)',
    'braun': 'Waerme w = b* + 0,5 a* mindestens 8 ueber dem Median(31 px), nicht weiss, L* >= 20',
    'rot': 'a* >= 25 oder a* mindestens 10 ueber dem Median(31 px); nicht weiss',
}
_c = {}


def klassenmaske(n, klasse):
    if (n, klasse) in _c:
        return _c[(n, klasse)]
    lab = grund_lab(n); L, a, b = lab[..., 0], lab[..., 1], lab[..., 2]
    C = np.hypot(a, b); fig = figur(n)
    weiss = (L >= 70) & (C <= 30)
    if klasse == 'weiss':
        m = weiss
    elif klasse == 'auge':
        bg = cv2.medianBlur(np.clip(L * 2.55, 0, 255).astype(np.uint8), 21).astype(np.float32) / 2.55
        m = (L - bg) >= 12
    elif klasse == 'braun':
        w = b + 0.5 * a
        wm = cv2.medianBlur(np.clip(w + 128, 0, 255).astype(np.uint8), 31).astype(np.float32) - 128
        m = ((w - wm) >= 8) & ~weiss & (L >= 20)
    elif klasse == 'rot':
        am = cv2.medianBlur(np.clip(a + 128, 0, 255).astype(np.uint8), 31).astype(np.float32) - 128
        m = ((a >= 25) | ((a - am) >= 10)) & ~weiss & (L >= 15)
    m = m & fig
    _c[(n, klasse)] = m
    return m


def fleck(n, ende_g, klasse, r_such=12, r_max=22):
    """Zusammenhangskomponente der Klasse, die dem Linienende am naechsten liegt (<= r_such), auf r_max um das Ende begrenzt."""
    m = klassenmaske(n, klasse); H, W = m.shape
    yy, xx = np.mgrid[0:H, 0:W]
    d2 = (xx - ende_g[0]) ** 2 + (yy - ende_g[1]) ** 2
    lab, k = ndi.label(m)
    ids = np.unique(lab[(d2 <= r_such ** 2) & m]); ids = ids[ids > 0]
    if len(ids) == 0:
        return np.zeros_like(m), None
    best = min(ids, key=lambda i: d2[lab == i].min())
    q = (lab == best) & (d2 <= r_max ** 2)
    q = ndi.binary_fill_holes(q)
    ys, xs = np.nonzero(q)
    L = grund_lab(n)
    ring = (cv2.dilate(q.astype(np.uint8), np.ones((9, 9), np.uint8)) > 0) & ~q & figur(n)
    info = dict(abstand_ende_px=round(float(np.sqrt(d2[q].min())), 1), px_grundlage=int(q.sum()),
                bbox_grundlage=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())],
                schwerpunkt_grundlage=[round(float(xs.mean()), 1), round(float(ys.mean()), 1)],
                lab_median_fleck=[round(float(np.median(L[..., i][q])), 1) for i in range(3)],
                lab_median_umgebung=[round(float(np.median(L[..., i][ring])), 1) for i in range(3)] if ring.any() else None,
                klasse=klasse, regel=KLASSEN[klasse])
    return q, info


def richtung(lf, farbe, ende, schritt=10):
    """Richtung der Linie am Ende (vom Linieninneren zum Ende), aus den Linienpixeln im Ring 6..schritt px."""
    import g_bau
    M, fig, _ = g_bau.lade(lf); s = M[farbe]
    lb, _ = ndi.label(cv2.dilate(s.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0)
    yy0, xx0 = np.nonzero(lb > 0); dd = np.hypot(xx0 - ende[0], yy0 - ende[1]); i = dd.argmin()
    s = s & (lb == lb[yy0[i], xx0[i]])  # nur der Strich, an dem das Ende liegt
    ys, xs = np.nonzero(s)
    d = np.hypot(xs - ende[0], ys - ende[1])
    sel = (d >= 5) & (d <= schritt)
    if not sel.any():
        return np.array([0.0, 0.0])
    v = np.array(ende, float) - np.array([xs[sel].mean(), ys[sel].mean()])
    return v / (np.linalg.norm(v) + 1e-9)


def wachsen(n, ende_g, rv, tol=12.0, r_max=22, vorlauf=4, ohne_weiss=True):
    """Gemeinte Stelle als Farbgebiet: Saat = Scheibe r 3 auf der Verlaengerung der Linie (vorlauf px hinter dem Ende),
    Wachstum ueber 4-Nachbarn mit Delta E (CIE76) zur Saat-Mitte <= tol, auf r_max um das Ende begrenzt."""
    lab = grund_lab(n); H, W = lab.shape[:2]; fig = figur(n)
    L, a, b = lab[..., 0], lab[..., 1], lab[..., 2]
    weiss = (L >= 70) & (np.hypot(a, b) <= 30)
    sx, sy = ende_g[0] + rv[0] * vorlauf, ende_g[1] + rv[1] * vorlauf
    yy, xx = np.mgrid[0:H, 0:W]
    saat = ((xx - sx) ** 2 + (yy - sy) ** 2 <= 9) & fig
    if ohne_weiss:
        saat &= ~weiss
    if not saat.any():
        return np.zeros((H, W), bool), None
    mu = lab[saat].mean(0)
    dE = np.sqrt(((lab - mu) ** 2).sum(2))
    erl = (dE <= tol) & fig & ((xx - ende_g[0]) ** 2 + (yy - ende_g[1]) ** 2 <= r_max ** 2)
    if ohne_weiss:
        erl &= ~weiss
    lb, _ = ndi.label(erl)
    # Runde 2 (Pruefer r1 Nr. 15): je Linienende genau EIN Farbgebiet = die Komponente am saatnaechsten Pixel
    sy_, sx_ = np.nonzero(saat & erl)
    if len(sx_) == 0:
        return np.zeros((H, W), bool), None
    j = np.argmin((sx_ - sx) ** 2 + (sy_ - sy) ** 2)
    q = lb == lb[sy_[j], sx_[j]]
    q = ndi.binary_fill_holes(q)
    if not q.any():
        return q, None
    ys, xs = np.nonzero(q)
    ring = (cv2.dilate(q.astype(np.uint8), np.ones((9, 9), np.uint8)) > 0) & ~q & fig
    d2 = (xs - ende_g[0]) ** 2 + (ys - ende_g[1]) ** 2
    info = dict(abstand_ende_px=round(float(np.sqrt(d2.min())), 1), px_grundlage=int(q.sum()),
                bbox_grundlage=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())],
                schwerpunkt_grundlage=[round(float(xs.mean()), 1), round(float(ys.mean()), 1)],
                saat_grundlage=[round(float(sx), 1), round(float(sy), 1)], saat_lab=[round(float(v), 1) for v in mu],
                lab_median_fleck=[round(float(np.median(lab[..., i][q])), 1) for i in range(3)],
                lab_median_umgebung=[round(float(np.median(lab[..., i][ring])), 1) for i in range(3)] if ring.any() else None,
                klasse='farbgebiet', regel='Saat %d px hinter dem Linienende in Linienrichtung, Delta E <= %.0f, r <= %d px%s' % (
                    vorlauf, tol, r_max, ', ohne Weiss' if ohne_weiss else ''))
    return q, info
