#!/usr/bin/env python3
"""Pruefer MESSUNG, Teil 4: Durchdringung aller Gruppenpaare, Querschnitte je Glied, Links gegen Rechts, Blick-Tiefe gegen Breite, Gesamtdrehung der Gelenke."""
import json, sys
import numpy as np
from m1_skelett import F, W, G, NAME, lade, fk, welt, FK, gier_y
from m3_innen import tris_bone, pose_tris, proben, windung
from m2_gelenke import swing_twist, eckpunkte

GR = dict(rumpf=[0, 7, 10], kopf=[21, 22, 23], schwanz=[1, 2, 3, 4, 5, 6], arm_l=[11, 12, 13, 14, 15, 28, 30], arm_r=[16, 17, 18, 19, 20, 29, 31], bein_l=[8, 24, 25], bein_r=[9, 26, 27])
REIHE = list(GR)


def gruppe(M, l):
    vv, tt, of = [], [], 0
    for i in l:
        v, t = M[i]
        vv.append(v)
        tt.append(t + of)
        of += len(v)
    return np.vstack(vv), np.vstack(tt)


def schnitt(v, t, achse, wert):
    """Schnittpunkte aller Kanten mit Ebene achse=wert. Rueckgabe Nx3."""
    k = np.vstack([t[:, [0, 1]], t[:, [1, 2]], t[:, [2, 0]]])
    a, b = v[k[:, 0]], v[k[:, 1]]
    d = (a[:, achse] - wert) * (b[:, achse] - wert)
    s = d < 0
    a, b = a[s], b[s]
    u = (wert - a[:, achse]) / (b[:, achse] - a[:, achse])
    return a + u[:, None] * (b - a)


GLIED = [('unterarm', 12, 17, 1), ('oberarm', 11, 16, 1), ('oberschenkel', 8, 9, 1), ('unterschenkel', 24, 26, 1), ('fuss', 25, 27, 0), ('hand', 13, 18, 0)]


def main():
    rng = np.random.default_rng(2)
    AUS = {}
    kn = {k['index']: k for k in G['knochen']}
    for x in 'abc':
        j, f = lade(x)
        print('\n=========== Rohbau', x.upper())
        # ---- Querschnitte
        print('Querschnitt je Glied in Knochenraum: Mitte (s=.5). Tiefe = Achse X oder Y quer, Breite = Z. Gerippe 2a/2b in Klammern. L und R, Verhaeltnis L/R. Volumen.')
        for nm, l, r, ax in GLIED:
            z = []
            for b in (l, r):
                v, t = tris_bone(f, b)
                L = v[:, ax].max()
                s = schnitt(v, t, ax, .5 * (L + v[:, ax].min()))
                quer = [c for c in range(3) if c != ax]
                ea, eb = (s[:, quer[0]].max() - s[:, quer[0]].min(), s[:, quer[1]].max() - s[:, quer[1]].min()) if len(s) else (0, 0)
                a, bb, c = v[t[:, 0]], v[t[:, 1]], v[t[:, 2]]
                vol = abs((a * np.cross(bb, c)).sum() / 6)
                z.append((ea, eb, L, vol))
            gi = kn[l]['dicke'][1]
            print('  %-14s L %4.0f x %4.0f  R %4.0f x %4.0f  (Gerippe %4.0f x %4.0f)  Verhaeltnis quer %.2f/%.2f  L/R Tiefe %.2f Breite %.2f Volumen %.2f  Laenge L/R %4.0f/%4.0f' % (
                nm, z[0][0], z[0][1], z[1][0], z[1][1], 2 * gi['a'], 2 * gi['b'], z[0][0] / max(z[0][1], 1), z[1][0] / max(z[1][1], 1), z[0][0] / max(z[1][0], 1), z[0][1] / max(z[1][1], 1), z[0][3] / max(z[1][3], 1), z[0][2], z[1][2]))
        # ---- Volumen gesamt, Finger
        print('Fingerlaengen (groesster Abstand Ecke zum Gelenk):', ' '.join('%s %.0f' % (NAME[i].replace('klaue_', ''), np.linalg.norm(eckpunkte(f, i), axis=1).max()) for i in (14, 15, 28, 30, 19, 20, 29, 31)))
        # ---- Tiefe gegen Breite im Blick
        print('Sichtbreite gegen Tiefe je Glied im Blick (Einheiten, quer zur Gliedachse, in Bildmitte Glied). Hoher Unterschied = Scheibe zur Kamera:')
        for n, c in ((1, 0), (2, 1)):
            R, T, k = fk(f, c)
            Rw, Tw = welt(j, n, R, T)
            km = FK.lade_kamera(n)
            rechts, unten, vor = km['R']
            z = []
            for nm, b in (('oberarm_r', 16), ('unterarm_r', 17), ('unterarm_l', 12), ('oberschenkel_l', 8), ('oberschenkel_r', 9), ('unterschenkel_l', 24), ('unterschenkel_r', 26)):
                v, t = tris_bone(f, b)
                L = v[:, 1].max()
                s = schnitt(v, t, 1, .5 * L)
                if not len(s):
                    continue
                # Richtung des Glieds in Welt
                ach = Rw[b] @ np.array([0, 1.0, 0])
                # Quer zur Achse: Bild-Quer = rechts/unten projiziert senkrecht zu ach und vor
                q1 = np.cross(ach, vor)
                q1 /= np.linalg.norm(q1)
                q2 = np.cross(ach, q1)
                pw = s @ Rw[b].T
                w1 = (pw @ q1).max() - (pw @ q1).min()
                w2 = (pw @ q2).max() - (pw @ q2).min()
                z.append('%s %4.0f/%4.0f=%.2f' % (nm, w1, w2, w1 / max(w2, 1)))
            print('  Bild %d: %s' % (n, '  '.join(z)))
        # ---- Gesamtdrehung
        print('Gesamtdrehung lokal in Grad (Achsenwinkel, nimmt die Euler-Doppeldeutigkeit weg):')
        for c, nm in ((0, 'Pose 1'), (1, 'Pose 2'), (2, 'Ruhe')):
            R, T, k = fk(f, c)
            z = []
            for i in (8, 9, 24, 26, 25, 27, 11, 16, 12, 17, 13, 18, 21, 22):
                p = f['knochen'][i]['eltern']
                r = R[p].T @ R[i]
                z.append('%s %.0f' % (NAME[i].replace('oberschenkel', 'OS').replace('unterschenkel', 'US').replace('oberarm', 'OA').replace('unterarm', 'UA'), np.degrees(np.arccos(np.clip((np.trace(r) - 1) / 2, -1, 1)))))
            print('  %s: %s' % (nm, ', '.join(z)))
        print('Handgelenk/Knoechel: Drehung um Gliedachse (Unterarm/Unterschenkel +Y):')
        for c, nm in ((0, 'Pose 1'), (1, 'Pose 2')):
            R, T, k = fk(f, c)
            z = []
            for i in (13, 18, 25, 27):
                p = f['knochen'][i]['eltern']
                sw, tw, _ = swing_twist(R[p].T @ R[i], np.array([0, 1.0, 0]))
                z.append('%s Schwenk %.0f Dreh %+.0f' % (NAME[i], sw, tw))
            print('  %s: %s' % (nm, '; '.join(z)))
        # ---- Paare
        print('Durchdringung: Anteil Oberflaeche der Zeilengruppe in Spaltengruppe (nur >= 0.03)')
        for c, nm in ((0, 'Pose 1'), (1, 'Pose 2'), (2, 'Ruhe')):
            R, T, k = fk(f, c)
            M = {i: pose_tris(f, R, T, i) for i in range(32)}
            z = []
            for g in REIHE:
                v, t = gruppe(M, GR[g])
                q = proben(v, t, 1500, rng)
                for h in REIHE:
                    if h == g or (g in ('kopf',) and h == 'rumpf') or (g == 'rumpf' and h == 'kopf') or (g == 'schwanz' and h == 'rumpf') or (g == 'rumpf' and h == 'schwanz'):
                        continue
                    if (g, h) in (('arm_l', 'rumpf'), ('arm_r', 'rumpf'), ('bein_l', 'rumpf'), ('bein_r', 'rumpf')):
                        pass
                    inn = np.zeros(len(q), bool)
                    for b in GR[h]:
                        vb, tb_ = M[b]
                        if len(tb_):
                            inn |= windung(q, vb, tb_) > .5
                    if inn.mean() >= .03:
                        z.append('%s in %s %.2f' % (g, h, inn.mean()))
            print('  %s: %s' % (nm, '; '.join(z)))
        # ---- Lage linker Arm Pose 1 im Bild
        R, T, k = fk(f, 0)
        Rw, Tw = welt(j, 1, R, T)
        km = FK.lade_kamera(1)
        pj = np.atleast_2d(FK.projiziere(km, np.array(Tw)))
        print('Linker Arm Pose 1 im Bild: Schulter (%.0f,%.0f) Ellbogen (%.0f,%.0f) Handgelenk (%.0f,%.0f); Hoehen %.0f %.0f %.0f' % (*pj[11], *pj[12], *pj[13], -T[11][1], -T[12][1], -T[13][1]))
        print('Kopf Pose 1 im Bild: Hals (%.0f,%.0f) Kopf (%.0f,%.0f) Kiefer (%.0f,%.0f)' % (*pj[21], *pj[22], *pj[23]))


if __name__ == '__main__':
    main()
