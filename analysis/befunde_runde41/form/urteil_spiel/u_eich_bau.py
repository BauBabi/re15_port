# -*- coding: utf-8 -*-
"""Pruefer Spielmodell: Knochenkarte der RE2-Eichmodelle. Je Knochen Eltern, Versatz, Flaechen, Ausdehnung im Knochenraum. Dazu Farbbild mit Nummern, damit Ellbogen, Knie, Schwanz belegt sind statt geraten."""
import colorsys
from PIL import Image, ImageDraw
from u_basis import *


def farbkarte(k, clip, bl, az, el, gross=620):
    mo, tex = modell(k)
    kf = kf_von(mo, clip, bl)
    form = lade(k)
    n = len(form['knochen'])
    R, T = mo.pose(kf)
    Rf = [r.astype(float) / 4096.0 for r in R]
    Tf = [t.astype(float) for t in T]
    W = welt(form, Rf, Tf)
    a, e = math.radians(az), math.radians(el)
    vor = np.array([-math.sin(a) * math.cos(e), math.sin(e), math.cos(a) * math.cos(e)])
    rechts = np.array([math.cos(a), 0.0, math.sin(a)])
    unten = np.cross(vor, rechts)
    if unten[1] < 0 and abs(el) < 80:
        unten = -unten
    A = np.stack([rechts, unten, vor])
    alle = np.vstack([w for w in W if len(w)])
    mitte = (alle.max(0) + alle.min(0)) / 2
    rad = float(np.linalg.norm(alle - mitte, axis=1).max()) or 1.0
    s = gross * 0.46 / rad
    fl = []
    for b in range(n):
        if len(W[b]) == 0:
            continue
        q = (W[b] - mitte) @ A.T
        F = dreiecke(form['meshes'][b])
        c = tuple(int(x * 255) for x in colorsys.hsv_to_rgb((b * 0.381966) % 1.0, 0.75, 0.95))
        for f in F:
            p = q[f]
            fl.append((p[:, 2].mean(), p[:, :2] * s + gross / 2, c))
    img = Image.new('RGB', (gross, gross), (40, 40, 44))
    d = ImageDraw.Draw(img)
    for z, p, c in sorted(fl, key=lambda t: -t[0]):
        d.polygon([tuple(x) for x in p], fill=c)
    for b in range(n):
        q = (Tf[b] - mitte) @ A.T
        xy = q[:2] * s + gross / 2
        d.text((xy[0], xy[1]), str(b), fill=(255, 255, 255))
    return np.array(img)


for k, clip, bl in (('K28', 0, 0), ('K34', 5, 40), ('K23', 0, 0), ('K22', 0, 0), ('K33', 0, 0)):
    form = lade(k)
    n = len(form['knochen'])
    print('==', k)
    for b in range(n):
        m = form['meshes'][b]
        V = np.array(m['ecken'], float).reshape(-1, 3)
        nf = len(m['dreiecke']) + len(m['vierecke'])
        bb = (V.max(0) - V.min(0)).astype(int) if len(V) else None
        lo = V.min(0).astype(int) if len(V) else None
        hi = V.max(0).astype(int) if len(V) else None
        print('  b%2d elt %2d vers %-20s fl %3d  min %s max %s' % (b, form['knochen'][b]['eltern'], form['knochen'][b].get('versatz'), nf, lo, hi))
    t = []
    for s_, az, el in (('links', 180, 6), ('vorn', 90, 8), ('3/4 hinten', -130, 24)):
        t.append(('%s %s' % (k, s_), farbkarte(k, clip, bl, az, el)))
    FB.montage(t, 3, AUS + '/eich_bau_%s.png' % k, '%s Knochenkarte, Nummern am Gelenkpunkt' % k)
