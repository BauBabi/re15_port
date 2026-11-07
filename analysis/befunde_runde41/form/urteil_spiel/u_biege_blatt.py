# -*- coding: utf-8 -*-
"""Pruefer Spielmodell: Nahblaetter der Biegeprobe. Ton, nur das Glied selbst (Eltern, Kind, Nachfolger), Blick entlang der Achse und von aussen auf den Knick."""
from u_biege import *


def filtern(mo, nur):
    class M:
        pass
    a = M()
    a.meshes = [mo.meshes[i] if i in nur else leer for i in range(len(mo.meshes))]
    a.skel = mo.skel
    return a


class Leer:
    tv = tn = tf = tuv = qv = qn = qf = quv = ()


leer = Leer()


def knick(mo, tex, form, p, c, teile, f, n, vz, g, seite, sicht, s=0.20, gross=300):
    R0, T0 = bindepose(form)
    Rn, Tn = gebeugt(form, R0, T0, {c: f(vz * g)})
    A0 = eltern_anfang(form, T0, p, c)
    J = Tn[c]
    u1 = (J - A0) / np.linalg.norm(J - A0)
    # Bisektor aussen: bei g = 0 Bewegung bei 10 Grad
    Rk, Tk = gebeugt(form, R0, T0, {c: f(vz * max(g, 10))})
    e = Tk[c] + (Tk[c] - T0[p]) * 0
    Ch = [x for x in teile if x not in (p,)]
    Ek = np.mean(np.vstack([np.array(form['meshes'][c]['ecken'], float) @ Rk[c].T + Tk[c]]), axis=0)
    u2 = (Ek - Tk[c]) / np.linalg.norm(Ek - Tk[c])
    o = u1 - u2
    o = o / np.linalg.norm(o)
    nn = n * seite
    if sicht == 'achse':
        A = ansicht(u1, nn)
    else:
        vor = -(o * 0.82 + nn * 0.57)
        vor = vor / np.linalg.norm(vor)
        unten = np.array([0, 1.0, 0]) - np.array([0, 1.0, 0]) @ vor * vor
        if np.linalg.norm(unten) < 0.1:
            unten = u1 - (u1 @ vor) * vor
        unten = unten / np.linalg.norm(unten)
        A = np.stack([np.cross(unten, vor), unten, vor])
    return tafel(filtern(mo, set(teile)), tex, Rn, Tn, A, J, s, gross)


def blatt(ziel, kopf, gelenk, winkel, sichten, s=0.20):
    name, p, c, e, ach, vz, seite, teile_fn = gelenk
    f, n = ACHSE[ach]
    t = []
    for k in ROHBAU:
        form = lade(k)
        mo, tex = modell(k)
        tb = teile_fn(form)
        for sicht in sichten:
            for g in winkel:
                t.append(('%s %s %d Grad %s' % (k, name, g, sicht), knick(mo, tex, form, p, c, tb, f, n, vz, g, seite, sicht, s=s)))
    FB.montage(t, len(winkel), ziel, kopf)


if __name__ == '__main__':
    arm_l = lambda form: [I['oberarm_l'], I['unterarm_l'], I['hand_l']]
    arm_r = lambda form: [I['oberarm_r'], I['unterarm_r'], I['hand_r']]
    bein_l = lambda form: [I['oberschenkel_l'], I['unterschenkel_l'], I['fuss_l']]
    bein_r = lambda form: [I['oberschenkel_r'], I['unterschenkel_r'], I['fuss_r']]
    rumpf = lambda form: [I['huefte'], I['wirbel'], I['brust']]
    schw = lambda form: [I['huefte']] + [I['schwanz%d' % i] for i in range(1, 7)]
    W = (0, 60, 90, 120, 150)
    blatt(AUS + '/biege_ellbogen.png', 'Ellbogen links, Ton, Achse und aussen', ('ellbogen_l', I['oberarm_l'], I['unterarm_l'], I['hand_l'], 'z', -1, -1, arm_l), W, ('achse', 'aussen'))
    blatt(AUS + '/biege_knie.png', 'Knie links, Ton, Achse und aussen', ('knie_l', I['oberschenkel_l'], I['unterschenkel_l'], I['fuss_l'], 'z', 1, -1, bein_l), W, ('achse', 'aussen'))
    blatt(AUS + '/biege_wirbel_z.png', 'Wirbel gegen Huefte, Nick, Ton', ('wirbel_z', I['huefte'], I['wirbel'], I['brust'], 'z', 1, -1, rumpf), (-45, -30, 0, 30, 45), ('achse', 'aussen'), s=0.13)
    blatt(AUS + '/biege_brust_z.png', 'Brust gegen Wirbel, Nick, Ton', ('brust_z', I['wirbel'], I['brust'], I['hals'], 'z', 1, -1, rumpf), (-45, -30, 0, 30, 45), ('achse', 'aussen'), s=0.13)
    blatt(AUS + '/biege_brust_y.png', 'Brust gegen Wirbel, seitlich, Ton', ('brust_y', I['wirbel'], I['brust'], I['hals'], 'y', 1, 1, rumpf), (-35, -20, 0, 20, 35), ('achse', 'aussen'), s=0.13)
    blatt(AUS + '/biege_schwanz2_z.png', 'Schwanz 2 gegen 1, Nick, Ton', ('schwanz2_z', I['schwanz1'], I['schwanz2'], I['schwanz3'], 'z', 1, -1, schw), (-45, -30, 0, 30, 45), ('achse', 'aussen'), s=0.18)
    blatt(AUS + '/biege_schwanz4_y.png', 'Schwanz 4 gegen 3, seitlich, Ton', ('schwanz4_y', I['schwanz3'], I['schwanz4'], I['schwanz5'], 'y', 1, 1, schw), (-45, -30, 0, 30, 45), ('achse', 'aussen'), s=0.18)
    blatt(AUS + '/biege_wurzel_z.png', 'Schwanzwurzel gegen Huefte, Nick, Ton', ('wurzel_z', I['huefte'], I['schwanz1'], I['schwanz2'], 'z', 1, -1, schw), (-45, -30, 0, 30, 45), ('achse', 'aussen'), s=0.13)
