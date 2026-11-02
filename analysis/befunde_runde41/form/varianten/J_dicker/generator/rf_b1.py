#!/usr/bin/env python3
"""Nacharbeit Abnahme 2, Stufe 1: Pranke. M3 rechte Hand Pose 1 flach, M4 Handverhaeltnis 1,3, Bild 2 linker Lappen. Form und Lage aller anderen Teile stehen.
python rf_b1.py VON AUS [--breit 0.8] [--gier 12] [--p 300] [--f 200] [--ende 150] [--ohne-umbau]"""
import argparse
import json
import math
import os

import numpy as np
import torch

from rf_kern import HIER, innen_tiefe, namen
import rf_plan
import rf_fit
from rf_fit import Passung
import rf_auflage
from rf_a1 import mat_euler, kette_R


def teil_nr(t, nm):
    return next(i for i, p in enumerate(t) if p.name == nm)


def schmal(s, t, f, fa=.98):
    """Pranke schmaler: innen (Ecke 2) um f, aussen (Ecke 6) nur um fa, Ballen sitzt zur Aussenseite (Bild 2 linker Lappen, Bild 1 Spitze rechts). Handruecken als hohe Kuppel: Bild 1 sieht sie hinter dem Unterarm nicht, Bild 2 fuellt sie die Oberkante der Hand."""
    i = teil_nr(t, 'hand_r')
    L = np.array(s['lr'][str(i)])
    m = (f + fa) / 2
    kuppel = [1.0, 1.05, 1.18, 1.30, 1.30, 1.15]
    for r in range(L.shape[0]):
        h = kuppel[r]
        L[r] += np.log([1.0, (1 + f) / 2, f, (f + h) / 2, h, (fa + h) / 2, fa, (1 + fa) / 2])
    s['lr'][str(i)] = L.tolist()
    c = np.array(s['c'][str(i)])
    c[:, 1] *= m
    s['c'][str(i)] = c.tolist()
    # Fingerwurzeln kraeftiger: Bild 2 zeigt dunkle Masse statt Faecher. Aussenfinger bleibt schlank.
    for nm, k in (('klaue_r2', 1.18), ('kralle_r', 1.2), ('klaue_r1', 1.18)):
        i = teil_nr(t, nm)
        L = np.array(s['lr'][str(i)])
        L[:2] += math.log(k)
        L[2] += math.log(.5 * (1 + k))
        s['lr'][str(i)] = L.tolist()


def welt_setzen(g, s, n, name, Rz):
    """Knochen auf Drehung Rz im Aktorraum setzen, Kinder folgen."""
    ix = namen(g)
    w = s['posen'][str(n)]['winkel']
    i = ix[name]
    e = g['knochen'][i]['eltern']
    R = kette_R(g, w)
    w[i] = mat_euler(R[e].T @ Rz)


def hand_flach(psi):
    """X nach vorn, um psi nach aussen (rechte Hand: -Z), Y senkrecht nach unten."""
    a = math.radians(psi)
    X = np.array([math.cos(a), 0, -math.sin(a)])
    Y = np.array([0, 1.0, 0])
    return np.stack([X, Y, np.cross(X, Y)], 1)


def arm_ik(pa, n, wurzel_knochen, ziel_xy, ziel_h, ellbogen_xy=None, schritte=500):
    """Nur Oberarm und Unterarm: Handgelenk an (Bildpunkt, Hoehe). Ohne Masken, reine Kette. Ergebnis geht in w0."""
    ix = pa.ix
    p = pa.pose[n]
    b0, b1, b2 = [ix[x] for x in wurzel_knochen]
    d = torch.zeros(len(pa.g['knochen']), 3, device=pa.dev, requires_grad=True)
    m = torch.zeros(len(pa.g['knochen']), 3, device=pa.dev)
    m[b0] = 1.0
    m[b1] = torch.tensor([0.0, 0.0, 1.0], device=pa.dev)        # Ellbogen ist ein Scharnier, Rolle des Unterarms bleibt
    opt = torch.optim.Adam([d], lr=4.0)
    for _ in range(schritte):
        opt.zero_grad()
        w = p['w0'] + d * m
        R, T = pa.kette(w, pa.versatz, p['wz0'] + p['dwz'], p['gier0'], p['lage0'])
        q = pa.kam[n](T[b2:b2 + 1])[0]
        L = ((q[0] - ziel_xy[0]) ** 2 + (q[1] - ziel_xy[1]) ** 2) + ((-T[b2, 1] - ziel_h) / 15.0) ** 2
        if ellbogen_xy is not None:
            e = pa.kam[n](T[b1:b1 + 1])[0]
            L = L + .25 * ((e[0] - ellbogen_xy[0]) ** 2 + (e[1] - ellbogen_xy[1]) ** 2)
        L = L + ((d * m) / 400.0).pow(2).sum() * .1
        L.backward()
        opt.step()
    with torch.no_grad():
        p['w0'] += d * m
        R, T = pa.kette(p['w0'], pa.versatz, p['wz0'] + p['dwz'], p['gier0'], p['lage0'])
        print('IK', [round(float(x)) for x in pa.kam[n](T[b2:b2 + 1])[0]], 'Hoehe', round(-float(T[b2, 1])), 'Ellbogen', [round(float(x)) for x in pa.kam[n](T[b1:b1 + 1])[0]], 'Winkel Oberarm', np.round(p['w0'][b0].cpu().numpy() * 360 / 4096, 1).tolist(), 'Unterarm', np.round(p['w0'][b1].cpu().numpy() * 360 / 4096, 1).tolist())


def main():
    a = argparse.ArgumentParser()
    a.add_argument('von')
    a.add_argument('aus')
    a.add_argument('--breit', type=float, default=.72)
    a.add_argument('--gier', type=float, default=12)
    a.add_argument('--p', type=int, default=300)
    a.add_argument('--f', type=int, default=200)
    a.add_argument('--ende', type=int, default=120)
    a.add_argument('--ohne-umbau', action='store_true')
    a.add_argument('--flach', type=float, default=6.0)
    a.add_argument('--anker', type=float, default=.4)
    a = a.parse_args()
    g, t = rf_plan.plan()
    print('Flaechen', sum(p.flaechen() for p in t))
    s = json.load(open(os.path.join(HIER, a.von)))
    if not a.ohne_umbau:
        schmal(s, t, a.breit)
    pa = Passung(g, t, s)
    pa.basis_jetzt()
    ix = pa.ix
    if not a.ohne_umbau:
        arm_ik(pa, 1, ('oberarm_r', 'unterarm_r', 'hand_r'), (394, 358), 300, (401, 294))
        # Hand flach: Drehung im Aktorraum, aus der neuen Armlage gerechnet
        wn = pa.pose[1]['w0'].cpu().numpy().tolist()
        R = kette_R(g, wn)
        e = g['knochen'][ix['hand_r']]['eltern']
        wn[ix['hand_r']] = mat_euler(R[e].T @ hand_flach(a.gier))
        pa.pose[1]['w0'][:] = torch.tensor(wn, dtype=torch.float32, device=pa.dev)
    # Harte Sperre: nur Arme, Haende, Finger bewegen sich, Rumpf, Beine, Schwanz, Kopf stehen
    glieder = ('oberarm_r', 'unterarm_r', 'hand_r', 'oberarm_l', 'unterarm_l', 'hand_l', 'klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4', 'klaue_l1', 'klaue_l2', 'klaue_l3', 'klaue_l4')
    for n in (1, 2):
        pa.pose_frei[n][:] = 0.0
        for b in glieder:
            pa.pose_frei[n][ix[b]] = 1.0
    frei = {'hand_r', 'kralle_r', 'klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4', 'unterarm_r'}
    pa.gesperrt = {p.name for p in t} - frei
    pa.pose_gew[1][:] = 8.0
    pa.pose_gew[2][:] = 8.0
    for b in ('oberarm_r', 'unterarm_r', 'hand_r', 'hand_l', 'unterarm_l', 'oberarm_l'):
        pa.pose_gew[1][ix[b]] = 1.0
        pa.pose_gew[2][ix[b]] = 1.0
    for b in ('klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4', 'klaue_l1', 'klaue_l2', 'klaue_l3', 'klaue_l4'):
        pa.pose_gew[1][ix[b]] = .2
        pa.pose_gew[2][ix[b]] = .2
    pa.mitte_gew = {'klaue_r%d' % i: 8.0 for i in (1, 2, 3, 4)}
    pa.mitte_gew['kralle_r'] = 8.0
    pa.entwurf.update({i: 12.0 for i in pa.netz.meister if t[i].name.startswith('kl') or t[i].name.startswith('kr')})
    pa.entwurf[teil_nr(t, 'hand_r')] = .5
    rf_fit.SPITZEN[1]['klaue_r4'] = ((381, 368), .12)     # Daumen fuellt die linke untere Ecke des Fusses
    rf_fit.SPITZEN[1]['kralle_r'] = ((412, 345), .02)
    rf_fit.SPITZEN[1]['klaue_l4'] = ((336, 358), .08)
    rf_fit.SPITZEN[2].update(klaue_r2=((157, 425), .05), kralle_r=((178, 462), .03), klaue_r1=((208, 474), .03))
    pa.gewicht.update(sicht=3.0, rolle=.6, fenster=.6, o1=.12, kerbe=.1, band=2.0)
    pa.anker[1] = [('hand_l', (0, 0, 0), (328, 352), 300, .06), ('unterarm_l', (0, 0, 0), (318, 298), None, .04), ('unterarm_r', (0, 0, 0), (403, 296), None, .05), ('klaue_l3', (0, 0, 0), (316, 351), None, .06), ('klaue_l4', (0, 0, 0), (338, 354), None, .03),
                   ('hand_r', (0, 0, 0), (394, 358), 300, a.anker)]
    pa.anker[2] = [('hand_r', (0, 0, 0), (272, 380), None, .1)]
    pa.gewicht.update(rund=1.5, laengs=1.5)
    kp = [pa.kappe['klaue_r%d' % i] for i in (1, 2, 3, 4)] + [pa.kappe['kralle_r']]
    wh = ix['hand_r']

    kk = [teil_nr(t, x) for x in ('kopf', 'kiefer')]
    ob = next(sel for nm, sel in pa.innen if nm == 'oberarm_r')

    def zusatz(n, R, T, w, xy, L_=None, C_=None):
        if n != 1:
            # Oberarm rechts bleibt aus Kopf und Kiefer (M5 Abnahme 2)
            q_ = w[ob]
            dm = None
            for i in kk:
                de = innen_tiefe(q_, t[i], pa.netz.pl[i], L_[i], C_[i], R, T)
                dm = de if dm is None else torch.maximum(dm, de)
            p_ = torch.relu(dm + 20.0) / 60.0
            return 2 * (p_ * p_).mean()
        L = a.flach * (1 - R[wh][1, 1])                       # Handflaeche waagerecht
        h = -w[torch.tensor(kp, device=w.device), 1]
        L = L + (torch.relu(h - 260.0) / 120.0).pow(2).mean() * 2   # keine Klinge steht hoch
        return L
    pa.zusatz = zusatz
    if a.p:
        pa.lauf(a.p, 2, 1.5, form=False, lr_w=3.0, lr_wz=0.0, zeige=60, tag='P')
    if a.f:
        pa.lauf(a.f, 2, 1.5, tag='F', zeige=60, lr_wz=0.0)
    pa.gewicht.update(rund=1.5, laengs=2.0, gestalt=0.02, mitte=0.06, naht=0.8)
    if a.ende:
        pa.lauf(a.ende, 1, 0.9, lr=0.004, lr_c=1.5, lr_w=0.8, tag='e', zeige=60, lr_wz=0.0)
    z = pa.stand()
    z.update({k: s[k] for k in ('m3', 'umzug', 'k7') if k in s})
    for n in ('1', '2'):
        z['posen'][n]['wurzel'] = s['posen'][n]['wurzel']
        z['posen'][n]['lage'] = s['posen'][n]['lage']
        z['posen'][n]['gier'] = s['posen'][n]['gier']
    os.makedirs(os.path.dirname(os.path.join(HIER, a.aus)), exist_ok=True)
    json.dump(z, open(os.path.join(HIER, a.aus), 'w'))
    rf_auflage.blatt(pa, os.path.join(HIER, a.aus.replace('.json', '.png')))
    print('fertig', a.aus)


if __name__ == '__main__':
    main()
