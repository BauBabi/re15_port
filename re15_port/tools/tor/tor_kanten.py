"""Subpixel-Kantenmessung in den Raumhintergruenden - Grundlage der Tor-Vermessung.

GRUNDSATZ: Die RICHTUNG einer Kante wird NICHT aus dem Bild geschaetzt, sondern aus der
Kamera gerechnet. Rahmen, Schild und Pfosten laufen entlang der Weltachsen X und Y
(geprueft: die Gelaenderholme folgen in Cut 0/11/12 den Fluchtlinien von Welt-X, die
Pfosten denen von Welt-Y - Kontrollbild vermessung_fluchtlinien_cutNN.png). Damit hat
jede Kante nur EINEN Freiheitsgrad (ihre Lage quer zur Fluchtlinie), und ueber ihre ganze
Laenge kann gemittelt werden. Bei 30..111 Bildpunkten Torbreite ist das der Unterschied
zwischen +-1 px und +-0,2 px.

Bilddaten: MDEC-Hintergruende, Farbe 4:2:0 (Farbwerte liegen nur in 2x2-Bloecken vor -
gemessen: die Gelbheit wiederholt sich paarweise in x und y). Kanten werden deshalb auf
der HELLIGKEIT gemessen; die Farbe dient nur zur Zuordnung.
"""
import numpy as np
from scipy import ndimage


def helligkeit(rgb):
    a = rgb.astype(float)
    return (a[..., 0] * 299 + a[..., 1] * 587 + a[..., 2] * 114) / 1000.0


def gelbheit(rgb):
    a = rgb.astype(float)
    return (a[..., 0] + a[..., 1]) / 2.0 - a[..., 2]


def fluchtpunkt(kam, richtung):
    """Bildpunkt, in dem alle Weltgeraden dieser Richtung zusammenlaufen."""
    v = kam.R.dot(np.asarray(richtung, float))
    return np.array([160.0 + kam.H * v[0] / v[2], 120.0 + kam.H * v[1] / v[2]])


def abtasten(feld, x, y):
    """Bilinear; Bildpunkt (i, j) hat seine MITTE bei (i+0.5, j+0.5) - dieselbe
    Vereinbarung wie die Rueckprojektion (Pixelmitte = ganzzahlige Koordinate + 0.5)."""
    return ndimage.map_coordinates(feld, [np.asarray(y) - 0.5, np.asarray(x) - 0.5],
                                   order=1, mode="nearest")


class Kante(object):
    """Ergebnis einer Kantenmessung: Gerade durch p mit Richtung d (zum Fluchtpunkt)."""

    def __init__(self, name, p, d, n, versatz, stufe, profil, quer, halb, teil):
        self.name = name
        self.p = np.asarray(p, float)       # Punkt auf der Kante (Bildkoordinaten)
        self.d = np.asarray(d, float)       # Richtung (Einheitsvektor)
        self.n = np.asarray(n, float)       # Normale (Einheitsvektor), Messrichtung
        self.versatz = versatz              # gefundene Verschiebung gegen den Startwert
        self.stufe = stufe                  # (Niveau diesseits, Niveau jenseits)
        self.profil = profil
        self.quer = quer
        self.halb = halb                    # Kantenlage der beiden Haelften einzeln
        self.teil = teil                    # Streuung ueber Teilstuecke (px)

    def als_dict(self):
        return dict(name=self.name, punkt=[round(float(v), 3) for v in self.p],
                    richtung=[round(float(v), 5) for v in self.d],
                    niveau=[round(float(v), 1) for v in self.stufe],
                    haelften_px=[round(float(v), 3) for v in self.halb],
                    streuung_px=round(float(self.teil), 3))


def _kreuzung(quer, prof, innen=None):
    """Lage des 50-%-Durchgangs zwischen den beiden Plateaus eines Stufenprofils.
    Plateaus = Mittel der aeusseren Viertel. Gesucht wird der Durchgang, der dem
    Profilmittelpunkt am naechsten liegt."""
    n = len(prof)
    q = max(2, n // 4)
    lo, hi = prof[:q].mean(), prof[-q:].mean()
    if abs(hi - lo) < 1e-6:
        return None, (lo, hi)
    mitte = 0.5 * (lo + hi)
    s = prof - mitte
    kand = []
    for i in range(n - 1):
        if s[i] == 0:
            kand.append(quer[i])
        elif s[i] * s[i + 1] < 0:
            t = s[i] / (s[i] - s[i + 1])
            kand.append(quer[i] + t * (quer[i + 1] - quer[i]))
    if not kand:
        return None, (lo, hi)
    kand = np.array(kand)
    return float(kand[np.argmin(np.abs(kand))]), (lo, hi)


def messe_kante(feld, name, a, b, fp, halbbreite=3.0, schritt=0.25, rand=0.0, runden=3):
    """Kante zwischen den Bildpunkten a und b (Startwert, von Hand aus der Vergroesserung).

    fp         Fluchtpunkt der Kantenrichtung (aus der Kamera). Die Kante laeuft durch
               die Mitte von a..b in Richtung fp; gesucht ist nur der Querversatz.
    halbbreite Halbe Laenge des Querprofils in px. Muss kleiner sein als der Abstand zur
               naechsten fremden Kante.
    rand       So viele px werden an beiden Enden ausgelassen (Ecken).
    Rueckgabe  Kante, oder None wenn kein Durchgang gefunden wurde.
    """
    a = np.asarray(a, float)
    b = np.asarray(b, float)
    m = 0.5 * (a + b)
    versatz_ges = 0.0
    quer = np.arange(-halbbreite, halbbreite + 1e-9, schritt)
    erg = None
    for _ in range(runden):
        d = np.asarray(fp, float) - m
        d /= np.linalg.norm(d)
        if np.dot(d, b - a) < 0:
            d = -d
        n = np.array([-d[1], d[0]])
        lang = np.linalg.norm(b - a)
        ts = np.arange(-lang / 2 + rand, lang / 2 - rand + 1e-9, 0.5)
        if len(ts) < 2:
            return None

        def profil(tt):
            px = m[0] + np.outer(tt, np.full_like(quer, d[0])) + np.outer(np.ones_like(tt), quer * n[0])
            py = m[1] + np.outer(tt, np.full_like(quer, d[1])) + np.outer(np.ones_like(tt), quer * n[1])
            return abtasten(feld, px, py).mean(0)

        prof = profil(ts)
        k, stufe = _kreuzung(quer, prof)
        if k is None:
            return None
        h1, _ = _kreuzung(quer, profil(ts[ts < 0]))
        h2, _ = _kreuzung(quer, profil(ts[ts >= 0]))
        # Streuung ueber Viertel
        vt = []
        for teil in np.array_split(ts, 4):
            if len(teil) >= 2:
                kk, _ = _kreuzung(quer, profil(teil))
                if kk is not None:
                    vt.append(kk)
        teilstreu = float(np.std(vt)) if len(vt) >= 2 else float("nan")
        m = m + k * n
        versatz_ges += k
        erg = Kante(name, m.copy(), d.copy(), n.copy(), versatz_ges, stufe, prof, quer,
                    (h1 if h1 is not None else float("nan"), h2 if h2 is not None else float("nan")),
                    teilstreu)
        if abs(k) < 0.02:
            break
    return erg


def schnitt(k1, k2):
    """Schnittpunkt zweier Kanten-Geraden (Bildkoordinaten)."""
    A = np.array([[k1.d[0], -k2.d[0]], [k1.d[1], -k2.d[1]]])
    r = k2.p - k1.p
    t = np.linalg.solve(A, r)
    return k1.p + t[0] * k1.d


def ebenen_winkel(kam, ka, kb):
    """Winkel zwischen den beiden Ebenen, die der Kameraort mit zwei Bildgeraden
    aufspannt. Fuer die beiden Silhouettenlinien eines ZYLINDERS ist das 2*asin(r/d),
    d = senkrechter Abstand Kameraort - Zylinderachse. Genau, ohne Kleinwinkelnaeherung."""
    def normale(k):
        r1 = kam.strahl(*(k.p - 20 * k.d))
        r2 = kam.strahl(*(k.p + 20 * k.d))
        nn = np.cross(r1, r2)
        return nn / np.linalg.norm(nn)
    na, nb = normale(ka), normale(kb)
    c = abs(float(np.dot(na, nb)))
    return float(np.arccos(min(1.0, c)))
