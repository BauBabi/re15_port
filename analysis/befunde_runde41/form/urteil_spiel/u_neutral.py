# -*- coding: utf-8 -*-
"""Pruefer Spielmodell: identische Posen fuer alle drei, vom Gerippe aus gesetzt, losgeloest von den Bildposen. Winkel innerhalb gerippe.json winkelgrenzen_grad.
Clip 0 bind, 1 kauer (Ellbogen und Knie 120 Grad), 2 sprung, 3 kurve (Schwanz seitlich, Rumpf gegen).
Schreibt neutral_<k>.EMD/.TIM in den Scratchpad, Blaetter neutral_ton.png, neutral_tex.png."""
import shutil
from u_basis import *
from u_biege import bindepose
from u_ton import Ton

SPEICHER = EICH + '/../biege'
SCHW = [I['schwanz%d' % i] for i in range(1, 7)]


def posen():
    P = {}
    P['bind'] = {}
    D = {}
    for s in 'lr':
        D[I['oberarm_' + s]] = Rz(60)
        D[I['unterarm_' + s]] = Rz(-120)
        D[I['hand_' + s]] = Rz(60)
        D[I['oberschenkel_' + s]] = Rz(-60)
        D[I['unterschenkel_' + s]] = Rz(120)
        D[I['fuss_' + s]] = Rz(-60)
    D[I['hals']] = Rz(-15)
    D[I['kopf']] = Rz(-15)
    P['kauer'] = D
    D = {}
    D[0] = Rz(-18)
    D[I['wirbel']] = Rz(-10)
    D[I['brust']] = Rz(-12)
    D[I['hals']] = Rz(25)
    D[I['kopf']] = Rz(15)
    D[I['kiefer']] = Rz(35)
    for s in 'lr':
        D[I['oberarm_' + s]] = Rz(-105)
        D[I['unterarm_' + s]] = Rz(-55)
        D[I['hand_' + s]] = Rz(45)
        D[I['oberschenkel_' + s]] = Rz(45)
        D[I['unterschenkel_' + s]] = Rz(35)
        D[I['fuss_' + s]] = Rz(25)
    for b, g in zip(SCHW, (-22, -14, -8, 8, 14, 22)):
        D[b] = Rz(g)
    P['sprung'] = D
    D = {}
    D[I['wirbel']] = Ry(-14)
    D[I['brust']] = Ry(-14)
    D[I['hals']] = Ry(-18)
    for s, g in (('l', 25), ('r', -25)):
        D[I['oberarm_' + s]] = Rz(g)
        D[I['unterarm_' + s]] = Rz(-45)
        D[I['oberschenkel_' + s]] = Rz(-g)
        D[I['unterschenkel_' + s]] = Rz(45)
    for b in SCHW:
        D[b] = Ry(28)
    P['kurve'] = D
    return P


def neu_form(form, P):
    R0, T0 = bindepose(form)
    neu = FE.neu()
    neu['knochen'] = form['knochen']
    neu['meshes'] = form['meshes']
    neu['tim'] = form['tim']
    for nm, D in P.items():
        Rn, Tn = gebeugt(form, R0, T0, D)
        win = []
        for b in range(len(form['knochen'])):
            p = form['knochen'][b]['eltern']
            L = Rn[b] if p < 0 else Rn[p].T @ Rn[b]
            win.append(FE.winkel_aus_matrix(L))
        FE.clip_dazu(neu, nm, [FE.bild(neu, wurzel=tuple(int(round(c)) for c in Tn[0]), winkel=win)])
    FE.boden_nachfuehren(neu, 0)
    return neu


if __name__ == '__main__':
    os.makedirs(SPEICHER, exist_ok=True)
    P = posen()
    SICHT = (('links', 180, 6), ('3/4 vorn', 50, 20), ('vorn', 90, 8), ('3/4 hinten', -130, 24))
    t1, t2 = [], []
    for k in ROHBAU:
        form = lade(k)
        neu = neu_form(form, P)
        emd = FE.schreibe_emd(neu, pruefen=False)
        open(SPEICHER + '/neutral_%s.EMD' % k, 'wb').write(emd)
        shutil.copyfile(K[k][1], SPEICHER + '/neutral_%s.TIM' % k)
        tim = open(K[k][1], 'rb').read()
        mo = FE.modell(emd, tim)
        tex = FB.Textur(tim)
        for ci, c in enumerate(neu['clips']):
            f0, _ = mo.clips[ci]
            kf = mo.frames[f0] & 0xFFF
            for s, az, el in SICHT:
                t1.append(('%s %s %s' % (k, c['name'], s), bild(mo, Ton(tex), kf, az, el, 330, grund=58)[0]))
                t2.append(('%s %s %s' % (k, c['name'], s), bild(mo, tex, kf, az, el, 330, gamma=0.6)[0]))
    FB.montage(t1, 4, AUS + '/neutral_ton.png', 'Identische Posen vom Gerippe aus, Ton')
    FB.montage(t2, 4, AUS + '/neutral_tex.png', 'Identische Posen vom Gerippe aus, Textur, Gamma 0,6')
