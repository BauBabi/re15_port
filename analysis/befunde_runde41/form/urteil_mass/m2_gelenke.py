#!/usr/bin/env python3
"""Pruefer MESSUNG, Teil 2: Lage/Massstab, Gelenkgrenzen, Beuge/Drehung, Boden, Chiralitaet, Blick, Klauenspitzen.
Rechnet aus FORM.EMD und form.json, nimmt keine Werte aus den Rohbau-Skripten."""
import json, sys
import numpy as np
from m1_skelett import F, G, NAME, lade, fk, welt, gier_y, FK, FE

GR = G['winkelgrenzen_grad']
KL = {k['index']: k['klasse'] for k in G['knochen']}
KL[30], KL[31] = 'klaue', 'klaue'


def norm180(a):
    return (a + 180.0) % 360.0 - 180.0


def eckpunkte(f, i):
    m = f['meshes'][i]
    return np.array(m['ecken'], float) if m['ecken'] else np.zeros((0, 3))


def spitze(f, i, R, T):
    """Aeusserster Punkt eines Fingers: Ecke mit groesstem Abstand vom Gelenk, im Aktorraum."""
    e = eckpunkte(f, i)
    if not len(e):
        return None
    p = e @ R[i].T + T[i]
    d = np.linalg.norm(e, axis=1)
    return p[int(np.argmax(d))], float(d.max())


def tief(f, liste, R, T):
    """Hoehe ueber Boden je Knochengruppe: -y groesster Wert entspricht tiefstem Punkt."""
    y = []
    for i in liste:
        e = eckpunkte(f, i)
        if len(e):
            y.append((e @ R[i].T + T[i])[:, 1].max())
    return -max(y)


def swing_twist(Rlok, achse):
    """Rlok = Swing * Twist um achse (Einheitsvektor im Kindraum). Rueckgabe: Swing-Winkel, Twist-Winkel in Grad, Swing-Achse."""
    q = _quat(Rlok)
    w, v = q[0], q[1:]
    p = v @ achse
    t = np.r_[w, p * achse]
    n = np.linalg.norm(t)
    if n < 1e-9:
        tw = 180.0
        t = np.array([1.0, 0, 0, 0])
    else:
        t = t / n
        tw = np.degrees(2 * np.arctan2(t[1:] @ achse, t[0]))
    # Swing = q * t^-1
    tc = np.r_[t[0], -t[1:]]
    s = _qmul(q, tc)
    sw = np.degrees(2 * np.arctan2(np.linalg.norm(s[1:]), abs(s[0])))
    ax = s[1:] / (np.linalg.norm(s[1:]) or 1) * np.sign(s[0] or 1)
    return sw, norm180(tw), ax


def _quat(R):
    t = np.trace(R)
    if t > 0:
        s = np.sqrt(t + 1) * 2
        return np.array([.25 * s, (R[2, 1] - R[1, 2]) / s, (R[0, 2] - R[2, 0]) / s, (R[1, 0] - R[0, 1]) / s])
    i = int(np.argmax(np.diag(R)))
    j, k = (i + 1) % 3, (i + 2) % 3
    s = np.sqrt(1 + R[i, i] - R[j, j] - R[k, k]) * 2
    q = np.zeros(4)
    q[0] = (R[k, j] - R[j, k]) / s
    q[i + 1] = .25 * s
    q[j + 1] = (R[j, i] + R[i, j]) / s
    q[k + 1] = (R[k, i] + R[i, k]) / s
    return q


def _qmul(a, b):
    w1, v1, w2, v2 = a[0], a[1:], b[0], b[1:]
    return np.r_[w1 * w2 - v1 @ v2, w1 * v2 + w2 * v1 + np.cross(v1, v2)]


def main():
    AUS = {}
    for x in 'abc':
        j, f = lade(x)
        print('\n===========================  Rohbau', x.upper())
        P = {c: fk(f, c) for c in range(3)}
        ERG = dict(lage={}, winkel={}, boden={}, spitzen={})
        # ---------- Lage und Massstab
        for n, c in ((1, 0), (2, 1)):
            R, T, k = P[c]
            Rw, Tw = welt(j, n, R, T)
            km = FK.lade_kamera(n)
            z = FK.tiefe(km, np.array(Tw))
            print('Bild %d Lage %s Gier %d (%.1f Grad): Tiefe Huefte %.0f, Brust %.0f, Kopf %.0f, Wurzelhoehe %.0f' % (
                n, j['bild%d' % n]['lage'], j['bild%d' % n]['gier'], j['bild%d' % n]['gier'] * 360 / 4096, z[0], z[10], z[22], -k['wurzel'][1]))
            ERG['lage'][n] = dict(z_huefte=float(z[0]))
        # ---------- Chiralitaet und Blick
        for n, c in ((1, 0), (2, 1)):
            R, T, k = P[c]
            Rw, Tw = welt(j, n, R, T)
            km = FK.lade_kamera(n)
            pj = FK.projiziere(km, np.array(Tw))
            # Blickrichtung Kamera (vorn) in Welt
            vor = km['R'][2]
            rumpf_vor = Rw[10][:, 0]            # +X der Brust in Welt
            kopf_vor = Rw[22][:, 0]
            zum_auge = np.array(km['auge'], float) - Tw[22]
            zum_auge /= np.linalg.norm(zum_auge)
            winkel_kopf = np.degrees(np.arccos(np.clip(kopf_vor @ zum_auge, -1, 1)))
            hals_brust = np.degrees(np.arccos(np.clip(Rw[22][:, 0] @ Rw[10][:, 0], -1, 1)))
            winkel_rumpf = np.degrees(np.arccos(np.clip(rumpf_vor @ vor, -1, 1)))
            print('Bild %d: Schulter L (%.0f,%.0f) R (%.0f,%.0f), Huefte L (%.0f,%.0f) R (%.0f,%.0f); Brust +X gegen Blick %.0f Grad (0 = von der Kamera weg), Kopf +X zum Auge %.0f Grad, Kopf gegen Brust %.0f Grad' % (
                n, *pj[11], *pj[16], *pj[8], *pj[9], winkel_rumpf, winkel_kopf, hals_brust))
            ok = (pj[11][0] < pj[16][0]) if n == 1 else (pj[11][0] > pj[16][0])
            ok2 = (pj[8][0] < pj[9][0]) if n == 1 else (pj[8][0] > pj[9][0])
            print('   Links/Rechts Schulter %s, Huefte %s' % ('richtig' if ok else 'VERTAUSCHT', 'richtig' if ok2 else 'VERTAUSCHT'))
            # Chiralitaet: Figur-Links = +Z, Vor = +X, Unten = +Y. det(X, Y, Z) je Knochen = +1 und Mesh-Windung
        # ---------- Gelenkgrenzen
        print('Gelenkgrenzen (gerippe.json): Ueberschreitung je Pose. ax ay az Grad.')
        for c, nm in ((0, 'Pose 1'), (1, 'Pose 2'), (2, 'Ruhe')):
            R, T, k = P[c]
            bad = []
            for i in range(32):
                g = GR.get(KL[i])
                if g is None:
                    g = GR.get('schwanz') if KL[i].startswith('schwanz') else None
                if g is None:
                    continue
                w = [norm180(v * 360.0 / 4096) for v in k['winkel'][i]]
                # Rechtes Glied gleiche Grenzen fuer x, z; Seite spiegelt ay/ax? Grenzen sind fuer beide angegeben
                seite = NAME[i].endswith('_r') or '_r' in NAME[i]
                for a in range(3):
                    lo, hi = g[a]
                    if seite and a in (0, 1) and KL[i] in ('oberarm', 'oberschenkel', 'hand', 'fuss', 'unterarm', 'unterschenkel'):
                        lo, hi = -hi, -lo
                    if w[a] < lo - .5 or w[a] > hi + .5:
                        bad.append((NAME[i], 'xyz'[a], round(w[a], 1), g[a]))
            print('  %s: %d Ueberschreitungen' % (nm, len(bad)))
            for b in bad:
                print('     ', b)
            ERG['winkel'][nm] = bad
        # ---------- Ellbogen, Knie: Beugung, Hinge, Drehung
        print('Ellbogen/Knie/Handgelenk: lokale Rotation. Beuge = Swing um die Gliedachse +Y, Drehung = Twist um die eigene Achse.')
        for c, nm in ((0, 'Pose 1'), (1, 'Pose 2'), (2, 'Ruhe')):
            R, T, k = P[c]
            z = []
            for i, t in ((12, 'Ellbogen L'), (17, 'Ellbogen R'), (24, 'Knie L'), (26, 'Knie R')):
                p = f['knochen'][i]['eltern']
                Rlok = R[p].T @ R[i]
                sw, tw, ax = swing_twist(Rlok, np.array([0, 1.0, 0]))
                # Vorzeichen der Beugung: Drehung um Z
                z.append('%s Beuge %.0f (Achse %.2f %.2f %.2f) Dreh %+.0f' % (t, sw, ax[0], ax[1], ax[2], tw))
            print('  %s: %s' % (nm, '; '.join(z)))
        # ---------- Oberarm: Drehung, Abspreizung
        for c, nm in ((0, 'Pose 1'), (1, 'Pose 2')):
            R, T, k = P[c]
            z = []
            for i, t in ((11, 'Oberarm L'), (16, 'Oberarm R'), (8, 'Oberschenkel L'), (9, 'Oberschenkel R')):
                p = f['knochen'][i]['eltern']
                sw, tw, ax = swing_twist(R[p].T @ R[i], np.array([0, 1.0, 0]))
                z.append('%s Schwenk %.0f Dreh %+.0f' % (t, sw, tw))
            print('  %s: %s' % (nm, '; '.join(z)))
        # ---------- Boden
        print('Hoehe tiefster Punkt ueber Boden je Gruppe (Einheiten):')
        gr = dict(hand_l=[13, 14, 15, 28, 30], hand_r=[18, 19, 20, 29, 31], fuss_l=[25], fuss_r=[27], schwanz=[1, 2, 3, 4, 5, 6], kopf=[21, 22, 23], rumpf=[0, 7, 10])
        for c, nm in ((0, 'Pose 1'), (1, 'Pose 2'), (2, 'Ruhe')):
            R, T, k = P[c]
            print('  %s: %s' % (nm, ', '.join('%s %.0f' % (g, tief(f, l, R, T)) for g, l in gr.items())))
            ERG['boden'][nm] = {g: float(tief(f, l, R, T)) for g, l in gr.items()}
        # ---------- Klauenspitzen im Bild
        ziele2 = dict(r=[(145, 353), (197, 476), (222, 475), (274, 443)], l=[(461, 392), (468, 417), (439, 418), (410, 392)])
        ziel1_r = (425, 363)
        print('Klauenspitzen Bild 2 (Ecke weitest vom Gelenk), Hoehe ueber Boden, Soll-Spitze naechste:')
        R, T, k = P[1]
        Rw, Tw = welt(j, 2, R, T)
        km = FK.lade_kamera(2)
        Y = gier_y(j['bild2']['gier'])
        lage = np.array(j['bild2']['lage'], float)
        for s, liste in (('r', (19, 20, 29, 31)), ('l', (14, 15, 28, 30))):
            for i in liste:
                e = eckpunkte(f, i)
                d = np.linalg.norm(e, axis=1)
                q = e[int(np.argmax(d))]
                pa = R[i] @ q + T[i]
                w = Y @ pa + lage
                px = np.atleast_2d(FK.projiziere(km, w))[0]
                so = min(ziele2[s], key=lambda t: np.hypot(t[0] - px[0], t[1] - px[1]))
                print('   %s Laenge %4.0f  Spitze Bild (%.0f,%.0f) Hoehe %.0f  naechste Soll %s Abstand %.1f' % (NAME[i], d.max(), px[0], px[1], -pa[1], so, np.hypot(so[0] - px[0], so[1] - px[1])))
        R, T, k = P[0]
        km = FK.lade_kamera(1)
        Y = gier_y(j['bild1']['gier'])
        lage = np.array(j['bild1']['lage'], float)
        print('Klauenspitzen Bild 1 rechte Hand:')
        for i in (19, 20, 29, 31):
            e = eckpunkte(f, i)
            d = np.linalg.norm(e, axis=1)
            q = e[int(np.argmax(d))]
            pa = R[i] @ q + T[i]
            px = np.atleast_2d(FK.projiziere(km, Y @ pa + lage))[0]
            print('   %s Spitze Bild (%.0f,%.0f) Hoehe %.0f  Abstand zu (425,363) %.1f' % (NAME[i], px[0], px[1], -pa[1], np.hypot(px[0] - 425, px[1] - 363)))
        print('Spreizung Finger rechte Hand: Winkel Finger gegen Handachse je Pose')
        for c, nm in ((0, 'Pose 1'), (1, 'Pose 2'), (2, 'Ruhe')):
            R, T, k = P[c]
            z = []
            for i in (19, 20, 29, 31, 14, 15, 28, 30):
                z.append('%s %.0f' % (NAME[i].replace('klaue_', ''), np.degrees(np.arccos(np.clip(R[i][:, 0] @ R[f['knochen'][i]['eltern']][:, 0], -1, 1)))))
            print('  %s: %s' % (nm, ' '.join(z)))
        AUS[x] = ERG
    json.dump(AUS, open(F + '/urteil_mass/m2.json', 'w'), indent=1, default=float)


if __name__ == '__main__':
    main()
