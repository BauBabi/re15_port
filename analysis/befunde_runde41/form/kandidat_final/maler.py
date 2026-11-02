#!/usr/bin/env python3
"""Kandidat A, Textur.

Jeder Texel kennt seine 3D-Lage (Pose-1-Raum), die Haut ist eine 3D-Funktion: keine Naehte, Inseln duerfen liegen, wie sie wollen.
Schichten:
  1. Grundhaut: Falten (quer zum Glied), Risszellen, Adern, Knoten, gemalte Relief-Schattierung, Kontaktschatten. Alles in der Palette, Grundton farbe_palette.json.
  2. Zeichnung: Sollstellen aus FORMBLATT 2.2 + Polygone lesung_zeichnung.json, in das jeweilige Bild gelegt und ueber die Kamera auf die sichtbare Flaeche gebacken.
     Bild 1 hat Vorrang, weil dort die Zeichnung sitzt und das Licht von oben alles Helle am Scheitel aufreisst (pruefung.md Abschnitt Licht).
Frei gesetzt ist die Struktur der Haut (Falten, Adern, Knoten): das Video loest unter rund 4 px nichts auf (FORMBLATT Abschnitt 5).
"""
import json, math, os, sys
import numpy as np
import cv2

HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.normpath(os.path.join(HIER, '..'))
TOOLS = os.path.normpath(os.path.join(HIER, '..', '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS); sys.path.insert(0, HIER)
import form_emd as FE
import form_kamera as FK
import entwurf as E
import volumen as V

BONES = V.BONES
IDX = V.IDX
TW, TH = 384, 256
YK = np.array([.299, .587, .114], np.float32)


def ruhe_w(j):
    """Clip 2: Pose 1, Kopf zur Mitte gedreht (in Pose 1 steht der Vorsprung zur Seite R, FORMBLATT 6D)."""
    g = np.array(j['g'], float)
    w = np.zeros((len(BONES), 3))
    i = IDX['hals']
    k = g[IDX['kopf']] - g[i]
    a = math.atan2(k[2], k[0])          # Drehung um y, die den Kopf auf z ~ 0 bringt
    z_soll = -g[i][2]
    r = math.hypot(k[0], k[2])
    b = math.asin(max(-1, min(1, z_soll / max(r, 1.0))))
    w[i] = (0.0, (a - b), 0.0)
    return w


def yrot(g):
    a = float(g) * 2 * math.pi / 4096
    c, s = math.cos(a), math.sin(a)
    return np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])


def rod(w):
    th = np.linalg.norm(w)
    if th < 1e-9:
        return np.eye(3)
    k = w / th
    K = np.array([[0, -k[2], k[1]], [k[2], 0, -k[0]], [-k[1], k[0], 0]])
    return np.eye(3) + math.sin(th) * K + (1 - math.cos(th)) * K @ K


def pose_fk(g, w):
    n = len(BONES)
    R, T = [None] * n, [None] * n
    for i in range(n):
        p = E.KNOCHEN[i][1]
        L = rod(np.asarray(w[i], float))
        if p < 0:
            R[i], T[i] = L, g[i].copy()
        else:
            R[i] = R[p] @ L
            T[i] = R[p] @ (g[i] - g[p]) + T[p]
    return R, T


def texel_lage(daten):
    """Je Texel Knochen, Lage, Normale im Modellraum (Pose 1)."""
    pos = np.zeros((TH, TW, 3), np.float32)
    nor = np.zeros((TH, TW, 3), np.float32)
    bid = np.full((TH, TW), -1, np.int32)
    for b, nm in enumerate(BONES):
        d = daten[nm]
        v, n = d['v'], d['n']
        for f in d['f']:
            r, t = f['v'], np.array(f['tex'], float)
            for i in range(1, len(r) - 1):
                tri = (0, i, i + 1)
                p = t[list(tri)]
                x0, y0 = np.floor(p.min(0)).astype(int) - 1
                x1, y1 = np.ceil(p.max(0)).astype(int) + 2
                x0, y0 = max(x0, 0), max(y0, 0)
                x1, y1 = min(x1, TW), min(y1, TH)
                fl = (p[1, 0] - p[0, 0]) * (p[2, 1] - p[0, 1]) - (p[2, 0] - p[0, 0]) * (p[1, 1] - p[0, 1])
                if abs(fl) < 1e-6:
                    continue
                X, Y = np.meshgrid(np.arange(x0, x1) + 0.0, np.arange(y0, y1) + 0.0)
                w1 = ((X - p[0, 0]) * (p[2, 1] - p[0, 1]) - (p[2, 0] - p[0, 0]) * (Y - p[0, 1])) / fl
                w2 = ((p[1, 0] - p[0, 0]) * (Y - p[0, 1]) - (X - p[0, 0]) * (p[1, 1] - p[0, 1])) / fl
                w0 = 1 - w1 - w2
                eps = -0.6 / max(math.sqrt(abs(fl)), 1.0)
                m = (w0 >= eps) & (w1 >= eps) & (w2 >= eps)
                if not m.any():
                    continue
                ri = [r[k] for k in tri]
                P = w0[..., None] * v[ri[0]] + w1[..., None] * v[ri[1]] + w2[..., None] * v[ri[2]]
                N = w0[..., None] * n[ri[0]] + w1[..., None] * n[ri[1]] + w2[..., None] * n[ri[2]]
                s = (slice(y0, y1), slice(x0, x1))
                pos[s][m] = P[m]
                nor[s][m] = N[m]
                bid[s][m] = b
    n = np.linalg.norm(nor, axis=-1, keepdims=True)
    nor = nor / np.maximum(n, 1e-6)
    return pos, nor, bid


def welt_pose(pos, bid, g, w, lage, gier):
    """Texel-Lage -> Welt in einer Pose."""
    R, T = pose_fk(g, w)
    Y = yrot(gier)
    aus = np.zeros_like(pos)
    for b in range(len(BONES)):
        m = bid == b
        if m.any():
            aus[m] = ((pos[m] - g[b]) @ R[b].T + T[b]) @ Y.T + lage
    return aus


def normale_pose(nor, bid, g, w, gier):
    R, T = pose_fk(g, w)
    Y = yrot(gier)
    aus = np.zeros_like(nor)
    for b in range(len(BONES)):
        m = bid == b
        if m.any():
            aus[m] = (nor[m] @ R[b].T) @ Y.T
    return aus


def tiefe_puffer(daten, g, w, lage, gier, kam):
    """Z-Puffer der Figur im Videobild, 640x480."""
    R, T = pose_fk(g, w)
    Y = yrot(gier)
    z = np.full((480, 640), 1e12, np.float64)
    for b, nm in enumerate(BONES):
        d = daten[nm]
        vw = ((d['v'] - g[b]) @ R[b].T + T[b]) @ Y.T + lage
        p = FK.projiziere(kam, vw)
        t = FK.tiefe(kam, vw)
        for f in d['f']:
            r = f['v']
            for i in range(1, len(r) - 1):
                tri = [r[0], r[i], r[i + 1]]
                q = p[tri]
                if not np.isfinite(q).all():
                    continue
                x0, y0 = np.maximum(np.floor(q.min(0)).astype(int), 0)
                x1, y1 = np.minimum(np.ceil(q.max(0)).astype(int) + 1, [640, 480])
                if x0 >= x1 or y0 >= y1:
                    continue
                fl = (q[1, 0] - q[0, 0]) * (q[2, 1] - q[0, 1]) - (q[2, 0] - q[0, 0]) * (q[1, 1] - q[0, 1])
                if abs(fl) < 1e-6:
                    continue
                X, Y2 = np.meshgrid(np.arange(x0, x1), np.arange(y0, y1))
                w1 = ((X - q[0, 0]) * (q[2, 1] - q[0, 1]) - (q[2, 0] - q[0, 0]) * (Y2 - q[0, 1])) / fl
                w2 = ((q[1, 0] - q[0, 0]) * (Y2 - q[0, 1]) - (X - q[0, 0]) * (q[1, 1] - q[0, 1])) / fl
                w0 = 1 - w1 - w2
                m = (w0 >= -1e-3) & (w1 >= -1e-3) & (w2 >= -1e-3)
                if not m.any():
                    continue
                Z = w0 * t[tri[0]] + w1 * t[tri[1]] + w2 * t[tri[2]]
                s = z[y0:y1, x0:x1]
                s[m] = np.minimum(s[m], Z[m])
    return z


# ═══════════════════════════════════════════════════ Rauschen, 3D
def _hash(ix, iy, iz, seed):
    h = (ix * 73856093) ^ (iy * 19349663) ^ (iz * 83492791) ^ (seed * 2654435)
    h = h & 0xFFFFFFFF
    h = (h ^ (h >> 13)) * 1274126177 & 0xFFFFFFFF
    h = (h ^ (h >> 16)) * 2246822519 & 0xFFFFFFFF
    return ((h ^ (h >> 15)) & 0xFFFFFF) / float(0x1000000)


def wert(p, seed=0):
    """Wert-Rauschen 0..1, p in Gitter-Einheiten (N,3)."""
    i = np.floor(p).astype(np.int64)
    t = p - i
    t = t * t * (3 - 2 * t)
    s = 0.0
    for dx in (0, 1):
        for dy in (0, 1):
            for dz in (0, 1):
                g = _hash(i[:, 0] + dx, i[:, 1] + dy, i[:, 2] + dz, seed)
                w = (t[:, 0] if dx else 1 - t[:, 0]) * (t[:, 1] if dy else 1 - t[:, 1]) * (t[:, 2] if dz else 1 - t[:, 2])
                s = s + g * w
    return s


def fbm(p, okt=3, seed=0, lac=2.03, gain=0.5):
    a, s, n = 1.0, 0.0, 0.0
    q = p.copy()
    for o in range(okt):
        s = s + a * wert(q, seed + o * 17)
        n += a
        a *= gain
        q = q * lac + 11.3
    return s / n


def worley(p, seed=0):
    """F1, F2 (Abstand in Gitter-Einheiten), Ort des naechsten Punkts, je Punkt."""
    i = np.floor(p).astype(np.int64)
    best1 = np.full(len(p), 9.0)
    best2 = np.full(len(p), 9.0)
    ort = np.zeros_like(p)
    ken = np.zeros(len(p))
    for dx in (-1, 0, 1):
        for dy in (-1, 0, 1):
            for dz in (-1, 0, 1):
                c = i + np.array([dx, dy, dz])
                j = np.stack([_hash(c[:, 0], c[:, 1], c[:, 2], seed + k) for k in range(3)], 1)
                q = c + j
                d = np.linalg.norm(q - p, axis=1)
                neu = d < best1
                best2 = np.where(neu, best1, np.minimum(best2, d))
                best1 = np.where(neu, d, best1)
                ort[neu] = q[neu]
                ken[neu] = _hash(c[:, 0], c[:, 1], c[:, 2], seed + 9)[neu]
    return best1, best2, ort, ken


# ═══════════════════════════════════════════════════ Grundhaut
def achse_je_teil(j):
    """Laengsachse je Teil fuer die Faltenrichtung: Gelenk zum Kind, sonst zum Eltern. Rumpf: x."""
    g = np.array(j['g'], float)
    a = {}
    for i, nm in enumerate(BONES):
        k = [c for c, q in enumerate(E.KNOCHEN) if q[1] == i]
        p = E.KNOCHEN[i][1]
        if nm in V.RUMPF:
            v = np.array([1.0, 0, 0])
        elif k:
            v = g[k[0]] - g[i]
        elif p >= 0:
            v = g[i] - g[p]
        else:
            v = np.array([1.0, 0, 0])
        a[i] = v / max(np.linalg.norm(v), 1.0)
    return a


TEXEL = 31.0        # Einheiten je Texel: alles Gemalte mindestens 1,5 Texel breit, sonst wird es Griess


def _falten(q, warp, G, w_tex, seed, versatz=0.0):
    f1, f2, c1, ken = worley((q + warp) / G + versatz, seed)
    return np.clip(1.0 - (f2 - f1) * G / (w_tex * TEXEL), 0, 1), f1, c1, ken


def haut(pos, nor, bid, j, prims):
    """Grundhaut als gemalte Flaechen: lange Hautfalten quer zum Glied, wenige dicke Adern, einzelne Knoten. Keine feine Koernung, kein Gestein.
    Falte = dunkle Linie, darunter Schatten, darueber heller Grat (Maler-Licht von oben). Rueckgabe H x W x 3 float."""
    m = bid >= 0
    p = pos[m].astype(np.float64)
    n = nor[m].astype(np.float64)
    ax = achse_je_teil(j)
    ach = np.stack([ax[b] for b in range(len(BONES))])[bid[m]]
    s = (p * ach).sum(1, keepdims=True)
    STAUCH = 1.7
    q = p + STAUCH * s * ach                    # entlang des Glieds dichter: Falten laufen quer zum Glied
    warp = np.stack([fbm(p / 620.0 + o, 2, 3 + i) - 0.5 for i, o in enumerate((1.7, 5.3, 9.1))], 1) * 520.0
    L = np.array([0.30, -0.90, 0.30])
    L = L / np.linalg.norm(L)
    lt = L - (n @ L)[:, None] * n
    lt = lt / np.maximum(np.linalg.norm(lt, axis=1, keepdims=True), 1e-6)
    lq = lt + STAUCH * (lt * ach).sum(1, keepdims=True) * ach      # Lichtrichtung im gestauchten Raum
    G = 390.0
    d = 1.3 * TEXEL
    falte, f1, c1, ken = _falten(q, warp, G, 2.6, 5)
    fs, _, _, _ = _falten(q + d * lq, warp, G, 2.6, 5)     # Falte liegt lichtseitig: hier Schatten
    fl_, _, _, _ = _falten(q - d * lq, warp, G, 2.6, 5)    # Falte liegt schattenseitig: hier Grat
    falte2, g1, d1, _ = _falten(q * 0.9, -warp * 0.7, 230.0, 1.9, 15, 13.0)
    # Adern: lange Linien auf einer Hoehenlinie grober Felder
    ad = fbm((p + warp * 0.6) / 800.0 + 3.3, 2, 61)
    ader = np.clip(1.0 - np.abs(ad - 0.5) / 0.016, 0, 1) * (fbm(p / 900.0 + 8.8, 1, 71) > 0.40)
    # Knoten: einzelne Kuppen
    k1, _, kc, kk = worley(p / 520.0 + 9.0, 77)
    knoten = np.clip(1.0 - k1 * 520.0 / 88.0, 0, 1) * (kk > 0.70)
    kpil = np.clip(((p / 520.0 + 9.0 - kc) * lt).sum(1) * 520.0 / 88.0, -1, 1)
    dunkel = np.array([20, 9, 13], float)
    grund = np.array([42, 18, 26], float)
    hell = np.array([66, 36, 46], float)
    t0 = 0.47 + 0.28 * (fbm(p / 560.0, 2, 83) - 0.5) + 0.34 * fl_ - 0.24 * fs
    t0 = np.clip(t0, 0, 1)[:, None]
    c = np.where(t0 < 0.5, dunkel + (grund - dunkel) * (t0 / 0.5), grund + (hell - grund) * ((t0 - 0.5) / 0.5))
    c = c * (1 - 0.72 * falte[:, None]) * (1 - 0.30 * falte2[:, None])
    rot = np.array([86, 34, 44], float)
    a_ = (ader * (1 - falte))[:, None]
    c = c * (1 - a_) + rot * a_
    kn = (knoten ** 0.7)[:, None]
    kton = np.where(kpil[:, None] > 0.0, hell * 1.15, dunkel)
    c = c + (kton - c) * np.clip(kn * (0.55 + 0.45 * np.abs(kpil[:, None])), 0, 0.9)
    # Kontaktschatten an Nachbarteilen, Bauch dunkler
    ao = np.ones(len(p))
    for b, nm in enumerate(BONES):
        qq = bid[m] == b
        if not qq.any():
            continue
        dd = np.full(qq.sum(), 1e9)
        for o in nachbarn(nm):
            if nm in V.RUMPF and o in V.RUMPF:
                continue
            dd = np.minimum(dd, V.teil_sdf(o, prims, p[qq]))
        ao[qq] = 0.40 + 0.60 * np.clip(dd / 150.0, 0, 1)
    unten = np.clip(n[:, 1], 0, 1)
    c = c * ao[:, None] * (1 - 0.35 * unten[:, None])
    aus = np.zeros((TH, TW, 3), np.float32)
    aus[m] = c
    fl = dict(falte=falte, riss=falte2)
    return aus, fl


KINDER = {}
for _i, _k in enumerate(E.KNOCHEN):
    if _k[1] >= 0:
        KINDER.setdefault(E.KNOCHEN[_k[1]][0], []).append(_k[0])


def nachbarn(nm):
    p = E.KNOCHEN[IDX[nm]][1]
    n = list(KINDER.get(nm, []))
    if p >= 0:
        n.append(E.KNOCHEN[p][0])
    return sorted(set(n))


# ═══════════════════════════════════════════════════ Zeichnung
def farben():
    import form_pruefung as P
    M = P.formblatt()['zeichnung']['materialien']
    f = {k: np.array(v['oberflaeche_rgb'], float) for k, v in M.items()}
    f['z9'] = np.array([85, 80, 60], float)
    f['rosa_stelle'] = np.array([198, 128, 152], float)         # innerhalb der Palettenspanne, oberes Rot: im Video Pink, nicht Lavendel
    f['untere_streifen'] = np.array([205, 182, 196], float)     # unter der Obergrenze, Formblatt 2.1 "echte Farbe dunkler"
    f['weisskerne'] = np.array([232, 238, 240], float)
    f['band_dunkel'] = np.array([104, 118, 128], float)
    f['linie_hell'] = np.array([140, 146, 170], float)
    f['band_hell'] = np.array([146, 168, 182], float)           # oberes Ende der Palettenspanne: Z2, Z4, Z6a stehen in Bild 1 auf steilen Flaechen
    f['feine_q'] = np.array([90, 99, 114], float)               # unteres Ende: die Bogenlinien sind im Video duenn und matt
    f['feine_q3'] = np.array([90, 99, 114], float)
    f['linie_dunkel'] = np.array([92, 100, 118], float)
    f['lila'] = np.array([78, 66, 92], float)
    f['rot_dunkel'] = np.array([84, 48, 64], float)
    f['rosa_dunkel'] = np.array([150, 90, 112], float)
    f['rosa_rand'] = np.array([112, 58, 80], float)
    return f


class Karte:
    """Malkarte eines Bilds: Material-Index, in Video-Koordinaten. Spaeter gemalt liegt oben."""

    def __init__(self):
        self.a = np.zeros((480, 640), np.uint8)
        self.namen = [None]

    def nr(self, name):
        if name not in self.namen:
            self.namen.append(name)
        return self.namen.index(name)

    def poly(self, punkte, name, dicke=0):
        q = np.round(np.array(punkte, float)).astype(np.int32)
        cv2.fillPoly(self.a, [q], self.nr(name))
        if dicke:
            cv2.polylines(self.a, [q], True, self.nr(name), dicke)

    def linie(self, punkte, name, dicke):
        q = np.round(np.array(punkte, float)).astype(np.int32)
        cv2.polylines(self.a, [q], False, self.nr(name), dicke, cv2.LINE_8)

    def ellipse(self, m, ach, winkel, name):
        cv2.ellipse(self.a, (int(m[0]), int(m[1])), (int(ach[0]), int(ach[1])), winkel, 0, 360, self.nr(name), -1)


def spindel(m, laenge, breite, winkel, n=24):
    """Spitze Spindel um m, Winkel in Bildgrad (0 = nach rechts, positiv = gegen den Uhrzeigersinn im Bild)."""
    t = np.linspace(0, 2 * np.pi, n, endpoint=False)
    x = np.cos(t) * laenge
    y = np.sin(t) * breite * (np.abs(np.cos(t * 0.5)) ** 0.6 + np.sin(t * 0.5) ** 2 * 0.2)
    a = math.radians(winkel)
    return np.stack([m[0] + x * math.cos(a) + y * math.sin(a), m[1] - x * math.sin(a) + y * math.cos(a)], 1)


def karte_bild1():
    import form_pruefung as P
    M = P.merkmale()
    k = Karte()
    pz = lambda i: M[i]['polygon']      # noqa: E731
    # Mitteltoene zuerst
    k.poly(spindel((319, 240), 30, 15, 58), 'rosa_rand')
    k.poly(pz('A35'), 'rot_dunkel')
    k.poly(spindel((320, 238), 25, 10.5, 60), 'rosa_dunkel')
    k.poly(pz('A01'), 'rosa_stelle', 1)
    k.poly(pz('A02'), 'band_hell', 1)
    k.poly(pz('A03'), 'weisskerne', 1)
    k.poly(pz('A04'), 'band_hell', 1)
    k.poly(P.bauteil(1, 'dreieck_gelenk_A')['polygon'], 'helle_baender')
    k.poly(pz('A12'), 'linie_hell')
    k.poly(pz('A14'), 'band_hell', 1)
    k.poly(pz('A15'), 'lila', 1)
    k.poly(pz('A16'), 'lila', 1)
    for i in ('A20', 'A22', 'A23'):
        k.poly(pz(i), 'feine_linien', 1)
    k.poly(pz('A09'), 'linie_hell', 1)
    k.poly(pz('A08'), 'linie_dunkel', 1)
    k.poly(pz('A07'), 'feine_linien', 1)
    k.poly(P.bauteil(1, 'schraegstrich_beule')['polygon'], 'z9')
    # weitere gemessene Stellen ohne Deutung, Ton nach ihrer Helligkeit im Video
    for i in ('A05', 'A06', 'A10', 'A25', 'A27', 'A33'):
        k.poly(pz(i), 'lila')
    for i in ('A11', 'A13', 'A21', 'A24', 'A26', 'A32', 'A39'):
        k.poly(pz(i), 'linie_dunkel')
    return k


Q_FERN = ('feine_q',)


def karte_bild2():
    import form_pruefung as P
    M = P.merkmale()
    k = Karte()
    pz = lambda i: M[i]['polygon']      # noqa: E731
    # Haken: gefuelltes Rotbraun, heller Rand (B08, B09)
    k.poly([(208, 222), (243, 218), (246, 236), (224, 262), (208, 258)], 'rot_dunkel')
    k.poly(pz('B08'), 'helle_baender', 1)
    k.poly(pz('B09'), 'helle_baender', 1)
    k.poly(P.teil_b2('H')['polygon'], 'helle_baender')
    # Sichel, Innenbogen, Baender
    k.poly(P.bauteil(2, 'scheitel_sichel')['polygon'], 'rotbraun_mittelton')
    k.poly(pz('B04'), 'rotbraun_mittelton')
    k.poly(pz('B15'), 'rot_dunkel')
    k.poly(pz('B14'), 'rot_dunkel')
    # Zug Q als eine Linie (FORMBLATT 2.3)
    k.linie([(300, 192), (314, 189), (345, 189), (353, 194), (372, 201), (384, 218), (388, 236), (383, 260), (374, 278), (363, 290), (365, 302)], 'linie_dunkel', 3)
    k.poly(pz('B01'), 'feine_q', 1)
    k.poly(pz('B02'), 'feine_q', 1)
    k.poly(pz('B06'), 'feine_q3', 1)
    k.poly(P.bauteil(2, 'innenlinie_rechts_unten')['polygon'], 'feine_q')
    k.poly(pz('B07'), 'helle_baender', 1)
    for i in ('B10', 'B11', 'B12'):
        k.poly(pz(i), 'rot_dunkel')
    k.poly(P.teil_b2('G')['polygon'], 'untere_streifen')
    k.poly(P.teil_b2('P')['polygon'], 'weisskerne', 1)
    return k


def sicht(daten, pos, nor, bid, g, w, lage, gier, kam):
    z = tiefe_puffer(daten, g, w, lage, gier, kam)
    W = welt_pose(pos, bid, g, w, lage, gier)
    N = normale_pose(nor, bid, g, w, gier)
    b = FK.projiziere(kam, W.reshape(-1, 3))
    t = FK.tiefe(kam, W.reshape(-1, 3))
    ok = np.isfinite(b).all(1)
    ix = np.clip(np.nan_to_num(b[:, 0]).astype(int), 0, 639)
    iy = np.clip(np.nan_to_num(b[:, 1]).astype(int), 0, 479)
    v = np.asarray(kam['auge'], float) - W.reshape(-1, 3)
    v = v / np.maximum(np.linalg.norm(v, axis=1, keepdims=True), 1e-6)
    nv = (N.reshape(-1, 3) * v).sum(1)
    s = ok & (t <= z[iy, ix] + 70) & (bid.ravel() >= 0)
    weit = ok & (t <= z[iy, ix] + 420) & (bid.ravel() >= 0)     # Malreihenfolge statt Z-Puffer: im Spiel kann die zweite Schicht vorn liegen
    return s, nv, b, ix, iy, weit


def in_clut(rgb, m):
    """rgb H x W x 3 float -> Index + CLUT. 5 Bit je Kanal wie die Hardware; mehr als 255 Farben: k-means auf den 5-Bit-Werten."""
    q = np.clip(np.round(rgb / 8.0), 0, 31).astype(np.int32)
    code = q[..., 0] | (q[..., 1] << 5) | (q[..., 2] << 10)
    u, inv = np.unique(code[m], return_inverse=True)
    px = np.ones((TH, TW), np.uint8)
    if len(u) > 255:
        d = np.stack([(u & 31), (u >> 5) & 31, (u >> 10) & 31], 1).astype(np.float32)
        z = np.bincount(inv, minlength=len(u)).astype(np.float32)
        wd = np.repeat(d, np.minimum(z.astype(int), 40), axis=0)
        _, lab, c = cv2.kmeans(wd, 255, None, (cv2.TERM_CRITERIA_MAX_ITER, 30, 0.1), 3, cv2.KMEANS_PP_CENTERS)
        di = ((d[:, None, :] - c[None, :, :]) ** 2).sum(2).argmin(1)
        pal = [tuple(int(round(float(x))) * 8 for x in cc) for cc in c]
        px[m] = (di[inv] + 1).astype(np.uint8)
    else:
        pal = [(int(c & 31) * 8, int((c >> 5) & 31) * 8, int((c >> 10) & 31) * 8) for c in u]
        px[m] = (inv + 1).astype(np.uint8)
    farb = [(0, 0, 0)] + [tuple(min(248, x) for x in p) for p in pal]
    return px, FE.clut_aus_rgb(farb)


def male(daten, gr, j, lauf, prims=None):
    pos, nor, bid = texel_lage(daten)
    m = bid >= 0
    g = gr.astype(float)
    c, fl = haut(pos, nor, bid, j, prims)
    F = farben()
    w0 = np.zeros((len(BONES), 3))
    k1, k2 = karte_bild1(), karte_bild2()
    # Sicht
    s1, nv1, b1, ix1, iy1, _w1 = sicht(daten, pos, nor, bid, g, w0, np.array(j['lage1'], float), float(j['gier1']), FK.lade_kamera(1))
    s2, nv2, b2, ix2, iy2, w2_ = sicht(daten, pos, nor, bid, g, np.array(j['w2'], float), np.array(j['lage2'], float), float(j['gier2']), FK.lade_kamera(2))
    q = c.reshape(-1, 3)
    # Rauschen in der Zeichnung, damit sie zur Haut gehoert
    var = np.zeros(TH * TW, np.float32)
    var[m.ravel()] = (0.5 * (1 - fl['falte']) + 0.5 * (1 - fl['riss'])).astype(np.float32)
    K = np.zeros(TH * TW, np.int32)
    namen = ['haut']
    q3_nr = k2.namen.index('feine_q3')
    for (k, s, nv, ix, iy, grenze) in ((k2, s2, nv2, ix2, iy2, 0.10), (k1, s1, nv1, ix1, iy1, 0.12)):
        wert_ = k.a[iy, ix]
        tr = s & (nv > grenze) & (wert_ > 0)
        if k is k2:
            tr = tr | (w2_ & (nv > grenze) & (wert_ == q3_nr))
        for i in np.unique(wert_[tr]):
            nm = k.namen[int(i)]
            q_ = tr & (wert_ == i)
            if nm == 'feine_q3':
                print('Q3: Texel in Polygon sichtbar %d, in Bild 1 sichtbar %d, nv2 Mittel %.2f' % (int(q_.sum()), int((q_ & s1).sum()), float(nv[q_].mean()) if q_.any() else 0.0))
            if k is k2:
                # Bild 1 hat Vorrang, ausser wo es nur streifend sieht. Die Bogenlinien Q1, Q3, Q5 duerfen weiter hinein, sie liegen in Bild 1 nah am rechten Rand.
                q_ = q_ & ~(s1 & (nv1 > {'feine_q': 0.80, 'feine_q3': 1.1}.get(nm, 0.08)))
            if nm == 'z9':
                q_ = q_ & (nv > 0.35)       # Strich liegt nahe am Rand: flach gesehene Texel wuerden ihn von der Seite zur Flaeche machen
            f = F[nm]
            streu = 0.92 + 0.08 * var[q_][:, None] if nm in ('helle_baender', 'band_hell', 'feine_linien', 'rosa_stelle', 'weisskerne', 'feine_q', 'feine_q3', 'linie_hell') else 0.82 + 0.18 * var[q_][:, None]
            q[q_] = f * streu
            if nm not in namen:
                namen.append(nm)
            K[q_] = namen.index(nm)
    print('Texel je Material:', {n: int((K == i).sum()) for i, n in enumerate(namen)})
    c = q.reshape(TH, TW, 3)
    # Texel ausserhalb jeder Flaeche: naechsten Nachbarn uebernehmen, damit gerundete UV-Ecken nie Schwarz treffen
    from scipy.ndimage import distance_transform_edt
    ind = distance_transform_edt(~m, return_distances=False, return_indices=True)
    c = c[ind[0], ind[1]]
    c[TH - 4:, TW - 4:] = (14, 7, 9)     # Sammeltexel der Rumpfdeckel, stets dunkel
    info = dict(K=K.reshape(TH, TW)[ind[0], ind[1]], namen=namen, ind=ind, s1=s1.reshape(TH, TW), nv1=nv1.reshape(TH, TW), ix1=ix1.reshape(TH, TW), iy1=iy1.reshape(TH, TW),
                s2=s2.reshape(TH, TW), nv2=nv2.reshape(TH, TW), ix2=ix2.reshape(TH, TW), iy2=iy2.reshape(TH, TW), m=m)
    return c, info


def texel_im_bild(pfad_json, pfad_tim, n):
    """Welcher Texel steht je Pixel wirklich vorn, in der Malreihenfolge des Spiels (kein Z-Puffer). Zwei Render mit einer Textur, deren Index die Texel-Lage traegt."""
    import form_pruefung as P
    tab = [((i & 31) * 8, (i >> 5) * 8, 8) for i in range(256)]
    clut = FE.clut_aus_rgb(tab, transparent0=False)
    ys, xs = np.mgrid[0:TH, 0:TW]
    aus = []
    for feld in ((xs >> 1).astype(np.uint8), ys.astype(np.uint8)):
        open(pfad_tim, 'wb').write(FE.tim_schreiben(feld, [clut]))
        _s, alb, mm, _k = P.Kandidat(pfad_json).rendern(n)
        aus.append((alb[..., 0].astype(np.int32) >> 3) | ((alb[..., 1].astype(np.int32) >> 3) << 5))
    return aus[0] * 2, aus[1], mm


def nachmalen(c, pfad_json, pfad_tim):
    """Q3 liegt im Spiel auf der Schicht, die in der Malreihenfolge vorn steht: direkt ueber die Texel-Karte setzen."""
    import form_pruefung as P
    u, v, mm = texel_im_bild(pfad_json, pfad_tim, 2)
    p = P.poly_maske(P.merkmale()['B06']['polygon']) & mm
    ys, xs = np.nonzero(p)
    f = farben()['feine_q3']
    for du in (0, 1):
        c[v[ys, xs], np.minimum(u[ys, xs] + du, TW - 1)] = f
    print('Q3 nachgemalt: %d Pixel' % len(ys))
    return c


def feinton(c, info, pfad_json, pfad_tim, runden=3):
    """Abgleich gegen die beiden Standbilder: Helligkeit der Haut Bild gegen Bild, grob (sigma 3 px), nur Vervielfachen, Farbton bleibt. Wirkt nur auf Haut und nicht gepruefte Mitteltoene.
    Das holt die grossen Hell-Dunkel-Flaechen des Videos in die Textur, die Malerei bleibt scharf. Video-Eigenheiten kommen nicht mit: Vergleich laeuft durch video_filter."""
    import form_pruefung as P
    import form_video as FV
    K, namen = info['K'], info['namen']
    frei = np.isin(K, [i for i, n in enumerate(namen) if n in ('haut', 'lila', 'rot_dunkel', 'linie_dunkel', 'rosa_rand')])
    faktor = np.ones((TH, TW), np.float32)
    bild1 = None
    for r in range(runden):
        cc = c * faktor[..., None]
        cc = np.minimum(cc, np.where(((K == 0) & info['m'])[..., None], np.array([78.0, 44.0, 54.0]), 255.0))
        px, clut = in_clut(cc, np.ones((TH, TW), bool))
        open(pfad_tim, 'wb').write(FE.tim_schreiben(px, [clut]))
        k = P.Kandidat(pfad_json)
        schritt = np.ones((TH, TW), np.float32)
        gew = np.zeros((TH, TW), np.float32)
        for n in (2, 1):
            sauber, alb, mm, _ = k.rendern(n)
            gef = FV.video_filter(sauber, bild=n, entzerren=False, rauschen=False)
            o = P.original(n).astype(np.float32) @ YK
            g_ = gef.astype(np.float32) @ YK
            inn = cv2.erode(mm.astype(np.uint8), np.ones((9, 9), np.uint8)).astype(np.float32)
            hg = cv2.GaussianBlur(inn, (0, 0), 3.0) + 1e-3
            R = (cv2.GaussianBlur(o * inn, (0, 0), 3.0) / hg + 6.0) / (cv2.GaussianBlur(g_ * inn, (0, 0), 3.0) / hg + 6.0)
            s, nv, ix, iy = info['s%d' % n], info['nv%d' % n], info['ix%d' % n], info['iy%d' % n]
            ok = s & (nv > 0.3) & (inn[iy, ix] > 0)
            schritt[ok] = R[iy[ok], ix[ok]]         # Bild 1 zuletzt: Vorrang
        schritt = np.clip(schritt, 0.7, 1.45) ** 0.6
        faktor = np.where(frei & info['m'], np.clip(faktor * schritt, 0.55, 2.0), faktor)
        print('Feinton Runde %d: Faktor Mittel %.3f, Spanne %.2f..%.2f' % (r, float(faktor[info['m']].mean()), float(faktor[info['m']].min()), float(faktor[info['m']].max())))
    cc = c * faktor[..., None]
    cc = np.minimum(cc, np.where(((K == 0) & info['m'])[..., None], np.array([78.0, 44.0, 54.0]), 255.0))
    return cc
