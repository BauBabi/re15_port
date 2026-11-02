# -*- coding: utf-8 -*-
"""Pruefer Spielmodell, Grundlage: Modelle laden, Posen, Dreiecke. Liest nur. Schreibt nichts ausser in urteil_spiel/ und den Scratchpad."""
import sys, os, json, math
import numpy as np
WT = 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form'
sys.path.insert(0, WT + '/re15_port/tools')
import form_emd as FE
import form_buehne as FB
import form_pruefung as PR

FORM = WT + '/analysis/befunde_runde41/form'
AUS = FORM + '/urteil_spiel'
EICH = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/eich'
K = {'A': (FORM + '/rohbau_a/FORM.EMD', FORM + '/rohbau_a/FORM.TIM'),
     'B': (FORM + '/rohbau_b/FORM.EMD', FORM + '/rohbau_b/FORM.TIM'),
     'C': (FORM + '/rohbau_c/FORM.EMD', FORM + '/rohbau_c/FORM.TIM')}
for _k in ('34', '33', '23', '28', '2B', '2E', '31', '30', '22', '20'):
    K['K' + _k] = (EICH + '/K%s.EMD' % _k, EICH + '/K%s.TIM' % _k)
ROHBAU = ('A', 'B', 'C')
NAMEN = 'huefte schwanz1 schwanz2 schwanz3 schwanz4 schwanz5 schwanz6 wirbel oberschenkel_l oberschenkel_r brust oberarm_l unterarm_l hand_l klaue_l1 klaue_l2 oberarm_r unterarm_r hand_r klaue_r1 klaue_r2 hals kopf kiefer unterschenkel_l fuss_l unterschenkel_r fuss_r klaue_l3 klaue_r3 klaue_l4 klaue_r4'.split()
I = {n: i for i, n in enumerate(NAMEN)}


def lade(k):
    return FE.lese_emd(open(K[k][0], 'rb').read())


def modell(k):
    emd = open(K[k][0], 'rb').read()
    tim = open(K[k][1], 'rb').read()
    return FE.modell(emd, tim), FB.Textur(tim)


def kf_von(mo, clip, bild=0):
    f0, _ = mo.clips[clip]
    return mo.frames[f0 + bild] & 0xFFF


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


def Rachse(a, g):
    a = np.asarray(a, float)
    a = a / np.linalg.norm(a)
    c, s = math.cos(math.radians(g)), math.sin(math.radians(g))
    K_ = np.array([[0, -a[2], a[1]], [a[2], 0, -a[0]], [-a[1], a[0], 0]])
    return np.eye(3) + s * K_ + (1 - c) * K_ @ K_


def gebeugt(form, R, T, D):
    """D = {Knochen: 3x3 im Weltraum der Ausgangspose, um den eigenen Gelenkpunkt}. Liefert R', T' der ganzen Kette."""
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


def bild(mo, tex, kf, az, el, gross, gamma=None, grund=92):
    a = np.full((gross, gross, 3), grund, np.uint8)
    m = FB.rastern(PR.rundum_dreiecke(mo, kf, az, el, gross), tex, a, dict(ax=1.0, bx=0.0, ay=1.0, by=0.0))
    if gamma:
        a[m] = np.clip(np.power(a[m] / 255.0, gamma) * 255, 0, 255).astype(np.uint8)
    return a, m
