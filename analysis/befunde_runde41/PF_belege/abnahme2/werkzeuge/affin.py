"""Affine Registrierung (eigene, NCC + Powell) des entzerrten Port-Inhalts gegen die entzerrte 2060-Vorlage.
Port (u, v) [Mitte = 0] -> Vorlage x = a*u + b*v + tx, y = c*u + d*v + ty.
Ausgabe: Massstab waagrecht |(a,c)|, senkrecht |(b,d)|, Verhaeltnis V, Neigung der Senkrechten atan(b/d), der Waagrechten atan(c/a) in Grad."""
import numpy as np, cv2, math
from scipy.optimize import minimize


def vorbereiten(img, sigma=1.0):
    g = cv2.cvtColor(img, cv2.COLOR_RGB2GRAY).astype(np.float32) if img.ndim == 3 else img.astype(np.float32)
    return cv2.GaussianBlur(g, (0, 0), sigma)


def ncc(T, R, p, uu, vv):
    a, b, c, d, tx, ty = p
    mx = (a * uu + b * vv + tx).astype(np.float32)
    my = (c * uu + d * vv + ty).astype(np.float32)
    s = cv2.remap(R, mx, my, cv2.INTER_LINEAR, borderMode=cv2.BORDER_CONSTANT, borderValue=-1)
    ok = s >= 0
    if ok.mean() < 0.97:
        return -1.0
    t, s = T[ok], s[ok]
    t = t - t.mean(); s = s - s.mean()
    return float((t * s).sum() / math.sqrt((t * t).sum() * (s * s).sum() + 1e-9))


def registriere(port, ref, rand=(0.10, 0.06)):
    T0 = vorbereiten(port)
    R = vorbereiten(ref)
    h, w = T0.shape
    x0, x1, y0, y1 = int(w * rand[0]), int(w * (1 - rand[0])), int(h * rand[1]), int(h * (1 - rand[1]))
    T = T0[y0:y1, x0:x1]
    hh, ww = T.shape
    vv, uu = np.mgrid[0:hh, 0:ww].astype(np.float32)
    uu -= ww / 2; vv -= hh / 2
    # Grob: Massstab-Gitter + matchTemplate fuer die Lage
    best = None
    for sx in np.arange(0.80, 1.25, 0.02):
        for sy in np.arange(0.80, 1.25, 0.02):
            t = cv2.resize(T, (int(round(ww * sx)), int(round(hh * sy))), interpolation=cv2.INTER_LINEAR)
            if t.shape[0] >= R.shape[0] or t.shape[1] >= R.shape[1]:
                continue
            r = cv2.matchTemplate(R, t, cv2.TM_CCOEFF_NORMED)
            _, m, _, loc = cv2.minMaxLoc(r)
            if best is None or m > best[0]:
                best = (m, sx, sy, loc[0] + t.shape[1] / 2, loc[1] + t.shape[0] / 2)
    m, sx, sy, cx, cy = best
    p0 = np.array([sx, 0, 0, sy, cx, cy])
    f = lambda p: -ncc(T, R, p, uu, vv)
    res = minimize(f, p0, method='Powell', options={'xtol': 1e-4, 'ftol': 1e-6, 'maxiter': 20000})
    a, b, c, d, tx, ty = res.x
    mx_, my_ = math.hypot(a, c), math.hypot(b, d)
    return dict(ncc=-res.fun, ncc_grob=m, a=a, b=b, c=c, d=d, tx=tx, ty=ty,
                V=mx_ / my_, senkrechte=math.degrees(math.atan2(b, d)), waagrechte=math.degrees(math.atan2(c, a)),
                achsenwinkel=math.degrees(math.acos((a * b + c * d) / (mx_ * my_))))


def scher_gegenprobe(port, grad):
    """Port-Bild kuenstlich scheren (Senkrechte um grad geneigt) -> Registrierung muss um grad abweichen."""
    h, w = port.shape[:2]
    t = math.tan(math.radians(grad))
    M = np.array([[1, t, -t * h / 2], [0, 1, 0]], np.float32)
    return cv2.warpAffine(port, M, (w, h), flags=cv2.INTER_LINEAR, borderMode=cv2.BORDER_REFLECT)
