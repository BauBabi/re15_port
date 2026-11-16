#!/usr/bin/env python3
"""K5g3 Stufe 'zaehne_vorn' - Vorderzaehne oben und unten um die Schnauzenspitze (Nutzer 16.11.2026: "Warum hat denn der vierbeiner Birkin
ganz vorne im Maul keine Zaehne?", Rueckfrage beantwortet mit "Oben und unten": echte Zaehne wie die seitlichen, rund um die Spitze).

Ursache (Untersuchung riesenform_zaehne/BERICHT.md): k5_kopf.py setzt die Zaehne nur in die zwei Seitenreihen und hoert vor der Spitze auf
(k5_kopf.py:121 x1 = L - 120, :160 x = 260 + t (Lj - 140 - 260)). Diese Stufe setzt die beiden Reihen um den Schnauzenbogen bis zur Mitte fort.

Port-Gestaltung (die Riesenform gibt es auf keiner Disc): keine @0x-Adresse; jede Zahl ist hier mit ihrer Regel/Messung belegt.

Bauteil   k_form.zahn (dreiseitige Pyramide, 4 Ecken, 3 Dreiecke) - dasselbe Bauteil wie die vorhandenen 34 Zaehne. Ecken/Normalen werden wie
          beim Kopfbau (k_bau.neu_netz -> form_emd.netz: ecke = round, normale = 4096 x Einheit) und die Windung wie form_emd.orientiere
          gesetzt. pruef_alt() baut mit DIESEM Weg alle 34 vorhandenen Zaehne aus der K5c-Vorgabe nach und vergleicht Ecken und Normalen
          ganzzahlig mit dem Netz (sonst Abbruch) - die neuen Zaehne entstehen also genau wie die alten.
Material  UV/CLUT/Seite je Dreieck vom vordersten vorhandenen Zahn derselben Seite desselben Netzes (Spender), Dreieck fuer Dreieck in
          derselben Lage (Ecken-Rollen G0/G1/G2/Spitze). Alle 34 Zaehne tragen dieselbe Zahnzeile 2 'zahn0' (CLUT 30848) mit drei UV-Mustern,
          eines je Dreieckslage; TIM bleibt bytegleich.
Knochen   oben Netz 22 (Kopf/Oberkiefer), unten Netz 23 (Unterkiefer). Nur ANGEHAENGT: alte Ecken/Normalen/Flaechen bleiben an ihrer Stelle.
Lage      Kopfvorgabe K5c (K5/K5c/kopf.json, sha256 geprueft) - aus ihr sind Kopf und Kiefer des Spiel-Assets gebaut (gemessen: alle
          vorderen Ecken von 22/23 == k5_kopf.kopf/kiefer(K5c) +-0,75).

Regeln der neuen Zaehne (je Reihe; i = fortlaufende Nummer der Reihe, die alten Zaehne sind i = 0..n-1):
 R1 Zahnlinie: ab dem vordersten alten Zahn (O1 bzw. U1) der Lippenumriss aus den Stationen von k5_kopf (stationen / kiefer_stationen)
    bis zur letzten Station L bzw. Lj und deren Vorderkante quer zur Mitte, nach innen versetzt um den Abstand der alten Reihe an O1/U1
    (oben 0,07 w = 18,6, unten 0,12 w_k = 29,0; k5_kopf.py:126/:164 z = 0,93 w / 0,88 w_k), Ecken auf Gehrung, Ende auf der Mittellinie.
    Der Faecher vor L/Lj (40 lang) ist Nase/Kinn, nicht Lippe (Messbild m1: dort sassen die Mittelzaehne auf der Nasenvorderseite).
 R2 Abstand: Abstand der beiden vordersten alten Zaehne in der Draufsicht (O1-O2 51,2, U1-U2 103,3); Zahl der Abstaende = gerundete
    Linienlaenge / Abstand, gleichmaessig verteilt; der letzte Zahn steht in der Mitte (z = 0), links und rechts spiegelgleich
    (G4: die Reihe laeuft ueber die ganze Breite durch, BERICHT.md 4). Ergebnis oben 6 Abstaende (5 je Seite + Mitte = 11), unten 3 (2 + 1 = 5).
 R3 Hoehe der Zahnbasis: oben Unterseite des Kopfrohrs an (x, z) minus die Wurzeltiefe von O1 (6,9, wurzel_o1); unten Oberseite des
    Kieferrohrs an (x, z) + 15 wie k5_kopf (yr + 15). (Erste Fassung "Unterkante - 30": Wurzeln an der Vorderkante 30 tief in der Lippe,
    Maler-Scan probe3 2-3 x so viele falsch gemalte Pixel wie die alten.)
 R4 Groesse: Laenge und Grundbreite nach dem Muster der alten Reihe (oben gross/klein nach i % 3 == 0, unten lang/kurz nach i % 2) mal f;
    f faellt linear ueber die Linie vom vordersten alten Zahn (1) bis zur Mitte auf 0,708 = Hoehe der Schnauze an der letzten Station L
    (237,8) / an O1 (335,9): die Schnauze wird zur Spitze niedriger, die Zaehne im selben Verhaeltnis kleiner (beide Reihen).
 R5 Richtung wie die alte Reihe, auf die Linie gedreht: oben 0,12 t + 1 y + 0,22 n, unten 0,1 t - 1 y + 0,18 n (k5_kopf.py:128/:166);
    t = Linie nach vorn/zur Mitte, n = nach aussen (Draufsicht); in der Mitte t = 0, n = vorn.
 R6 Kein Durchstich: Laenge = hoechstens die Nennlaenge (R4), gekappt auf die groesste Laenge, mit der der Zahn in KEINEM Key (alle 771,
    Ganzzahl-Pose wie das Spiel) - um RAND = 2 verlaengert - im Rohr des Gegenkiefers liegt oder hindurchtritt, als unterer vor der Nase
    ueber der Mundlinie steht, oder tiefer in einen Zahn der Gegenreihe dringt als die alten Zaehne untereinander im selben Key.
    Links/rechts dieselbe (kleinere) Laenge. Die Kappen stehen mit Protokoll in generator/k5g3_zaehne_vorn.json (laengen_eichen; sha in
    bau.json); geprueft mit kf_k5g3_zaehne_scan.py und riegel_k5g3_zaehne.py.
"""
import copy
import hashlib
import json
import math
import os
import sys

sys.dont_write_bytecode = True
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import numpy as np  # noqa: E402
import k5_kopf as KO  # noqa: E402

KF = KO.KF
K5 = os.path.dirname(HIER)
WT = os.path.normpath(os.path.join(K5, '..', '..', '..', '..', '..'))
sys.path.insert(0, os.path.join(WT, 're15_port', 'tools'))
import form_emd as FE  # noqa: E402

KOPF_JSON = os.path.join(K5, 'K5c', 'kopf.json')
KOPF_JSON_SHA = 'e064f8e98dba1f9746c8d048a320da608cbcee967fb5f100d5b034d674403937'   # K5c/kopf.json (Zeilenende LF), Commit 6297650e1
OBEN, UNTEN = 22, 23
ZAHN_ZEILE = 2                # bau.json zeilen[2] 'zahn0'
LAENGEN = os.path.join(HIER, 'k5g3_zaehne_vorn.json')      # R6-Kappen (laengen_eichen), sha256 in bau.json
POSEN_EMD = os.path.join(K5, 'k5g', 'K5g3', 'geo', 'BAHNSTEIG_FORM.EMD')   # Posen (Sektionen 0-6 == Spiel-Asset, Riegel R6/TG)
RAND = 2.0                    # R6: Abstand der Spitze zur Gegenflaeche - Ganzzahl-Rundung: Ecke +-0,5, Pose >> 12 je Stufe bis 1


def sha(b):
    return hashlib.sha256(b).hexdigest()


def vorgabe():
    b = open(KOPF_JSON, 'rb').read().replace(b'\r\n', b'\n')
    if KOPF_JSON_SHA and sha(b) != KOPF_JSON_SHA:
        raise SystemExit('K5c/kopf.json hat nicht die sha256 der Vorgabe (%s)' % KOPF_JSON_SHA[:16])
    return json.loads(b.decode('utf-8'))['kopf']


# ------------------------------------------------------------------ alte Reihen (wie k5_kopf.kopf / kiefer, nur die Zaehne)
def alte_oben(p):
    xs, ws, _yts, _ybs = KO.stationen(p)
    n = int(p['zaehne'])
    x0, x1 = p['x_auge'] + 120.0, p['L'] - 120.0
    out = []
    for vz in (-1, 1):
        for i in range(n):
            x = x0 + (i + 0.5) / n * (x1 - x0)
            w = np.interp(x, xs, ws)
            gross = i in (2, 3, n - 3) or (i % 3 == 0)
            lang = p['zahn_lang'] * p['zahn_oben'] * (1.3 if gross else 0.8)
            out.append(dict(netz=OBEN, seite=vz, i=i, basis=(x, p['y_gaumen'] - 30, vz * 0.93 * w), richtung=(0.12, 1, vz * 0.22), lang=lang,
                            breit=40.0 if gross else 32.0))
    return out


def alte_unten(p):
    xs, ws, _ybs, _tf, yr, Lj = KO.kiefer_stationen(p)
    n = int(p['uk_zaehne'])
    out = []
    for vz in (-1, 1):
        for i in range(n):
            x = 260 + (i + 0.5) / n * (Lj - 140 - 260)
            w = np.interp(x, xs, ws)
            lang = p['uk_zahn_lang'] * (1.0 if i % 2 == 0 else 0.8)
            out.append(dict(netz=UNTEN, seite=vz, i=i, basis=(x, yr + 15, vz * 0.88 * w), richtung=(0.1, -1, vz * 0.18), lang=lang, breit=46.0))
    return out


def zahn_teile(z):
    """k_form.zahn -> Ecken (4 x 3 float), Flaechen (Index-Tripel), Normalen (4 x 3 Einheit)."""
    o = KF.zahn(z['basis'], z['richtung'], z['lang'], z['breit'], KF.ATLAS['zahn'])
    P, F, _T, N = o.arrays()
    return P, F, N


# ------------------------------------------------------------------ Huellen (Rohre ohne Zaehne/Augen) fuer Hoehe und Pruefung
def huelle_oben(p):
    """Schaedel-/Oberkieferrohr wie k5_kopf.kopf (ohne Augenhoecker, Augen, Zaehne): Ecken (n x 3), Dreiecke (m x 3)."""
    xs, ws, yts, ybs = KO.stationen(p)
    Z = np.array([[x, 0.0, 0.0] for x in xs])
    ea = np.tile([0, 0, 1.0], (len(xs), 1))
    eb = np.tile([0, 1.0, 0], (len(xs), 1))
    R = np.array([KO._ring(w, a, b) for w, a, b in zip(ws, yts, ybs)])
    o = KF.rohr(Z, ea, eb, R, KF.ATLAS['kopf'], spitze0=(xs[0] - 120.0, (yts[0] + ybs[0]) / 2, 0.0),
                spitze1=(p['L'] + 40.0, (yts[-1] + ybs[-1]) / 2 + 30, 0.0), bogen=True)
    return _tris(o)


def huelle_unten(p):
    """Unterkieferrohr wie k5_kopf.kiefer (ohne Zaehne), im Raum von Knochen 23."""
    xs, ws, ybs, tf, yr, Lj = KO.kiefer_stationen(p)
    Z = np.array([[x, 0.0, 0.0] for x in xs])
    ea = np.tile([0, 0, 1.0], (len(xs), 1))
    eb = np.tile([0, 1.0, 0], (len(xs), 1))
    R = np.array([KF.ring_kiefer(w, yr, b, t) for w, b, t in zip(ws, ybs, tf)])
    o = KF.rohr(Z, ea, eb, R, KF.ATLAS['kiefer'], spitze0=(-400.0, yr + 120, 0.0), spitze1=(Lj + 40.0, yr + 30, 0.0), bogen=True)
    return _tris(o)


def _tris(o):
    P, F, _T, _N = o.arrays()
    T = []
    for f in F:
        T += [[f[0], f[1], f[2]]] if len(f) == 3 else [[f[0], f[1], f[2]], [f[0], f[2], f[3]]]
    return P, np.array(T, int)


def senkrecht(P, T, x, z):
    """y-Werte, an denen die senkrechte Gerade (x, z) die Huelle schneidet (aufsteigend)."""
    A, B, C = P[T[:, 0]], P[T[:, 1]], P[T[:, 2]]
    out = []
    for a, b, c in zip(A, B, C):
        d = (b[0] - a[0]) * (c[2] - a[2]) - (c[0] - a[0]) * (b[2] - a[2])
        if abs(d) < 1e-12:
            continue
        u = ((x - a[0]) * (c[2] - a[2]) - (c[0] - a[0]) * (z - a[2])) / d
        v = ((b[0] - a[0]) * (z - a[2]) - (x - a[0]) * (b[2] - a[2])) / d
        if u >= -1e-9 and v >= -1e-9 and u + v <= 1 + 1e-9:
            out.append(a[1] + u * (b[1] - a[1]) + v * (c[1] - a[1]))
    return sorted(out)


# ------------------------------------------------------------------ Zahnlinien bis zur Spitze (R1, R3)
def umriss(p, netz):
    """Lippenumriss (Draufsicht, z >= 0): die Stationen bis zur letzten (L bzw. Lj), dann deren Vorderkante quer zur Mitte (x = L bzw. Lj).
    Der Faecher davor (spitze1, 40 lang) steigt oben steil zur Nasenspitze an (Unterkante 220 -> 131) und faellt unten zum Kinn ab: das ist
    Nase/Kinn, nicht Lippe. Messbild m1 (Linie bis spitze1): die mittleren Zaehne sassen auf der Nasenvorderseite ueber der Mundlinie."""
    if netz == OBEN:
        xs, ws, _yts, _ybs = KO.stationen(p)
    else:
        xs, ws, _ybs, _tf, _yr, _Lj = KO.kiefer_stationen(p)
    return np.array(list(xs) + [xs[-1]]), np.array(list(ws) + [0.0])


def _schnitt(p0, t0, p1, t1):
    M = np.array([[t0[0], -t1[0]], [t0[1], -t1[1]]])
    a = np.linalg.solve(M, p1 - p0)
    return p0 + a[0] * t0


def zahnlinie(p, netz, x_start, faktor):
    """R1: Zahnlinie ab dem vordersten alten Zahn (x_start) bis zur Mitte: Lippenumriss um d = (1 - faktor) w(x_start) nach innen
    versetzt (an x_start der Abstand der alten Reihe zur Lippe), Ecken auf Gehrung, Ende auf der Mittellinie z = 0. Polylinie (k x 2)."""
    X, W = umriss(p, netz)
    w0 = float(np.interp(x_start, X, W))
    d = (1.0 - faktor) * w0
    pts = np.array([(x_start, w0)] + [(x, w) for x, w in zip(X, W) if x > x_start])
    lin = []
    for (x0, w0_), (x1, w1) in zip(pts[:-1], pts[1:]):
        t = np.array([x1 - x0, w1 - w0_])
        t /= np.linalg.norm(t)
        n_in = np.array([t[1], -t[0]])
        if n_in[1] > 0:                                           # nach innen = zur Mittellinie
            n_in = -n_in
        lin.append((np.array([x0, w0_]) + d * n_in, t))
    out = [np.array([x_start, faktor * w0])]                     # Start = der vorderste alte Zahn selbst (z = faktor w)
    for (p0, t0), (p1, t1) in zip(lin[:-1], lin[1:]):
        q = _schnitt(p0, t0, p1, t1)
        if q[0] > x_start:                                       # Gehrungspunkte hinter dem Start (Station knapp vor O1) entfallen
            out.append(q)
    pe, te = lin[-1]
    out.append(_schnitt(pe, te, np.array([0.0, 0.0]), np.array([1.0, 0.0])))
    return np.array(out), d


def wurzel_o1(p):
    """Wurzeltiefe des vordersten alten oberen Zahns O1: Unterseite des Kopfrohrs senkrecht ueber/unter seiner Basismitte minus Basis-y
    (gemessen 6,9; die alten oberen liegen zwischen -10,8 und +7,5, weil k5_kopf y_gaumen - 30 fest nimmt und die Lippe schraeg ist)."""
    z = alte_oben(p)[-1]
    P, F = huelle_oben(p)
    return senkrecht(P, F, z['basis'][0], z['basis'][2])[-1] - z['basis'][1]


def basis_y(p, netz, x, z, H):
    """R3: oben Unterseite des Kopfrohrs an (x, z) minus die Wurzeltiefe von O1 (wurzel_o1, 6,9). Erst Linie/Hoehe 'Unterkante - 30' wie
    k5_kopf: an der Vorderkante 30 tief in der Lippe -> Messung Maler-Scan probe3: neue Zaehne 2-3 x so viele falsch gemalte Pixel wie die
    alten (Wurzel ueber die Lippe gemalt, maldbg Key 551). Unten Oberseite des Kieferrohrs an (x, z) (Rand, Mundboden oder Kegel) + 15 wie
    k5_kopf (yr + 15; dort 15 unter dem flachen Rand)."""
    if netz == OBEN:
        P, F = huelle_oben(p)
        ys = senkrecht(P, F, x, z)
        if not ys:
            raise SystemExit('Oberkiefer: keine Unterseite bei (%.1f, %.1f)' % (x, z))
        return ys[-1] - wurzel_o1(p)
    ys = senkrecht(H[0], H[1], x, z)
    if not ys:
        raise SystemExit('Unterkiefer: keine Oberseite bei (%.1f, %.1f)' % (x, z))
    return ys[0] + 15.0


def name(z):
    return '%d|%+d|%d' % (z['netz'], z['seite'], z['i'])


def kappen_laden():
    """R6: Laengen-Kappen aus generator/k5g3_zaehne_vorn.json (geschrieben von laengen_eichen; fehlt die Datei: keine Kappen)."""
    if not os.path.exists(LAENGEN):
        return {}
    return {k: float(v) for k, v in json.load(open(LAENGEN, encoding='utf-8'))['lang_max'].items()}


def neue_reihe(p, netz, kappen=None):
    """Neue Zaehne einer Reihe (beide Seiten + Mitte). Rueckgabe Liste dict wie alte_*, dazu 'f', 'bogen', 'lang_nenn'; Bericht.
    kappen: {name: hoechste Laenge} (R6); None = aus LAENGEN."""
    if kappen is None:
        kappen = kappen_laden()
    xs, ws, yts, ybs = KO.stationen(p)
    h_vorn = ybs[-1] - yts[-1]                                                                   # R4: Schnauzenhoehe an der Station L
    if netz == OBEN:
        alt = [z for z in alte_oben(p) if z['seite'] == 1]
        lang_f = lambda i: p['zahn_lang'] * p['zahn_oben'] * (1.3 if i % 3 == 0 else 0.8)
        breit_f = lambda i: 40.0 if i % 3 == 0 else 32.0
        a_t, a_y, a_n, faktor = 0.12, 1.0, 0.22, 0.93
        H = None
    else:
        alt = [z for z in alte_unten(p) if z['seite'] == 1]
        lang_f = lambda i: p['uk_zahn_lang'] * (1.0 if i % 2 == 0 else 0.8)
        breit_f = lambda i: 46.0
        a_t, a_y, a_n, faktor = 0.1, -1.0, 0.18, 0.88
        H = huelle_unten(p)
    x_o1 = alte_oben(p)[-1]['basis'][0]
    h_alt = float(np.interp(x_o1, xs, np.array(ybs) - np.array(yts)))                           # an O1
    F_MITTE = h_vorn / h_alt
    z1, z2 = alt[-1], alt[-2]
    abstand = math.hypot(z1['basis'][0] - z2['basis'][0], z1['basis'][2] - z2['basis'][2])     # R2
    L, d = zahnlinie(p, netz, z1['basis'][0], faktor)
    seg = np.hypot(np.diff(L[:, 0]), np.diff(L[:, 1]))
    s = np.concatenate([[0.0], np.cumsum(seg)])
    S = float(s[-1])
    k = max(1, int(round(S / abstand)))
    out = []
    for j in range(1, k + 1):
        sj = S * j / k
        q = int(min(np.searchsorted(s, sj, side='right') - 1, len(seg) - 1))
        t2 = (L[q + 1] - L[q]) / seg[q]
        x, z = L[q] + (sj - s[q]) * t2
        mitte = j == k
        f = 1.0 + (F_MITTE - 1.0) * sj / S                                                        # R4
        i = len(alt) - 1 + j
        y = basis_y(p, netz, x, 0.0 if mitte else z, H)
        n2 = np.array([-t2[1], t2[0]])
        if n2[1] < 0:
            n2 = -n2
        if np.dot(n2, [1.0, 0.0]) < 0 and abs(n2[1]) < 1e-9:
            n2 = -n2
        for vz in ((1,) if mitte else (-1, 1)):
            tv = np.zeros(3) if mitte else np.array([t2[0], 0.0, vz * t2[1]])
            nv = np.array([1.0, 0.0, 0.0]) if mitte else np.array([n2[0], 0.0, vz * n2[1]])
            r = a_t * tv + np.array([0.0, a_y, 0.0]) + a_n * nv                                 # R5
            z_ = dict(netz=netz, seite=0 if mitte else vz, i=i, basis=(float(x), y, 0.0 if mitte else float(vz * z)),
                      richtung=tuple(float(c) for c in r), lang_nenn=lang_f(i) * f, breit=breit_f(i) * f, f=f, bogen=float(sj))
            z_['lang'] = min(z_['lang_nenn'], kappen.get(name(z_), float('inf')))
            out.append(z_)
    return out, dict(abstand=abstand, strecke=S, versatz=d, f_mitte=F_MITTE, h_vorn=h_vorn, h_alt=h_alt, n_abstaende=k,
                     linie=[[round(a, 1), round(b, 1)] for a, b in L])


# ------------------------------------------------------------------ Einsetzen
def _rollen(m, e, P):
    """Ecken-Indizes e eines Netzes -> Rolle 0..3 (Index in P) ueber gleiche Ganzzahl-Lage."""
    rq = [tuple(int(round(c)) for c in q) for q in P]
    return [rq.index(tuple(m['ecken'][i][:3])) for i in e]


def finde_zahn(m, z):
    """Flaechen eines vorhandenen Zahns im Netz (Ecken == gerundete k_form.zahn-Ecken, Normalen == 4096 x Einheit). Rueckgabe Liste
    (flaechen-index, rollen) in Netz-Reihenfolge, Ecken-Indizes, Normalen-Indizes."""
    P, F, N = zahn_teile(z)
    rq = [tuple(int(round(c)) for c in q) for q in P]
    ecken = {}
    for idx, e in enumerate(m['ecken']):
        t = tuple(e[:3])
        if t in rq:
            ecken.setdefault(rq.index(t), []).append(idx)
    if len(ecken) != 4:
        raise SystemExit('alter Zahn %s nicht im Netz %d (Ecken gefunden %s)' % (z['basis'], z['netz'], sorted(ecken)))
    alle = set(i for v in ecken.values() for i in v)
    fl = [(fi, _rollen(m, f['e'], P)) for fi, f in enumerate(m['dreiecke']) if set(f['e']) <= alle]
    if len(fl) != 3:
        raise SystemExit('alter Zahn %s: %d Dreiecke statt 3' % (z['basis'], len(fl)))
    return fl, P, N


def _normale_q12(v):
    v = np.asarray(v, float)
    v = v / (np.linalg.norm(v) or 1.0) * 4096.0
    return [int(round(c)) for c in v]


def pruef_alt(form, p):
    """Baut alle vorhandenen Zaehne mit zahn_teile nach: Ecken (gerundet) und Normalen (4096 x Einheit, gerundet) muessen die des Netzes sein,
    das Rollenmuster der drei Dreiecke ist bei allen gleich. Rueckgabe Muster je Netz {netz: [(rollen, uv, clut, page) x 3]} vom Spender
    (vorderster Zahn, Seite +1) und die Zahl der geprueften Zaehne."""
    spender, n = {}, 0
    for z in alte_oben(p) + alte_unten(p):
        m = form['meshes'][z['netz']]
        fl, P, N = finde_zahn(m, z)
        nq = [_normale_q12(v) for v in N]
        for fi, rollen in fl:
            f = m['dreiecke'][fi]
            for k, ni in enumerate(f['n']):
                if list(m['normalen'][ni][:3]) != nq[rollen[k]]:
                    raise SystemExit('alter Zahn %s: Normale %s != nachgebaut %s' % (z['basis'], m['normalen'][ni], nq[rollen[k]]))
        n += 1
        if z['seite'] == 1 and z['i'] == (int(p['zaehne']) if z['netz'] == OBEN else int(p['uk_zaehne'])) - 1:
            spender[z['netz']] = [(rollen, copy.deepcopy(m['dreiecke'][fi]['uv']), int(m['dreiecke'][fi]['clut']), int(m['dreiecke'][fi]['page']))
                                  for fi, rollen in sorted(fl, key=lambda a: sorted(a[1]))]
    return spender, n


def einsetzen(form, p, neue=None):
    """Haengt die neuen Zaehne an Netz 22/23 an (Ecken, Normalen, 3 Dreiecke je Zahn). Alte Daten bleiben unberuehrt. Rueckgabe Bericht."""
    spender, n_alt = pruef_alt(form, p)
    if neue is None:
        neue = []
        for netz in (OBEN, UNTEN):
            neue += neue_reihe(p, netz)[0]
    ber = dict(alte_zaehne_geprueft=n_alt, neu={})
    for z in neue:
        m = form['meshes'][z['netz']]
        P, F, N = zahn_teile(z)
        v0, n0 = len(m['ecken']), len(m['normalen'])
        for q in P:
            FE.ecke(m, q)
        for q in N:
            FE.normale(m, q)
        # Rollen-Muster des Spenders: dieselben drei Dreiecke (Ecken-Rollen) mit derselben UV/CLUT/Seite
        rq = [tuple(int(round(c)) for c in q) for q in P]
        for rollen, uv, clut, page in spender[z['netz']]:
            f = dict(e=[v0 + r for r in rollen], n=[n0 + r for r in rollen], uv=copy.deepcopy(uv), clut=clut, page=page)
            if FE._falsch_gewunden(m, f):
                raise SystemExit('neuer Zahn %s: Windung gegen die Normalen (Muster des Spenders passt nicht)' % (z['basis'],))
            m['dreiecke'].append(f)
        if len(set(rq)) != 4:
            raise SystemExit('neuer Zahn %s: Ecken fallen zusammen' % (z['basis'],))
        ber['neu'].setdefault(str(z['netz']), 0)
        ber['neu'][str(z['netz'])] += 1
    return ber


def bericht_reihen(p):
    out = {}
    for netz in (OBEN, UNTEN):
        zz, info = neue_reihe(p, netz)
        out[str(netz)] = dict(info=info, zaehne=[dict(seite=z['seite'], i=z['i'], basis=[round(c, 1) for c in z['basis']],
                                                     richtung=[round(c, 3) for c in z['richtung']], lang=round(z['lang'], 1),
                                                     breit=round(z['breit'], 1), f=round(z['f'], 3), bogen=round(z['bogen'], 1)) for z in zz])
    return out


# ------------------------------------------------------------------ R6: Laengen-Kappen ueber alle Keys
class Keypose:
    """Alle Keys der Posen-EMD (erstes Bild je Key, Ganzzahl wie das Spiel): Unterkiefer im Kopfraum; Rohre gerundet."""

    def __init__(self, p):
        import kf_k5g2_geo as GG
        import kf_k5g3_zaehne_scan as S
        self.S = S
        emd = open(POSEN_EMD, 'rb').read()
        mo = FE.modell(emd, S.N52.TIM0)
        cb, R, T = GG.posen(mo)
        first = {}
        for i, (c, b, k) in enumerate(cb):
            first.setdefault(int(k), i)
        self.keys = sorted(first)
        self.R = {k: R[first[k]] for k in self.keys}
        self.T = {k: T[first[k]] for k in self.keys}
        self.emd_sha = sha(emd)
        P22, F22 = huelle_oben(p)
        P23, F23 = huelle_unten(p)
        self.H = {OBEN: (np.rint(P22).astype(np.int64), F22), UNTEN: (np.rint(P23).astype(np.int64), F23)}
        self.hw = {}
        # Mundlinie der Schnauze von vorn: Unterseite des Kopfrohrs knapp hinter der Vorderkante (x = L - 0,5) je |z| (R6 d)
        self.x_vorn = float(p['L'])
        zs = np.linspace(0.0, float(KO.stationen(p)[1][-1]), 48)
        ys = [senkrecht(P22, F22, self.x_vorn - 0.5, zz) for zz in zs]
        self.lippe = (np.array([zz for zz, y in zip(zs, ys) if y]), np.array([y[-1] for y in ys if y]))

    def welt(self, k, netz, V):
        return self.S._kopfraum(self.R[k], self.T[k], netz, np.asarray(V, np.int64))[0]

    def gegen(self, k, netz):
        """Dreiecke des Gegenrohrs im Kopfraum (Key k) fuer einen Zahn des Netzes netz."""
        g = UNTEN if netz == OBEN else OBEN
        key = (k, g)
        if key not in self.hw:
            P, F = self.H[g]
            self.hw[key] = self.welt(k, g, P)[F]
        return self.hw[key]


def _ecken(z, lang=None):
    zz = dict(z)
    if lang is not None:
        zz['lang'] = lang
    P, _F, _N = zahn_teile(zz)
    return np.rint(P).astype(np.int64)


def zahn_zu_zahn(Ea, Eb, S):
    """Groesste Eindringtiefe der Probepunkte eines Zahns in die (konvexe) Pyramide des anderen, beidseitig."""
    d = 0.0
    for P_, Ew in ((S._proben(Ea).reshape(-1, 3), Eb), (S._proben(Eb).reshape(-1, 3), Ea)):
        if (P_.min(0) > Ew.max(0)).any() or (P_.max(0) < Ew.min(0)).any():
            continue
        ab = np.array([P_ @ n - dd for n, dd in S._ebenen(Ew)])
        dr = (ab < 0).all(0)
        if dr.any():
            d = max(d, float((-ab[:, dr]).min(0).max()))
    return d


def verletzung(KP, z, lang, gegenreihe, ref_zz, alle=True):
    """R6 fuer einen neuen Zahn der Laenge lang ueber alle Keys. Rueckgabe Liste (key, art, wert):
      'austritt'  Zahn um RAND verlaengert tritt in das Gegenrohr ein und wieder aus (Durchstich),
      'drin'      Zahn um RAND verlaengert liegt im Rohr des Gegenkiefers. Die alten oberen tun das in keinem Key (Scan 0,0); die alten
                  unteren stecken bis 117,6 tief im Kopf - erster Stand liess das fuer die neuen unteren zu (bis 29,7): Maler-Scan probe4
                  'fremd' (Zahnspitze durch die Schnauze gemalt) nah_schraeg_r 32 Pixel neu gegen 6 alt, Zahn 23|-1|6 -> fuer beide Reihen verboten,
      'zahn'      Eindringen in einen Zahn der Gegenreihe tiefer als die alten Zaehne untereinander im selben Key (ref_zz),
      'vor_nase'  (nur untere) Zahn um RAND verlaengert steht VOR der Schnauze (x > L, ausserhalb des Kopfrohrs) hoeher als deren Mundlinie:
                  von vorn saehe man ihn ueber der Nase - Messbild m3 'vorn zu' (Unterkiefer ragt 30 vor, Mittelzahn stand vor der Nase)."""
    S = KP.S
    out = []
    E_lang = _ecken(z, lang + RAND)
    E = _ecken(z, lang)
    for k in KP.keys:
        W = KP.welt(k, z['netz'], E_lang)
        Pr = S._proben(W)
        Tg = KP.gegen(k, z['netz'])
        inn = S._innen(Pr.reshape(-1, 3), Tg).reshape(Pr.shape[:2])
        if inn.any():
            out.append((k, 'drin', int(inn.sum())))
        for kante in inn:
            ein = np.nonzero(kante)[0]
            if len(ein) and (~kante[ein[0]:]).any():
                out.append((k, 'austritt', 1))
                break
        if z['netz'] == UNTEN:
            q = Pr.reshape(-1, 3)[~inn.reshape(-1)]
            q = q[(q[:, 0] > KP.x_vorn) & (np.abs(q[:, 2]) <= KP.lippe[0][-1])]
            if len(q) and (q[:, 1] < np.interp(np.abs(q[:, 2]), *KP.lippe)).any():
                out.append((k, 'vor_nase', int((q[:, 1] < np.interp(np.abs(q[:, 2]), *KP.lippe)).sum())))
        Wz = KP.welt(k, z['netz'], E)
        for w in gegenreihe:
            Ew = KP.welt(k, w['netz'], w['_E'])
            d = zahn_zu_zahn(Wz, Ew, S)
            if d > ref_zz[k] + 1e-9:
                out.append((k, 'zahn', round(d - ref_zz[k], 2)))
        if out and not alle:
            return out
    return out


def laengen_eichen(schritte=10, log=print):
    """R6 bestimmen und nach LAENGEN schreiben: je neuem Zahn die groesste Laenge <= Nennlaenge (R4) ohne Verletzung in irgendeinem Key.
    Reihenfolge: erst die unteren (sie stecken bei geschlossenem Kiefer in der Schnauze), dann die oberen gegen die gekappten unteren."""
    p = vorgabe()
    KP = Keypose(p)
    S = KP.S
    alte = alte_oben(p) + alte_unten(p)
    for z in alte:
        z['_E'] = _ecken(z)
    # Bezug: alte Zaehne untereinander, je Key die tiefste Durchdringung oben <-> unten
    ref = {}
    ao = [z for z in alte if z['netz'] == OBEN]
    au = [z for z in alte if z['netz'] == UNTEN]
    for k in KP.keys:
        WO = [KP.welt(k, OBEN, z['_E']) for z in ao]
        WU = [KP.welt(k, UNTEN, z['_E']) for z in au]
        ref[k] = max([zahn_zu_zahn(a, b, S) for a in WO for b in WU] + [0.0])
    neu = {OBEN: neue_reihe(p, OBEN, kappen={})[0], UNTEN: neue_reihe(p, UNTEN, kappen={})[0]}
    kappen, proto = {}, []
    for netz in (UNTEN, OBEN):
        g = OBEN if netz == UNTEN else UNTEN
        gegenreihe = [z for z in alte if z['netz'] == g] + [dict(z, _E=_ecken(z, kappen.get(name(z), z['lang_nenn']))) for z in neu[g]]
        for z in neu[netz]:
            L0 = z['lang_nenn']
            v = verletzung(KP, z, L0, gegenreihe, ref)
            if not v:
                proto.append(dict(zahn=name(z), nenn=round(L0, 2), kappe=None, verletzungen_nenn=0))
                continue
            lo, hi = 0.0, L0
            for _ in range(schritte):
                m = 0.5 * (lo + hi)
                if verletzung(KP, z, m, gegenreihe, ref, alle=False):
                    hi = m
                else:
                    lo = m
            if verletzung(KP, z, lo, gegenreihe, ref, alle=False):
                raise SystemExit('R6: Zahn %s auch mit Laenge %.1f verletzt' % (name(z), lo))
            kappen[name(z)] = round(lo, 2)
            arten = sorted(set(a for _k, a, _w in v))
            proto.append(dict(zahn=name(z), nenn=round(L0, 2), kappe=round(lo, 2), verletzungen_nenn=len(v), arten=arten,
                              beispiel=v[:3]))
            log('  %s Nenn %.1f -> Kappe %.1f (%d Verletzungen bei Nennlaenge: %s)' % (name(z), L0, lo, len(v), arten))
        # spiegelgleich (R2): links und rechts dieselbe Laenge = die kleinere (die Bewegung ist nicht ganz symmetrisch; kuerzer bleibt in der
        # Huelle des laengeren Zahns, kann also keine neue Verletzung bringen - der Riegel misst es trotzdem nach)
        for z in neu[netz]:
            if z['seite'] != 1:
                continue
            a, b = name(z), name(dict(z, seite=-1))
            if a in kappen or b in kappen:
                m = min(kappen.get(a, z['lang_nenn']), kappen.get(b, z['lang_nenn']))
                kappen[a] = kappen[b] = m
                proto.append(dict(zahn='%s / %s' % (a, b), spiegelgleich=m))
    d = dict(lang_max=kappen, rand=RAND, schritte=schritte, posen_emd_sha256=KP.emd_sha, keys=len(KP.keys),
             quelle='R6 (kf_k5g3_zaehne.laengen_eichen): groesste Laenge <= Nennlaenge (R4), mit der der Zahn in KEINEM der %d Keys (Posen der '
                    'K5g3-Geometrie-EMD, Sektionen 0-6 == Spiel-Asset) (a) um RAND verlaengert in das Gegenrohr ein- und wieder austritt, (b) um RAND '
                    'verlaengert im Rohr des Gegenkiefers liegt (alte obere: in keinem Key) und (c) tiefer in einen Zahn der '
                    'Gegenreihe eindringt als die alten Zaehne untereinander im selben Key, (d) als unterer Zahn um RAND verlaengert vor der Schnauze '
                    '(x > L) hoeher steht als deren Mundlinie. Bisektion %d Schritte.' % (len(KP.keys), schritte),
             protokoll=proto)
    json.dump(d, open(LAENGEN, 'w', encoding='utf-8', newline='\n'), indent=1, ensure_ascii=False)
    log('R6 geschrieben: %s' % LAENGEN)
    return d


if __name__ == '__main__':
    a = sys.argv[1:]
    p = vorgabe()
    if a and a[0] == 'reihen':
        print(json.dumps(bericht_reihen(p), indent=1))
    elif a and a[0] == 'eichen':
        laengen_eichen()
    elif a and a[0] == 'probe':           # probe <ein.EMD> <aus.EMD>: an ein fertiges Asset anhaengen (Erkundung)
        import kf_k5g2_geo as GG
        emd = open(a[1], 'rb').read()
        form = FE.lese_emd(emd)
        print(einsetzen(form, p))
        open(a[2], 'wb').write(GG.emd_mit_md1(emd, form))
    else:
        raise SystemExit(__doc__)
