# -*- coding: utf-8 -*-
"""Kandidat C: Knochen, UV-Karten, Backen der Lage- und Normalenkarten, Ausgabe als EMD."""
from teile import *
import form_emd as FE

TEX_B, TEX_H = 384, 256


def tri_von_flaeche(f):
    """Ring -> Dreiecke wie der Zeichner: (0,1,2), (0,2,3)."""
    return [(f[0], f[i], f[i + 1]) for i in range(1, len(f) - 1)]


def karte_bauen(t, res, gew_sp=None, gew_ze=None):
    """Rechteck-Karte eines Teils. Spalten = Ringecken, Zeilen = Pol, Ringe, Pol. gew_sp, gew_ze: Gewicht je Spalte, Zeile (sichtbare Seiten bekommen mehr Texel)."""
    V, ringe, pole = t['V'], t['ringe'], t['pole']
    n = len(ringe[0])
    R = np.array(ringe)
    e = np.linalg.norm(V[R] - V[np.roll(R, -1, axis=1)], axis=2).mean(0)
    z = [V[pole[0]][None, :].repeat(n, 0)] + [V[r] for r in R] + [V[pole[1]][None, :].repeat(n, 0)]
    sg = np.array([float(np.linalg.norm(z[k + 1] - z[k], axis=1).mean()) for k in range(len(z) - 1)])
    w = e * (np.ones(n) if gew_sp is None else np.asarray(gew_sp))
    h = sg * (np.ones(len(sg)) if gew_ze is None else np.asarray(gew_ze))
    x = np.concatenate([[0.0], np.cumsum(w)])
    y = np.concatenate([[0.0], np.cumsum(h)])
    return dict(um=float(x[-1]), lg=float(y[-1]), x=x / x[-1], y=y / y[-1], res=res, name=t['name'])


def packen(karten, b=TEX_B, h=TEX_H, rand=2):
    """Regalpacken, Massstab wird gesucht, bis alles passt. karten: Liste dict(um, lg, x, y, res). Rueckgabe Massstab; setzt px, py, w, h."""
    lo, hi = 5.0, 60.0
    einzig = []
    for k in karten:
        if not any(k is q for q in einzig):
            einzig.append(k)
    karten = einzig

    def versuche(ein):
        """Skyline: jedes Rechteck an die tiefste Stelle."""
        L = sorted(range(len(karten)), key=lambda i: -karten[i]['lg'] / karten[i]['res'])
        sky = np.full(b, rand, int)
        pos = {}
        for i in L:
            k = karten[i]
            w = max(int(math.ceil(k['um'] / (ein * k['res']))) + 1, 12)
            hh = max(int(math.ceil(k['lg'] / (ein * k['res']))) + 1, 8)
            if w + 2 * rand > b:
                return None
            best = None
            for x in range(rand, b - w - rand + 1, 2):
                y = int(sky[x - rand:x + w + rand].max())
                if best is None or y < best[0]:
                    best = (y, x)
            y, x = best
            if y + hh + rand > h:
                return None
            pos[i] = (x, y, w, hh)
            sky[x - rand:x + w + rand] = y + hh + rand
        return pos
    pos = None
    for _ in range(24):
        m = 0.5 * (lo + hi)
        p = versuche(m)
        if p is None:
            lo = m
        else:
            hi = m
            pos = p
    for i, k in enumerate(karten):
        k['px'], k['py'], k['w'], k['h'] = pos[i]
    return hi


def uv_ecke(k, zeile, spalte):
    """Texel von Gitterpunkt (Zeile 0 = Pol 0, 1.. Ringe, letzte = Pol 1; Spalte 0..n)."""
    return (k['px'] + k['x'][spalte] * (k['w'] - 1), k['py'] + k['y'][zeile] * (k['h'] - 1))


def flaechen_uv(t, k):
    """Liste (Ring-Indizes, uv je Ecke) aller Flaechen eines Teils in Gitterform."""
    R = t['ringe']
    p0, p1 = t['pole']
    n = len(R[0])
    m = len(R)
    aus = []
    for i in range(n):
        j = (i + 1) % n
        aus.append(((p0, R[0][j], R[0][i]), (uv_ecke(k, 0, i + 0.5), uv_ecke(k, 1, i + 1), uv_ecke(k, 1, i))))
        aus.append(((p1, R[-1][i], R[-1][j]), (uv_ecke(k, m + 1, i + 0.5), uv_ecke(k, m, i), uv_ecke(k, m, i + 1))))
        for q in range(m - 1):
            aus.append(((R[q][i], R[q][j], R[q + 1][j], R[q + 1][i]), (uv_ecke(k, q + 1, i), uv_ecke(k, q + 1, i + 1), uv_ecke(k, q + 2, i + 1), uv_ecke(k, q + 2, i))))
    return aus


def uv_ecke_fein(k, zeile, spalte):
    return uv_ecke(k, zeile, spalte)


def _interp(v, s):
    i = int(math.floor(s))
    f = s - i
    if f == 0:
        return v[i]
    return v[i] * (1 - f) + v[i + 1] * f


def uv_ecke(k, zeile, spalte):                                      # noqa: F811  Spalte darf halbzahlig sein
    return (k['px'] + _interp(k['x'], spalte) * (k['w'] - 1), k['py'] + _interp(k['y'], zeile) * (k['h'] - 1))


def eck_normalen(t):
    V = t['V']
    T = flaechen_dreiecke(t['F'])
    return eckennormalen(V, T)


def karten_rastern(teile, karten):
    """Fuer jedes Teil: Texel -> Punkt und Normale. Rueckgabe P (H x W x 3), N, ID (Teilnummer, -1 frei)."""
    P = np.zeros((TEX_H, TEX_W_(), 3), np.float32)
    N = np.zeros((TEX_H, TEX_W_(), 3), np.float32)
    ID = np.full((TEX_H, TEX_W_()), -1, np.int16)
    gesehen = []
    for ti, (t, k) in enumerate(zip(teile, karten)):
        if any(k is q for q in gesehen):
            continue
        gesehen.append(k)
        V = t['V']
        Vn = eck_normalen(t)
        for ring, uv in flaechen_uv(t, k):
            for a, b, c in tri_von_flaeche(list(range(len(ring)))):
                u = np.array([uv[a], uv[b], uv[c]], float)
                pa = V[[ring[a], ring[b], ring[c]]]
                na = Vn[[ring[a], ring[b], ring[c]]]
                x0 = int(math.floor(u[:, 0].min())) - 1
                x1 = int(math.ceil(u[:, 0].max())) + 2
                y0 = int(math.floor(u[:, 1].min())) - 1
                y1 = int(math.ceil(u[:, 1].max())) + 2
                x0, y0 = max(x0, 0), max(y0, 0)
                x1, y1 = min(x1, TEX_W_()), min(y1, TEX_H)
                if x1 <= x0 or y1 <= y0:
                    continue
                XX, YY = np.meshgrid(np.arange(x0, x1) + 0.0, np.arange(y0, y1) + 0.0)
                fl = (u[1, 0] - u[0, 0]) * (u[2, 1] - u[0, 1]) - (u[2, 0] - u[0, 0]) * (u[1, 1] - u[0, 1])
                if abs(fl) < 1e-9:
                    continue
                w1 = ((XX - u[0, 0]) * (u[2, 1] - u[0, 1]) - (u[2, 0] - u[0, 0]) * (YY - u[0, 1])) / fl
                w2 = ((u[1, 0] - u[0, 0]) * (YY - u[0, 1]) - (XX - u[0, 0]) * (u[1, 1] - u[0, 1])) / fl
                w0 = 1 - w1 - w2
                e = -0.6 / max(abs(fl) ** 0.5, 1.0)
                m = (w0 >= e) & (w1 >= e) & (w2 >= e)
                if not m.any():
                    continue
                pp = w0[..., None] * pa[0] + w1[..., None] * pa[1] + w2[..., None] * pa[2]
                nn = w0[..., None] * na[0] + w1[..., None] * na[1] + w2[..., None] * na[2]
                s = (slice(y0, y1), slice(x0, x1))
                P[s][m] = pp[m]
                N[s][m] = nn[m]
                ID[s][m] = ti
    return P, N, ID


def TEX_W_():
    return TEX_B


def z_puffer(teile, s, trafo=None):
    """Tiefenbild (640 x 480, inf = frei). trafo(ti, V) -> V in der Pose von s."""
    Z = np.full((480, 640), np.inf, np.float32)
    auge = np.asarray(s.k['auge'], float)
    for ti, t in enumerate(teile):
        V = t['V'] if trafo is None else trafo(ti, t['V'])
        p = s.bild(V)
        d = s.tiefe(V)
        for f in t['F']:
            for a, b, c in tri_von_flaeche(f):
                q = p[[a, b, c]]
                if not np.isfinite(q).all():
                    continue
                x0 = max(int(math.floor(q[:, 0].min())), 0)
                x1 = min(int(math.ceil(q[:, 0].max())) + 1, 640)
                y0 = max(int(math.floor(q[:, 1].min())), 0)
                y1 = min(int(math.ceil(q[:, 1].max())) + 1, 480)
                if x1 <= x0 or y1 <= y0:
                    continue
                XX, YY = np.meshgrid(np.arange(x0, x1, dtype=np.float32), np.arange(y0, y1, dtype=np.float32))
                fl = (q[1, 0] - q[0, 0]) * (q[2, 1] - q[0, 1]) - (q[2, 0] - q[0, 0]) * (q[1, 1] - q[0, 1])
                if abs(fl) < 1e-6:
                    continue
                w1 = ((XX - q[0, 0]) * (q[2, 1] - q[0, 1]) - (q[2, 0] - q[0, 0]) * (YY - q[0, 1])) / fl
                w2 = ((q[1, 0] - q[0, 0]) * (YY - q[0, 1]) - (XX - q[0, 0]) * (q[1, 1] - q[0, 1])) / fl
                w0 = 1 - w1 - w2
                m = (w0 >= -0.02) & (w1 >= -0.02) & (w2 >= -0.02)
                dd = w0 * d[a] + w1 * d[b] + w2 * d[c]
                z = Z[y0:y1, x0:x1]
                u = m & (dd < z)
                z[u] = dd[u]
    return Z


def blende_fuellen(rgb, m, n=6):
    """Farben ueber die Kartengrenzen auslaufen lassen (Bleed)."""
    a = rgb.astype(np.float32)
    ok = m.copy()
    k = np.ones((3, 3), np.uint8)
    for _ in range(n):
        g = cv2.dilate(ok.astype(np.uint8), k).astype(bool)
        neu = g & ~ok
        if not neu.any():
            break
        s = cv2.blur(a * ok[..., None], (3, 3))
        w = cv2.blur(ok.astype(np.float32), (3, 3))
        a[neu] = s[neu] / np.maximum(w[neu, None], 1e-3)
        ok = g
    return a
