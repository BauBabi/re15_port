#!/usr/bin/env python3
"""Form bemalt, Maler: Textur wie ein gemaltes Spielmodell (Stil wie RE2-Gegnertexturen: Form ins Bild gemalt, Licht von oben, dunkle Falten, Adern, Struktur).
Farben nur aus farbe_palette.json (Schirmfarbe = Textur mal Licht; Violett ist Video). Die Zeichnung an ihrer Bildstelle per Rueckprojektion (rf_textur.Bake).

Schichten (je Texel, Atlas wie im Rohbau, Streifen je Teil):
 1. Grund: Grundhaut der Palette zwischen dunkel und hell (Spanne von/bis der Palette), gesteuert durch gemaltes Oberlicht (Normale in der Ruhepose) und Rauschen.
 2. Gemalte Form: Oberlicht (oben heller, unten dunkler), Falten (Naehe zu anderen Teilen in der Ruhepose wird dunkel, wie gemalte Umgebungsverdeckung).
 3. Struktur: Hautflecken (3D-Rauschen im Knochenraum, also nahtlos), dunkelrote Adern.
 4. Bildstruktur: wo Bild 1 oder Bild 2 die Flaeche zeigen, traegt die Textur deren Hell-Dunkel im Kleinen (Bandpass der Bildhelligkeit). So kommen Knollen und Lappen
    des linken Beins (Bild 1) an ihre Stelle. Nicht am Schwanz (E4: glatt).
 5. Zonen: Haende, Krallen, Zaehne, Maul, Augen (rf_textur.zonen, Kopf und Gesicht frei).
 6. Zeichnung (rf_marken.liste) mit weichem Rand, Z5 als Dreieck rund 32x24 px nach E1 bis (417,293). Faktoren je Marke aus zw/marken.json (fb_stell.py).
Gegenproben N1/N2 (form_pruefung.gegenproben): dort bleibt die Albedo unter der Helligkeit von Rotbraun (Y 59).
Kein RE-Wert."""
import json
import math
import os

import cv2
import numpy as np
from scipy.spatial import cKDTree

from rf_kern import FORM, HIER
import form_emd as FE
import form_pruefung as P
import rf_textur as RT

TEX_W, TEX_H = 384, 256
PAL = {x['name']: x for x in json.load(open(os.path.join(FORM, 'farbe_palette.json'), encoding='utf-8'))['materialien']}
G_VON = np.array(PAL['grundhaut_dunkel']['oberflaeche_von'], float)
G_BIS = np.array(PAL['grundhaut_dunkel']['oberflaeche_bis'], float)
G_MIT = np.array(PAL['grundhaut_dunkel']['oberflaeche_rgb'], float)
R_MIT = np.array(PAL['rotbraun_mittelton']['oberflaeche_rgb'], float)
# Hauttoene (frei innerhalb der Palette): Tiefe = Faltengrund, Schatten, Mitte (Palette), Licht (obere Grenze), Glanz (Rotbraun von, nur ausserhalb N1/N2)
# m3 (fb_hautmass.py): Haut im Render halb so hell wie die zurueckgerechnete Schirmfarbe (Bild 1 Y 6-16 gegen 15-24, Bild 2 6 gegen 13). Toene darum im oberen Teil
# der Grundhaut-Spanne, der Glanz in Rotbraun; je Knochen gleicht zw/haut.json (fb_stell.py) den Rest an die Schirmfarbe an.
# m4 (Filter gegen Original ueber der Figur): Bild 1 Rot +24 zu hoch bei gleichem Gruen/Blau, Bild 2 Gruen -8, Blau -6 zu tief. Also weniger Rot gegen Gruen und Blau,
# R/G rund 1,5 statt 2,4 (immer noch in der Spanne der Grundhaut). Den Rest stellt zw/hautton.json (fb_stell.py) je Kanal ein.
HAUT_TIEF = np.array([24, 12, 18], float)
HAUT_SCHATTEN = np.array([38, 23, 31], float)
HAUT_LICHT = np.array([54, 36, 48], float)
HAUT_GLANZ = np.array([84, 56, 74], float)
R_BIS = np.array(PAL['rotbraun_mittelton']['oberflaeche_bis'], float)
ADER = np.array([62, 14, 20], float)
Y_N = 57.0      # Grenze der Gegenproben (Rotbraun von, Y 59) mit Abstand
# Z5 (Auftrag 3, E1): Dreieck rund 30x28 px, Spitze oben, Unterrand laeuft bis (417,293). Gelesen am Ausschnitt zw/z5_ausschnitt.png (8fach, Gamma 0,5).
Z5_DREIECK = [(400, 276), (386, 299), (402, 300), (418, 295), (410, 286)]      # Spitze auf b1.kappe_spitze (400,277), Grenze 2,8 px
OHNE_BILDSTRUKTUR = ('schwanz',)
ADERN_STAERKE = 0.55
# Staerke der Bildstruktur: m6 mit 0,85 las im Filter noch glatt gegen das Original (Knollen am linken Bein, Lichter am Arm fehlten)
BILD_GAIN = 1.7


def texeldaten(form):
    """Je Texel: Knochen, Lage und Normale im Knochenraum (baryzentrisch aus den Ecken). Wie rf_textur.texelkarte, dazu Normalen."""
    kn = np.full((TEX_H, TEX_W), -1, np.int32)
    pos = np.zeros((TEX_H, TEX_W, 3), np.float32)
    nrm = np.zeros((TEX_H, TEX_W, 3), np.float32)
    for b, m in enumerate(form['meshes']):
        if not m['ecken']:
            continue
        ek = np.array(m['ecken'], np.float32)
        nk = np.array(m['normalen'], np.float32) / 4096.0
        for f, tri in [(q, ((0, 1, 3), (0, 3, 2))) for q in m['vierecke']] + [(d, ((0, 1, 2),)) for d in m['dreiecke']]:
            t = np.array(f['tex'], np.float32)
            e = ek[f['e']]
            nn = nk[f['n']]
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
                q = w0[..., None] * nn[a] + w1[..., None] * nn[bb] + w2[..., None] * nn[c]
                kn[y0:y1, x0:x1][ok] = b
                pos[y0:y1, x0:x1][ok] = p[ok]
                nrm[y0:y1, x0:x1][ok] = q[ok]
    nrm /= np.maximum(np.linalg.norm(nrm, axis=2, keepdims=True), 1e-6)
    return kn, pos, nrm


def _hash(ix, iy, iz, saat):
    h = (ix * 73856093) ^ (iy * 19349663) ^ (iz * 83492791) ^ (saat * 2654435761)
    h = (h ^ (h >> 13)) * 1274126177
    return ((h ^ (h >> 16)) & 0xFFFF).astype(np.float32) / 65535.0


def rauschen(p, skala, saat=1):
    """Wertrauschen 3D, glatt (Hermite), Bereich -1..1. p[..., 3] Einheiten."""
    q = p / skala
    i = np.floor(q).astype(np.int64)
    f = q - i
    f = f * f * (3 - 2 * f)
    out = np.zeros(p.shape[:-1], np.float32)
    for dx in (0, 1):
        for dy in (0, 1):
            for dz in (0, 1):
                w = (f[..., 0] if dx else 1 - f[..., 0]) * (f[..., 1] if dy else 1 - f[..., 1]) * (f[..., 2] if dz else 1 - f[..., 2])
                out += w * _hash(i[..., 0] + dx, i[..., 1] + dy, i[..., 2] + dz, saat)
    return out * 2 - 1


def fraktal(p, skala, oktaven=3, saat=1):
    s, a, ges = 0.0, 1.0, 0.0
    for o in range(oktaven):
        s = s + a * rauschen(p, skala / (2 ** o), saat + 17 * o)
        ges += a
        a *= 0.5
    return s / ges


def pose(form, clip):
    R, T = FE.pose_aus_bild(form, form['clips'][clip]['bilder'][0])
    return [r.astype(float) / 4096.0 for r in R], [t.astype(float) for t in T]


def teilkarte(teile, lage, kn):
    """Je Texel: Teilindex (Atlas-Rechteck und Knochen), U, V im Streifen."""
    tid = np.full(kn.shape, -1, np.int32)
    U = np.zeros(kn.shape, np.float32)
    V = np.zeros(kn.shape, np.float32)
    for i, (p, r) in enumerate(zip(teile, lage)):
        x, y, w, h = r
        sub = kn[y:y + h, x:x + w] == p.knochen
        t = tid[y:y + h, x:x + w]
        t[sub & (t < 0)] = i
        uu, vv = np.meshgrid((np.arange(w) + .5) / w, (np.arange(h) + .5) / h)
        U[y:y + h, x:x + w][sub] = uu[sub]
        V[y:y + h, x:x + w][sub] = vv[sub]
    return tid, U, V


def falten(form, kn, pos, nrm, Rr, Tr):
    """Gemalte Umgebungsverdeckung: Naehe der Texel-Lage (Ruhepose) zu Ecken anderer Knochen. -> Faktor 0..1 (1 = frei)."""
    ecken, eig = [], []
    for b, m in enumerate(form['meshes']):
        if m['ecken']:
            e = np.array(m['ecken'], float) @ Rr[b].T + Tr[b]
            ecken.append(e)
            eig += [b] * len(e)
    E = np.concatenate(ecken)
    eig = np.array(eig)
    baum = cKDTree(E)
    m = kn >= 0
    w = np.zeros(kn.shape + (3,), np.float32)
    nw = np.zeros(kn.shape + (3,), np.float32)
    for b in np.unique(kn[m]):
        s = kn == b
        w[s] = pos[s] @ Rr[b].T.astype(np.float32) + Tr[b].astype(np.float32)
        nw[s] = nrm[s] @ Rr[b].T.astype(np.float32)
    pts = w[m]
    d, j = baum.query(pts, k=24)
    fremd = eig[j] != kn[m][:, None]
    d = np.where(fremd, d, np.inf).min(1)
    ao = np.ones(kn.shape, np.float32)
    ao[m] = 1 - 0.55 * np.exp(-(d / 170.0) ** 2)
    return ao, w, nw


def bildstruktur(bk, n, kn, tid, teile, figur):
    """Bandpass der Bildhelligkeit an der Stelle jedes sichtbaren Texels (Verhaeltnis zum Umfeld). -> D (0 wo nicht sichtbar), Gewicht."""
    Y = P.original(n).astype(np.float32) @ P.YK
    if n == 2:
        Y = cv2.GaussianBlur(Y, (0, 0), 1.4)       # Zeilenstruktur der Abfilmung (Periode 5,2 px) nicht in die Haut uebernehmen
    else:
        Y = cv2.GaussianBlur(Y, (0, 0), 0.7)
    um = cv2.GaussianBlur(Y, (0, 0), 6.0)
    D = np.clip((Y + 10) / (um + 10) - 1, -0.55, 0.9)
    q = bk.q[n]
    x, y = np.nan_to_num(q[..., 0], nan=-10), np.nan_to_num(q[..., 1], nan=-10)
    inn = bk.vis[n] & (x >= 1) & (x < P.BREITE - 2) & (y >= 1) & (y < P.HOEHE - 2)
    out = np.zeros(kn.shape, np.float32)
    gew = np.zeros(kn.shape, np.float32)
    xi, yi = x[inn], y[inn]
    out[inn] = cv2.remap(D, xi.astype(np.float32).reshape(-1, 1), yi.astype(np.float32).reshape(-1, 1), cv2.INTER_LINEAR).reshape(-1)
    fm = cv2.remap(figur.astype(np.float32), xi.astype(np.float32).reshape(-1, 1), yi.astype(np.float32).reshape(-1, 1), cv2.INTER_LINEAR).reshape(-1)
    gew[inn] = np.clip(fm, 0, 1)
    for i, p in enumerate(teile):
        if p.name.startswith(OHNE_BILDSTRUKTUR):
            gew[tid == i] = 0
    return out, gew


def figurmaske(n):
    m = np.zeros((P.HOEHE, P.BREITE), np.uint8)
    import rf_kern
    ms, logo = rf_kern.masken()
    for t in rf_kern.TEILE[n]:
        m |= ms[t].astype(np.uint8)
    m = cv2.erode(m, np.ones((3, 3), np.uint8))
    return cv2.GaussianBlur(m.astype(np.float32), (0, 0), 1.5) * (~logo)


def backen(form, g, teile, lage, stand, marken=True, L=None, C=None, ausgabe=None):
    kn, pos, nrm = texeldaten(form)
    m = kn >= 0
    tid, U, V = teilkarte(teile, lage, kn)
    Rr, Tr = pose(form, 2)
    ao, w, nw = falten(form, kn, pos, nrm, Rr, Tr)
    oben = np.clip(-nw[..., 1], -1, 1)                # Ruhepose: Normale nach oben (Y unten)
    # 1. Grund + 2. gemalte Form: grosse weiche Flecken, Oberlicht, Falten
    n1 = fraktal(pos, 330.0, 3, 1)
    n3 = fraktal(pos, 120.0, 2, 5)
    t = np.clip(0.44 + 0.42 * oben + 0.20 * n1 + 0.07 * n3, 0, 1)
    t = t * (0.5 + 0.5 * ao)
    grund = np.where((t < .5)[..., None], HAUT_TIEF + (HAUT_SCHATTEN - HAUT_TIEF) * (t / .5)[..., None], HAUT_SCHATTEN + (HAUT_LICHT - HAUT_SCHATTEN) * ((t - .5) / .5)[..., None])
    # Glanz auf den hoechsten Woelbungen (gemalt, die Engine hat kein Glanzlicht)
    gl = np.clip((oben - 0.45) / 0.5, 0, 1) * np.clip(0.55 + 0.9 * n3, 0, 1) * ao
    grund = grund + (HAUT_GLANZ - grund) * (0.6 * gl)[..., None]
    # 3. Struktur: Querfalten (Runzeln) laengs der Knochenachse, Muskelfurchen laengs, Adern. Schwanz nur schwach (E4 glatt).
    achse_s = np.zeros(kn.shape, np.float32)
    fal_ab = np.zeros(kn.shape, np.float32)
    fal_st = np.zeros(kn.shape, np.float32)
    furche = np.zeros(kn.shape, np.float32)
    for i, p in enumerate(teile):
        s_ = tid == i
        if not s_.any():
            continue
        achse_s[s_] = (pos[s_] - p.o) @ p.d
        art_ = 'rumpf' if p.name in ('huefte', 'wirbel', 'brust') else 'schwanz' if p.name.startswith('schwanz') else 'hals' if p.name in ('hals',) else 'glied' if p.name.startswith(('ober', 'unter', 'fuss', 'hand')) else 'sonst'
        fal_ab[s_], fal_st[s_] = {'rumpf': (190.0, .45), 'schwanz': (170.0, .12), 'hals': (95.0, .55), 'glied': (125.0, .50), 'sonst': (120.0, 0.0)}[art_]
        if art_ == 'glied':
            for u0 in (0.2, 0.58):
                furche[s_] = np.maximum(furche[s_], np.exp(-((U[s_] - u0 + 0.05 * n1[s_]) / 0.035) ** 2))
    ph = achse_s / np.maximum(fal_ab, 1) + 0.55 * n3 + 0.3 * n1
    f_ = 0.5 + 0.5 * np.cos(2 * np.pi * ph)
    runzel = np.clip((f_ - 0.82) / 0.18, 0, 1) * np.clip(0.3 + fraktal(pos, 160.0, 2, 13), 0, 1) * fal_st
    grund = grund * (1 - 0.55 * runzel)[..., None]
    grund = grund * (1 - 0.35 * furche)[..., None]
    a1 = np.abs(fraktal(pos * np.array([1, 1.6, 1], np.float32), 210.0, 2, 9))
    ader = np.clip(1 - a1 / 0.05, 0, 1) * ADERN_STAERKE * np.clip(0.4 + 0.6 * (1 - ao), 0.3, 1)
    grund = grund + (ADER - grund) * ader[..., None]
    # Falten: Grund noch tiefer, wo andere Teile sehr nah sind
    grund = grund * (0.72 + 0.28 * ao)[..., None]
    tp = os.path.join(HIER, 'zw', 'hautton.json')
    if os.path.exists(tp):
        grund = grund * np.array(json.load(open(tp)), np.float32)[None, None]
        # m32: Texel, die spaeter 'grund' statt ihrer Zone bekommen (rechte Hand, die Bild 1 von hinten sieht), standen sonst grau-gruen (13,17,15) da.
        grund[..., 1] = np.minimum(grund[..., 1], grund[..., 0] / 1.3)
        grund[..., 2] = np.minimum(grund[..., 2], grund[..., 0])
    # 4. Bildstruktur aus Bild 1 und 2
    import rf_textur
    geschont = np.zeros(kn.shape, bool)
    for p, r in zip(teile, lage):
        if p.feld is not None:
            x, y, ww, hh = r
            geschont[y:y + hh, x:x + ww] = True
    bk = rf_textur.Bake(form, stand, kn, pos, geschont)
    D = np.zeros(kn.shape, np.float32)
    Wg = np.zeros(kn.shape, np.float32)
    for n, k_ in ((1, 1.0), (2, 0.8)):
        d, gw = bildstruktur(bk, n, kn, tid, teile, figurmaske(n))
        D += k_ * d * gw
        Wg += k_ * gw
    Dm = np.where(Wg > 0, D / np.maximum(Wg, 1e-6), 0) * np.clip(Wg, 0, 1)
    fak = np.clip(1 + BILD_GAIN * Dm, 0.45, 2.2)
    grund = grund * fak[..., None]
    # Lichtflecken: wo das Bild deutlich heller ist als sein Umfeld, ist im Video ein bläulich-weisser Fleck (neutral helles Grau, farbe.md 1). Gemalt in
    # der Farbe "feine helle Linien" der Palette, nach der Hautregel (sonst wuerde er rotbraun).
    fleck = np.clip((Dm - 0.28) / 0.45, 0, 1)
    # 5. Zonen (Haende, Krallen, Zaehne, Maul, Augen): rf_textur.zonen auf Merkfarbe, geaenderte Texel uebernehmen und mit derselben Form schattieren
    merk = np.full((TEX_H, TEX_W, 3), 7, np.uint8)
    z = RT.zonen(merk.copy(), teile, lage, L, C)
    spez = (z != 7).any(-1) & m
    # m17: Handruecken und Krallen in rf_textur.ROTBRAUN (96,48,50) lasen orange-rosa (Handschuhe). Palette Rotbraun ist (103,63,84): violetter. Ersetzt, etwas dunkler,
    # damit der Hautabgleich Raum hat.
    # m32: Auge aus rf_textur als rotes Quadrat (120,24,24) las im Spielblick als Fleck. Kopf und Gesicht sind frei: Auge kleiner (Kern), dunkler, Rand Haut.
    au = (z == np.array(RT.AUGE, np.uint8)).all(-1) & spez
    kern = cv2.erode(au.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0
    z[au & ~kern] = 7
    spez = spez & ~(au & ~kern)
    z[kern] = (92, 22, 20)
    rb = (z == np.array(RT.ROTBRAUN, np.uint8)).all(-1) & spez
    z = z.astype(np.float32)
    z[rb] = R_MIT * 0.75
    schatten = np.clip(0.62 + 0.30 * oben, 0.4, 1.0) * (0.6 + 0.4 * ao) * np.clip(1 + 0.5 * Dm, 0.7, 1.4)
    farbe = grund.copy()
    farbe[spez] = z[spez].astype(np.float32) * schatten[spez][:, None]
    # Krallen: zur Spitze heller (Horn), Wurzel dunkel
    for i, p in enumerate(teile):
        if p.name.startswith(('klaue', 'kralle', 'zeh')):
            s = tid == i
            hv = np.clip((V[s] - 0.45) / 0.5, 0, 1)
            farbe[s] = farbe[s] * (0.75 + 0.5 * hv)[:, None]
    # Bild 1 sieht den rechten Arm von hinten: dort Haut statt Rotbraun (rf_textur, Pruefstelle N2)
    for i, p in enumerate(teile):
        if p.name in ('hand_r', 'unterarm_r', 'kralle_r', 'klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4'):
            x, y, ww, hh = lage[i]
            sub = bk.vis[1][y:y + hh, x:x + ww] & (tid[y:y + hh, x:x + ww] == i)
            if p.name != 'unterarm_r':
                sub = sub & ~bk.vis[2][y:y + hh, x:x + ww]
            else:
                sub = cv2.dilate(sub.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0
            farbe[y:y + hh, x:x + ww][sub] = grund[y:y + hh, x:x + ww][sub]
    # Hautabgleich je Knochen (zw/haut.json, fb_stell.py): Faktor auf Haut und Zonen, Zeichnung kommt danach
    hp = os.path.join(HIER, 'zw', 'haut.json')
    hg = json.load(open(hp)) if os.path.exists(hp) else {}
    if hg:
        f_ = np.ones(kn.shape, np.float32)
        for b_, v_ in hg.items():
            f_[kn == int(b_)] = v_
        fz = np.where(spez, np.minimum(f_, 1.5), f_)      # m15: Hand und Unterarm vorn lasen als rosa Handschuhe (Rotbraun x 1,8); m17 mit 1,0 fiel Bild 2 (NCC 0,64), jetzt Palettenton und 1,5
        farbe = farbe * fz[..., None]
        farbe = np.minimum(farbe, R_BIS[None, None])      # nie heller als Rotbraun (Palette, oberflaeche_bis)
    # Palette (Auftrag 3): Haut bleibt rotbraun. m5 hatte mit dem Ton aus Bild 1 Grau-Blau (R < G), das ist keine Farbe der Palette.
    # Regel: R mindestens 1,3 x G, B hoechstens 1,0 x R, Kanaele in der Huelle Grundhaut von .. Rotbraun bis.
    haut = m & ~spez
    fr = farbe[haut]
    fr[:, 1] = np.minimum(fr[:, 1], fr[:, 0] / 1.3)
    fr[:, 2] = np.minimum(fr[:, 2], fr[:, 0] * 1.0)
    farbe[haut] = np.clip(fr, G_VON, R_BIS)
    FEIN = np.array(PAL['feine_linien']['oberflaeche_rgb'], float)
    # Freie Seiten (keins der Bilder zeigt sie, Spielansicht ROOM5080 Schnitt 1 sieht vor allem diese): dieselbe Sprache wie die Bildseite, kraeftiger gemalt,
    # damit die Figur im Spiel mehr als ein Umriss ist. Frei gesetzt: Dichte und Groesse der Flecken, Faltenstaerke, Glanz.
    frei = (~(bk.vis[1] | bk.vis[2])) & haut
    fw = cv2.GaussianBlur(frei.astype(np.float32), (0, 0), 1.2) * frei
    for i, p in enumerate(teile):
        if p.name.startswith('schwanz'):
            fw[tid == i] *= 0.35       # E4: Schwanz glatt, nur wenig Zeichnung
    nf = fraktal(pos, 85.0, 2, 21)
    fleck_frei = np.clip((nf - 0.42) / 0.18, 0, 1) * np.clip(oben + 0.3, 0, 1) * fw
    fleck = np.maximum(fleck, fleck_frei)
    farbe = farbe * (1 - 0.35 * runzel * fw)[..., None]
    glf = np.clip((oben - 0.2) / 0.6, 0, 1) * np.clip(0.4 + 0.9 * n3, 0, 1) * fw
    farbe = farbe + (HAUT_GLANZ[None, None] * np.clip(f_ if hg else 1.0, 0.9, 1.3)[..., None] - farbe) * (0.55 * glf)[..., None]
    fl_ = fleck * haut
    farbe = farbe + (FEIN[None, None] * np.clip(f_ if hg else 1.0, 0.8, 1.2)[..., None] - farbe) * (0.9 * fl_)[..., None]
    # weich: kleine Blockflecken der Rueckprojektion glaetten, nur innerhalb desselben Teils
    for i in np.unique(tid[tid >= 0]):
        s_ = (tid == i).astype(np.float32)
        x_, y_, ww_, hh_ = lage[i]
        sub = farbe[y_:y_ + hh_, x_:x_ + ww_] * s_[y_:y_ + hh_, x_:x_ + ww_, None]
        mm = cv2.GaussianBlur(s_[y_:y_ + hh_, x_:x_ + ww_], (0, 0), 0.65)
        bl = cv2.GaussianBlur(sub, (0, 0), 0.65) / np.maximum(mm, 1e-3)[..., None]
        q = s_[y_:y_ + hh_, x_:x_ + ww_] > 0
        farbe[y_:y_ + hh_, x_:x_ + ww_][q] = bl[q]
    # Gegenproben N1 (Bild 2) und N2 (Bild 1): Albedo dort unter Y 59
    for n, (x0, y0, x1, y1) in ((2, (288, 223, 342, 282)), (1, (382, 300, 413, 356))):
        zone = np.zeros((P.HOEHE, P.BREITE), np.uint8)
        zone[y0:y1, x0:x1] = 1
        s = bk.stelle(n, zone)
        Y = farbe @ P.YK.astype(np.float32)
        zu = s & (Y > Y_N)
        farbe[zu] *= (Y_N / Y[zu])[:, None]
    farbe[~m] = HAUT_SCHATTEN
    rgb = np.clip(farbe, 0, 255)
    if marken:
        rgb = zeichnung(rgb, bk)
    if ausgabe is not None:
        ausgabe.update(dict(kn=kn, tid=tid, ao=ao, Dm=Dm, oben=oben, bk=bk))
    return np.clip(np.round(rgb), 0, 255).astype(np.uint8), bk


def zeichnung(rgb, bk):
    """Marken weich: Bildmaske leicht weichgezeichnet, je Texel Deckkraft aus der Maske am Bildpunkt."""
    import rf_marken as RM
    p = os.path.join(HIER, 'zw', 'marken.json')
    k = json.load(open(p)) if os.path.exists(p) else {}
    rgb = rgb.astype(np.float32)
    # m8: G am Unterarm las in Bild 2 dunkel, weil jeder Texel ausfiel, den Bild 1 sieht. Pruefstelle N2 verlangt das nur fuer den Armzylinder unter Z5
    # (384-411, 302-354): nur dort (mit 4 px Rand) bleibt die Vorderseite dunkel.
    # m9: so lief G auf Texel anderer Knochen (linkes Bein, Bake-Sicht mit 140 Tiefe) und an den Ellbogen, den Bild 1 sieht. Darum: G nur auf Unterarm und
    # Hand rechts (Knochen 17, 18), und nicht, wo Bild 1 den rechten Arm zeigt (Nutzermaske right_arm, 4 px weiter).
    import rf_kern
    ms_, _ = rf_kern.masken()
    Yb1 = cv2.GaussianBlur(P.original(1).astype(np.float32) @ P.YK, (0, 0), 1.0)
    arm1 = np.zeros((P.HOEHE, P.BREITE), bool)      # m10: ganze Armmaske nahm G fast ganz weg (Y 64); m13: dunkle Armstellen (Y < 40) ebenso (Y 66). Nur N2 bleibt.
    arm1[296:360, 378:418] = True                                                                             # N2 mit Rand
    arm1t = bk.stelle(1, arm1.astype(np.uint8))
    g_ok = (np.isin(bk.kn, [17, 18]) | ~bk.vis[1]) & ~arm1t      # m14: nur 17/18 nahm sichtbare Nachbarflaechen unter dem Unterarm weg (Y 68)
    # m11/m12: Dieselben Texel des Unterarms zeigt Bild 2 als G und Bild 1 neben Z5 (366-424, 266-304). Rot dort war in Bild 1 ein Fleck, dunkel nahm G
    # in Bild 2 fast ganz weg (Y 62). Dort traegt G darum die Farbe der Baender (Z5): hell in beiden Bildern, Material passt zu beiden (untere_streifen ab Y 59).
    z5z = np.zeros((P.HOEHE, P.BREITE), np.uint8)
    z5z[266:304, 366:424] = 1
    z5t = bk.stelle(1, z5z)
    band = np.array(RMF('band'), float) * float(k.get('Z5', 1.0))
    for i, n, mk, f in liste():
        a = deckkraft(bk, n, mk)
        if i in ('Gg', 'Gr'):
            a = a * g_ok
            ab = a * z5t
            rgb = rgb + (band[None, None] - rgb) * ab[..., None]
            a = a * ~z5t
        elif i == 'P':
            a = a * (~bk.vis[1])
        ziel = np.array(f, float) * float(k.get(i, 1.0))
        rgb = rgb + (ziel[None, None] - rgb) * a[..., None]
    return rgb


def RMF(name):
    import rf_marken as RM
    return RM.FARBE[name]


def deckkraft(bk, n, maske):
    weich = cv2.GaussianBlur(maske.astype(np.float32), (0, 0), 0.8)
    weich = np.clip(weich * 2.2, 0, 1)      # m28: mit 1,6 fiel Z8 auf Material 0,60 (Rand mischte zu viel Haut ein)
    q = bk.q[n]
    x, y = np.nan_to_num(q[..., 0], nan=-10), np.nan_to_num(q[..., 1], nan=-10)
    inn = bk.vis[n] & (x >= 0) & (x < P.BREITE - 1) & (y >= 0) & (y < P.HOEHE - 1)
    a = np.zeros(bk.kn.shape, np.float32)
    a[inn] = cv2.remap(weich, x[inn].astype(np.float32).reshape(-1, 1), y[inn].astype(np.float32).reshape(-1, 1), cv2.INTER_LINEAR).reshape(-1)
    return np.clip(a, 0, 1)


def liste():
    """rf_marken.liste, Z5 als Dreieck (Auftrag 3, E1)."""
    import rf_marken as RM
    L = []
    for i, n, mk, f in RM.liste():
        if i == 'Z5':
            mk = RT.poly([Z5_DREIECK])
        L.append((i, n, mk, f))
    return L
