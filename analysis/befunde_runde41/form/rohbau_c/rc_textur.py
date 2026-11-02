#!/usr/bin/env python3
"""Rohbau C, Textur: nur Materialzonen (farbe_palette.json) und die Zeichnung an ihrer Bildstelle (FORMBLATT2 Abschnitt 6), per Rueckprojektion auf die Oberflaeche. Malen kommt spaeter.
Atlas: je Teil ein Streifen, U = Umfang, V = Laenge; Lage steht in atlas.json."""
import json
import os

import cv2
import numpy as np

from rc_kern import FORM, HIER, E
import form_emd as FE
import form_kamera as FK
import form_pruefung as P

TEX_W, TEX_H = 384, 256


def palette():
    with open(os.path.join(FORM, 'farbe_palette.json'), encoding='utf-8') as f:
        j = json.load(f)
    m = {x['name']: tuple(x['oberflaeche_rgb']) if x.get('oberflaeche_rgb') else None for x in j['materialien']}
    return m, j


def texelkarte(form):
    """Je Texel: Knochen, Lage im Knochenraum, Normale im Knochenraum. Gemeinsame Streifen behalten die letzte Flaeche; sie bekommen keine Bildzeichnung."""
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
    """Maler wie das Spiel: von hinten nach vorn. -> Tiefe je Pixel."""
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
    def __init__(self, form, g, kn, pos, geschont):
        self.form, self.kn, self.pos = form, kn, pos
        self.q, self.z, self.vis, self.d = {}, {}, {}, {}
        self.geschont = geschont
        for n in (1, 2):
            p = g['posen']['bild%d' % n]
            Rw, Tw = pose_matrizen(form, n - 1, p['gier'], p['lage'])
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

    def stelle_bild(self, n, poly, weite, zusatz):
        return self.stelle(n, poly, weite, zusatz)

    def stelle(self, n, poly, weite=0, zusatz=None):
        """Sichtbare Texel, deren Bildpunkt im Polygon liegt."""
        m = np.zeros((P.HOEHE, P.BREITE), np.uint8)
        for p in poly:
            cv2.fillPoly(m, [np.round(np.array(p, np.float32)).astype(np.int32)], 1)
        if weite:
            m = cv2.dilate(m, cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (2 * weite + 1, 2 * weite + 1)))
        if zusatz is not None:
            m = m & zusatz
        q = self.q[n]
        x, y = np.round(np.nan_to_num(q[..., 0], nan=-1)).astype(int), np.round(np.nan_to_num(q[..., 1], nan=-1)).astype(int)
        inn = self.vis[n] & (x >= 0) & (x < P.BREITE) & (y >= 0) & (y < P.HOEHE)
        a = np.zeros(self.kn.shape, bool)
        a[inn] = m[y[inn], x[inn]] > 0
        return a


# Texturwerte: Palette, wo das Licht von Bild 1 (L0 fast senkrecht, oben bis Faktor 1) sie sonst ueberstrahlt, untere Grenze der Spanne. Bild 2 hat nur Umgebungslicht.
GRUND = (26, 10, 16)
BRAUN = (82, 46, 68)
ZAHN = (40, 26, 28)
HORN = (70, 50, 52)


def zonen(rgb, teile, lage, form, pal):
    """Materialzonen im Streifen: Grundhaut, Rotbraun auf Graten, die im Bild nicht zaehlen. u = 0..1 rund um den Ring (j/n), v = 0..1 laengs."""
    for p, r in zip(teile, lage):
        x, y, w, h = r
        U, V = np.meshgrid((np.arange(w) + .5) / w, (np.arange(h) + .5) / h)
        nm = p.name
        s = rgb[y:y + h, x:x + w]
        if nm.startswith('zahn'):
            s[:] = ZAHN
        elif nm.startswith('klaue') or nm.startswith('zeh'):
            s[V > .62] = BRAUN
            s[V > .84] = HORN
        elif nm.startswith('hauer'):
            s[:] = ZAHN
        elif nm == 'kopf':
            # Augen: dunkelrote Zone in der Augenhoehle, Ring 5 Ecke 3 und 7. [f]
            for uu in (.35, .75):
                s[(np.abs(U - uu) < .05) & (np.abs(V - .56) < .05)] = BRAUN
    return rgb


def backen(form, g, teile, lage, marken=True):
    pal, roh = palette()
    grund = np.array(GRUND, float)
    rgb = np.zeros((TEX_H, TEX_W, 3), np.uint8)
    rgb[:] = np.round(grund).astype(np.uint8)
    rgb = zonen(rgb, teile, lage, form, pal)
    kn, pos = texelkarte(form)
    geschont = np.zeros(kn.shape, bool)
    for p, r in zip(teile, lage):
        if p.feld is not None:
            x, y, w, h = r
            geschont[y:y + h, x:x + w] = True
    bk = Bake(form, g, kn, pos, geschont)
    weiss = (150, 150, 150)
    FARBE = dict(rosa_stelle=(155, 117, 128), helle_baender=(102, 117, 124), feine_linien=(84, 94, 108), rotbraun_mittelton=BRAUN, weisskerne=weiss, untere_streifen=(170, 150, 165), z9=(85, 80, 60))
    if marken:
        # Weitere gemessene Flecken der Zeichnungslesung, nach Material der Palette, erst die Toene, darueber die Stellen des Formblatts
        M = P.merkmale()
        for x in roh['merkmale']:
            i, mat = x.get('id'), x.get('material')
            if i in M and mat in ('rotbraun_mittelton', 'feine_linien') and 'polygon' in M[i]:
                n = 1 if i.startswith('A') else 2
                rgb[bk.stelle(n, [M[i]['polygon']])] = np.array(FARBE[mat], float)
        for s in P.stellen():
            # duenne Linien: ein Texel deckt rund 3 Bildpixel, sonst zerfaellt der Zug
            fl = sum(cv2.contourArea(np.array(q, np.float32)) for q in s['polygone'])
            a = bk.stelle(s['bild'], s['polygone'], 2 if fl < 220 else 1)
            rgb[a] = np.array(FARBE[s['material']], float)
        # G: gerippter Streifen am rechten Unterarm. Rippen aus dem Bild: heller Anteil im Polygon behaelt Streifenfarbe, der Rest geht auf Rotbraun.
        for s in P.stellen():
            if s['id'] == 'G':
                o = P.original(2).astype(np.float32) @ P.YK
                z = (o > 120).astype(np.uint8)
                a = bk.stelle_bild(2, s['polygone'], 1, z)
                rgb[bk.stelle(2, s['polygone'], 1)] = np.array(BRAUN, float)
                rgb[a] = np.array(FARBE['untere_streifen'], float)
        # Schwanzspitze, rosa Ende
        rgb[bk.stelle(1, [P.teil_b1('X')['polygon']], 1)] = np.array((112, 72, 82), float)
    return rgb, bk
