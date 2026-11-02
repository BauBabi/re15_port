# -*- coding: utf-8 -*-
"""Pruefer Spielmodell: RE2-Gelenke im Ton in echten Clip-Bildern p90 und p99, Blick auf der Achse und von aussen. Kalibriert, was am Knick normal ist. Nur Handwerk, keine Form."""
from u_biege import *
from u_biege_blatt import filtern
from u_krater import aussen_ansicht

d = json.load(open(AUS + '/biege.json'))['eich']
REIHE = (('K34', 'ellbogen'), ('K34', 'knie'), ('K34', 'wirbel'), ('K28', 'ellbogen'), ('K28', 'knie'), ('K28', 'schwanz1'), ('K28', 'schwanz2'), ('K33', 'knie'), ('K33', 'ellbogen2'), ('K23', 'schwanz2'), ('K22', 'knie'))
t = []
for k, nm in REIHE:
    form = lade(k)
    mo, tex = modell(k)
    for x in EICH[k]:
        if x[0] == nm:
            _, p, j, e = x
    teile = [p, j, e]
    if k == 'K28' and nm == 'schwanz1':
        teile = [18, 19, 20]
    for q in ('90', '99'):
        m = d[k][nm]['mess'][q]
        R, T = pose(form, m['clip'], m['bild'])
        u1 = T[j] - T[p]
        u2 = T[e] - T[j]
        n_w = np.cross(u1, u2)
        n_w = n_w / np.linalg.norm(n_w)
        L = max(np.linalg.norm(u1), np.linalg.norm(u2))
        s = 300 * 0.40 / L
        u1n, u2n = u1 / np.linalg.norm(u1), u2 / np.linalg.norm(u2)
        q_m = filtern(mo, set(teile))
        a1 = tafel(q_m, tex, R, T, ansicht(u1n, n_w), T[j], s, 300)
        a2 = tafel(q_m, tex, R, T, aussen_ansicht(u1n, u2n, n_w), T[j], s, 300)
        a3 = tafel(q_m, tex, R, T, aussen_ansicht(u1n, u2n, -n_w), T[j], s, 300)
        t.append(('%s %s p%s %.0f Grad achse' % (k, nm, q, m['g']), a1))
        t.append(('%s %s p%s aussen' % (k, nm, q), a2))
        t.append(('%s %s p%s aussen 2' % (k, nm, q), a3))
FB.montage(t, 6, AUS + '/eich_gelenke.png', 'RE2-Gelenke in echten Clip-Bildern, Ton. Handwerks-Eichmass, keine Formvorlage')
