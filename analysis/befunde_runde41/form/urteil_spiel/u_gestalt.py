# -*- coding: utf-8 -*-
"""Pruefer Spielmodell: Proportionen in Ruhe. Stuetzt das Urteil, ersetzt es nicht. Masse als Volumen je Gruppe (Divergenzsatz je Teil), Gliedstaerke als Radius der Teilmitte."""
from u_basis import *
from u_mass import querschnitt


def volumen(V, F):
    a, b, c = V[F[:, 0]], V[F[:, 1]], V[F[:, 2]]
    return abs(float(np.einsum('ij,ij->i', a, np.cross(b, c)).sum() / 6.0))


GR = {'Rumpf': ('huefte', 'wirbel', 'brust'), 'Kopf': ('hals', 'kopf', 'kiefer'), 'Schwanz': tuple('schwanz%d' % i for i in range(1, 7)),
      'Arme': ('oberarm_l', 'unterarm_l', 'hand_l', 'oberarm_r', 'unterarm_r', 'hand_r'), 'Beine': ('oberschenkel_l', 'unterschenkel_l', 'fuss_l', 'oberschenkel_r', 'unterschenkel_r', 'fuss_r')}
for k in ROHBAU:
    form = lade(k)
    R, T = pose(form, 2)
    W = welt(form, R, T)
    P = np.vstack([w for w in W if len(w)])
    e = P.max(0) - P.min(0)
    ge = {}
    for g, t in GR.items():
        ge[g] = sum(volumen(np.array(form['meshes'][I[x]]['ecken'], float), dreiecke(form['meshes'][I[x]])) for x in t) / 1e9
    su = sum(ge.values())
    rumpf = np.vstack([W[I[x]] for x in ('huefte', 'wirbel', 'brust')])
    tiefe = rumpf[:, 1].max() - rumpf[:, 1].min()
    breite = rumpf[:, 2].max() - rumpf[:, 2].min()
    bauch = -float(rumpf[:, 1].max())
    rucken = -float(rumpf[:, 1].min())
    print(k, 'Laenge %d Hoehe %d Breite %d | Rumpf tief %d breit %d, Bauch ueber Boden %d, Ruecken %d | Masse %s' % (e[0], e[1], e[2], tiefe, breite, bauch, rucken, {g: '%.0f %%' % (100 * v / su) for g, v in ge.items()}))
for k, cl, rumpf, arm, bein, schw in (('K34', (5, 40), (0, 9, 10), (12, 13), (1, 2, 3), ()), ('K28', (0, 0), (0, 1), (9, 10), (13, 14), (16, 18, 19, 20, 21, 22)), ('K33', (0, 0), (0, 1, 2), (11, 12), (23, 24), ())):
    form = lade(k)
    R, T = pose(form, *cl)
    W = welt(form, R, T)
    P = np.vstack([w for w in W if len(w)])
    e = P.max(0) - P.min(0)
    v = lambda t: sum(volumen(np.array(form['meshes'][x]['ecken'], float), dreiecke(form['meshes'][x])) for x in t) / 1e9
    su = v([b for b in range(len(form['knochen'])) if form['meshes'][b]['ecken']])
    print(k, 'Laenge %d Hoehe %d Breite %d | Rumpf %.0f %%, Arm je Paar %.0f %%, Bein %.0f %%, Schwanz %.0f %%' % (e[0], e[1], e[2], 100 * v(rumpf) / su, 200 * v(arm) / su, 200 * v(bein) / su, 100 * v(schw) / su if schw else 0))
