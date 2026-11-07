#!/usr/bin/env python3
"""Kandidat B: ganzer Durchgang.
    python b_alles.py --fit zw/fit4.pt --tag 5 [--ab mesh|blender|snap|bau] [--snap 25]
"""
import argparse, json, math, os, subprocess, sys, time, pickle
import numpy as np, cv2, torch
from b_geom import *
import b_teile as T
import b_bau as BB
import b_mesh as BM
import b_naht as BN
import b_snap as SN
import b_lauf as L
import b_mal2 as M2

WAHL = dict(fl_t1=(16, 0, 1.0), fl_f=(8, 0, 0.9), fl_a=(9, 0, 0.92), fl_k=(5, 0, 1.0), nr_i1=(7, 4, 0.92))
BLENDER = 'C:/Program Files/Blender Foundation/Blender 5.2/blender.exe'
BUDGET = {'kuppel': 640, 'k01': 56, 'k02': 56, 'k03': 44, 'k04': 40, 'k05': 118, 'k06': 64, 'k07': 48, 'k08': 44, 'k11': 72, 'k12': 62, 'k13': 88, 'k14': 56, 'k15': 54, 'k16': 64, 'k17': 62, 'k18': 88, 'k19': 44, 'k20': 58}


def boden_dy(teile, R, Tt, J):
    u = -1e9
    for st in teile:
        b = st['bone']
        y = ((R[b] @ (st['V'] - J[b]).T).T + Tt[b])[:, 1].max()
        u = max(u, y)
    return -u


def snap_lauf(fit, quad, schritte):
    import b_lage2 as LG
    J = np.asarray(fit['J'], float)
    w2 = np.asarray(fit['w2'], float).copy()
    w2[[T.IDX['rumpf_m'], T.IDX['rumpf'], T.IDX['maul_l'], T.IDX['maul_r']]] = 0
    n = len(T.KNOCHEN)
    teile = SN.stuecke(quad)
    R1, T1 = BB.fk(np.zeros((n, 3)), J, T.ELTERN, 0.0)
    R2, T2 = BB.fk(w2, J, T.ELTERN, fit['dy2'])
    ziele = SN.baue_ziele()
    lage2, gier2 = np.array(fit['lage2'], float), float(fit['gier2'])

    def sichten():
        d1 = boden_dy(teile, R1, T1, J)
        d2 = boden_dy(teile, R2, T2, J)
        T1b = [t + [0, d1, 0] for t in T1]
        T2b = [t + [0, d2, 0] for t in T2]
        return SN.Sicht(1, K1, O1, G1, R1, T1b, J), SN.Sicht(2, K2, lage2, gier2, R2, T2b, J), T2b
    for it in range(schritte):
        s1, s2, _ = sichten()
        b = SN.schritt([s1], teile, ziele, rate=0.6)
        if it % 4 == 0 or it == schritte - 1:
            print('Snap B1 %2d: %d Ecken, Mittel %.1f, max %.1f px' % (it, *b[1]), flush=True)
    s1, s2, T2b = sichten()
    lage2, gier2 = LG.suche(teile, R2, T2b, J, lage2, gier2)
    kuppel = [st for st in teile if st['name'] == 'kuppel']
    for it in range(6):
        s1, s2, _ = sichten()
        b = SN.schritt([s2], kuppel, ziele, rate=0.5, sigma=170.0)
        s1, s2, _ = sichten()
        c = SN.schritt([s1], teile, ziele, rate=0.45)
        print('Snap B2 %2d: %d Ecken, Mittel %.1f, max %.1f px | B1 %d, %.1f, %.1f' % (it, *b.get(2, (0, 0, 0)), *c[1]), flush=True)
    for it in range(8):
        s1, s2, _ = sichten()
        b = SN.schritt([s1], teile, ziele, rate=0.5, sigma=70.0)
    print('Snap B1 fein: %d Ecken, Mittel %.1f, max %.1f px' % b[1], flush=True)
    for it in range(8):
        s1, s2, _ = sichten()
        c = SN.schritt([s2], kuppel, ziele, rate=0.6, sigma=120.0)
        s1, s2, _ = sichten()
        b = SN.schritt([s1], teile, ziele, rate=0.45, sigma=70.0)
    print('Snap wechselnd: B1 %d, %.1f, %.1f  B2 %d, %.1f, %.1f' % (*b[1], *c.get(2, (0, 0, 0))), flush=True)
    s1, s2, T2b = sichten()
    gl = [T.IDX[k] for k in ('fr_ober', 'fr_unter', 'nr_ober', 'nr_unter', 'nl_ober', 'fl_ober', 'fl_unter')]
    lage2, gier2, w2 = LG.suche(teile, R2, T2b, J, lage2, gier2, schritte=(30, 15), g_schritte=(6, 3), w2=w2, dy=fit['dy2'], gelenke=gl)
    return {st['name']: st['V'] for st in teile}, w2, lage2, gier2


def wertung(aus):
    R = json.load(open(aus + '/bericht.json', encoding='utf-8'))
    z = 0
    for n in (1, 2):
        B = R['bild%d' % n]
        for k in ('punkte', 'flaechen', 'zeichnung', 'verbote'):
            z += sum(1 for x in B[k] if x['zaehlt'] and x['ok'])
    return (z, -R['bild1']['umriss']['schwarz_getroffen'], round(R['bild1']['ncc_weiss'] + R['bild2']['ncc_weiss'], 3))


def knoten_schieben(fit, wahl):
    """Einzelne Knoten in der Bildebene von Bild 1 nachstellen (dx, dy Pixel, Radiusfaktor). Handarbeit an Stellen, die der Fit nicht trifft."""
    p = np.array(fit['pos'], float)
    r = np.array(fit['rad'], float)
    ni = {n: i for i, n in enumerate(fit['namen'])}
    f = K1['brennweite_bild_px']
    for k, (dx, dy, fr) in wahl.items():
        i = ni[k]
        w = welt(p[i])
        t = FK.tiefe(K1, w)
        w2 = w + K1['R'][0] * dx * t / f['fx'] + K1['R'][1] * dy * t / f['fy']
        p[i] = lokal(w2)
        r[i] *= fr
    fit = dict(fit)
    fit['pos'], fit['rad'] = p, r
    return fit


def alle_sichten(fit, teile, w2):
    J = np.asarray(fit['J'], float)
    n = len(T.KNOCHEN)
    R1, T1 = BB.fk(np.zeros((n, 3)), J, T.ELTERN, 0.0)
    R2, T2 = BB.fk(w2, J, T.ELTERN, fit['dy2'])
    d1 = boden_dy(teile, R1, T1, J)
    d2 = boden_dy(teile, R2, T2, J)
    return {1: SN.Sicht(1, K1, O1, G1, R1, [t + [0, d1, 0] for t in T1], J), 2: SN.Sicht(2, K2, fit['lage2'], float(fit['gier2']), R2, [t + [0, d2, 0] for t in T2], J)}


def durchgang(fit_pfad, tag, ab='mesh', schritte=14, malen=None, runden=8, wahl=None):
    import b_fein as FN
    roh = BM.lade(fit_pfad)
    if wahl:
        roh = knoten_schieben(roh, wahl)
    fit, rinne = BN.ergaenze(roh)
    z = 'zw/g%s' % tag
    os.makedirs(z, exist_ok=True)
    if ab == 'mesh':
        BM.baue(fit, z + '/mesh', kuppel_extra=rinne)
        ab = 'blender'
    if ab == 'blender':
        json.dump(BUDGET, open(z + '/budget.json', 'w'))
        r = subprocess.run([BLENDER, '-b', '-P', 'b_blender.py', '--', z + '/mesh', z + '/quad', z + '/budget.json'], capture_output=True, text=True, encoding='utf-8', errors='replace')
        for l in r.stdout.splitlines():
            if l.startswith(('Rueckfall', 'Traceback')) or 'Error' in l:
                print(l[:120])
        ab = 'snap'
    if ab == 'snap':
        pickle.dump(snap_lauf(fit, z + '/quad', schritte), open(z + '/snap.pkl', 'wb'))
    V, w2, fit['lage2'], fit['gier2'] = pickle.load(open(z + '/snap.pkl', 'rb'))
    fit['w2'] = w2
    torch.save(fit, z + '/fit.pt')
    teile = SN.stuecke(z + '/quad')
    if ab == 'fein' and os.path.exists(z + '/snap_fein.pkl'):
        V, w2, fit['lage2'], fit['gier2'] = pickle.load(open(z + '/snap_fein.pkl', 'rb'))
    for st in teile:
        st['V'] = np.array(V[st['name']], float)
    form = None
    beste = None
    import b_ruhe
    ruhe_w = b_ruhe.ruhe_winkel(fit)
    for r_ in range(runden):
        V = {st['name']: st['V'].copy() for st in teile}
        form = L.lauf(z + '/fit.pt', z + '/quad', z + '/v', malen=malen if r_ == runden - 1 or True else None, snap=V, leise=r_ < runden - 1, ruhe_w=ruhe_w)
        pickle.dump((V, w2, fit['lage2'], fit['gier2']), open(z + '/snap_fein.pkl', 'wb'))
        w_ = wertung(z + '/v')
        if beste is None or w_ > beste[0]:
            beste = (w_, {k: v.copy() for k, v in V.items()})
            print('neue Bestmarke', w_, flush=True)
        n = FN.runde(teile, alle_sichten(fit, teile, w2), z + '/v')
        M2.faktor_nachziehen(z + '/v')
        print('Feinrunde', r_, 'offen', n, flush=True)
        if n == 0 and r_ >= 2:
            break
        pickle.dump((V, w2, fit['lage2'], fit['gier2']), open(z + '/snap_fein.pkl', 'wb'))
    if beste is not None:
        V = beste[1]
        pickle.dump((V, w2, fit['lage2'], fit['gier2']), open(z + '/snap_fein.pkl', 'wb'))
        form = L.lauf(z + '/fit.pt', z + '/quad', z + '/v', malen=malen, snap=V, ruhe_w=ruhe_w)
    return form


if __name__ == '__main__':
    ap = argparse.ArgumentParser()
    ap.add_argument('--fit', required=True)
    ap.add_argument('--tag', required=True)
    ap.add_argument('--ab', default='mesh')
    ap.add_argument('--snap', type=int, default=14)
    ap.add_argument('--runden', type=int, default=8)
    a = ap.parse_args()
    t0 = time.time()
    durchgang(a.fit, a.tag, a.ab, a.snap, malen=M2.malen, wahl=WAHL, runden=a.runden)
    print('%.0fs' % (time.time() - t0))
