#!/usr/bin/env python3
"""Einzelnachweise: L2-Maske, Lochring, Roentgen B, Dossieraussagen."""
import json, math, os, sys
import numpy as np
import cv2
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import geo as G
sys.path.insert(0, G.TOOLS)
import form_pruefung as P
import form_video as FV
import geo2 as G2
import geo3 as G3
import geo4 as G4
ZW = os.path.join(G.FORM, 'bewertung_mass_zw')


def l2():
    z = cv2.imread(os.path.join(G.FORM, 'ziel_bild1.png'), 0)
    zone = P.poly_maske(P.zone_b1('L2'))
    kern = zone & (z == 0)
    n, lab, st, _ = cv2.connectedComponentsWithStats(kern.astype(np.uint8), connectivity=8)
    print('L2 Kern: Pixel', int(kern.sum()), 'Komponenten', [tuple(int(v) for v in st[i]) for i in range(1, n)])
    for k in 'abc':
        m = np.load(os.path.join(ZW, 'maske_%s_1.npy' % k))
        t = np.argwhere(m & kern)
        print(' ', k, 'Treffer (y,x):', t.tolist())
    print(' Maske um (314,350):')
    print(z[345:356, 308:322])
    o = P.original(1)
    Y = o.astype(np.float32) @ P.YK
    print(' Video Y um (314,350):')
    print(np.round(Y[345:356, 308:322]).astype(int))


def zoom(n, box, s, ziel, modelle='abc'):
    x0, y0, x1, y1 = box
    o = P.original(n)
    zm = cv2.imread(os.path.join(G.FORM, 'ziel_bild%d.png' % n), 0)
    Z = []
    def gr(a, inter=cv2.INTER_NEAREST):
        return cv2.resize(a[y0:y1, x0:x1], None, fx=s, fy=s, interpolation=inter)
    Z.append(gr(o, cv2.INTER_CUBIC))
    h = np.zeros(o.shape, np.uint8)
    h[zm == 255] = (240, 240, 240)
    h[zm == 128] = (110, 110, 110)
    Z.append(gr(h))
    for k in modelle:
        K = G3.kand(k)
        sa, alb, m, lk = K.rendern(n)
        a = gr(sa)
        q = cv2.resize(m.astype(np.uint8), None, fx=s, fy=s, interpolation=cv2.INTER_NEAREST)[y0 * s:y1 * s, x0 * s:x1 * s].astype(bool)
        r = q & ~P.enge(q, 1)
        u = gr(o, cv2.INTER_CUBIC)
        u[r] = (255, 245, 0)
        Z.append(np.vstack([a, u]))
    k0 = np.vstack([Z[0], Z[1]])
    b = np.hstack([k0] + Z[2:])
    cv2.imwrite(os.path.join(ZW, ziel), cv2.cvtColor(b, cv2.COLOR_RGB2BGR))


def roentgen(k, gruppe, ziel):
    """Seitenansichten: Rumpfumriss (Fuellung grau), Gruppe rot, drueber nur der Teil der Gruppe, der im Rumpf steckt, gelb. Clip 0 und 1."""
    f, _ = G.lade(k)
    N = G4.Netz(f)
    Z = []
    gr = G2.GRUPPEN[k]
    for ci in (0, 1):
        R, T = G4.pose_frei(f, ci)
        Vt, Ft, _ = N.alles(R, T, gr['TORSO'])
        Vg, Fg, _ = N.alles(R, T, gr[gruppe])
        p, idx = G2.flaechen_proben(Vg, Fg, 1 / 900.0)
        w = np.abs(G2.windung(p, Vt, Ft)) > .5
        for az in (0, 90):            # Blick entlang +z (Seite), Blick entlang x (Stirn)
            a = np.full((420, 460, 3), 40, np.uint8)
            if az == 0:
                pr = lambda q: np.stack([q[:, 0] * .085 + 230, q[:, 1] * .085 + 300], 1)
            else:
                pr = lambda q: np.stack([q[:, 2] * .085 + 230, q[:, 1] * .085 + 300], 1)
            for tri in pr(Vt)[Ft]:
                cv2.fillConvexPoly(a, np.round(tri * 16).astype(np.int32), (135, 135, 135), lineType=cv2.LINE_8, shift=4)
            for q, inn in zip(pr(p), w):
                cv2.circle(a, (int(q[0]), int(q[1])), 1, (255, 225, 0) if inn else (225, 40, 40), -1)
            cv2.line(a, (0, 300), (459, 300), (90, 200, 90), 1)
            cv2.putText(a, 'Clip %d, %s, %d%% im Rumpf' % (ci, 'Seite' if az == 0 else 'Stirn', round(100 * w.mean())), (8, 20), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1, cv2.LINE_AA)
            Z.append(a)
    cv2.imwrite(os.path.join(ZW, ziel), cv2.cvtColor(np.vstack([np.hstack(Z[:2]), np.hstack(Z[2:])]), cv2.COLOR_RGB2BGR))


def euler_zerl(Q):
    """Rotation Q in Gier (um y), Nick (um z, x-vorn), Roll (um x), kleine Winkel genuegen als Anhalt."""
    fw = Q @ np.array([1, 0, 0.0])
    gier = math.degrees(math.atan2(-fw[2], fw[0]))
    nick = math.degrees(math.asin(np.clip(fw[1], -1, 1)))
    return round(gier, 1), round(nick, 1)


def aussagen():
    f, _ = G.lade('a')
    L0, R0, T0 = G.lokale(f, 0)
    L1, R1, T1 = G.lokale(f, 1)
    nm = G.NAMEN['a']
    r, h, kk, b = nm.index('rumpf'), nm.index('hals'), nm.index('kopf'), 0
    print('A kopf gegen rumpf Clip 0>1: %.1f Grad, Blickrichtung-Gier/Nick im Rumpfraum:' % G.weg_winkel(R0[r].T @ R0[kk], R1[r].T @ R1[kk])[0], euler_zerl((R1[r].T @ R1[kk]) @ (R0[r].T @ R0[kk]).T))
    print('A becken Clip 1 gegen Bind (Aktorraum): Gesamt %.1f, Gier/Nick' % G.weg_winkel(R0[b], R1[b])[0], euler_zerl(R1[b]))
    print('A rumpf  Clip 1 gegen Bind: Gesamt %.1f, Gier/Nick' % G.weg_winkel(R0[r], R1[r])[0], euler_zerl(R1[r]))
    for k in 'abc':
        ff, _ = G.lade(k)
        c = [x['bilder'][0]['kf'] for x in ff['clips']]
        pl = ff['pool']
        d02 = max(abs(a - b) for u, v in zip(pl[c[0]]['winkel'], pl[c[2]]['winkel']) for a, b in zip(u, v))
        print(k, 'Keyframes je Clip', c, 'groesste Winkeldifferenz Clip 0 zu Clip 2 (1/4096)', d02)
    # Texeldichte
    for k in 'abc':
        ff, _ = G.lade(k)
        w = []
        for b in range(len(ff['knochen'])):
            m = ff['meshes'][b]
            V = np.array(m['ecken'], float)
            for q in m['vierecke']:
                a, b_, c, d = q['e']
                t = q.get('tex')
                if t is None:
                    continue
                f3 = 0.5 * np.linalg.norm(np.cross(V[b_] - V[a], V[c] - V[a]))
                ta = np.array(t, float)
                fu = 0.5 * abs((ta[1][0] - ta[0][0]) * (ta[2][1] - ta[0][1]) - (ta[2][0] - ta[0][0]) * (ta[1][1] - ta[0][1]))
                if fu > 0.5:
                    w.append(math.sqrt(f3 / fu))
        print(k, 'Einheiten je Texel Median %.0f (Quads %d)' % (np.median(w), len(w)) if w else 'keine tex')


if __name__ == '__main__':
    l2()
    aussagen()
    zoom(1, (285, 330, 335, 365), 8, 'zoom_l2.png')
    zoom(1, (205, 345, 300, 440), 5, 'zoom_nl.png')
    zoom(2, (195, 200, 300, 280), 5, 'zoom_haken.png')
    roentgen('b', 'FL', 'roentgen_b_fl.png')
    roentgen('c', 'FR', 'roentgen_c_fr.png')
    roentgen('a', 'NR', 'roentgen_a_nr.png')
