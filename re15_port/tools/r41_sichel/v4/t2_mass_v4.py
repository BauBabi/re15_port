"""SF V4 Runde 3: EIN Messverfahren fuer die T2-Drehung (Entscheid O2 des Orchestrators, Pruefung V4 r2 Zahlen 4.1).

Verfahren = Pruefer-Mittellinie (pz2_t2orig.linie / pz2_mess.schwanz, Zahlen-Pruefer V4 r2), weil es am Original ueber die drei
Leser-Masken stabil ist (65,8 / 69,5 / 70,1 Grad), die Bauer-Mittellinie (v3/P_schwanz) dagegen nicht (53,3 / 65,3 / 61,8):
  1. Schwanzmaske = groesste 8er-Komponente; Saat = vorderster Schwanzpixel (groesstes x) an der Koerpergrenze (Port: Ids 16-22 an
     Id 7/8 dilatiert 3x3; Original: vorderster Maskenpixel).
  2. Geodaetischer Abstand (8er-Nachbarn) ab der Saat; Baender je 3 px; Mittellinie = Schwerpunkt je Band, 3er-Mittel.
  3. Drehung = groesste Richtungsaenderung zwischen der zentrierten Tangente +-0,025 H am Anfang und am Ende eines 0,10-H-Fensters
     der Bogenlaenge; nur Fenster, deren Anfang hinter beginn_x - 0,10 H liegt (Original: Nutzerpunkt Beginn x 357, Messblatt 1a).
Ziel und Toleranz (O2): Ziel = Mittel der Originalwerte, Toleranz = groesser von (Spanne der Originalwerte, 5 Grad).
Bauer, Pruefer Zahlen und Pruefer Sicht benutzen dieses Werkzeug (riegel_v4.py T2).

    python t2_mass_v4.py [EMD ...]      -> Original-Werte, Ziel, Toleranz; je EMD die Drehung in Pose "85"
"""
import os, sys
from collections import deque
import numpy as np, cv2

HIER = os.path.dirname(os.path.abspath(__file__))
MASKEN = [os.path.join(HIER, 't2_masken', f + '.npz') for f in ('schwanz_maske', 'schwanz_maske_hi', 'schwanz_maske_lo')]
H2 = 253.0          # Messblatt V4 Abschnitt 1 "H2"
BEGINN_X_ORIG = 357  # Messblatt V4 Abschnitt 1a: Nutzerpunkt "Schwanz Beginn" (357,243)


def geod(mask, seed):
    D = np.full(mask.shape, -1, int)
    q = deque([seed])
    D[seed[1], seed[0]] = 0
    while q:
        x, y = q.popleft()
        for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1), (1, 1), (1, -1), (-1, 1), (-1, -1)):
            X, Y = x + dx, y + dy
            if 0 <= X < mask.shape[1] and 0 <= Y < mask.shape[0] and mask[Y, X] and D[Y, X] < 0:
                D[Y, X] = D[y, x] + 1
                q.append((X, Y))
    return D


def linie(t, seed):
    n, lb, st, _ = cv2.connectedComponentsWithStats(t.astype(np.uint8), connectivity=8)
    t = lb == (1 + int(np.argmax(st[1:, 4])))
    if not t[seed[1], seed[0]]:
        ys, xs = np.nonzero(t); i = np.argmin((xs - seed[0]) ** 2 + (ys - seed[1]) ** 2); seed = (int(xs[i]), int(ys[i]))
    D = geod(t, seed); pts = []
    for d0 in range(0, D.max() + 1, 3):
        m = (D >= d0) & (D < d0 + 3)
        if m.any():
            yy, xx = np.nonzero(m); pts.append((xx.mean(), yy.mean()))
    P = np.array(pts); P = np.array([P[max(0, j - 1):j + 2].mean(0) for j in range(len(P))])
    s = np.r_[0, np.cumsum(np.linalg.norm(np.diff(P, axis=0), axis=1))]
    return P, s


def drehung(P, s, H, xmax):
    """Groesste Drehung (Grad) und Mitte des Fensters (px); Profil der Richtung je 0,025 H (0 = waagerecht nach hinten)."""
    at = lambda x: np.array([np.interp(x, s, P[:, 0]), np.interp(x, s, P[:, 1])])
    best = (0.0, 0.0)
    for x in np.arange(0, s[-1] - 0.10 * H, 0.5):
        if at(x)[0] > xmax:
            continue
        t0 = at(x + 0.025 * H) - at(max(0.0, x - 0.025 * H)); t1 = at(min(s[-1], x + 0.125 * H)) - at(x + 0.075 * H)
        w = float(np.degrees(np.arctan2(t1[1], -t1[0]) - np.arctan2(t0[1], -t0[0])))
        if w > best[0]:
            best = (w, float(x))
    prof = []
    for x in np.arange(0, s[-1], 0.025 * H):
        d = at(min(s[-1], x + 0.025 * H)) - at(max(0.0, x - 0.025 * H))
        prof.append((round(x / H, 3), round(float(np.degrees(np.arctan2(d[1], -d[0]))), 1)))
    return best[0], at(best[1] + 0.05 * H), prof


def original():
    """Drehung je Leser-Maske des Originals (Vorlage 2), Ziel = Mittel, Toleranz = max(Spanne, 5 Grad)."""
    w = []
    for f in MASKEN:
        d = np.load(f)
        t = np.unpackbits(d['m'])[:int(np.prod(d['shape']))].reshape(tuple(d['shape'])).astype(bool)
        ys, xs = np.nonzero(t); seed = (int(xs.max()), int(ys[np.argmax(xs)]))
        P, s = linie(t, seed)
        w.append(drehung(P, s, H2, BEGINN_X_ORIG - 0.10 * H2)[0])
    ziel = float(np.mean(w)); tol = max(float(np.ptp(w)), 5.0)
    return dict(werte=[round(x, 1) for x in w], ziel=round(ziel, 1), tol=round(tol, 1))


def port(ids, H):
    """Drehung am Port-Render (ids 640x480): Saat und Beginn wie der Zahlen-Pruefer (pz2_mess.schwanz)."""
    t = (ids >= 16) & (ids <= 22)
    k = (ids == 7) | (ids == 8)
    kd = cv2.dilate(k.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0
    grenz = t & kd
    if not grenz.any():
        return None
    ys, xs = np.nonzero(grenz); i = int(np.argmax(ys)); beg = (int(xs[i]), int(ys[i]))
    n, lb, st, _ = cv2.connectedComponentsWithStats(t.astype(np.uint8), connectivity=8)
    big = lb == (1 + int(np.argmax(st[1:, 4])))
    gy, gx = np.nonzero(big & kd)
    if not len(gx):
        gy, gx = np.nonzero(big)
    seed = (int(gx[np.argmax(gx)]), int(gy[np.argmax(gx)]))
    P, s = linie(t, seed)
    w, mitte, prof = drehung(P, s, H, beg[0] - 0.10 * H)
    return dict(drehung=w, mitte=mitte, profil=prof, saat=seed, beginn=beg, P=P)


if __name__ == '__main__':
    o = original()
    print('Original (Masken A / hi / lo):', o['werte'], 'Ziel %.1f, Toleranz %.1f Grad' % (o['ziel'], o['tol']))
    if len(sys.argv) > 1:
        sys.path.insert(0, HIER)
        import v4_gemein as G, b2
        P = G.posen()
        for e in sys.argv[1:]:
            mo = b2.modell(e)
            img, ids, iv = G.zeichne(mo, P['85'], voll=True)
            _ok, _so, H = G.h_port(iv)
            r = port(ids, H)
            print('%s: Drehung %.1f Grad, Mitte (%.1f, %.1f), Saat %s, Beginn %s' % (os.path.basename(e), r['drehung'], *r['mitte'],
                                                                                 r['saat'], r['beginn']))
            print('   Profil:', [p for p in r['profil'] if 0.2 <= p[0] <= 0.6])
