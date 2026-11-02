# -*- coding: utf-8 -*-
"""Pruefer Spielmodell: Biegetest. Je Gelenk Abriss und Durchblick, dazu benannte Posen wie die Tick-Clips (Gehen, Hieb, Biss, Drohen, Seitenhieb)."""
import sys, os, json, math
import numpy as np
import cv2
from scipy.spatial import cKDTree
from g_gelenk import *

sys.path.insert(0, WT + '/re15_port/tools')
import form_buehne as FB
import form_pruefung as PR

BAU = {
    'A': dict(FR=(1, 4, 5), FL=(2, 7, 8), NR=(16, 17, 18), NL=(11, 12, 13), mitte=9, vorn=10, kopf=(20, 21), kiefer=22, namen=NAMEN['A']),
    'B': dict(FR=(5, 6, 7), FL=(1, 2, 3), NR=(16, 17, 18), NL=(11, 12, 13), mitte=9, vorn=10, kopf=(), kiefer=None, lippen=(15, 20), namen=NAMEN['B']),
    'C': dict(FR=(1, 2, 3), FL=(5, 6, 7), NR=(11, 12, 13), NL=(16, 17, 18), mitte=9, vorn=10, kopf=(14, 15), kiefer=20, namen=NAMEN['C']),
}


def posen(k):
    b = BAU[k]

    def bein(D, ch, a, c, f, x=0):
        D[ch[0]] = Rz(a) @ Rx(x)
        D[ch[1]] = Rz(c)
        D[ch[2]] = Rz(f)
    P = {}
    # Gehen: Diagonale FR+NL hinten, FL+NR vorn und angehoben
    D = {}
    bein(D, b['FR'], 24, -8, -16)
    bein(D, b['NL'], 24, -8, -16)
    bein(D, b['FL'], -34, 48, -14)
    bein(D, b['NR'], -34, 48, -14)
    D[b['mitte']] = Ry(5)
    D[b['vorn']] = Ry(5)
    P['gehen'] = D
    # Hieb: Schlagglied NR hoch, Rumpf richtet sich auf
    D = {}
    D[b['mitte']] = Rz(-7)
    D[b['vorn']] = Rz(-8)
    bein(D, b['NR'], -118, 34, 22, x=12)
    bein(D, b['NL'], 12, -6, 0)
    P['hieb'] = D
    # Biss: Rumpf neigt sich vor, Maul oder Kiefer auf
    D = {}
    D[b['mitte']] = Rz(9)
    D[b['vorn']] = Rz(11)
    bein(D, b['NR'], -30, 30, 0)
    bein(D, b['NL'], -30, 30, 0)
    for x, w in zip(b['kopf'], (-14, -8)):
        D[x] = Rz(w)
    if b.get('kiefer') is not None:
        D[b['kiefer']] = Rz(32)
    if b.get('lippen'):
        D[b['lippen'][0]] = Ry(20)
        D[b['lippen'][1]] = Ry(-20)
    P['biss'] = D
    # Drohen: aufrichten um die Wurzel, Arme hoch
    D = {}
    D[0] = Rz(-26)
    D[b['mitte']] = Rz(-5)
    D[b['vorn']] = Rz(-6)
    bein(D, b['FR'], 26, 0, 0)
    bein(D, b['FL'], 26, 0, 0)
    bein(D, b['NR'], -70, 40, 10, x=-14)
    bein(D, b['NL'], -70, 40, 10, x=14)
    P['drohen'] = D
    # Seitenhieb: Rumpf dreht, NL schwingt quer
    D = {}
    D[b['vorn']] = Ry(16)
    D[b['mitte']] = Ry(8)
    bein(D, b['NL'], -60, 25, 0, x=62)
    P['seitenhieb'] = D
    return P


def welt_von(form, Rn, Tn):
    return welt(form, Rn, Tn)


def abriss(form, R, T, tris, W, namen):
    """Je Gelenk: Teilbaum um Z und X bis 60 Grad drehen; groesster Abstand zum Rest. Probenabstand 18, unter 30 gilt als Kontakt."""
    n = len(form['knochen'])
    S = [proben(W[i], tris[i]) for i in range(n)]
    aus = []
    for b in range(1, n):
        if len(W[b]) == 0:
            continue
        tb = set(teilbaum(form, b))
        rest = [i for i in range(n) if i not in tb and len(S[i])]
        if not rest:
            continue
        baum = cKDTree(np.vstack([S[i] for i in rest]))
        pts = np.vstack([S[i] for i in tb if len(S[i])])
        schlimm = 0.0
        wo = None
        schw = None
        for ax, f in (('z', Rz), ('x', Rx)):
            for g in (-60, -40, -20, 20, 40, 60):
                M = f(g)
                q = (pts - T[b]) @ M.T + T[b]
                d, _ = baum.query(q, k=1)
                m = float(d.min())
                if m > schlimm:
                    schlimm, wo = m, (ax, g)
                if m > 30 and (schw is None or abs(g) < abs(schw[1])):
                    schw = (ax, g)
        aus.append((b, namen[b], round(schlimm, 1), wo, schw))
    return aus


def rumpf_probe(form, R, T, tris, W, b, name):
    """Durchblick des Rumpfes bei Biegung der Rumpfteile gegeneinander."""
    z = []
    for ax, f in (('z', Rz), ('y', Ry), ('x', Rx)):
        for g in (6, 12, 20, 30):
            Rn, Tn = gebeugt(form, R, T, {b: f(g)})
            W2 = welt(form, Rn, Tn)
            s, bst, _ = durchblick(form, W2, tris, azs=range(0, 360, 45), els=(-35, 0, 30, 65))
            Rn, Tn = gebeugt(form, R, T, {b: f(-g)})
            W3 = welt(form, Rn, Tn)
            s2, bst2, _ = durchblick(form, W3, tris, azs=range(0, 360, 45), els=(-35, 0, 30, 65))
            z.append((ax, g, s, s2))
    return z


def neu_form(form, R, T, poses):
    neu = FE.neu()
    neu['knochen'] = form['knochen']
    neu['meshes'] = form['meshes']
    neu['tim'] = form['tim']
    Rr = [np.round(r * 4096).astype(np.int64) for r in R]
    for nm, D in poses.items():
        Rn, Tn = gebeugt(form, R, T, D)
        win = []
        for b in range(len(form['knochen'])):
            p = form['knochen'][b]['eltern']
            L = Rn[b] if p < 0 else Rn[p].T @ Rn[b]
            win.append(FE.winkel_aus_matrix(L))
        FE.clip_dazu(neu, nm, [FE.bild(neu, wurzel=tuple(int(round(c)) for c in Tn[0]), winkel=win)])
    FE.boden_nachfuehren(neu, 0)
    return neu


def blatt(k, neu, tim_b, ziel, gamma=True, gross=340):
    emd = FE.schreibe_emd(neu, pruefen=False)
    mo = FE.modell(emd, tim_b)
    tex = FB.Textur(tim_b)
    SICHT = (('3/4 vorn', 50, 18), ('Seite', 180, 4), ('3/4 hinten', -135, 22))
    tafeln = []
    for ci, c in enumerate(neu['clips']):
        f0, _n = mo.clips[ci]
        kf = mo.frames[f0] & 0xFFF
        for t, az, el in SICHT:
            a = np.full((gross, gross, 3), 92, np.uint8)
            m = FB.rastern(PR.rundum_dreiecke(mo, kf, az, el, gross), tex, a, dict(ax=1.0, bx=0.0, ay=1.0, by=0.0))
            if gamma:
                a[m] = np.clip(np.power(a[m] / 255.0, 0.6) * 255, 0, 255).astype(np.uint8)
            tafeln.append(('%s %s %s' % (k, c['name'], t), a))
    FB.montage(tafeln, 3, ziel, '%s: Biegetest, Pose aus Ruhe-Clip gerechnet, gleiche Winkel je Kandidat, Gamma 0,6' % k)


if __name__ == '__main__':
    erg = {}
    for k in ('A', 'B', 'C'):
        form = lade(k)
        n = len(form['knochen'])
        R, T = pose(form, 2)
        W = welt(form, R, T)
        tris = [dreiecke(m) for m in form['meshes'][:n]]
        namen = BAU[k]['namen']
        print('==', k)
        print(' Bodenkontakt je Glied (tiefster Punkt des Teilbaums, Clip 0/1/2):')
        for gl, ch in (('FR', BAU[k]['FR']), ('FL', BAU[k]['FL']), ('NR', BAU[k]['NR']), ('NL', BAU[k]['NL'])):
            bb = teilbaum(form, ch[0])
            z = []
            for c in (0, 1, 2):
                Rc, Tc = pose(form, c)
                Wc = welt(form, Rc, Tc)
                z.append(round(max(float(Wc[i][:, 1].max()) for i in bb if len(Wc[i]))))
            print('  ', gl, z)
        ab = abriss(form, R, T, tris, W, namen)
        print(' Abriss (Teilbaum gegen Rest, bis +-60 Grad um z, x): groesster Abstand, wo; erste Abrissstufe > 30')
        for r in ab:
            if r[2] > 30:
                print('  ', r)
        print('  Gelenke ohne Abriss:', sum(1 for r in ab if r[2] <= 30), 'von', len(ab))
        P = posen(k)
        erg[k] = dict(abriss=ab, posen={})
        s0, b0, _ = durchblick(form, W, tris)
        print(' Durchblick Ruhe', s0, 'schlechtester', b0[0], b0[1])
        for nm, D in P.items():
            Rn, Tn = gebeugt(form, R, T, D)
            W2 = welt(form, Rn, Tn)
            s, bst, _ = durchblick(form, W2, tris)
            print(' Durchblick', nm, s, 'schlechtester', bst[0], bst[1])
            erg[k]['posen'][nm] = [s, bst[0], bst[1]]
            if bst[2] is not None:
                g = np.zeros(bst[2].shape + (3,), np.uint8)
                g[bst[3] > 0] = (150, 150, 150)
                g[bst[2] > 0] = (255, 0, 0)
                cv2.imwrite(AUS + '/loch_%s_%s.png' % (k, nm), g[:, :, ::-1])
        print(' Rumpfbiegung, Durchblick (Achse, Grad, plus, minus), Teil vorn:')
        for r in rumpf_probe(form, R, T, tris, W, BAU[k]['vorn'], 'vorn'):
            print('  ', r)
        tim_b = open(K[k][1], 'rb').read()
        neu = neu_form(form, R, T, P)
        blatt(k, neu, tim_b, AUS + '/biege_%s.png' % k)
    json.dump(erg, open(AUS + '/biege.json', 'w'), indent=1, default=str)
