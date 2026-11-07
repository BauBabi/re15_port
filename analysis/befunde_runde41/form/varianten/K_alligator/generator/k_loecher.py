#!/usr/bin/env python3
"""Variante K, Nachtrag Nutzer (Loecher Bein/Rumpf): Loecher und Spalte messen statt schaetzen.

    python k_loecher.py messen EMD --aus DATEI.json [--name X] [--bilder 8]
    python k_loecher.py kanten EMD                      offene Kanten je Mesh

Je Clip (alle Clips des EMD) bis zu --bilder gleichmaessig verteilte Bilder, je Bild ANSICHTEN (frei um die Figur, orthografisch, fester Massstab
MASSSTAB px je Einheit). Zeichnen wie der Port: Rueckseiten verworfen nach re15_nclip_r41_weg (Dreieck bei MAC0 <= 0, Viereck bei MAC0 < 0, Bild-y unten,
form_buehne.dreiecke), dann Tiefenpuffer. Loch = Hintergrund-Flaeche (4er-Nachbarschaft), die den Bildrand nicht erreicht, also rundum von Figur umgeben ist.
Nahtloch = Loch, dessen Rand (2 px) Pixel beider Knochen eines Gelenkpaars NAEHTE beruehrt und dessen Schwerpunkt naeher als NAHT_RADIUS am Drehpunkt liegt.
Kleinste gezaehlte Flaeche MIN_PX (darunter Rasterrauschen an Kanten). Kein RE-Wert."""
import argparse
import json
import math
import os
import sys

import cv2
import numpy as np

import k_kern as KK

FE = KK.FE
GROSS = 560
MASSSTAB = 0.075          # px je Einheit (Spiel: Bild 2 rund 0,07-0,08 px je Einheit bei Tiefe 5000-6500, fx 451)
MIN_PX = 3
NAHT_RADIUS = 900.0       # Einheiten um den Drehpunkt (Gelenkringe 300-900)
ANSICHTEN = (('vorn', 90, 0), ('hinten', -90, 0), ('links', 180, 0), ('rechts', 0, 0), ('oben', 0, 89.9), ('unten', 0, -89.9),
             ('3/4 vorn rechts oben', 45, 22), ('3/4 hinten links oben', -135, 22), ('3/4 vorn links unten', 135, -30), ('3/4 hinten rechts unten', -45, -30))
# Gelenke: Name -> (Kind, Eltern, Nahtgruppe = Knochen, die an dieser Naht zusammenstossen, Gelenkradius in Einheiten = groesster Ring des Kindes am Ansatz,
# rf_plan.py: Oberschenkel 334, Oberarm 385, Hals 540, Schwanz1 600, Kopf 450, Unterschenkel 276, Unterarm 325, Fuss 215, Hand 330, Schwanzglieder 340)
NAEHTE = {'Bein l - Huefte': (8, 0, (8, 0, 7), 334), 'Bein r - Huefte': (9, 0, (9, 0, 7), 334), 'Arm l - Brust': (11, 10, (11, 10, 7), 385),
          'Arm r - Brust': (16, 10, (16, 10, 7), 385), 'Hals - Brust': (21, 10, (21, 10), 540), 'Schwanz - Becken': (1, 0, (1, 0, 7), 600),
          'Kopf - Hals': (22, 21, (22, 21, 10), 450), 'Wirbel - Huefte': (7, 0, (7, 0), 700), 'Brust - Wirbel': (10, 7, (10, 7), 700),
          'Kiefer - Kopf': (23, 22, (23, 22), 400),
          'Knie l': (24, 8, (24, 8), 276), 'Knie r': (26, 9, (26, 9), 276), 'Ellbogen l': (12, 11, (12, 11), 325), 'Ellbogen r': (17, 16, (17, 16), 325),
          'Fuss l': (25, 24, (25, 24), 215), 'Fuss r': (27, 26, (27, 26), 215), 'Hand l': (13, 12, (13, 12), 330), 'Hand r': (18, 17, (18, 17), 330),
          'Schwanz 1-2': (2, 1, (2, 1), 340), 'Schwanz 2-3': (3, 2, (3, 2), 340), 'Schwanz 3-4': (4, 3, (4, 3), 340), 'Schwanz 4-5': (5, 4, (5, 4), 340),
          'Schwanz 5-6': (6, 5, (6, 5), 340)}
NAHT_ANTEIL = 0.5       # so viel des Lochrands muss zur Nahtgruppe gehoeren (Rest: Glieder davor, z. B. Hand vor dem Hals)
NAHT_WEITE = 1.5        # kein Lochpixel weiter als 1,5 Gelenkradien vom Drehpunkt: sonst ist es eine Luecke zwischen Gliedern (Arm vor Bein), keine Naht
HAUPT = ('Bein l - Huefte', 'Bein r - Huefte', 'Arm l - Brust', 'Arm r - Brust', 'Hals - Brust', 'Schwanz - Becken', 'Kopf - Hals')


def blick(az, el):
    a, e = math.radians(az), math.radians(el)
    vor = np.array([-math.sin(a) * math.cos(e), math.sin(e), math.cos(a) * math.cos(e)])
    rechts = np.array([math.cos(a), 0.0, math.sin(a)])
    unten = np.cross(vor, rechts)
    if unten[1] < 0 and abs(el) < 80:
        unten = -unten
    return np.stack([rechts, unten, vor])


class Figur:
    def __init__(self, emd):
        self.emd = open(emd, 'rb').read() if isinstance(emd, str) else emd
        self.form = FE.lese_emd(self.emd)
        d = FE.verzeichnis(self.emd)[1]
        import emd_ansichtsblatt as EA
        self.skel = EA.emr_parse(self.emd[d[2]:d[3]])
        self.clips, self.frames = EA.edd_parse(self.emd[d[1]:d[2]])
        self.mo = type('M', (), {})()
        self.mo.skel = self.skel
        self.pose = lambda kf: EA.Modell.pose(self.mo, kf)
        # Flaechen je Mesh: Ecken, Viereck-Kennung
        self.faces = []
        for b, m in enumerate(self.form['meshes']):
            V = np.array(m['ecken'], float).reshape(-1, 3)
            T = [(f['e'], 0) for f in m['dreiecke']] + [(f['e'], 1) for f in m['vierecke']]
            self.faces.append((V, T))

    def kf(self, clip, bild):
        f0, k = self.clips[clip]
        return self.frames[f0 + min(bild, k - 1)] & 0xFFF

    def welt(self, kf):
        R, T = self.pose(kf)
        return [np.asarray(r, float) / 4096.0 for r in R], [np.asarray(t, float) for t in T]


def rendern(fig, kf, A, mitte=None, gross=GROSS, s=MASSSTAB):
    """Kennung je Pixel (-1 leer), Drehpunkte im Bild, Mitte. Orthografisch, Port-Rueckseitenregel, Tiefenpuffer."""
    R, T = fig.welt(kf)
    alle = []
    for b, (V, F) in enumerate(fig.faces):
        if len(V) and b < len(R):
            alle.append(V @ R[b].T + T[b])
    if mitte is None:
        P = np.vstack(alle)
        mitte = (P.max(0) + P.min(0)) / 2
    tief = np.full((gross, gross), np.inf)
    idb = np.full((gross, gross), -1, int)
    for b, (V, F) in enumerate(fig.faces):
        if not len(V) or b >= len(R):
            continue
        q = ((V @ R[b].T + T[b]) - mitte) @ A.T
        sx, sy, sz = gross / 2 + q[:, 0] * s, gross / 2 + q[:, 1] * s, q[:, 2]
        for e, vier in F:
            p0, p1, p2 = (sx[e[0]], sy[e[0]]), (sx[e[1]], sy[e[1]]), (sx[e[2]], sy[e[2]])
            m0 = p0[0] * p1[1] + p1[0] * p2[1] + p2[0] * p0[1] - p0[0] * p2[1] - p1[0] * p0[1] - p2[0] * p1[1]
            if (m0 < 0) if vier else (m0 <= 0):
                continue
            tris = ((e[0], e[1], e[3]), (e[0], e[3], e[2])) if vier else ((e[0], e[1], e[2]),)
            for t in tris:
                x, y, z = sx[list(t)], sy[list(t)], sz[list(t)]
                x0, x1 = max(int(math.floor(x.min())), 0), min(int(math.ceil(x.max())) + 1, gross)
                y0, y1 = max(int(math.floor(y.min())), 0), min(int(math.ceil(y.max())) + 1, gross)
                if x0 >= x1 or y0 >= y1:
                    continue
                fl = (x[1] - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (y[1] - y[0])
                if abs(fl) < 1e-9:
                    continue
                PX, PY = np.meshgrid(np.arange(x0, x1) + 0.5, np.arange(y0, y1) + 0.5)
                w1 = ((PX - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (PY - y[0])) / fl
                w2 = ((x[1] - x[0]) * (PY - y[0]) - (PX - x[0]) * (y[1] - y[0])) / fl
                w0 = 1 - w1 - w2
                inn = (w0 >= -1e-6) & (w1 >= -1e-6) & (w2 >= -1e-6)
                if not inn.any():
                    continue
                zz = w0 * z[0] + w1 * z[1] + w2 * z[2]
                tb, ib = tief[y0:y1, x0:x1], idb[y0:y1, x0:x1]
                neu = inn & (zz < tb)
                tb[neu] = zz[neu]
                ib[neu] = b
    piv = {}
    for nm, (k, e, _g, _r) in NAEHTE.items():
        if k < len(T):
            q = (T[k] - mitte) @ A.T
            piv[nm] = (gross / 2 + q[0] * s, gross / 2 + q[1] * s)
    return idb, piv, mitte


def loecher(idb, piv, s=MASSSTAB):
    """Eingeschlossene Hintergrundflaechen. Rueckgabe Liste dict(px, mitte, naht, knochen, box). Naht = Gelenk, dessen Gruppe den Rand zu >= NAHT_ANTEIL stellt
    und dessen Drehpunkt hoechstens NAHT_WEITE Gelenkradien vom fernsten Lochpixel liegt."""
    leer = (idb < 0).astype(np.uint8)
    n, lab, st, cen = cv2.connectedComponentsWithStats(leer, connectivity=4)
    h, w = idb.shape
    aus = []
    for i in range(1, n):
        x, y, bw, bh, a = st[i]
        if x == 0 or y == 0 or x + bw >= w or y + bh >= h or a < MIN_PX:
            continue
        m = lab == i
        rand = cv2.dilate(m.astype(np.uint8), np.ones((5, 5), np.uint8)).astype(bool) & ~m
        rk = idb[rand]
        rk = rk[rk >= 0]
        kn = sorted(set(int(v) for v in np.unique(rk)))
        ys, xs = np.nonzero(m)
        naht, best = None, 1e9
        for nm, (k, e, grp, r) in NAEHTE.items():
            if nm not in piv or k not in kn or e not in kn:
                continue
            ant = float(np.isin(rk, grp).mean()) if len(rk) else 0.0
            d = float(np.sqrt((xs - piv[nm][0]) ** 2 + (ys - piv[nm][1]) ** 2).max()) / s
            if ant >= NAHT_ANTEIL and d <= NAHT_WEITE * r and d / r < best:
                naht, best = nm, d / r
        c = cen[i]
        aus.append(dict(px=int(a), mitte=[round(float(c[0]), 1), round(float(c[1]), 1)], naht=naht, knochen=kn, box=[int(x), int(y), int(bw), int(bh)]))
    return aus


def bilder_von(fig, clip, nb):
    k = fig.clips[clip][1]
    return sorted(set(int(round(x)) for x in np.linspace(0, k - 1, min(k, nb))))


def messen(emd, nb=8, name=None, nur_clips=None):
    fig = Figur(emd)
    R = dict(name=name or os.path.basename(emd), massstab_px_je_einheit=MASSSTAB, min_px=MIN_PX, ansichten=[a[0] for a in ANSICHTEN], clips=[])
    for c in range(len(fig.clips)):
        if nur_clips is not None and c not in nur_clips:
            continue
        zc = dict(clip=c, bilder=[], loecher=0, loch_px=0, naht=0, naht_px=0, je_naht={})
        for b in bilder_von(fig, c, nb):
            kf = fig.kf(c, b)
            for an, az, el in ANSICHTEN:
                idb, piv, _ = rendern(fig, kf, blick(az, el))
                L = loecher(idb, piv)
                if not L:
                    continue
                e = dict(bild=b, ansicht=an, loecher=L)
                zc['bilder'].append(e)
                zc['loecher'] += len(L)
                zc['loch_px'] += sum(x['px'] for x in L)
                for x in L:
                    if x['naht']:
                        zc['naht'] += 1
                        zc['naht_px'] += x['px']
                        zc['je_naht'][x['naht']] = zc['je_naht'].get(x['naht'], 0) + x['px']
        zc['posen'] = len(bilder_von(fig, c, nb)) * len(ANSICHTEN)
        R['clips'].append(zc)
        print('Clip %2d: %3d Posen, %3d Loecher (%5d px), davon Naht %3d (%5d px) %s' % (c, zc['posen'], zc['loecher'], zc['loch_px'], zc['naht'], zc['naht_px'],
                                                                                    ', '.join('%s %d' % kv for kv in sorted(zc['je_naht'].items(), key=lambda t: -t[1]))), flush=True)
    R['summe'] = dict(loecher=sum(c['loecher'] for c in R['clips']), loch_px=sum(c['loch_px'] for c in R['clips']),
                      naht=sum(c['naht'] for c in R['clips']), naht_px=sum(c['naht_px'] for c in R['clips']))
    return R


def kanten(emd):
    """Offene Kanten je Mesh: Kante (Eckpaar nach Lage, nicht nach Index) nur in einer Flaeche."""
    form = FE.lese_emd(open(emd, 'rb').read())
    aus = {}
    for b, m in enumerate(form['meshes']):
        V = [tuple(v) for v in m['ecken']]
        cnt = {}
        for f in m['dreiecke'] + m['vierecke']:
            e = f['e']
            ring = [e[0], e[1], e[2]] if len(e) == 3 else [e[0], e[1], e[3], e[2]]
            for i in range(len(ring)):
                a, c = V[ring[i]], V[ring[(i + 1) % len(ring)]]
                if a == c:
                    continue
                k = (min(a, c), max(a, c))
                cnt[k] = cnt.get(k, 0) + 1
        aus[b] = dict(kanten=len(cnt), offen=sum(1 for v in cnt.values() if v == 1), mehrfach=sum(1 for v in cnt.values() if v > 2))
    return aus


if __name__ == '__main__':
    a = argparse.ArgumentParser()
    a.add_argument('befehl', choices=('messen', 'kanten'))
    a.add_argument('emd')
    a.add_argument('--aus')
    a.add_argument('--name')
    a.add_argument('--bilder', type=int, default=8)
    a.add_argument('--clips', default=None)
    x = a.parse_args()
    if x.befehl == 'kanten':
        k = kanten(x.emd)
        for b, v in k.items():
            print(b, v)
        print('offene Kanten gesamt', sum(v['offen'] for v in k.values()))
        if x.aus:
            json.dump(k, open(x.aus, 'w'), indent=0)
    else:
        nc = [int(c) for c in x.clips.split(',')] if x.clips else None
        R = messen(x.emd, x.bilder, x.name, nc)
        print('Summe', R['summe'])
        if x.aus:
            json.dump(R, open(x.aus, 'w', encoding='utf-8'), indent=0)
