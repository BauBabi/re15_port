#!/usr/bin/env python3
"""Variante K5, Kern: Figur aus J (Rumpf, Glieder, Schwanz, Textur H2) mit frei stellbarem Gerippe (Winkel je Clip, Knochenversatz, Halslaenge),
K5-Kopf aus k5_kopf, Tiefenpuffer mit Teil-Kennungen (Knochen 0-31, Augen 100/101), Landmarken-Projektion und die Messung R1-R10 aus
FORMBLATT_V2 Abschnitt 6. Kamera und Raster aus k_kern / form_kamera (unveraendert). Kein RE-Wert."""
import copy
import json
import math
import os
import sys

import cv2
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
KGEN = os.path.normpath(os.path.join(HIER, '..', '..', 'K_alligator', 'generator'))
sys.path.insert(0, KGEN)
import k_kern as KK  # noqa: E402
import k_kopf_fit as KFIT  # noqa: E402
import k5_kopf as K5  # noqa: E402

FE, FK = KK.FE, KK.FK
FORM = KK.FORM
K5_DIR = os.path.normpath(os.path.join(HIER, '..'))
H, W = KK.H, KK.W
AUGE_L, AUGE_R = 100, 101
SCRATCH_PRUEFER = os.environ.get('K5_PRUEFER', r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/leser/pruefer')
mat = KFIT.mat
RECHTER_ARM = (16, 17, 18, 19, 20, 29, 31)
LINKER_ARM = (11, 12, 13, 14, 15, 28, 30)
KOPF = (21, 22, 23)

# ───────────────────────── Ziele (FORMBLATT_V2 2.1, Bild 2 Original 640x480)
L1 = (280.0, 275.0)      # linkes Auge (W), rechts (B)
L2 = (217.0, 275.0)      # rechtes Auge (W), links (B)
L4 = (245.0, 371.0)      # OK-Spitze
L5_GRAD = 1.9            # Kopfmittellinie Augenmitte (248,5/275) -> L4 gegen die Senkrechte
L6_NUTZER = (279.0, 399.0)
L6_TIEF = (264.0, 408.0)
L7 = (329.0, 326.0)      # Maulwinkel links (W)
L8 = (312.0, 331.0)      # Strichende "Oberseite vom Maul"
L8_KNICK = (316.0, 303.0)
LANDMARKEN_BILD = dict(L1=L1, L2=L2, L4=L4, L6=L6_NUTZER, L7=L7, L8=L8)


def boden_masken():
    """R9: Bodenpixel in fuss_links/fuss_rechts (Pruefer, kNN in Lab; 189 + 160 px)."""
    return np.load(os.path.join(SCRATCH_PRUEFER, 'boden_fuss_links.npy')), np.load(os.path.join(SCRATCH_PRUEFER, 'boden_fuss_rechts.npy'))


def lappen_maske():
    """R5: Lappen der Unterkiefer-Maske unter y 372 (FORMBLATT_V2 2.1 L6: x 243-308, tiefster Punkt (264,408))."""
    m = KK.maske('unterkiefer').copy()
    m[:373] = False
    return m


# ───────────────────────── Figur
class Figur:
    """J (FORM.EMD/TIM) + K4-Krallen-Form (unveraendert von J) + K5-Kopf. Haelt form (form_emd-Struktur), Winkel je Clip (pool), Versatz,
    Kopfparameter. Gelenkkoerper kommen erst im Bau (k5_bau) dazu; die Messung im Fit laeuft ohne sie (wie k_kopf_fit, k_kopf_fit2 nahm nur die
    Kopfkoerper mit)."""

    def __init__(self, kopf_p=None, basis_dir=None):
        self.form, self.mo, self.fj = KK.lade(basis_dir or KK.J_DIR)
        self.kopf_p = dict(K5.VORGABE)
        if kopf_p:
            self.kopf_p.update(kopf_p)
        self.versatz = [np.array(k['versatz'], float) for k in self.form['knochen']]
        self.eltern = [k['eltern'] for k in self.form['knochen']]
        self.winkel = {c: [list(w) for w in self.form['pool'][c]['winkel']] for c in range(3)}
        self.wurzel = {c: np.array(self.form['pool'][c]['wurzel'], float) for c in range(3)}
        self.hals_basis = np.array(self.form['meshes'][21]['ecken'], float)
        self.halslaenge = 503.0
        self._netze()

    # Geometrie je Knochen als (V, F) in Dreiecken, Kennung je Dreieck
    def _netze(self):
        self.geo = {}
        for b, m in enumerate(self.form['meshes']):
            if b in (21, 22, 23) or not m['ecken']:
                continue
            self.geo[b] = (np.array(m['ecken'], float), KK.flaechen(m), np.full(len(KK.flaechen(m)), b))
        self.kopf_neu()
        self.hals_neu(self.halslaenge)

    def kopf_neu(self, p=None):
        if p:
            self.kopf_p.update(p)
        p = self.kopf_p
        o = K5.kopf(p)
        P, F, _T, _N = o.arrays()
        Ft = KFIT.flaechen_tris(F)
        kenn = np.full(len(Ft), 22)
        # Augen-Dreiecke an der Texelstelle 'auge' (Atlas) erkennen, Seite nach z
        T = np.array(o.t)
        ax, ay, aw, ah = K5.ATLAS['auge']
        im_auge = np.array([np.all((T[f, 0] >= ax) & (T[f, 0] <= ax + aw) & (T[f, 1] >= ay) & (T[f, 1] <= ay + ah)) for f in Ft])
        z = P[Ft].mean(1)[:, 2]
        kenn[im_auge & (z > 0)] = AUGE_L
        kenn[im_auge & (z <= 0)] = AUGE_R
        self.geo[22] = (P, Ft, kenn)
        o = K5.kiefer(p)
        P, F, _T, _N = o.arrays()
        Ft = KFIT.flaechen_tris(F)
        self.geo[23] = (P, Ft, np.full(len(Ft), 23))
        self.versatz[23] = np.array([p['gx'], p['gy'], 0.0])
        self.lm = K5.landmarken(p)

    def hals_neu(self, laenge):
        """Hals (Knochen 21) auf `laenge` strecken: Ecken mit x > 0 linear (x * laenge / 503), Querschnitt bleibt; Kopfversatz (22) = laenge."""
        self.halslaenge = float(laenge)
        V = self.hals_basis.copy()
        k = self.halslaenge / 503.0
        V[:, 0] = np.where(V[:, 0] > 0, V[:, 0] * k, V[:, 0])
        F = KK.flaechen(self.form['meshes'][21])
        self.geo[21] = (V, F, np.full(len(F), 21))
        self.versatz[22] = np.array([self.halslaenge, 0.0, 0.0])

    # Gerippe
    def welt(self, clip, winkel=None, lage_gier=None):
        """Weltmatrizen (Gleitkomma, mat_euler) je Knochen fuer Clip `clip` mit Winkel-Ueberschreibung {Knochen: [ax, ay, az]}."""
        W = [list(w) for w in self.winkel[clip]]
        for b, w in (winkel or {}).items():
            W[int(b)] = list(w)
        lage, gier = lage_gier or self._lage(clip)
        g = gier * 2 * math.pi / 4096.0
        Y = np.array([[math.cos(g), 0, math.sin(g)], [0, 1, 0], [-math.sin(g), 0, math.cos(g)]])
        Rl, Tl = [None] * 32, [None] * 32
        for b in range(32):
            e = self.eltern[b]
            M = mat(W[b])
            if e < 0:
                Rl[b], Tl[b] = M, self.wurzel[clip].copy()
            else:
                Rl[b] = Rl[e] @ M
                Tl[b] = Rl[e] @ self.versatz[b] + Tl[e]
        Rw = [Y @ r for r in Rl]
        Tw = [Y @ t + np.asarray(lage, float) for t in Tl]
        return Rw, Tw

    def _lage(self, clip):
        n = {0: 1, 1: 2}.get(clip)
        if n is None:
            return (0, 0, 0), 0
        s = self.fj['bild%d' % n]
        return s['lage'], s['gier']

    def dreiecke(self, Rw, Tw, knochen=None, ohne=()):
        P, K = [], []
        for b, (V, F, kenn) in self.geo.items():
            if (knochen is not None and b not in knochen) or b in ohne or not len(F):
                continue
            v = V @ Rw[b].T + Tw[b]
            P.append(v[F])
            K.append(kenn)
        if not P:
            return np.zeros((0, 3, 3)), np.zeros(0, int)
        return np.concatenate(P), np.concatenate(K)

    def punkt(self, Rw, Tw, b, p):
        return Rw[b] @ np.asarray(p, float) + Tw[b]

    def landmarken_welt(self, Rw, Tw):
        out = {}
        for b, d in self.lm.items():
            for k, v in d.items():
                out[k] = self.punkt(Rw, Tw, b, v)
        return out


def raster(kam, tris, kenn, tief=None, idb=None):
    return KK.raster(kam, tris, kenn, tief, idb)


# ───────────────────────── Messung R1-R10
def _winkel_bild(a, b):
    """Winkel der Strecke a->b gegen die Senkrechte (Grad, + = unten nach rechts)."""
    d = np.asarray(b, float) - np.asarray(a, float)
    return math.degrees(math.atan2(d[0], d[1]))


def sichtbar(idb, px, kennungen, r=2):
    x, y = int(round(px[0])), int(round(px[1]))
    if not (0 <= x < W and 0 <= y < H):
        return False
    sub = idb[max(y - r, 0):y + r + 1, max(x - r, 0):x + r + 1]
    return bool(np.isin(sub, kennungen).any())


def messen_bild2(idb, lmb, kam=None, umriss=None, iou_ok=None):
    """R1-R7 und die Kiefer-IoU aus dem Tiefenpuffer Bild 2 (idb mit Augen-Kennungen) und den projizierten Landmarken lmb (Bildpunkte)."""
    r = {}
    # R1 Augen sichtbar und nahe am Ziel (Schwerpunkt der sichtbaren Augenpixel, sonst Projektion)
    au = {}
    for nm, kenn, ziel in (('L1', AUGE_L, L1), ('L2', AUGE_R, L2)):
        ys, xs = np.nonzero(idb == kenn)
        proj = lmb['auge_l' if kenn == AUGE_L else 'auge_r']
        if len(xs):
            c = (float(xs.mean()), float(ys.mean()))
        else:
            c = (float(proj[0]), float(proj[1]))
        au[nm] = dict(px=int(len(xs)), schwerpunkt=[round(c[0], 1), round(c[1], 1)], projektion=[round(float(proj[0]), 1), round(float(proj[1]), 1)],
                      abstand=round(float(math.hypot(c[0] - ziel[0], c[1] - ziel[1])), 1))
    ok1 = all(au[k]['px'] >= 3 and au[k]['abstand'] <= 6 for k in au)
    r['R1'] = dict(ok=ok1, wert='L1 %s px %.1f / L2 %s px %.1f' % (au['L1']['px'], au['L1']['abstand'], au['L2']['px'], au['L2']['abstand']), augen=au)
    # R2 Augenlinie
    a, b = au['L2']['schwerpunkt'] if au['L2']['px'] else au['L2']['projektion'], au['L1']['schwerpunkt'] if au['L1']['px'] else au['L1']['projektion']
    ab = math.hypot(b[0] - a[0], b[1] - a[1])
    wk = math.degrees(math.atan2(b[1] - a[1], b[0] - a[0]))
    r['R2'] = dict(ok=abs(wk) <= 3 and 57 <= ab <= 69, wert='%.1f Grad, %.1f px' % (wk, ab), grad=round(wk, 1), abstand=round(ab, 1))
    # R3 OK-Spitze
    t = lmb['ok_spitze']
    d3 = math.hypot(t[0] - L4[0], t[1] - L4[1])
    r['R3'] = dict(ok=d3 <= 6, wert='%.1f px (%.0f/%.0f)' % (d3, t[0], t[1]), abstand=round(d3, 1), punkt=[round(float(t[0]), 1), round(float(t[1]), 1)],
                   sichtbar=sichtbar(idb, t, (22,)))
    # R4 Kopfmittellinie Augenmitte -> OK-Spitze gegen L5
    m = ((a[0] + b[0]) / 2, (a[1] + b[1]) / 2)
    w4 = _winkel_bild(m, t)
    r['R4'] = dict(ok=abs(w4 - (-L5_GRAD)) <= 5, wert='%.1f Grad (Ziel %.1f)' % (w4, -L5_GRAD), grad=round(w4, 1))
    # R5 UK-Vorderende fuellt den Lappen, Nutzerpunkt im Modell-UK
    lap = lappen_maske()
    uk = (idb == 23)
    dk = float((uk & lap).sum()) / max(float(lap.sum()), 1.0)
    pn = sichtbar(idb, L6_NUTZER, (23,), r=1)
    r['R5'] = dict(ok=dk >= 0.6 and pn, wert='Lappen %.0f %%, (279,399) %s' % (100 * dk, 'drin' if pn else 'nicht drin'), deckung=round(dk, 3), nutzerpunkt=pn,
                   uk_spitze=[round(float(x), 1) for x in lmb['uk_spitze']])
    # R6 Maulwinkel links (W)
    q = lmb['maulwinkel_l']
    d6 = math.hypot(q[0] - L7[0], q[1] - L7[1])
    r['R6'] = dict(ok=d6 <= 8, wert='%.1f px (%.0f/%.0f)' % (d6, q[0], q[1]), abstand=round(d6, 1), punkt=[round(float(q[0]), 1), round(float(q[1]), 1)])
    # R7 sichtbarer Unterkiefer: Schwerpunkt rechts (B) der Mittellinie, Achse 30-47 Grad steigend
    ys, xs = np.nonzero(uk)
    if len(xs) > 20:
        cx, cy = xs.mean(), ys.mean()
        # Mittellinie m -> t, Abstand vorzeichenbehaftet (+ = rechts im Bild)
        d = np.array(t, float) - np.array(m, float)
        nrm = np.array([d[1], -d[0]]) / max(np.linalg.norm(d), 1e-6)
        if nrm[0] < 0:
            nrm = -nrm
        seite = float((np.array([cx, cy]) - np.array(m)) @ nrm)
        C = np.cov(np.stack([xs - cx, ys - cy]))
        ev, evec = np.linalg.eigh(C)
        v = evec[:, -1]
        achse = math.degrees(math.atan2(-v[1], v[0]))
        achse = achse % 180.0
        r['R7'] = dict(ok=seite > 0 and 30 <= achse <= 47, wert='%+.0f px, Achse %.1f Grad' % (seite, achse), seite=round(seite, 1), achse=round(achse, 1),
                       schwerpunkt=[round(float(cx), 1), round(float(cy), 1)], px=int(len(xs)))
    else:
        r['R7'] = dict(ok=False, wert='UK sichtbar %d px' % len(xs), px=int(len(xs)))
    okm, ukm = KK.maske('oberkiefer'), KK.maske('unterkiefer')
    kopf = np.isin(idb, (22, AUGE_L, AUGE_R))
    r['iou'] = dict(ok=round(KK.iou(kopf, okm), 3), uk=round(KK.iou(uk, ukm), 3))
    if umriss is not None:
        r['iou']['umriss2_raster'] = umriss
    return r


def messen_bild1(idb):
    """R9 Bodenpixel frei, R10 Schwanz-Scheitel hoechster Punkt (y <= 190)."""
    bl, br = boden_masken()
    gl, gr = int((idb[bl] >= 0).sum()), int((idb[br] >= 0).sum())
    ys, xs = np.nonzero(idb >= 0)
    y0 = int(ys.min())
    oben = np.unique(idb[y0][idb[y0] >= 0]).tolist()
    # hoechster Punkt des Schwanzes (Knochen 1-6) und des Rests
    sch = np.isin(idb, (1, 2, 3, 4, 5, 6))
    ys_s = np.nonzero(sch)[0]
    rest = (idb >= 0) & ~sch
    ys_r = np.nonzero(rest)[0]
    ys_ = int(ys_s.min()) if len(ys_s) else 999
    yr_ = int(ys_r.min()) if len(ys_r) else 999
    return dict(R9=dict(ok=gl == 0 and gr == 0, wert='%d + %d von %d + %d px gedeckt' % (gl, gr, int(bl.sum()), int(br.sum())), links=gl, rechts=gr),
                R10=dict(ok=ys_ <= 190 and ys_ <= yr_, wert='Schwanz y %d, Rest y %d' % (ys_, yr_), schwanz_y=ys_, rest_y=yr_, oben=oben))


def umriss(idb, n):
    z = cv2.imread(os.path.join(FORM, 'ziel2_bild%d.png' % n), 0)
    weiss, grau = z == 255, z == 128
    fig = idb >= 0
    return round(float((fig & weiss).sum()) / max(float(((fig | weiss) & ~grau).sum()), 1.0), 4)


def projiziere_lm(kam, lmw):
    return {k: FK.projiziere(kam, v) for k, v in lmw.items()}
