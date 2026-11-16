#!/usr/bin/env python3
"""K5g2 Geometrie-Stufe (E1 Streifen, E2 Krallen) - Hilfen fuer kf_k5g2.py und riegel_k5g2.py.

E1 (Orchestrator-Entscheid 2026-11-09, streifen/STREIFEN_HERKUNFT.md): Kopf 22 steckt in Brust 10 / Hals 21; das Spiel malt Dreiecke nach
mittlerer Tiefe ohne Tiefenpuffer (render_pc.c Einfuegesortierung nach s_textri_depth, main.c npc_avg_z = (wz0+wz1+wz2)/3). Behebung NUR ueber
die Geometrie: die Kopf-Ecken, die in irgendeinem Bild irgendeines Clips in Brust 10 oder Hals 21 liegen, werden entlang der Geraden zum Anker
(Kopfraum) zurueckgenommen, bis sie in ALLEN Bildern mindestens `rand` ausserhalb liegen (Weg a). Gesicht/Augen/Maul (Flaechen der Zonen
29, 30, 41, 42) bleiben fest.

Innen-Regel wie streifen/skripte/s_alle.py (naechste Flaeche, Lot in die Flaeche, Aussen = -Kreuzprodukt, gemessen in s_schnitt.py).
Keil-Pixel (E1-Ziel): Bildpunkte, an denen die Maler-Ordnung (Dreiecks-Mitteltiefe, fern zuerst) eine Flaeche aus {22, 23} zeigt, der
Tiefenpuffer aber eine aus {10, 21} (oder umgekehrt). Gerastert werden nur die Teile 10/21/22/23 (strenger: nichts davor verdeckt).
"""
import os
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
K5 = os.path.dirname(HIER)
WT = os.path.normpath(os.path.join(K5, '..', '..', '..', '..', '..'))
sys.path.insert(0, os.path.join(WT, 're15_port', 'tools'))
import form_emd as FE  # noqa: E402
import form_buehne as FB  # noqa: E402
import emd_ansichtsblatt as EA  # noqa: E402

KOPF, KIEFER, BRUST, HALS = 22, 23, 10, 21
GRUPPE_A, GRUPPE_B = (22, 23), (10, 21)


# ------------------------------------------------------------------ Posen
def posen(mo):
    """Alle Bilder aller Clips: Liste (clip, bild, kf) und R (n x 32 x 3 x 3), T (n x 32 x 3), ganzzahlig wie der Port."""
    cb, R, T = [], [], []
    for c, (f0, n) in enumerate(mo.clips):
        for i in range(n):
            kf = mo.frames[f0 + i] & 0xFFF
            r, t = mo.pose(kf)
            cb.append((c, i, kf))
            R.append(np.array([np.asarray(x, np.int64) for x in r]))
            T.append(np.array([np.asarray(x, np.int64) for x in t]))
    return cb, np.array(R), np.array(T)


def welt(R, T, b, V):
    V = np.asarray(V, np.int64).reshape(-1, 3)
    return (((R[b] @ V.T) >> 12).T + T[b]).astype(float)


def dreiecke_idx(mesh):
    out = [list(d['e']) for d in mesh['dreiecke']]
    for q in mesh['vierecke']:
        e = q['e']
        out += [[e[0], e[1], e[3]], [e[0], e[3], e[2]]]
    return np.array(out, int)


def naechster_punkt(Pt, A, B, C):
    """= streifen/skripte/s_geo.naechster_punkt (Abstand^2, Punkt, Lot in der Flaeche)."""
    P = Pt[:, None, :]
    n = np.cross(B - A, C - A)[None]
    nn = np.maximum((n ** 2).sum(-1), 1e-12)
    d = ((P - A[None]) * n).sum(-1) / nn
    X = P - d[..., None] * n
    c0 = np.cross(B - A, X - A[None]); c1 = np.cross(C - B, X - B[None]); c2 = np.cross(A - C, X - C[None])
    innen = ((c0 * n).sum(-1) >= 0) & ((c1 * n).sum(-1) >= 0) & ((c2 * n).sum(-1) >= 0)
    best = np.where(innen[..., None], X, np.inf)
    bd = np.where(innen, ((P - X) ** 2).sum(-1), np.inf)
    for S, T_ in ((A, B), (B, C), (C, A)):
        e = (T_ - S)[None]
        tt = np.clip(((P - S[None]) * e).sum(-1) / np.maximum((e ** 2).sum(-1), 1e-12), 0, 1)
        Y = S[None] + e * tt[..., None]
        dY = ((P - Y) ** 2).sum(-1)
        w = dY < bd
        best = np.where(w[..., None], Y, best); bd = np.where(w, dY, bd)
    return bd, best, innen


def tiefe(P, Tri):
    """Vorzeichenabstand zur Huelle eines Teils (> 0 = innen; Regel s_alle.py). Tri: m x 3 x 3 Weltkoordinaten."""
    if len(P) == 0:
        return np.zeros(0)
    A_, B_, C_ = Tri[:, 0], Tri[:, 1], Tri[:, 2]
    d2, X, innen = naechster_punkt(P, A_, B_, C_)
    ar = np.arange(len(P))
    j = d2.argmin(1); d = np.sqrt(d2[ar, j]); Xj = X[ar, j]
    n = -np.cross(B_ - A_, C_ - A_)[j]
    n /= np.maximum(np.linalg.norm(n, axis=1, keepdims=True), 1e-9)
    drin = (((P - Xj) * n).sum(1) < 0) & innen[ar, j]
    return np.where(drin, d, -d)


class Huellen:
    """Brust 10 und Hals 21 je Bild in Weltkoordinaten (fest, die Teile werden nicht veraendert). Dreiecke je Bild zwischengespeichert."""

    def __init__(self, form, R, T, teile=GRUPPE_B):
        self.teile = teile
        self.idx = {b: dreiecke_idx(form['meshes'][b]) for b in teile}
        self.E = {b: np.array(form['meshes'][b]['ecken'], np.int64) for b in teile}
        self.R, self.T = R, T
        self._c = {}

    def tri(self, i, b):
        k = (i, b)
        if k not in self._c:
            Tr = welt(self.R[i], self.T[i], b, self.E[b])[self.idx[b]]
            self._c[k] = (Tr, Tr.reshape(-1, 3).min(0) - 60, Tr.reshape(-1, 3).max(0) + 60)
        return self._c[k]

    def tiefe_max(self, i, P):
        """groesste Eindringtiefe von P (Welt) in die Teile (> 0 innen; weit ausserhalb des Huellquaders: -1e9)."""
        out = np.full(len(P), -1e9)
        for b in self.teile:
            Tr, lo, hi = self.tri(i, b)
            nah = np.all((P >= lo) & (P <= hi), 1)
            if nah.any():
                d = np.full(len(P), -1e9)
                d[nah] = tiefe(P[nah], Tr)
                out = np.maximum(out, d)
        return out


def eindringen(form, R, T, teil, ecken=None, huellen=None):
    """Matrix Bilder x Ecken: Eindringtiefe der Ecken von `teil` in Brust 10 / Hals 21 (> 0 innen)."""
    H = huellen or Huellen(form, R, T)
    E = np.array(form['meshes'][teil]['ecken'] if ecken is None else ecken, np.int64)
    D = np.zeros((len(R), len(E)))
    for i in range(len(R)):
        D[i] = H.tiefe_max(i, welt(R[i], T[i], teil, E))
    return D


def zuruecknehmen(form, R, T, teil, fest, anker, rand, schritte=14, huellen=None, log=None):
    """Weg (a): jede bewegliche Ecke von `teil`, die in irgendeinem Bild innen liegt oder naeher als `rand` an der Huelle, wandert auf der
    Geraden zum Anker (Teilraum): p(t) = p + t (anker - p), t in [0, 1]. Kleinstes t (Bisektion, `schritte`) mit Eindringtiefe <= -rand in
    ALLEN Bildern; Ecke auf ganze Einheiten gerundet und so nachgeprueft. Rueckgabe: neue Ecken, Protokoll je Ecke, Liste der betroffenen."""
    H = huellen or Huellen(form, R, T)
    E = np.array(form['meshes'][teil]['ecken'], np.int64)
    D0 = eindringen(form, R, T, teil, huellen=H)
    betroffen = [j for j in range(len(E)) if D0[:, j].max() > -rand]
    anker = np.asarray(anker, float)
    neu = E.copy()
    proto = {}
    beweg = [j for j in betroffen if j not in fest]
    for j in betroffen:
        if j in fest:
            proto[j] = dict(status='fest (Gesicht/Augen/Maul)', tiefe_vorher=round(float(D0[:, j].max()), 1))
    if not beweg:
        return neu, proto, betroffen
    p = E[beweg].astype(float)

    def pruef(t):
        q = np.rint(p + t[:, None] * (anker[None] - p)).astype(np.int64)
        d = np.full(len(q), -1e9)
        for i in range(len(R)):
            d = np.maximum(d, H.tiefe_max(i, welt(R[i], T[i], teil, q)))
        return d <= -rand, q, d
    g1, q1, d1 = pruef(np.ones(len(p)))
    lo, hi = np.zeros(len(p)), np.ones(len(p))
    q_hi, d_hi = q1.copy(), d1.copy()
    for _ in range(schritte):
        m = 0.5 * (lo + hi)
        g, q, d = pruef(m)
        hi = np.where(g, m, hi); lo = np.where(g, lo, m)
        q_hi = np.where(g[:, None], q, q_hi); d_hi = np.where(g, d, d_hi)
    for k, j in enumerate(beweg):
        if not g1[k]:
            proto[j] = dict(status='Anker reicht nicht', tiefe_am_anker=round(float(d1[k]), 1))
            continue
        neu[j] = q_hi[k]
        proto[j] = dict(alt=[int(x) for x in E[j]], neu=[int(x) for x in q_hi[k]], t=round(float(hi[k]), 5),
                        weg=round(float(np.linalg.norm(q_hi[k] - E[j])), 1), tiefe_vorher=round(float(D0[:, j].max()), 1),
                        tiefe_nachher=round(float(d_hi[k]), 1))
        if log:
            log('  %d Ecke %d: %s' % (teil, j, proto[j]))
    return neu, proto, betroffen


def emd_mit_md1(emd, form):
    """EMD-Bytes mit ersetzter MD1-Sektion (letzte Sektion vor dem Verzeichnis); Sektionen 0-6 bytegleich, Verzeichnis nachgezogen."""
    import struct
    dir_off, d = FE.verzeichnis(emd)
    md1 = FE.md1_schreiben(form)
    if len(md1) % 4:
        raise SystemExit('MD1 nicht auf 4 Byte')
    kopf = emd[:d[7]]
    neu_dir = d[7] + len(md1)
    return struct.pack('<II', neu_dir, 8) + kopf[8:] + md1 + struct.pack('<8I', *d)


# ------------------------------------------------------------------ Kameras und Keil-Pixel
def kamera_rid(raum, schnitt):
    s = FB.Raum(raum).schnitt(schnitt)
    return FB.Kamera.aus_rid(s['pos'], s['ziel'], fov=s['fov'])


def dreiecke_teile(mo, R, T, lage, gier, kam, teile=(10, 21, 22, 23)):
    """= form_buehne.dreiecke (gleiche Ganzzahlrechnung, Rueckseiten, vz < 64 weg), nur `teile`; Tupel (tief, p[3], vz[3], teil, (fi, viereck))."""
    s, c = EA.sin_q12(gier), EA.cos_q12(gier)
    Y = np.array([[c, 0, s], [0, 4096, 0], [-s, 0, c]], dtype=np.int64)
    lage = np.array(lage, dtype=np.int64)
    aus = []
    for b in teile:
        m = mo.meshes[b]
        W = (Y @ R[b]) >> 12
        t = ((Y @ T[b]) >> 12) + lage
        VW = (kam.V @ W) >> 12
        vT = ((kam.V @ t) >> 12) + kam.t
        for (verts, faces, viereck) in ((m.tv, m.tf, 0), (m.qv, m.qf, 1)):
            if not faces:
                continue
            v = np.array([x[:3] for x in verts], dtype=np.int64)
            q = (VW @ v.T) >> 12
            vx, vy, vz = q[0] + vT[0], q[1] + vT[1], q[2] + vT[2]
            sx, sy = FB._gte(vx, vy, vz, kam.h)
            for fi, g in enumerate(faces):
                ie = g[1::2]
                if any(vz[i] < 64 for i in ie):
                    continue
                p = [(int(sx[i]), int(sy[i])) for i in ie]
                m0 = FB._mac0(p[0], p[1], p[2])
                if (m0 < 0) if viereck else (m0 <= 0):
                    continue
                z = [float(vz[i]) for i in ie]
                if viereck:
                    for a, bb, cc in ((0, 1, 3), (0, 3, 2)):
                        aus.append(((z[a] + z[bb] + z[cc]) / 3.0, (p[a], p[bb], p[cc]), (z[a], z[bb], z[cc]), b, (fi, viereck)))
                else:
                    aus.append((sum(z) / 3.0, tuple(p), tuple(z), b, (fi, viereck)))
    return aus


def _matrix(ab):
    if 'matrix' in ab:
        return np.asarray(ab['matrix'], float).reshape(3, 3)
    return np.array([[ab['ax'], 0, ab['bx']], [0, ab['ay'], ab['by']], [0, 0, 1]], float)


def keil_raster(tris, H, W, ab):
    """Maler (fern zuerst, stabil) und Tiefenpuffer (1/vz perspektivisch im Port-Bildschirm) in einem Durchgang; ohne Textur (alles deckend).
    Rueckgabe: maler_teil, puffer_teil (H x W, -1 leer), maler_dreieck, puffer_dreieck (Index in tris)."""
    Mg = _matrix(ab)
    Minv = np.linalg.inv(Mg)
    ab2 = dict(matrix=Mg.tolist())
    mt = np.full((H, W), -1, np.int16)
    pt = np.full((H, W), -1, np.int16)
    mi = np.full((H, W), -1, np.int32)
    pi_ = np.full((H, W), -1, np.int32)
    pz = np.full((H, W), np.inf)
    for k in sorted(range(len(tris)), key=lambda i: -tris[i][0]):
        _t, p, z, teil, _tag = tris[k]
        x, y = FB.abbilden(np.array([q[0] + FB.PHASE for q in p], float), np.array([q[1] + FB.PHASE for q in p], float), ab2)
        fl = (x[1] - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (y[1] - y[0])
        if abs(fl) < 1e-9:
            continue
        x0 = max(int(np.floor(x.min())), 0); x1 = min(int(np.ceil(x.max())) + 1, W)
        y0 = max(int(np.floor(y.min())), 0); y1 = min(int(np.ceil(y.max())) + 1, H)
        if x0 >= x1 or y0 >= y1:
            continue
        PX, PY = np.meshgrid(np.arange(x0, x1) + 0.5, np.arange(y0, y1) + 0.5)
        w1 = ((PX - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (PY - y[0])) / fl
        w2 = ((x[1] - x[0]) * (PY - y[0]) - (PX - x[0]) * (y[1] - y[0])) / fl
        w0 = 1.0 - w1 - w2
        innen = (w0 >= -1e-9) & (w1 >= -1e-9) & (w2 >= -1e-9)
        if not innen.any():
            continue
        den = Minv[2, 0] * PX + Minv[2, 1] * PY + Minv[2, 2]
        qx = (Minv[0, 0] * PX + Minv[0, 1] * PY + Minv[0, 2]) / den - FB.PHASE
        qy = (Minv[1, 0] * PX + Minv[1, 1] * PY + Minv[1, 2]) / den - FB.PHASE
        px_ = np.array([a[0] for a in p], float); py_ = np.array([a[1] for a in p], float)
        f2 = (px_[1] - px_[0]) * (py_[2] - py_[0]) - (px_[2] - px_[0]) * (py_[1] - py_[0])
        V = np.array(z, float)
        if abs(f2) < 1e-9:
            zz = np.full(PX.shape, V.mean())
        else:
            b1 = ((qx - px_[0]) * (py_[2] - py_[0]) - (px_[2] - px_[0]) * (qy - py_[0])) / f2
            b2 = ((px_[1] - px_[0]) * (qy - py_[0]) - (qx - px_[0]) * (py_[1] - py_[0])) / f2
            b0 = 1 - b1 - b2
            zz = 1.0 / np.maximum(b0 / V[0] + b1 / V[1] + b2 / V[2], 1e-9)
            zz = np.clip(zz, V.min(), V.max())
        sl = (slice(y0, y1), slice(x0, x1))
        mt[sl][innen] = teil
        mi[sl][innen] = k
        vorn = innen & (zz < pz[sl])
        pt[sl][vorn] = teil
        pi_[sl][vorn] = k
        pz[sl][vorn] = zz[vorn]
    return mt, pt, mi, pi_


def keil_zaehlen(mt, pt):
    a = np.isin(mt, GRUPPE_A) & np.isin(pt, GRUPPE_B)
    b = np.isin(mt, GRUPPE_B) & np.isin(pt, GRUPPE_A)
    return int(a.sum()), int(b.sum()), a, b


def ansichten():
    """Kameras: Name -> (Kamera, Lage, Gier, H, W, Abbildung).
    bild1/bild2 = Buehnen-Kameras der Tafeln (form_pruefung.kamera, kamera_bildN.json) mit dem Stand der Grundlage (zonen_v2.json meta.posen);
    spiel5080 = RID 5080 Schnitt 1 mit der Spielpose RE15_BB_POSE -20711,-15984,756 (kf_k5f_spiel.sh P1); spiel5090 = RID 5090 Schnitt 11 mit
    941,-23776,3874 (P2); spiel5080_vorn: 5080 Schnitt 1, Gier 2095 (streifen/STREIFEN_HERKUNFT.md 2: gleiche Blickrichtung zur Kamera wie 5090)."""
    import form_pruefung as P
    k1, k2 = P.kamera(1), P.kamera(2)
    d2 = dict(ax=2.0, bx=0.0, ay=2.0, by=0.0)
    k80, k90 = kamera_rid('5080', 1), kamera_rid('5090', 11)
    return {
        'bild1': (k1, (-18564, 0, -18386), 80, 480, 640, k1.abbildung),
        'bild2': (k2, (941, 0, -23776), 3874, 480, 640, k2.abbildung),
        'spiel5080': (k80, (-20711, 0, -15984), 756, 480, 640, d2),
        'spiel5080_vorn': (k80, (-20711, 0, -15984), 2095, 480, 640, d2),
        'spiel5090': (k90, (941, 0, -23776), 3874, 480, 640, d2),
    }


_W = {}


def _keil_job(arg):
    emd_pfad, tim_pfad, i, name = arg
    key = (emd_pfad, tim_pfad)
    if _W.get('key') != key:
        _W['mo'] = FE.modell(open(emd_pfad, 'rb').read(), open(tim_pfad, 'rb').read())
        _W['cb'], _W['R'], _W['T'] = posen(_W['mo'])
        _W['an'] = ansichten()
        _W['key'] = key
    kam, lage, gier, H, W, ab = _W['an'][name]
    tris = dreiecke_teile(_W['mo'], _W['R'][i], _W['T'][i], lage, gier, kam)
    mt, pt, mi, pi_ = keil_raster(tris, H, W, ab)
    a, b, ma, mb = keil_zaehlen(mt, pt)
    c, bi, _kf = _W['cb'][i]
    fl = {}
    for m, idx in ((ma, mi), (mb, mi)):
        for k in np.unique(idx[m]):
            t = tris[int(k)]
            fl['%d|%d|%d' % (t[3], t[4][1], t[4][0])] = fl.get('%d|%d|%d' % (t[3], t[4][1], t[4][0]), 0) + int((idx[m] == k).sum())
    return dict(ansicht=name, clip=c, bild=bi, kopf_ueber_rumpf=a, rumpf_ueber_kopf=b, figur=int((mt >= 0).sum()), maler_flaechen=fl)


def keil_alle(emd_pfad, tim_pfad, namen=None, prozesse=14, bilder=None):
    """Keil-Pixel je Ansicht und Bild ueber alle Clips (Pool). Rueckgabe Liste von dicts."""
    from multiprocessing import Pool
    mo = FE.modell(open(emd_pfad, 'rb').read(), open(tim_pfad, 'rb').read())
    n = sum(k for _f, k in mo.clips)
    namen = namen or list(ansichten())
    jobs = [(emd_pfad, tim_pfad, i, nm) for nm in namen for i in (bilder if bilder is not None else range(n))]
    with Pool(prozesse) as p:
        return p.map(_keil_job, jobs, chunksize=4)


def keil_bild(emd_pfad, tim_pfad, name, clip, bild):
    """Ein Bild: Maler-/Puffer-Teil je Pixel (fuer Tafeln)."""
    mo = FE.modell(open(emd_pfad, 'rb').read(), open(tim_pfad, 'rb').read())
    f0, n = mo.clips[clip]
    r, t = mo.pose(mo.frames[f0 + min(bild, n - 1)] & 0xFFF)
    R = np.array([np.asarray(x, np.int64) for x in r]); T = np.array([np.asarray(x, np.int64) for x in t])
    kam, lage, gier, H, W, ab = ansichten()[name]
    mt, pt, _mi, _pi = keil_raster(dreiecke_teile(mo, R, T, lage, gier, kam), H, W, ab)
    return mt, pt


# ------------------------------------------------------------------ Krallen (E2)
SPIEGEL = np.diag([1, 1, -1]).astype(np.int64)   # Koerpermitte: +z = linke Seite des Wesens (STREIFEN_HERKUNFT R3.1; gerippe/rig.py Rastpose)
KRALLEN_PAARE = ((14, 19), (15, 20), (28, 29), (30, 31))   # klaue_l1..l4 / klaue_r1..r4 (form_bemalt/fb_bau.py Teilliste; form.json right_arm/left_arm_front)


def spiegel_kralle(form, links, rechts):
    """Rechte Kralle = Spiegelbild der linken im Raum der Hand (Rastpose, alle Winkel 0: Knochenrahmen = Modellachsen).
    Ecke der linken Kralle im Handraum: v_h = versatz_l + v_l. Gespiegelt: S v_h. Im Raum der rechten Kralle: v_r = S v_h - versatz_r.
    Normalen: S n. Flaechen: Reihenfolge gedreht (Spiegelung kehrt den Umlauf um): Dreieck (a,b,c) -> (a,c,b), Viereck (a,b,c,d) -> (a,c,b,d)
    (Z-Reihenfolge a-b/a-c, d gegenueber a). UV/CLUT/Seite je Ecke mitgenommen."""
    ml, kl, kr = form['meshes'][links], form['knochen'][links], form['knochen'][rechts]
    ol, orr = np.array(kl['versatz'], np.int64), np.array(kr['versatz'], np.int64)
    ecken = [[int(x) for x in (SPIEGEL @ (ol + np.array(e, np.int64)) - orr)] for e in ml['ecken']]
    normalen = [[int(x) for x in (SPIEGEL @ np.array(n, np.int64))] for n in ml['normalen']]
    dr, vi = [], []
    for d in ml['dreiecke']:
        o = (0, 2, 1)
        dr.append(dict(e=[d['e'][k] for k in o], n=[d['n'][k] for k in o], uv=[list(d['uv'][k]) for k in o], clut=int(d['clut']), page=int(d['page'])))
    for q in ml['vierecke']:
        o = (0, 2, 1, 3)
        vi.append(dict(e=[q['e'][k] for k in o], n=[q['n'][k] for k in o], uv=[list(q['uv'][k]) for k in o], clut=int(q['clut']), page=int(q['page'])))
    form['meshes'][rechts] = dict(ecken=ecken, normalen=normalen, dreiecke=dr, vierecke=vi)
    return form['meshes'][rechts]


def kralle_im_handraum(form, teil):
    """Ecken der Kralle im Raum ihrer Hand, Rastpose (versatz + Ecke)."""
    o = np.array(form['knochen'][teil]['versatz'], np.int64)
    return np.array(form['meshes'][teil]['ecken'], np.int64) + o


# ------------------------------------------------------------------ Runde 2: Keil-Flaechen beider Partner (fuer die Unterteilung, E1 Weg b')
def _keil_paar_job(arg):
    emd_pfad, tim_pfad, i, name = arg
    key = (emd_pfad, tim_pfad, os.path.getmtime(emd_pfad))
    if _W.get('key') != key:
        _W['mo'] = FE.modell(open(emd_pfad, 'rb').read(), open(tim_pfad, 'rb').read())
        _W['cb'], _W['R'], _W['T'] = posen(_W['mo'])
        _W['an'] = ansichten()
        _W['key'] = key
    kam, lage, gier, H, W, ab = _W['an'][name]
    tris = dreiecke_teile(_W['mo'], _W['R'][i], _W['T'][i], lage, gier, kam)
    mt, pt, mi, pi_ = keil_raster(tris, H, W, ab)
    a, b, ma, mb = keil_zaehlen(mt, pt)
    m = ma | mb
    fl = set()
    for idx in (mi, pi_):
        for k in np.unique(idx[m]):
            t = tris[int(k)]
            fl.add((int(t[3]), int(t[4][1]), int(t[4][0])))
    c, bi, _kf = _W['cb'][i]
    return dict(ansicht=name, clip=int(c), bild=int(bi), kopf_ueber_rumpf=a, rumpf_ueber_kopf=b, flaechen=sorted(fl))


def keil_paare_alle(emd_pfad, tim_pfad, namen=None, prozesse=6):
    from multiprocessing import Pool
    mo = FE.modell(open(emd_pfad, 'rb').read(), open(tim_pfad, 'rb').read())
    n = sum(k for _f, k in mo.clips)
    namen = namen or list(ansichten())
    jobs = [(emd_pfad, tim_pfad, i, nm) for nm in namen for i in range(n)]
    with Pool(prozesse) as p:
        return p.map(_keil_paar_job, jobs, chunksize=8)


def kralle_drehpunkt(form, links, rechts, R_bild, verschub):
    """Runde 2 (Pruefer r1 Sicht 5, gemessen): die Krallen drehen sich in der Animation um ihren EIGENEN Knochen (EMR-Versatz, bleibt); der
    rechte Drehpunkt liegt nicht im Spiegelbild des linken (z. B. 29: [900,40,-340] gegen S [684,30,258] = [684,30,-258]). Mit der
    Rastpose-Spiegelung (spiegel_kralle) weicht die Kralle deshalb in jeder Haltung mit Krallendrehung vom Spiegelbild ab. Hier: v_r = S v_l + c
    mit c = R_rel^T (S o_l - o_r), R_rel = Drehung der Kralle zur Hand in der Bild-2-Haltung -> dort exakt das Spiegelbild der linken Kralle
    im Raum der Hand. Dazu `verschub` (Kralle-Raum, ganzzahlig, par geometrie.krallen.verschub mit Quelle). Normalen S n (Drehung bleibt)."""
    S = np.diag([1, 1, -1]).astype(float)
    hand = form['knochen'][rechts]['eltern']
    Rr = (R_bild[hand].astype(float).T @ R_bild[rechts].astype(float)) / 4096.0 ** 2
    ol = np.array(form['knochen'][links]['versatz'], float)
    orr = np.array(form['knochen'][rechts]['versatz'], float)
    c = Rr.T @ (S @ ol - orr)
    Vl = np.array(form['meshes'][links]['ecken'], float)
    V = (S @ Vl.T).T + c + np.asarray(verschub, float)
    form['meshes'][rechts]['ecken'] = [[int(x) for x in np.rint(v)] for v in V]
    return dict(drehpunkt_ausgleich=[round(float(x), 1) for x in c], verschub=[int(x) for x in verschub],
                winkel_kralle_zur_hand_bild2=round(float(np.degrees(np.arccos(np.clip((np.trace(Rr) - 1) / 2, -1, 1)))), 1))


# ------------------------------------------------------------------ Runde 2: Geometrie-Stufe in EINEM Aufruf (O2 Krallen + O1 Mulden-Tabelle)
# Aufruf:   python kf_k5g2_geo.py bauen [<ziel.EMD>]     -> K5/k5g/K5g2/geo/BAHNSTEIG_FORM.EMD (laeuft mit der TIM von K5e_f1)
#           python kf_k5g2_geo.py pruef                  -> baut zweimal im Speicher, vergleicht die Bytes mit der Datei (Exit 1 bei Unterschied)
# Eingaenge (alle versioniert): K5/ruhe/K5e_f1/BAHNSTEIG_FORM.EMD, generator/k5g2_krallen.json (Krallen-Rezept),
# generator/k5g2_mulde_tabelle.json (fertige Netze 10, 16, 21, 22 mit Mulde, aus kf_k5g2_mulde.py tabelle).
# Reihenfolge: ERST die Krallen (das Rezept spiegelt die linken Krallen 14, 15, 28, 30 so, wie sie in K5e_f1 stehen; 28 vorher auf K5c-Form),
# DANN die Netze der Tabelle. Beide Schritte fassen verschiedene Teile an (Krallen 19, 20, 28, 29, 31 / Tabelle 10, 16, 21, 22).
# Keine Gleitkommazahl, kein Zufall, keine Uhr: zweiter Lauf ergibt dieselben Bytes. EDD/EMR/Clips (Sektionen 0-6) bleiben die Bytes von K5e_f1.
GEO_BASIS = os.path.join(K5, 'ruhe', 'K5e_f1', 'BAHNSTEIG_FORM.EMD')
GEO_BASIS_TIM = os.path.join(K5, 'ruhe', 'K5e_f1', 'BAHNSTEIG_FORM.TIM')
GEO_BASIS_SHA = 'f277916d7d6518b5fee621dc3b1a50ebc8cf5861ba6cc290818139449da9a5e1'
GEO_KRALLEN = os.path.join(HIER, 'k5g2_krallen.json')
GEO_TABELLE = os.path.join(HIER, 'k5g2_mulde_tabelle.json')
GEO_ZIEL = os.path.join(K5, 'k5g', 'K5g2', 'geo', 'BAHNSTEIG_FORM.EMD')
KRALLEN_LINKS, KRALLEN_RECHTS = (14, 15, 28, 30), (19, 20, 29, 31)
KRALLEN_TEILE = (19, 20, 28, 29, 31)          # was die Krallen-Stufe anfasst
MULDEN_TEILE = (10, 16, 21, 22)               # was die Mulden-Tabelle setzt (16 steht unveraendert darin)


def _q12(M, v):
    """Ganzzahlige Drehung: (M * v + 2048) >> 12, M in Q12 (4096 = 1,0)."""
    x, y, z = int(v[0]), int(v[1]), int(v[2])
    return [(int(M[i][0]) * x + int(M[i][1]) * y + int(M[i][2]) * z + 2048) >> 12 for i in range(3)]


def kralle_spiegelnetz(links, M):
    """Spiegelbild des linken Krallennetzes im Krallenraum (z -> -z, Wurzel = Ursprung, also an der EIGENEN Wurzel der rechten Kralle),
    danach die feste Drehung M (Q12) um die Wurzel. Normalen genauso. Umlauf gedreht (Dreieck 0,2,1 / Viereck 0,2,1,3), UV/CLUT/Seite je Ecke
    mitgenommen. Reine Ganzzahlen."""
    ecken = [_q12(M, (e[0], e[1], -e[2])) for e in links['ecken']]
    normalen = [_q12(M, (n[0], n[1], -n[2])) for n in links['normalen']]
    dr, vi = [], []
    for d in links['dreiecke']:
        o = (0, 2, 1)
        dr.append(dict(e=[d['e'][k] for k in o], n=[d['n'][k] for k in o], uv=[list(d['uv'][k]) for k in o], clut=int(d['clut']), page=int(d['page'])))
    for q in links['vierecke']:
        o = (0, 2, 1, 3)
        vi.append(dict(e=[q['e'][k] for k in o], n=[q['n'][k] for k in o], uv=[list(q['uv'][k]) for k in o], clut=int(q['clut']), page=int(q['page'])))
    return dict(ecken=ecken, normalen=normalen, dreiecke=dr, vierecke=vi)


def krallen_form(form, rezept):
    """O2 / E2 in der Form (aus FE.lese_emd von K5e_f1), an Ort und Stelle:
    28 = Ecken und Normalen der K5c-Form (rezept kralle28, Flaechen/UV wie K5e_f1); 19, 20, 29, 31 = Spiegelnetze der linken 14, 15, 28, 30
    mit je einer festen Drehung um die Wurzel (rezept rechts/<teil>/matrix_q12). Knochen und Versaetze bleiben."""
    k28 = rezept['kralle28']
    m = form['meshes'][28]
    if len(m['ecken']) != len(k28['ecken']) or len(m['normalen']) != len(k28['normalen']):
        raise SystemExit('Kralle 28: Eckenzahl passt nicht zur K5c-Form')
    m['ecken'] = [[int(x) for x in e] for e in k28['ecken']]
    m['normalen'] = [[int(x) for x in e] for e in k28['normalen']]
    for links, rechts in rezept['paare']:
        form['meshes'][int(rechts)] = kralle_spiegelnetz(form['meshes'][int(links)], rezept['rechts'][str(rechts)]['matrix_q12'])
    return form


def geometrie_form(emd0=None, krallen=GEO_KRALLEN, tabelle=GEO_TABELLE, stufen=('krallen', 'mulde')):
    """K5e_f1 -> Form mit der fertigen Geometrie. stufen erlaubt Teilstaende fuer Gegenproben (nur krallen / nur mulde)."""
    import hashlib
    import json
    if emd0 is None:
        emd0 = open(GEO_BASIS, 'rb').read()
    ist = hashlib.sha256(emd0).hexdigest()
    if ist != GEO_BASIS_SHA:
        raise SystemExit('Basis ist nicht K5e_f1: sha256 %s, erwartet %s' % (ist, GEO_BASIS_SHA))
    form = FE.lese_emd(emd0)
    if 'krallen' in stufen:
        rezept = json.load(open(krallen, encoding='utf-8'))
        if rezept['basis']['sha256'] != GEO_BASIS_SHA:
            raise SystemExit('Krallen-Rezept gehoert zu einer anderen Basis')
        if tuple(tuple(p) for p in rezept['paare']) != KRALLEN_PAARE:
            raise SystemExit('Krallen-Rezept: andere Paare als %s' % (KRALLEN_PAARE,))
        krallen_form(form, rezept)
    if 'mulde' in stufen:
        tab = json.load(open(tabelle, encoding='utf-8'))
        if tab['basis']['sha256'] != GEO_BASIS_SHA:
            raise SystemExit('Mulden-Tabelle gehoert zu einer anderen Basis')
        if tuple(sorted(int(t) for t in tab['teile'])) != MULDEN_TEILE:
            raise SystemExit('Mulden-Tabelle: andere Teile als %s' % (MULDEN_TEILE,))
        for t in sorted(tab['teile'], key=int):
            form['meshes'][int(t)] = tab['teile'][t]
    return emd0, form


def geometrie_bytes(stufen=('krallen', 'mulde'), krallen=GEO_KRALLEN, tabelle=GEO_TABELLE):
    emd0, form = geometrie_form(krallen=krallen, tabelle=tabelle, stufen=stufen)
    return emd_mit_md1(emd0, form)


def geometrie_bauen(ziel=None, log=print):
    """EIN Aufruf: K5e_f1 + Krallen-Rezept + Mulden-Tabelle -> Geometrie-EMD. Gibt (Pfad, sha256) zurueck."""
    import hashlib
    aus = geometrie_bytes()
    ziel = ziel or GEO_ZIEL
    os.makedirs(os.path.dirname(ziel), exist_ok=True)
    with open(ziel, 'wb') as f:
        f.write(aus)
    h = hashlib.sha256(aus).hexdigest()
    log('EMD %s  %d Byte  sha256 %s' % (ziel.replace(chr(92), '/'), len(aus), h))
    return ziel, h


def mulde_auf_form(form, tabelle=GEO_TABELLE):
    """Fuer die Textur-Stufe (laeuft NACH der Geometrie): form = K5e_f1-Flaechen der Teile 10, 16, 21, 22 (Schluessel wie zonen_v2), aber mit
    NEUER Textur-Zuordnung je Flaeche (clut, page, uv verschoben, z. B. Fleck-Kopien). Setzt die Netze der Mulden-Tabelle ein und laesst jede
    Tabellenflaeche von ihrer Mutter erben: clut und Seite der Mutter, UV = Tabellen-UV + Verschiebung der Mutter (neu gegen K5e_f1, in Texeln
    der ganzen Textur: x = u + (page & 15) * 128). Wurde eine Mutter nicht rein verschoben (gedreht, gestreckt), bricht es ab.
    Rueckgabe: Bericht (verschobene Muetter, Kinder, Kinder mit UV ausserhalb des Mutter-Rechtecks - die lesen nach einer Kopie fremde Texel)."""
    import json
    tab = json.load(open(tabelle, encoding='utf-8'))
    basis = FE.lese_emd(open(GEO_BASIS, 'rb').read())
    art = ('dreiecke', 'vierecke')

    def gx(f):
        return [(int(u) + (int(f['page']) & 15) * 128, int(v)) for u, v in f['uv']]

    ber = dict(muetter_verschoben=0, kinder_verschoben=0, kinder_clut_neu=0, kinder_ausserhalb=[])
    delta = {}
    for t in MULDEN_TEILE:
        for a in (0, 1):
            alt, neu = basis['meshes'][t][art[a]], form['meshes'][t][art[a]]
            if len(alt) != len(neu):
                raise SystemExit('Teil %d: Flaechenzahl ist nicht die von K5e_f1 (erst texturieren, dann mulde_auf_form)' % t)
            for i, (fa, fn) in enumerate(zip(alt, neu)):
                if fa['e'] != fn['e']:
                    raise SystemExit('Teil %d Flaeche %d/%d: Ecken sind nicht die von K5e_f1' % (t, a, i))
                d = set((xn - xa, yn - ya) for (xa, ya), (xn, yn) in zip(gx(fa), gx(fn)))
                if len(d) != 1:
                    raise SystemExit('Teil %d Flaeche %d/%d: UV nicht rein verschoben (%s)' % (t, a, i, sorted(d)))
                delta[(t, a, i)] = (d.pop(), int(fn['clut']), int(fn['page']), int(fa['clut']), gx(fa))
                if delta[(t, a, i)][0] != (0, 0):
                    ber['muetter_verschoben'] += 1
    netze = {int(t): json.loads(json.dumps(n)) for t, n in tab['teile'].items()}
    for t, a, i, mt, ma, mi in tab['herkunft']:
        (dx, dy), clut, page, clut_alt, mxy = delta[(mt, ma, mi)]
        f = netze[t][art[a]][i]
        if int(f['clut']) != clut_alt:
            raise SystemExit('Tabelle: Kind %d|%d|%d hat nicht die CLUT-Zeile der Mutter' % (t, a, i))
        if clut != clut_alt:
            ber['kinder_clut_neu'] += 1
        if (dx, dy) != (0, 0) or page != int(f['page']):
            xy = gx(f)
            x0, x1 = min(x for x, _y in mxy) - 1, max(x for x, _y in mxy) + 1
            y0, y1 = min(y for _x, y in mxy) - 1, max(y for _x, y in mxy) + 1
            if any(not (x0 <= x <= x1 and y0 <= y <= y1) for x, y in xy):
                ber['kinder_ausserhalb'].append('%d|%d|%d' % (t, a, i))
            uv = [[x + dx - (page & 15) * 128, y + dy] for x, y in xy]
            if min(u for u, _v in uv) < 0 or max(u for u, _v in uv) > 255 or min(v for _u, v in uv) < 0 or max(v for _u, v in uv) > 255:
                raise SystemExit('Kind %d|%d|%d: UV ausserhalb der Seite nach Verschiebung' % (t, a, i))
            f['uv'] = uv
            f['page'] = page
            ber['kinder_verschoben'] += 1
        f['clut'] = clut
    for t in MULDEN_TEILE:
        form['meshes'][t] = netze[t]
    return ber


if __name__ == '__main__':
    import hashlib
    sys.dont_write_bytecode = True
    was = sys.argv[1] if len(sys.argv) > 1 else ''
    if was == 'bauen':
        geometrie_bauen(sys.argv[2] if len(sys.argv) > 2 else None)
    elif was == 'pruef':
        a, b = geometrie_bytes(), geometrie_bytes()
        d = open(GEO_ZIEL, 'rb').read() if os.path.exists(GEO_ZIEL) else bytes()
        gleich = a == b == d
        print('zwei Laeufe und Datei byte-gleich: %s  sha256 %s' % ('JA' if gleich else 'NEIN', hashlib.sha256(a).hexdigest()))
        sys.exit(0 if gleich else 1)
    else:
        raise SystemExit('Aufruf: python kf_k5g2_geo.py bauen [<ziel.EMD>] | pruef')
