#!/usr/bin/env python3
"""zensus_bild.py - Tuerblatt im Raumhintergrund finden, ausschneiden, entzerren (R31 T1).

Nur LESEND gegenueber allen Originaldaten; schreibt nach build/r31_tueren/t1/.

Verfahren je Tuerseite (Dossier §4):
  1. Tuerkante: die Kante des Tuer-Rechtecks (Door_aot_set pc+6..13), die vom Ankunftsort
     der Gegenseite (Ziel x/z der Rueckrichtung, liegt in DIESEM Raum) am weitesten weg
     liegt; lange Kanten zuerst. Ohne Gegenseite: die Kante, hinter der kein RVD-Bereich
     des Raums liegt (dort ist kein begehbarer Raum mehr).
  2. Blatt: senkrechtes Rechteck BLATT_B x BLATT_H (06_massstab: 1953 / 3549), Unterkante auf
     y = -Band*1800, mittig auf der Kante.
  3. Je Cut projizieren (Kamera = tor_kamera.Kamera exakt, H = fov>>7, Bildmitte 160/120);
     Cut zaehlt, wenn alle Ecken vor der Kamera liegen, die Vorderseite zur Kamera zeigt,
     mindestens 60 % der Blattflaeche im Bild liegt und das Blatt mindestens 24 Pixel hoch ist.
  4. Verfeinerung am Bild: Verschiebung quer (dn) und laengs (dt) zur Kante sowie Breite W so,
     dass die Kanten des projizierten Blatts auf Helligkeitsspruengen liegen (Mittel des
     Gradientenbetrags quer zu linker, rechter, oberer Kante; untere Kante halb gewichtet).
  5. Ausschnitt (Blatt-Huelle + Rand, 3-fach NEAREST) und entzerrtes Blatt (Homographie auf
     128 x 218 = RE2-Blattbereich v 0..218, bilinear).
"""
import math
import os
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import zensus_lib as L    # noqa: E402

BILD_W, BILD_H = 320, 240
ENTZ_W, ENTZ_H = 128, 218
MIN_PX_H = 24.0            # Messwerkzeug-Schwelle: darunter ist ein Blatt nicht vergleichbar
MIN_IM_BILD = 0.60


# ----------------------------------------------------------------------------
# Geometrie
# ----------------------------------------------------------------------------
def kanten(pts):
    """[(a, b, laenge)] der vier Kanten des Vierecks (Weltpunkte x,z)."""
    aus = []
    for i in range(4):
        a = np.array(pts[i], float)
        b = np.array(pts[(i + 1) % 4], float)
        aus.append((a, b, float(np.linalg.norm(b - a))))
    return aus


def aussen_normale(pts, a, b):
    """Einheitsnormale der Kante a-b, die vom Viereck-Mittelpunkt WEG zeigt."""
    m = np.mean(np.array(pts, float), axis=0)
    t = (b - a) / max(1e-9, np.linalg.norm(b - a))
    n = np.array([-t[1], t[0]])
    if n.dot((a + b) / 2 - m) < 0:
        n = -n
    return n, t


def kandidatenkanten(pts):
    """Lange Kanten (>= 0,8 x laengste) als dict(a, b, n aussen, t)."""
    ks = kanten(pts)
    lmax = max(k[2] for k in ks)
    aus = []
    for (a, b, l) in ks:
        if l >= 0.8 * lmax:
            n, t = aussen_normale(pts, a, b)
            aus.append(dict(a=a, b=b, n=n, t=t, grund=""))
    return aus


def tuerkante(pts, ankunft, bereiche):
    """-> dict(a, b, n (aussen = in die Wand), t, grund).
    ankunft = [(x,z)] Ankunftsorte der Gegenseite in diesem Raum; bereiche = {cut: Viereck}."""
    ks = kanten(pts)
    lmax = max(k[2] for k in ks)
    lang = [k for k in ks if k[2] >= 0.8 * lmax]
    best = None
    if ankunft:
        for (a, b, l) in lang:
            n, t = aussen_normale(pts, a, b)
            mid = (a + b) / 2
            wert = min(float(n.dot(mid - np.array(p, float))) for p in ankunft)
            if best is None or wert > best[0]:
                best = (wert, a, b, n, t, "gegen-ankunft %.0f" % wert)
    else:
        for (a, b, l) in lang:
            n, t = aussen_normale(pts, a, b)
            mid = (a + b) / 2
            probe = mid + 900.0 * n
            drin = sum(1 for q in bereiche.values() if L.im_viereck(q, probe[0], probe[1]))
            d = min([L.abstand_viereck(q, probe[0], probe[1]) for q in bereiche.values()] or [0.0])
            wert = (-drin, d)
            if best is None or wert > best[0]:
                best = (wert, a, b, n, t, "rvd-aussen drin=%d abstand=%.0f" % (drin, d))
    _, a, b, n, t, grund = best
    return dict(a=a, b=b, n=n, t=t, grund=grund)


def blatt_welt(kante, band, dn=0.0, dt=0.0, W=L.BLATT_B, H=L.BLATT_H):
    """Vier Weltecken (x,y,z) des Blatts: [L_oben, R_oben, R_unten, L_unten] entlang t."""
    a, b, n, t = kante["a"], kante["b"], kante["n"], kante["t"]
    mid = (a + b) / 2 + dn * n + dt * t
    yb = -float(band & 0x7F) * L.BAND
    p0 = mid - 0.5 * W * t
    p1 = mid + 0.5 * W * t
    return np.array([[p0[0], yb - H, p0[1]], [p1[0], yb - H, p1[1]],
                     [p1[0], yb, p1[1]], [p0[0], yb, p0[1]]])


def projiziere(cam, welt):
    sx, sy, vz = cam.bild(np.asarray(welt, float))
    return np.stack([sx, sy], -1), vz


def bild_ordnung(q):
    """Ecken so ordnen, dass [TL, TR, BR, BL] im BILD gilt (oben = kleines y)."""
    q = np.asarray(q, float)
    oben = q[:2]
    unten = q[2:][::-1]           # welt: [L_oben, R_oben, R_unten, L_unten]
    if oben[0][0] > oben[1][0]:   # t zeigt im Bild nach links -> spiegeln
        oben = oben[::-1]
        unten = unten[::-1]
    return np.array([oben[0], oben[1], unten[1], unten[0]])


def flaeche(poly):
    p = np.asarray(poly, float)
    if len(p) < 3:
        return 0.0
    x, y = p[:, 0], p[:, 1]
    return 0.5 * abs(float(np.dot(x, np.roll(y, -1)) - np.dot(y, np.roll(x, -1))))


def clip_rechteck(poly, x0=0, y0=0, x1=BILD_W, y1=BILD_H):
    """Sutherland-Hodgman gegen das Bildrechteck."""
    def schnitt(p, q, achse, wert):
        t = (wert - p[achse]) / (q[achse] - p[achse])
        return p + t * (q - p)
    out = [np.asarray(p, float) for p in poly]
    for achse, wert, innen in ((0, x0, lambda p: p[0] >= x0), (0, x1, lambda p: p[0] <= x1),
                               (1, y0, lambda p: p[1] >= y0), (1, y1, lambda p: p[1] <= y1)):
        inp, out = out, []
        if not inp:
            break
        s = inp[-1]
        for e in inp:
            if innen(e):
                if not innen(s):
                    out.append(schnitt(s, e, achse, wert))
                out.append(e)
            elif innen(s):
                out.append(schnitt(s, e, achse, wert))
            s = e
    return out


def sichtbarkeit(cam, welt, kante):
    """-> dict(ok, im_bild, px_h, px_w, vorn, quad) fuer einen Cut."""
    q, vz = projiziere(cam, welt)
    mitte = welt.mean(axis=0)
    zur_kamera = np.array(cam.ort, float) - mitte
    innen = -np.array([kante["n"][0], 0.0, kante["n"][1]])      # ins Rauminnere
    vorn = float(innen.dot(zur_kamera)) > 0
    if (vz <= 300).any():
        return dict(ok=False, grund="hinter der Kamera", vorn=vorn)
    qb = bild_ordnung(q)
    A = flaeche(qb)
    im = flaeche(clip_rechteck(qb)) / A if A > 0 else 0.0
    px_h = 0.5 * (np.linalg.norm(qb[3] - qb[0]) + np.linalg.norm(qb[2] - qb[1]))
    px_w = 0.5 * (np.linalg.norm(qb[1] - qb[0]) + np.linalg.norm(qb[2] - qb[3]))
    # aufrecht: beide Seitenkanten weniger als 45 Grad gegen die Bildsenkrechte, oben ueber unten
    # (sonst sieht die Kamera steil von oben auf das Blatt - kein vergleichbares Bild)
    aufrecht = True
    for (o, u) in ((qb[0], qb[3]), (qb[1], qb[2])):
        d = u - o
        if d[1] <= 0 or d[1] < 0.7071 * np.linalg.norm(d):
            aufrecht = False
    ok = vorn and aufrecht and im >= MIN_IM_BILD and px_h >= MIN_PX_H
    grund = "ok" if ok else ("Rueckseite" if not vorn else ("nicht aufrecht" if not aufrecht else (
        "zu wenig im Bild" if im < MIN_IM_BILD else "zu klein")))
    return dict(ok=ok, grund=grund, im_bild=im, px_h=float(px_h), px_w=float(px_w), vorn=vorn,
                quad=qb.tolist(), vz=float(vz.mean()))


# ----------------------------------------------------------------------------
# Bild: Helligkeit, Gradient, Verfeinerung
# ----------------------------------------------------------------------------
def helligkeit(rgb):
    rgb = rgb.astype(float)
    return rgb[..., 0] * 0.299 + rgb[..., 1] * 0.587 + rgb[..., 2] * 0.114


def abtasten(img, x, y):
    """Bilinear, kontinuierliche Koordinaten (Pixel i deckt [i, i+1)). img 2D oder 3D."""
    h, w = img.shape[:2]
    fx = np.clip(np.asarray(x, float) - 0.5, 0, w - 1.001)
    fy = np.clip(np.asarray(y, float) - 0.5, 0, h - 1.001)
    x0 = np.floor(fx).astype(int)
    y0 = np.floor(fy).astype(int)
    ax, ay = fx - x0, fy - y0
    if img.ndim == 3:
        ax = ax[..., None]
        ay = ay[..., None]
    return (img[y0, x0] * (1 - ax) * (1 - ay) + img[y0, x0 + 1] * ax * (1 - ay) +
            img[y0 + 1, x0] * (1 - ax) * ay + img[y0 + 1, x0 + 1] * ax * ay)


def kanten_wert(lum, quads, n_ab=16):
    """quads (N,4,2) im Bild [TL,TR,BR,BL] -> (N,) Kantenwert: Mittel ueber links/rechts/oben
    (+ halb unten) des Medians von |L(p+1n) - L(p-1n)| an n_ab Punkten der mittleren 70 %."""
    s = np.linspace(0.15, 0.85, n_ab)
    werte = []
    gew = []
    for (i, j, g) in ((3, 0, 1.0), (1, 2, 1.0), (0, 1, 1.0), (2, 3, 0.5)):   # links, rechts, oben, unten
        a = quads[:, i, :]
        b = quads[:, j, :]
        d = b - a
        Ln = np.linalg.norm(d, axis=1, keepdims=True) + 1e-9
        t = d / Ln
        n = np.stack([-t[:, 1], t[:, 0]], 1)
        p = a[:, None, :] + s[None, :, None] * d[:, None, :]
        pa = p + n[:, None, :]
        pb = p - n[:, None, :]
        v = np.abs(abtasten(lum, pa[..., 0], pa[..., 1]) - abtasten(lum, pb[..., 0], pb[..., 1]))
        werte.append(np.median(v, axis=1) * g)
        gew.append(g)
    return np.sum(werte, axis=0) / sum(gew)


def verfeinern(cam, lum, kante, band, dn_ber=(-900, 900, 50), dt_ber=(-600, 600, 50),
               W_ber=None, strafe=1.0, W0=L.BLATT_B):
    """Gitter-Suche ueber (dn, dt, W). Gewertet wird Kantenwert - strafe * Median * (groesste
    Eckverschiebung im Bild gegen die Datenlage / 10 px). Die Strafe verhindert, dass die Suche
    auf eine innere Fuellungslinie oder den Zargen-Aussenrand springt; strafe = 1 ist an den 11
    vermessenen Tueren gewaehlt (Dossier §4.2, Tabelle der Varianten) - Messwerkzeug, kein
    Spielwert. -> dict mit bestem Satz, Kantenwert und Wert der Datenlage."""
    dns = np.arange(dn_ber[0], dn_ber[1] + 1, dn_ber[2], dtype=float)
    dts = np.arange(dt_ber[0], dt_ber[1] + 1, dt_ber[2], dtype=float)
    if W_ber is None:
        W_ber = (W0 - 300, W0 + 300, 100)
    Ws = np.arange(W_ber[0], W_ber[1] + 1, W_ber[2], dtype=float)
    G = np.array(np.meshgrid(dns, dts, Ws, indexing="ij")).reshape(3, -1).T
    a, b, n, t = kante["a"], kante["b"], kante["n"], kante["t"]
    mid = (a + b) / 2
    yb = -float(band & 0x7F) * L.BAND
    c = mid[None, :] + G[:, 0:1] * n[None, :] + G[:, 1:2] * t[None, :]
    p0 = c - 0.5 * G[:, 2:3] * t[None, :]
    p1 = c + 0.5 * G[:, 2:3] * t[None, :]
    N = len(G)
    welt = np.zeros((N, 4, 3))
    welt[:, 0] = np.stack([p0[:, 0], np.full(N, yb - L.BLATT_H), p0[:, 1]], 1)
    welt[:, 1] = np.stack([p1[:, 0], np.full(N, yb - L.BLATT_H), p1[:, 1]], 1)
    welt[:, 2] = np.stack([p1[:, 0], np.full(N, yb), p1[:, 1]], 1)
    welt[:, 3] = np.stack([p0[:, 0], np.full(N, yb), p0[:, 1]], 1)
    sx, sy, vz = cam.bild(welt.reshape(-1, 3))
    q = np.stack([sx, sy], -1).reshape(N, 4, 2)
    vz = vz.reshape(N, 4)
    # Bildordnung: t zeigt im Bild nach rechts oder links - fuer alle Kandidaten gleich
    if q[0, 0, 0] > q[0, 1, 0]:
        q = q[:, [1, 0, 3, 2], :]
    ok = (vz > 300).all(axis=1)
    w = np.where(ok, kanten_wert(lum, q), -1.0)
    i0 = int(np.argmin(np.abs(G[:, 0]) + np.abs(G[:, 1]) + np.abs(G[:, 2] - W0)))
    med = float(np.median(w[ok])) if ok.any() else 0.0
    dpx = np.abs(q - q[i0]).max(axis=(1, 2))
    wert = np.where(ok, w - strafe * med * dpx / 10.0, -1e9)
    i = int(np.argmax(wert))
    return dict(dn=float(G[i, 0]), dt=float(G[i, 1]), W=float(G[i, 2]), wert=float(w[i]),
                wert_start=float(w[i0]), median=med, verschiebung_px=float(dpx[i]),
                quad=q[i].tolist(), quad_start=q[i0].tolist())


# ----------------------------------------------------------------------------
# Ausschnitt und Entzerrung
# ----------------------------------------------------------------------------
def homographie(src, dst):
    """3x3 H mit dst ~ H * src (4 Punktpaare)."""
    A = []
    for (x, y), (u, v) in zip(src, dst):
        A.append([x, y, 1, 0, 0, 0, -u * x, -u * y, -u])
        A.append([0, 0, 0, x, y, 1, -v * x, -v * y, -v])
    _, _, Vt = np.linalg.svd(np.array(A, float))
    Hm = Vt[-1].reshape(3, 3)
    return Hm / Hm[2, 2]


def entzerren(rgb, quad, w=ENTZ_W, h=ENTZ_H):
    """quad [TL,TR,BR,BL] im Bild -> w x h RGB (uint8), bilinear."""
    ziel = [(0, 0), (w, 0), (w, h), (0, h)]
    Hm = homographie(ziel, quad)           # Zielpixel -> Bildpunkt
    ys, xs = np.mgrid[0:h, 0:w]
    P = np.stack([xs.ravel() + 0.5, ys.ravel() + 0.5, np.ones(w * h)], 0)
    Q = Hm.dot(P)
    qx, qy = Q[0] / Q[2], Q[1] / Q[2]
    out = abtasten(rgb.astype(float), qx, qy).reshape(h, w, 3)
    innen = (qx >= 0) & (qx <= rgb.shape[1]) & (qy >= 0) & (qy <= rgb.shape[0])
    out = out * innen.reshape(h, w, 1)
    return np.clip(out, 0, 255).astype(np.uint8), float(innen.mean())


def ausschnitt(rgb, quad, f=3, rand=0.35, min_rand=10):
    """Huelle des Blatts + Rand, f-fach NEAREST; Blattumriss rot. -> PIL.Image, (x0,y0,x1,y1)."""
    from PIL import Image, ImageDraw
    q = np.asarray(quad, float)
    x0, y0 = q.min(axis=0)
    x1, y1 = q.max(axis=0)
    rx = max(min_rand, rand * (x1 - x0))
    ry = max(min_rand, rand * 0.5 * (y1 - y0))
    X0 = int(max(0, math.floor(x0 - rx)))
    Y0 = int(max(0, math.floor(y0 - ry)))
    X1 = int(min(rgb.shape[1], math.ceil(x1 + rx)))
    Y1 = int(min(rgb.shape[0], math.ceil(y1 + ry)))
    if X1 - X0 < 4 or Y1 - Y0 < 4:
        return None, None
    im = Image.fromarray(rgb[Y0:Y1, X0:X1]).resize(((X1 - X0) * f, (Y1 - Y0) * f), Image.NEAREST)
    return im, (X0, Y0, X1, Y1)


def umriss(im, quad, box, f, farbe=(255, 40, 40)):
    from PIL import ImageDraw
    dr = ImageDraw.Draw(im)
    X0, Y0 = box[0], box[1]
    P = [((x - X0) * f, (y - Y0) * f) for (x, y) in quad]
    for i in range(4):
        dr.line([P[i], P[(i + 1) % 4]], fill=farbe, width=1)
    return im
