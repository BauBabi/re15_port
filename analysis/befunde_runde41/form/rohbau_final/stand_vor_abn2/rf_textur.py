#!/usr/bin/env python3
"""Rohbau final, Textur: Materialzonen (farbe_palette.json) und die Zeichnung an ihrer Bildstelle (FORMBLATT2 Abschnitt 6), per Rueckprojektion auf die Oberflaeche.
Malen kommt spaeter. Marken in echter Groesse: Form aus der Bildhelligkeit (halbe Hoehe) innerhalb des Prueffelds, nicht das ganze Prueffeld. Helligkeit: Farbe je Marke gegen die zurueckgerechnete Schirmfarbe eingestellt (zw/marken.json).
Atlas: je Teil ein Streifen, U = Umfang (j/n), V = Laenge; Lage in atlas.json."""
import json
import os

import cv2
import numpy as np

from rf_kern import E, FORM, HIER, ringecken
import form_emd as FE
import form_kamera as FK
import form_pruefung as P

TEX_W, TEX_H = 384, 256
GRUND = (32, 18, 22)        # Grundhaut untere Haelfte der Palette: im Filter Kern wie Video (Bewertung Bild 5), im Spiel rotbraun statt violett (Violett gehoert dem Video, farbe.md 1)
ROTBRAUN = (96, 48, 50)
ROTWARM = (112, 50, 46)
HORN = (86, 64, 62)
ZAHN = (52, 40, 40)
MUND = (30, 9, 12)
AUGE = (120, 24, 24)
KAMM = (88, 94, 106)


def palette():
    with open(os.path.join(FORM, 'farbe_palette.json'), encoding='utf-8') as f:
        j = json.load(f)
    return {x['name']: x for x in j['materialien']}, j


def texelkarte(form):
    kn = np.full((TEX_H, TEX_W), -1, np.int32)
    pos = np.zeros((TEX_H, TEX_W, 3), np.float32)
    for b, m in enumerate(form['meshes']):
        ek = np.array(m['ecken'], np.float32) if m['ecken'] else np.zeros((0, 3), np.float32)
        for f, tri in [(q, ((0, 1, 3), (0, 3, 2))) for q in m['vierecke']] + [(d, ((0, 1, 2),)) for d in m['dreiecke']]:
            t = np.array(f['tex'], np.float32)
            e = ek[f['e']]
            for a, bb, c in tri:
                u, v = t[[a, bb, c], 0], t[[a, bb, c], 1]
                x0, x1 = int(np.floor(u.min())), int(np.ceil(u.max())) + 1
                y0, y1 = int(np.floor(v.min())), int(np.ceil(v.max())) + 1
                x0, y0, x1, y1 = max(x0, 0), max(y0, 0), min(x1, TEX_W), min(y1, TEX_H)
                if x0 >= x1 or y0 >= y1:
                    continue
                fl = (u[1] - u[0]) * (v[2] - v[0]) - (u[2] - u[0]) * (v[1] - v[0])
                if abs(fl) < 1e-6:
                    continue
                X, Y = np.meshgrid(np.arange(x0, x1) + .5, np.arange(y0, y1) + .5)
                w1 = ((X - u[0]) * (v[2] - v[0]) - (u[2] - u[0]) * (Y - v[0])) / fl
                w2 = ((u[1] - u[0]) * (Y - v[0]) - (X - u[0]) * (v[1] - v[0])) / fl
                w0 = 1 - w1 - w2
                ok = (w0 >= -.15) & (w1 >= -.15) & (w2 >= -.15)
                if not ok.any():
                    continue
                p = w0[..., None] * e[a] + w1[..., None] * e[bb] + w2[..., None] * e[c]
                s = kn[y0:y1, x0:x1]
                q = pos[y0:y1, x0:x1]
                s[ok] = b
                q[ok] = p[ok]
    return kn, pos


def pose_matrizen(form, clip, gier, lage):
    b = form['clips'][clip]['bilder'][0]
    R, T = FE.pose_aus_bild(form, b)
    a = gier * E
    Y = np.array([[np.cos(a), 0, np.sin(a)], [0, 1, 0], [-np.sin(a), 0, np.cos(a)]])
    Rw = [Y @ (r.astype(float) / 4096.0) for r in R]
    Tw = [Y @ t.astype(float) + np.asarray(lage, float) for t in T]
    return Rw, Tw


def tiefenbild(form, Rw, Tw, kam):
    tr = []
    for b, m in enumerate(form['meshes']):
        if not m['ecken']:
            continue
        v = np.array(m['ecken'], float) @ Rw[b].T + Tw[b]
        q = FK.projiziere(kam, v)
        z = FK.tiefe(kam, v)
        for f, ix in [(x, ((0, 1, 3), (0, 3, 2))) for x in m['vierecke']] + [(x, ((0, 1, 2),)) for x in m['dreiecke']]:
            for t in ix:
                e = [f['e'][i] for i in t]
                tr.append((float(z[e].mean()), q[e]))
    tr.sort(key=lambda x: -x[0])
    d = np.full((P.HOEHE, P.BREITE), 1e9, np.float32)
    for zm, q in tr:
        if not np.isfinite(q).all():
            continue
        cv2.fillConvexPoly(d, np.round(q * 8).astype(np.int32), float(zm), lineType=cv2.LINE_8, shift=3)
    return d


class Bake:
    def __init__(self, form, stand, kn, pos, geschont):
        self.form, self.kn, self.pos = form, kn, pos
        self.q, self.z, self.vis, self.d = {}, {}, {}, {}
        for n in (1, 2):
            p = stand['posen'][str(n)]
            Rw, Tw = pose_matrizen(form, n - 1, round(p['gier']), [round(p['lage'][0]), 0, round(p['lage'][2])])
            kam = FK.lade_kamera(n)
            H, W = kn.shape
            w = np.zeros((H, W, 3), np.float32)
            for b in range(len(form['knochen'])):
                s = kn == b
                if s.any():
                    w[s] = pos[s] @ Rw[b].T.astype(np.float32) + Tw[b].astype(np.float32)
            m = kn >= 0
            q = np.full((H, W, 2), np.nan, np.float32)
            z = np.full((H, W), 1e9, np.float32)
            q[m] = FK.projiziere(kam, w[m])
            z[m] = FK.tiefe(kam, w[m])
            d = tiefenbild(form, Rw, Tw, kam)
            x, y = np.round(np.nan_to_num(q[..., 0], nan=-1)).astype(int), np.round(np.nan_to_num(q[..., 1], nan=-1)).astype(int)
            inn = (x >= 0) & (x < P.BREITE) & (y >= 0) & (y < P.HOEHE) & m
            v = np.zeros((H, W), bool)
            v[inn] = z[inn] <= d[y[inn], x[inn]] + 140
            self.q[n], self.z[n], self.vis[n], self.d[n] = q, z, v & ~geschont, d

    def stelle(self, n, maske):
        """Sichtbare Texel, deren Bildpunkt in der Bildmaske liegt."""
        q = self.q[n]
        x, y = np.round(np.nan_to_num(q[..., 0], nan=-1)).astype(int), np.round(np.nan_to_num(q[..., 1], nan=-1)).astype(int)
        inn = self.vis[n] & (x >= 0) & (x < P.BREITE) & (y >= 0) & (y < P.HOEHE)
        a = np.zeros(self.kn.shape, bool)
        a[inn] = maske[y[inn], x[inn]] > 0
        return a


def poly(pl, weite=0):
    m = np.zeros((P.HOEHE, P.BREITE), np.uint8)
    for p in pl:
        cv2.fillPoly(m, [np.round(np.array(p, np.float32)).astype(np.int32)], 1)
    if weite:
        m = cv2.dilate(m, cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (2 * weite + 1, 2 * weite + 1)))
    return m


def echt(n):
    return cv2.imread(os.path.join(FORM, 'farbe_bild%d_echt.png' % n))[:, :, ::-1].astype(np.float32)


def marke_form(n, m, anteil=.5, rand=4):
    """Echte Form einer Marke: Pixel in der Maske, die ueber halber Hoehe zwischen Umgebung und Spitze liegen."""
    y = cv2.GaussianBlur(P.original(n).astype(np.float32) @ P.YK, (0, 0), .8)
    g = cv2.dilate(m, np.ones((2 * rand + 1, 2 * rand + 1), np.uint8)) > 0
    ring = g & ~(m > 0)
    hi = float(np.percentile(y[m > 0], 92))
    lo = float(np.median(y[ring])) if ring.any() else float(y[m > 0].min())
    s = lo + anteil * (hi - lo)
    return ((y >= s) & g).astype(np.uint8), hi, lo


def ringzeilen(p, r, l, c):
    """Zeile (Texel) je Ring im Streifen, wie teil_uv im Bau."""
    import rf_bau
    ring, _, _ = rf_bau.teil_ecken(p, l, c)
    u, v, _, _ = rf_bau.teil_uv(p, r, ring)
    return v


def zaehne(s, U, V, u0, du, v0, v1, n, farbe):
    """Zahnreihe in die Textur: n Spitzen zwischen v0 und v1, Basis bei u0, Spitze um du versetzt. RE2 legt die Zaehne ebenso in die Textur."""
    w = (U - u0) / du
    q = ((V - v0) / (v1 - v0) * n) % 1.0
    z = 1 - np.abs(2 * q - 1)
    s[(V >= v0) & (V <= v1) & (w >= 0) & (w <= z)] = farbe


def zonen(rgb, teile, lage, L, C):
    """Zonen im Streifen. U = 0..1 rund um den Ring (j/n), V = 0..1 laengs. Bild 2 sieht Vorderseite, Bild 1 Rueckseite, darum tragen Haende und Armfronten Rotbraun (Video Hand R 75, Arm R 106), die Rueckseiten Grundhaut."""
    for i, (p, r) in enumerate(zip(teile, lage)):
        x, y, w, h = r
        U, V = np.meshgrid((np.arange(w) + .5) / w, (np.arange(h) + .5) / h)
        nm = p.name
        s = rgb[y:y + h, x:x + w]
        if nm.startswith('fang'):
            s[:] = ZAHN
        elif nm.startswith('wange'):
            s[:] = (70, 50, 60)
            s[V > .6] = HORN
        elif nm.startswith('klaue') or nm.startswith('kralle') or nm.startswith('zeh'):
            s[:] = ROTBRAUN
            s[V > .52] = (72, 40, 44)
            s[V > .74] = HORN
        elif nm.startswith('hand'):
            ruecken = np.abs(U - .5) < .36
            s[ruecken] = ROTBRAUN
        elif nm.startswith('unterarm'):
            vorn = np.abs(U - .5) > .22
            s[vorn] = ROTWARM
        elif nm == 'kiefer':
            s[np.abs(U - .5) < .14] = MUND
            for u0, vz in ((.36, 1), (.64, -1)):
                zaehne(s, U, V, min(u0, u0 + vz * .045), .045, .3, .95, 7, ZAHN) if vz > 0 else zaehne(s, 1 - U, V, 1 - u0, .045, .3, .95, 7, ZAHN)
        elif nm == 'kopf':
            zv = ringzeilen(p, r, L[i], C[i])
            v6 = (zv[6] - y) / h
            s[((U < .1) | (U > .9)) & (V > v6 - .1)] = MUND
            # Zahnreihe am Rand der Schnauze, Augen in der Hoehle hinter dem Brauengrat
            zaehne(s, U, V, .1, .05, v6, .97, 8, ZAHN)
            zaehne(s, 1 - U, V, .1, .05, v6, .97, 8, ZAHN)
            for uu in (.3, .7):
                s[(np.abs(U - uu) < .035) & (np.abs(V - (zv[5] + .35 * (zv[6] - zv[5]) - y) / h) < .03)] = AUGE
    return rgb


def backen(form, g, teile, lage, stand, marken=True, L=None, C=None):
    pal, roh = palette()
    rgb = np.zeros((TEX_H, TEX_W, 3), np.uint8)
    rgb[:] = GRUND
    rgb = zonen(rgb, teile, lage, L, C)
    kn, pos = texelkarte(form)
    geschont = np.zeros(kn.shape, bool)
    for p, r in zip(teile, lage):
        if p.feld is not None:
            x, y, w, h = r
            geschont[y:y + h, x:x + w] = True
    bk = Bake(form, stand, kn, pos, geschont)
    # Bild 1 sieht den rechten Arm von hinten: dort dunkel (Armzylinder Video (30,30,52)). Rotbraun nur, was Bild 1 nicht sieht oder was Bild 2 braucht.
    for p, r in zip(teile, lage):
        if p.name in ('hand_r', 'unterarm_r', 'kralle_r', 'klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4'):
            x, y, w, h = r
            sub = bk.vis[1][y:y + h, x:x + w]
            if p.name != 'unterarm_r':
                sub = sub & ~bk.vis[2][y:y + h, x:x + w]     # Unterarm: Bild 1 hat Vorrang (Pruefstelle N2), Hand und Finger zeigt Bild 2 mehr
            else:
                sub = cv2.dilate(sub.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0     # Randtexel: Rueckprojektion rundet, ein Saum bleibt sonst rot
            rgb[y:y + h, x:x + w][sub] = GRUND
    # Pruefstelle N2: alles, was Bild 1 im Armzylinder (384-411,302-354) zeigt, bleibt Grundhaut, auch wenn Bild 2 es sieht
    n2 = np.zeros((P.HOEHE, P.BREITE), np.uint8)
    n2[298:358, 380:415] = 1
    n2t = bk.stelle(1, n2)
    for p, r in zip(teile, lage):
        if p.name in ('hand_r', 'kralle_r', 'klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4'):
            x, y, w, h = r
            rgb[y:y + h, x:x + w][n2t[y:y + h, x:x + w]] = GRUND
    if marken:
        import rf_marken
        rgb = rf_marken.malen(rgb, bk, pal)
    return rgb, bk
