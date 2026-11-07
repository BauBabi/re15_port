# -*- coding: utf-8 -*-
"""Rohbau B: Feinanpassung gegen die Nutzer-Teile. Die Form bleibt von Hand gesetzt, hier werden nur ganze Ringe skaliert und geschoben und Gelenkwinkel leicht nachgezogen.
Koordinatensuche, ein Parameter nach dem anderen. Links und rechts teilen sich die Masse. Ergebnis: ringfit.json, posen.json.
python anpassen.py [--teile rumpf,arm,...] [--posen] [--runden N]"""
import argparse, json, math, os, sys, time
import numpy as np
import cv2
import figur, schau
from schau import FK, MASKEN, LOGO, TEILE

HIER = schau.HIER
E = schau.E
RINGFIT = os.path.join(HIER, 'ringfit.json')
ZIEL = {n: cv2.imread(os.path.join(schau.FORM, 'ziel2_bild%d.png' % n), cv2.IMREAD_GRAYSCALE) for n in (1, 2)}


def gruppe_von(name):
    """Links und rechts teilen sich einen Namen; 'zeh', 'zahn', Finger bleiben aussen vor."""
    for e in ('_l', '_r'):
        if name.endswith(e):
            return name[:-2]
    return name


class Modell:
    def __init__(self):
        self.f = figur.bauen(mit_fit=False)
        self.fig = figur.fertig(self.f)
        self.kam = {n: FK.lade_kamera(n) for n in (1, 2)}
        self.t = self.f.teile
        self.basis = []
        for t in self.t:
            t.auto_pole()
            self.basis.append(dict(ringe=[r.copy() for r in t.ringe], pol_a=t.pol_a.copy(), pol_e=t.pol_e.copy()))
        self.netz = []
        from ringe import aussen_drehen
        for t in self.t:
            V, F, G = t.netz()
            self.netz.append(aussen_drehen(V, F, G, t))
        self.posen = {n: self.fig['posen']['bild%d' % n] for n in (1, 2)}
        self.par = {}             # (gruppe, ring) -> [sa, sb, da, db]
        self.cache = {}

    def anwenden_teil(self, ti):
        """Ringe des Teils aus Basis + Parametern."""
        t, b = self.t[ti], self.basis[ti]
        g = gruppe_von(t.name)
        pa = self.par
        for i, r in enumerate(b['ringe']):
            p = pa.get(g + '/%d' % i)
            if p is None:
                t.ringe[i] = r.copy()
                continue
            c = r.mean(0)
            q = r - c
            a = q @ t.A
            bb = q @ t.B
            e = q - np.outer(a, t.A) - np.outer(bb, t.B)
            t.ringe[i] = c + e + np.outer(a * p[0] + p[2], t.A) + np.outer(bb * p[1] + p[3], t.B)
        t.pol_a = b['pol_a'].copy()
        t.pol_e = b['pol_e'].copy()

    def eck(self, ti):
        return np.vstack(self.t[ti].ringe + [self.t[ti].pol_a[None], self.t[ti].pol_e[None]])

    def boden(self, n):
        p = self.posen[n]
        R, T = schau.fk(self.f.eltern, self.f.versatz, p['wurzel'], p['winkel'])
        tief = -1e9
        for ti, t in enumerate(self.t):
            V = self.eck(ti)
            tief = max(tief, float((V @ R[t.knochen].T + T[t.knochen])[:, 1].max()))
        return tief

    def kontakt(self, n):
        """Luecke zwischen Boden und tiefstem Punkt der Glieder, die stehen sollen. Summe ueber 40 Einheiten."""
        p = self.posen[n]
        R, T = schau.fk(self.f.eltern, self.f.versatz, p['wurzel'], p['winkel'])
        tief = {}
        for ti, t in enumerate(self.t):
            V = self.eck(ti)
            tief[t.knochen] = max(tief.get(t.knochen, -1e9), float((V @ R[t.knochen].T + T[t.knochen])[:, 1].max()))
        unten = max(tief.values())
        namen = {1: ('fuss_l', 'fuss_r', 'hand_r'), 2: ('fuss_l', 'hand_l', 'hand_r')}[n]
        s = 0.0
        for nm in namen:
            b = self.f.ix[nm]
            if nm.startswith('hand'):
                g = unten - max(tief[b], *[tief[self.f.ix[k]] for k in self.f.namen if k.startswith('klaue_' + nm[-1])])
            else:
                g = unten - tief[b]
            s += max(0.0, g - 40.0)
        return s

    def karte(self, n):
        """Knochenkarte Bild n, wie das Spiel."""
        p = self.posen[n]
        kam = self.kam[n]
        tief = self.boden(n)
        w = list(p['wurzel'])
        w[1] -= tief
        R, T = schau.fk(self.f.eltern, self.f.versatz, w, p['winkel'])
        Y = schau.ry(p['gier'] * E)
        la = np.array(p['lage'], float)
        Q = []
        Pm = kam['P']
        for ti, t in enumerate(self.t):
            b = t.knochen
            V = self.eck(ti)
            Wv = (V @ R[b].T + T[b]) @ Y.T + la
            h = np.c_[Wv, np.ones(len(Wv))] @ Pm.T
            q = h[:, :2] / h[:, 2:3]
            d = (Wv - kam['auge']) @ kam['R'][2]
            for ff in self.netz[ti]:
                ff = list(ff)
                pts = q[ff]
                x, y = pts[:, 0], pts[:, 1]
                s = float((x * np.roll(y, -1) - np.roll(x, -1) * y).sum())
                if s >= 0:
                    continue
                Q.append((float(d[ff].mean()), b, pts))
        Q.sort(key=lambda z: -z[0])
        kn = np.full((480, 640), -1, np.int16)
        for _d, b, pts in Q:
            cv2.fillConvexPoly(kn, np.round(pts * 8).astype(np.int32), int(b), lineType=cv2.LINE_8, shift=3)
        return kn

    def kosten(self, gewicht=None):
        J = 0.0
        det = {}
        frei = ~LOGO
        z = self.fig['zuordnung']
        for n in (1, 2):
            kn = self.karte(n)
            for t in TEILE[n]:
                ids = z[n][t]
                v = np.isin(kn, ids)
                M = MASKEN[t]
                neutral = np.zeros_like(M)
                for o in TEILE[n]:
                    if o != t:
                        neutral |= M & MASKEN[o] & np.isin(kn, z[n][o])
                W = M & ~neutral & frei
                V = v & frei
                iou = float((V & W).sum()) / max(int((V | W).sum()), 1)
                det[t] = iou
                J += (1 - iou) * (gewicht or {}).get(t, 1.0)
            if n == 2:
                J += float((kn[225:262, 400:414] < 0).sum()) / 250.0        # Rand rechts mindestens x=413 (FORMBLATT 1.4, W9)
            J += self.kontakt(n) / 500.0                                   # nichts schwebt
            sch = int(((kn >= 0) & (ZIEL[n] == 0)).sum())
            weiss = int(((kn < 0) & (ZIEL[n] == 255)).sum())
            det['schwarz%d' % n] = sch
            det['weiss%d' % n] = weiss
            J += sch / 1500.0 + weiss / 6000.0
        # Glaettung
        for k, p in self.par.items():
            J += .4 * ((p[0] - 1) ** 2 + (p[1] - 1) ** 2) + 2e-7 * (p[2] ** 2 + p[3] ** 2)
            g, i = k.rsplit('/', 1)
            q = self.par.get(g + '/%d' % (int(i) + 1))
            if q is not None:
                J += 1.2 * ((p[0] - q[0]) ** 2 + (p[1] - q[1]) ** 2) + 1e-6 * ((p[2] - q[2]) ** 2 + (p[3] - q[3]) ** 2)
        return J, det


def ring_gruppen(t):
    """Ringe, die enger als 70 Einheiten liegen, bilden eine Gruppe (Plattenstufen)."""
    aus, cur = [], [0]
    m = [r.mean(0) for r in t.ringe]
    for i in range(1, len(m)):
        if np.linalg.norm(m[i] - m[i - 1]) < 70:
            cur.append(i)
        else:
            aus.append(cur)
            cur = [i]
    aus.append(cur)
    return aus


MITTE = ('huefte', 'wirbel', 'brust', 'hals', 'kopf', 'kiefer', 'schwanz1', 'schwanz2', 'schwanz3', 'schwanz4', 'schwanz5', 'schwanz6')
GLIEDER = {'rumpf': ['huefte', 'wirbel', 'brust'], 'schwanz': ['schwanz%d' % i for i in range(1, 7)], 'bein': ['oberschenkel', 'unterschenkel', 'fuss'], 'arm': ['oberarm', 'unterarm', 'hand'], 'kopf': ['hals', 'kopf', 'kiefer']}


def lauf(args):
    m = Modell()
    if os.path.exists(RINGFIT):
        m.par = json.load(open(RINGFIT))
        for ti in range(len(m.t)):
            m.anwenden_teil(ti)
    wahl = []
    for w in args.teile.split(','):
        if w:
            wahl += GLIEDER.get(w, [w])
    para = []     # (art, schluessel, index, schritt)
    gesehen = set()
    for ti, t in enumerate(m.t):
        g = gruppe_von(t.name)
        if g not in wahl or g in gesehen:
            continue
        gesehen.add(g)
        for gr in ring_gruppen(t):
            para.append(('ring', g, gr, ti))
    J, det = m.kosten()
    print('Start J %.3f' % J, {k: round(v, 3) for k, v in det.items()})
    schritt = np.array([.05, .05, 30.0, 30.0])
    wsch = 12
    wlist = []
    if args.posen:
        for n, namen in ((1, args.posen1.split(',')), (2, args.posen2.split(','))):
            for nm in namen:
                if nm and nm in m.f.ix:
                    for a_ in range(3):
                        wlist.append((n, m.f.ix[nm], a_))
    w0 = {n: [list(x) for x in m.posen[n]['winkel']] for n in (1, 2)}
    t0 = time.time()
    for runde in range(args.runden):
        besser = 0
        for art, g, gr, ti in para:
            for a in range(4):
                if a == 3 and g in MITTE:
                    continue                    # Mittelteile bleiben symmetrisch, seitlich verschiebt nur die Pose
                for vz in (1, -1):
                    alt = {}
                    for i in gr:
                        k = g + '/%d' % i
                        alt[k] = m.par.get(k)
                        p = list(m.par.get(k, [1.0, 1.0, 0.0, 0.0]))
                        p[a] += vz * schritt[a]
                        if a < 2:
                            p[a] = min(max(p[a], .55), 1.5)
                        else:
                            p[a] = min(max(p[a], -450), 450)
                        m.par[k] = p
                    betroffen = [j for j, q in enumerate(m.t) if gruppe_von(q.name) == g]
                    for j in betroffen:
                        m.anwenden_teil(j)
                    J2, det2 = m.kosten()
                    if J2 < J - 1e-4:
                        J, det = J2, det2
                        besser += 1
                        break
                    for k, v in alt.items():
                        if v is None:
                            del m.par[k]
                        else:
                            m.par[k] = v
                    for j in betroffen:
                        m.anwenden_teil(j)
        for n, b, a_ in wlist:
            for vz in (1, -1):
                alt = m.posen[n]['winkel'][b][a_]
                m.posen[n]['winkel'][b][a_] = alt + vz * wsch
                dJ = 2e-7 * ((alt + vz * wsch - w0[n][b][a_]) ** 2 - (alt - w0[n][b][a_]) ** 2)
                J2, det2 = m.kosten()
                if J2 + dJ < J - 1e-4:
                    J, det = J2, det2
                    besser += 1
                    break
                m.posen[n]['winkel'][b][a_] = alt
        print('Runde %d J %.3f, %d Treffer, %.0f s' % (runde, J, besser, time.time() - t0), {k: round(v, 3) for k, v in det.items()})
        json.dump(m.par, open(RINGFIT, 'w'))
        if wlist:
            pf = os.path.join(HIER, 'posen.json')
            d = json.load(open(pf, encoding='utf-8'))
            for n, b, a_ in wlist:
                d.setdefault('bild%d' % n, {}).setdefault('winkel', {})[m.f.namen[b]] = [int(c) for c in m.posen[n]['winkel'][b]]
            json.dump(d, open(pf, 'w', encoding='utf-8'), indent=0)
        if not besser:
            schritt = schritt * .5
            wsch = max(4, wsch // 2)
            if schritt[0] < .012:
                break


if __name__ == '__main__':
    a = argparse.ArgumentParser()
    a.add_argument('--teile', default='rumpf')
    a.add_argument('--runden', type=int, default=3)
    a.add_argument('--posen', action='store_true')
    a.add_argument('--posen1', default='schwanz1,schwanz2,schwanz3,schwanz4,schwanz5,schwanz6,oberschenkel_l,unterschenkel_l,oberschenkel_r,unterschenkel_r,fuss_r,oberarm_r,unterarm_r,hand_r,wirbel,brust')
    a.add_argument('--posen2', default='brust,oberarm_l,unterarm_l,hand_l,oberarm_r,unterarm_r,hand_r,oberschenkel_r,unterschenkel_r,huefte,wirbel,klaue_r1,klaue_r2,klaue_r3,klaue_r4,klaue_l1,klaue_l2,klaue_l3')
    a = a.parse_args()
    lauf(a)
