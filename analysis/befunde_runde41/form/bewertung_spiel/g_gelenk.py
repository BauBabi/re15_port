# -*- coding: utf-8 -*-
"""Pruefer Spielmodell: Gelenke, Bodenkontakt, Durchblick (Pixel nur von Rueckseiten bedeckt = Loch im Spiel)."""
import sys, os, json, math
import numpy as np
import cv2
from scipy.spatial import cKDTree

WT = 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form'
sys.path.insert(0, WT + '/re15_port/tools')
import form_emd as FE

FORM = WT + '/analysis/befunde_runde41/form'
AUS = FORM + '/bewertung_spiel'
K = {'A': (FORM + '/kandidat_a/FORM.EMD', FORM + '/kandidat_a/FORM.TIM', 2),
     'B': (FORM + '/kandidat_b/FORM.EMD', FORM + '/kandidat_b/FORM.TIM', 2),
     'C': (FORM + '/kandidat_c/FORM.EMD', FORM + '/kandidat_c/FORM.TIM', 2),
     'G4': (WT + '/re15_port/shared_assets/RE2/BAHNSTEIG_G4.EMD', WT + '/re15_port/shared_assets/RE2/BAHNSTEIG_G4.TIM', 5)}
NAMEN = {
    'A': 'becken huefte_r huefte_l fr_ober fr_unter fr_fuss fl_ober fl_unter fl_fuss rumpf_mitte rumpf nl_ober nl_unter nl_hand nl_innen nl_daumen nr_ober nr_unter nr_hand nr_daumen hals kopf kiefer'.split(),
    'B': 'rumpf_f fl_ober fl_unter fl_fuss fl_zeh fr_ober fr_unter fr_fuss fr_zeh rumpf_m rumpf nl_ober nl_unter nl_faust nl_innen maul_l nr_ober nr_unter nr_faust nr_innen maul_r'.split(),
    'C': 'rumpf_h fr_huefte fr_bein fr_fuss fr_zeh fl_huefte fl_bein fl_fuss fl_zeh rumpf_m rumpf_v nr_ober nr_unter nr_hand hals vorsprung nl_ober nl_kugel_o nl_kugel_u nl_unter kiefer'.split(),
}


def lade(k):
    return FE.lese_emd(open(K[k][0], 'rb').read())


def dreiecke(m):
    t = [tuple(f['e']) for f in m['dreiecke']]
    for f in m['vierecke']:
        a, b, c, d = f['e']
        t.append((a, b, d))
        t.append((a, d, c))
    return np.array(t, dtype=np.int64).reshape(-1, 3)


def pose(form, clip, bild=0):
    b = form['clips'][clip]['bilder'][bild]
    R, T = FE.pose_aus_bild(form, b)
    return [r.astype(float) / 4096.0 for r in R], [t.astype(float) for t in T]


def welt(form, R, T):
    aus = []
    for i, m in enumerate(form['meshes'][:len(form['knochen'])]):
        v = np.array(m['ecken'], float).reshape(-1, 3)
        aus.append(v @ R[i].T + T[i])
    return aus


def proben(p, F, abstand=18.0, seed=1):
    if len(F) == 0:
        return np.zeros((0, 3))
    a, b, c = p[F[:, 0]], p[F[:, 1]], p[F[:, 2]]
    fl = 0.5 * np.linalg.norm(np.cross(b - a, c - a), axis=1)
    n = np.maximum(np.ceil(fl / (abstand * abstand)).astype(int), 1)
    rng = np.random.default_rng(seed)
    idx = np.repeat(np.arange(len(F)), n)
    r1 = np.sqrt(rng.random(len(idx)))[:, None]
    r2 = rng.random(len(idx))[:, None]
    return (1 - r1) * a[idx] + r1 * (1 - r2) * b[idx] + r1 * r2 * c[idx]


def gwn(q, p, F):
    """Verallgemeinerte Windungszahl der Punkte q gegen das Netz, Betrag; offen vertraeglich."""
    a = p[F[:, 0]][None] - q[:, None]
    b = p[F[:, 1]][None] - q[:, None]
    c = p[F[:, 2]][None] - q[:, None]
    la, lb, lc = [np.linalg.norm(x, axis=2) for x in (a, b, c)]
    z = np.einsum('ijk,ijk->ij', a, np.cross(b, c))
    n = la * lb * lc + np.einsum('ijk,ijk->ij', a, b) * lc + np.einsum('ijk,ijk->ij', b, c) * la + np.einsum('ijk,ijk->ij', c, a) * lb
    return np.abs(np.arctan2(z, n).sum(axis=1) / (2 * math.pi))


def kinder(form):
    n = len(form['knochen'])
    k = [[] for _ in range(n)]
    for i, b in enumerate(form['knochen']):
        if b['eltern'] >= 0:
            k[b['eltern']].append(i)
    return k


def teilbaum(form, b):
    k = kinder(form)
    s = [b]
    aus = []
    while s:
        x = s.pop()
        aus.append(x)
        s += k[x]
    return aus


def Rz(g):
    c, s = math.cos(math.radians(g)), math.sin(math.radians(g))
    return np.array([[c, -s, 0], [s, c, 0], [0, 0, 1.0]])


def Rx(g):
    c, s = math.cos(math.radians(g)), math.sin(math.radians(g))
    return np.array([[1.0, 0, 0], [0, c, -s], [0, s, c]])


def Ry(g):
    c, s = math.cos(math.radians(g)), math.sin(math.radians(g))
    return np.array([[c, 0, s], [0, 1.0, 0], [-s, 0, c]])


def gebeugt(form, R, T, D):
    """D = {Knochen: 3x3 im Weltraum der Ruhelage, um den eigenen Gelenkpunkt}. Liefert R', T' der ganzen Kette."""
    n = len(form['knochen'])
    Rw, tw = [None] * n, [None] * n
    Rn, Tn = [None] * n, [None] * n
    for b in range(n):
        p = form['knochen'][b]['eltern']
        J = D.get(b, np.eye(3))
        if p < 0 or p >= b:
            Rw[b] = J
            tw[b] = T[b] - J @ T[b]
        else:
            Rw[b] = Rw[p] @ J
            tw[b] = Rw[p] @ (T[b] - J @ T[b]) + tw[p]
        Rn[b] = Rw[b] @ R[b]
        Tn[b] = Rw[b] @ T[b] + tw[b]
    return Rn, Tn


# ═══════════════════ Durchblick
def blickachsen(az, el):
    a, e = math.radians(az), math.radians(el)
    vor = np.array([-math.sin(a) * math.cos(e), math.sin(e), math.cos(a) * math.cos(e)])
    rechts = np.array([math.cos(a), 0.0, math.sin(a)])
    unten = np.cross(vor, rechts)
    if unten[1] < 0 and abs(el) < 80:
        unten = -unten
    return np.stack([rechts, unten, vor])


def durchblick(form, W, tris, gross=900, azs=range(0, 360, 30), els=(-35, 0, 20, 40, 65), bild_aus=False):
    """Zaehlt je Blick Pixel, die eine Rueckseite traegt, aber keine Vorderseite. Oeffnung 3 x 3: unter 3 px je Richtung zaehlt nicht."""
    alle = np.vstack([w for w in W if len(w)])
    mitte = (alle.max(0) + alle.min(0)) / 2
    rad = float(np.linalg.norm(alle - mitte, axis=1).max())
    s = gross * 0.46 / rad
    ke = np.ones((3, 3), np.uint8)
    summe = 0
    bester = (0, None, None, None)
    je = {}
    for az in azs:
        for el in els:
            A = blickachsen(az, el)
            vorne = np.zeros((gross, gross), np.uint8)
            hinten = np.zeros((gross, gross), np.uint8)
            for w, F in zip(W, tris):
                if len(F) == 0:
                    continue
                q = (w - mitte) @ A.T
                xy = np.round(q[:, :2] * s * 16 + gross * 8).astype(np.int32)
                t = xy[F]
                d = (t[:, 1, 0] - t[:, 0, 0]) * (t[:, 2, 1] - t[:, 0, 1]) - (t[:, 2, 0] - t[:, 0, 0]) * (t[:, 1, 1] - t[:, 0, 1])
                # Spielregel: sichtbar, wenn Mac0 >= 0 im Bild (form_buehne), also Windung wie gespeichert
                for tt, dd in zip(t, d):
                    cv2.fillConvexPoly(vorne if dd > 0 else hinten, tt, 1, lineType=cv2.LINE_8, shift=4)
            loch = (hinten > 0) & (vorne == 0)
            loch = cv2.morphologyEx(loch.astype(np.uint8), cv2.MORPH_OPEN, ke)
            n = int(loch.sum())
            summe += n
            je[(az, el)] = n
            if n > bester[0]:
                bester = (n, (az, el), loch.copy(), vorne.copy())
    return summe, bester, s


def kennzahlen(k):
    form = lade(k)
    n = len(form['knochen'])
    ruhe = K[k][2]
    R, T = pose(form, ruhe)
    W = welt(form, R, T)
    tris = [dreiecke(m) for m in form['meshes'][:n]]
    namen = NAMEN.get(k, ['k%d' % i for i in range(n)])
    print('==', k, 'Ruhe-Clip', ruhe)
    print(' b  name          elt  Gelenk(x,y,z)         Laenge  bbox(dx,dy,dz)        Gelenk in Teil/Eltern  Abstand Teil-Eltern')
    proben_b = [proben(W[i], tris[i]) for i in range(n)]
    baum = [cKDTree(p) if len(p) else None for p in proben_b]
    zeilen = []
    for b in range(n):
        el = form['knochen'][b]['eltern']
        if len(W[b]) == 0:
            continue
        bb = W[b].max(0) - W[b].min(0)
        Lg = float(np.linalg.norm(T[b] - T[el])) if el >= 0 else 0.0
        inn_k = inn_e = -1
        ab = -1.0
        if el >= 0 and len(W[el]):
            q = T[b][None]
            inn_k = float(gwn(q, W[b], tris[b])[0])
            inn_e = float(gwn(q, W[el], tris[el])[0])
            d, _ = baum[el].query(proben_b[b], k=1)
            ab = float(d.min())
        zeilen.append(dict(b=b, name=namen[b], eltern=el, gelenk=[round(float(x)) for x in T[b]], laenge=round(Lg), bbox=[round(float(x)) for x in bb],
                           in_teil=round(inn_k, 2), in_eltern=round(inn_e, 2), abstand=round(ab, 1)))
        print(' %2d %-12s %3d  %6d %6d %6d  %5d  %5d %5d %5d   %5.2f %5.2f   %6.1f' % (b, namen[b], el, *T[b], Lg, *bb, inn_k, inn_e, ab))
    return form, R, T, W, tris, zeilen


def boden(form, tris, clips):
    """Tiefster Punkt je Blattknochen (Glied-Ende) je Clip, Y unten; 0 = Boden."""
    n = len(form['knochen'])
    k = kinder(form)
    blaetter = [b for b in range(n) if not k[b] and form['meshes'][b]['ecken']]
    aus = {}
    for c in clips:
        R, T = pose(form, c)
        W = welt(form, R, T)
        aus[c] = {b: round(float(W[b][:, 1].max())) for b in blaetter}
        print('  Clip', c, 'tiefster Punkt je Blatt', aus[c], 'Gesamt', round(max(float(w[:, 1].max()) for w in W if len(w))))
    return aus


if __name__ == '__main__':
    erg = {}
    for k in ('A', 'B', 'C', 'G4'):
        form, R, T, W, tris, z = kennzahlen(k)
        clips = [0, 1, 2] if k != 'G4' else [5]
        boden(form, tris, clips)
        s, b, sk = durchblick(form, W, tris)
        print('  Durchblick Ruhe: Summe px', s, 'schlechtester Blick', b[0], b[1], 'Massstab px je Einheit %.3f' % sk)
        erg[k] = dict(gelenke=z, durchblick_ruhe=s, schlechtester=[b[0], b[1]], massstab=sk)
    json.dump(erg, open(AUS + '/gelenk.json', 'w'), indent=1, default=str)
