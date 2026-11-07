#!/usr/bin/env python3
"""Rundumblatt Pose 1 und Pose 2 fuer alle, gemeinsam gesehene Flaeche, Grau-Ausnutzung, Frau, Bodenpunkte.
Liest ids_*.npy aus geo2 (Z-Puffer je Kandidat und Bild)."""
import json, math, os, sys
import numpy as np
import cv2
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import geo as G
sys.path.insert(0, G.TOOLS)
import form_pruefung as P
import form_kamera as FK
import form_buehne as FB
import form_video as FV
import geo2 as G2
ZW = os.path.join(G.FORM, 'bewertung_mass_zw')


def kand(k):
    p = os.path.join(ZW, 'b_eingabe.json') if k == 'b' else os.path.join(G.FORM, 'kandidat_%s/lage.json' % k)
    return P.Kandidat(p)


def rundum_posen(k):
    K = kand(k)
    for ci in (0, 1):
        P.rundum(K, os.path.join(ZW, 'rund_%s_clip%d.png' % (k, ci)), os.path.join(ZW, 'rund_%s_clip%d_hell.png' % (k, ci)), ci, 0)


def flaechen(f, ci):
    V, Fa, Bi = G2.dreiecke_welt(f, ci, range(len(f['knochen'])))
    a, b, c = V[Fa[:, 0]], V[Fa[:, 1]], V[Fa[:, 2]]
    return 0.5 * np.linalg.norm(np.cross(b - a, c - a), axis=1), Bi


def gemeinsam(k):
    f, _ = G.lade(k)
    i1 = np.load(os.path.join(ZW, 'ids_%s_1.npy' % k))
    i2 = np.load(os.path.join(ZW, 'ids_%s_2.npy' % k))
    fl, Bi = flaechen(f, 0)
    s1 = np.zeros(len(fl), bool); s2 = np.zeros(len(fl), bool)
    # Fuer eine Flaeche zaehlt sichtbar ab 4 Pixeln (sonst Rand-Treffer)
    z1 = np.bincount(i1[i1 >= 0], minlength=len(fl)); z2 = np.bincount(i2[i2 >= 0], minlength=len(fl))
    s1, s2 = z1 >= 4, z2 >= 4
    ges = fl.sum()
    r = dict(nur1=fl[s1 & ~s2].sum() / ges, nur2=fl[s2 & ~s1].sum() / ges, beide=fl[s1 & s2].sum() / ges, keine=fl[~s1 & ~s2].sum() / ges)
    # Bezogen auf das, was ueberhaupt gesehen wird
    r['beide_von_gesehen'] = fl[s1 & s2].sum() / fl[s1 | s2].sum()
    # je Pixel in Bild 2: Anteil der Pixel, deren Flaeche auch in Bild 1 sichtbar ist
    q2 = i2[i2 >= 0]
    r['px2_auch_in_1'] = float(s1[q2].mean())
    q1 = i1[i1 >= 0]
    r['px1_auch_in_2'] = float(s2[q1].mean())
    return {a: round(float(b), 3) for a, b in r.items()}


def grau(k):
    """Render auf Grau: wie hell ist das Video dort? Dunkle Masse darf dort stehen, heller Boden und Feuer widersprechen, ohne dass die Maske es zaehlt."""
    K = kand(k)
    aus = {}
    for n in (1, 2):
        z = cv2.imread(os.path.join(G.FORM, 'ziel_bild%d.png' % n), 0)
        o = P.original(n)
        s, alb, m, lk = K.rendern(n)
        f = FV.video_filter(s, bild=n, entzerren=False, rauschen=False)
        Y = o.astype(np.float32) @ P.YK
        Yr = f.astype(np.float32) @ P.YK
        gr = (z == 128) & m
        d = dict(render=int(m.sum()), auf_grau=int(gr.sum()))
        for t in (60, 90, 120, 150):
            d['grau_video_ueber_%d' % t] = int((gr & (Y > t)).sum())
        # Gefilterter Render um mehr als 40 dunkler als das Video: da zeichnet die Figur dunkel, wo das Video hell ist
        d['grau_render_dunkler_40'] = int((gr & (Yr < Y - 40)).sum())
        d['grau_render_dunkler_80'] = int((gr & (Yr < Y - 80)).sum())
        # Umgekehrt: ausserhalb Render, Ziel grau, Video dunkel: dunkle Masse ohne Figur
        aussen = (z == 128) & ~m & (Y < 40)
        d['grau_ohne_render_video_dunkel'] = int(aussen.sum())
        # Filter gegen Video im Weiss + Grau um die Figur: MAE und NCC ueber Figur und Saum (Maske: Render oder Weiss, 12 px geweitet)
        w = P.weite((z == 255) | m, 12)
        a, b = Yr[w], Y[w]
        d['ncc_weit'] = round(float(np.corrcoef(a, b)[0, 1]), 3)
        d['mae_weit'] = round(float(np.abs(a - b).mean()), 1)
        aus[n] = d
        np.save(os.path.join(ZW, 'maske_%s_%d.npy' % (k, n)), m)
    return aus


def welt_punkte(k, n, ci, bs):
    f, _ = G.lade(k)
    Y, lage = G2.welt_kamera(k, n)
    V, Fa, Bi = G2.dreiecke_welt(f, ci, bs)
    return V @ Y.T + lage, f


def frau(k):
    """Frau steht bei Spawn (600,0,-25300), Hoehe 2785. Abstand zur Figur in Bild 2 und ihr Schnitt im Bild."""
    Wv, f = welt_punkte(k, 2, 1, range(21 if k != 'a' else 23))
    d = np.hypot(Wv[:, 0] - 600, Wv[:, 2] + 25300)
    kam = FK.lade_kamera(2)
    t = (Wv - np.asarray(kam['auge'], float)) @ kam['R'][2]
    tf = (np.array([600, 0, -25300], float) - np.asarray(kam['auge'], float)) @ kam['R'][2]
    return dict(naechster_abstand=round(float(d.min())), tiefe_figur=[round(float(t.min())), round(float(np.median(t))), round(float(t.max()))], tiefe_frau=round(float(tf)))


def bodenpunkte(k):
    """Fuesse im Bild: wohin fallen sie, was zeigt das Video dort?"""
    aus = {}
    f, _ = G.lade(k)
    for n, ci in ((1, 0), (2, 1)):
        kam = FK.lade_kamera(n)
        Y_, lage = G2.welt_kamera(k, n)
        o = P.original(n)
        Yb = o.astype(np.float32) @ P.YK
        for nm, bs in G.BEINE[k].items():
            V, Fa, Bi = G2.dreiecke_welt(f, ci, bs)
            i = V[:, 1].argmax()
            w = V[i] @ Y_.T + lage
            w0 = w.copy()
            p = FK.projiziere(kam, w0)
            x, y = int(round(p[0])), int(round(p[1]))
            v = float(Yb[np.clip(y, 0, 479), np.clip(x, 0, 639)]) if 0 <= x < 640 and 0 <= y < 480 else -1
            aus['Bild%d %s' % (n, nm)] = dict(h=round(float(-V[i, 1])), bild=[x, y], Y_video=round(v), welt=[round(float(w[0])), round(float(w[2]))])
    return aus


if __name__ == '__main__':
    was = sys.argv[1] if len(sys.argv) > 1 else 'alles'
    for k in 'abc':
        if was in ('alles', 'rund'):
            rundum_posen(k)
        if was in ('alles', 'zahl'):
            print('\n=====', k.upper())
            print('Gesehene Flaeche:', gemeinsam(k))
            g = grau(k)
            print('Grau Bild 1:', g[1])
            print('Grau Bild 2:', g[2])
            print('Frau:', frau(k))
            for a, b in bodenpunkte(k).items():
                print(' ', a, b)
