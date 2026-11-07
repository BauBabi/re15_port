#!/usr/bin/env python3
"""Variante H (E15): Farben aus G als weiche Farbkarte, darueber eine Stoff-Schicht auf Texelebene wie in den Spieltexturen. Kein RE-Wert.

 1. Farbkarte: G (G_treu/textur.png, gleiche UV-Lage wie C bis G) je Teil weich gemacht (Gauss STOFF_KARTE Texel nur ueber Texel desselben Teils):
    keine harten Bereichsgrenzen. Zeichnungen an ihrem Ort mit weichem Rand (Maske Gauss 1,0 als Deckkraft ueber der weichen Haut); E14 gilt weiter:
    die Karte mischt nur Farben, die schon in G an der Stelle sind (keine neuen Farbtoene; die Stoff-Schicht aendert nur die Helligkeit).
 2. Stoff-Schicht, nur Helligkeit, mit Mittel 1:
    H1 prozedural: in 4-facher Aufloesung erzeugt (Lage je Unter-Texel aus der Bindlage), Koernung (3 Oktaven), Poren, Knoten (Zellen),
       Falten laengs der Knochenachse, dann 4 x 4 gemittelt wie ein heruntergerechnetes Bild.
    H2 Material aus RE2-Birkin (CDEMD0.EMS 0x31, 0x33, 0x34): STIL-UEBERNAHME. Nur Haut- und Fleischtexel (keine roten Wunden, kein Schwarz),
       Hochpass der Helligkeit (Y / Gauss(Y, 3)), also nur die Feinstruktur, nicht deren Zeichnung oder Form; je Teil ein Ausschnitt, gespiegelt gekachelt,
       auf die Farbkarte gelegt (umgefaerbt, weil nur Helligkeit).
 3. Z5-Rand wie G (Pruefpunkte kappe_*), Gegenproben N1/N2 wie G. CLUT wie bisher (drei Zeilen, k-Mittel 255 je Zeile).
"""
import os

import cv2
import numpy as np

import vm_mal
import vm_ordnung as VO

YK = np.array([.299, .587, .114], np.float32)
STOFF_KARTE = 1.6           # Texel, Weichheit der Farbkarte
STAERKE = {'h1': 0.20, 'h2': 1.15}       # gemessen gegen die Spieltexturen (vm_gezeichnet): h1 0,24 zu unruhig (gleich 0,17), h2 0,9 zu glatt (hart 0,076)


def weich_je_teil(ml, rgb, sigma, maske=None):
    out = rgb.copy()
    for i, p in enumerate(ml.teile):
        x, y, w, h = ml.lage[i]
        sub = ml.tid[y:y + h, x:x + w] == i
        if maske is not None:
            sub = sub & maske[y:y + h, x:x + w]
        if not sub.any():
            continue
        mm = sub.astype(np.float32)
        R = rgb[y:y + h, x:x + w] * mm[..., None]
        bl = cv2.GaussianBlur(R, (0, 0), sigma) / np.maximum(cv2.GaussianBlur(mm, (0, 0), sigma), 1e-3)[..., None]
        out[y:y + h, x:x + w][sub] = bl[sub]
    return out


def farbkarte(ml, g):
    m = ml.m
    alle = np.zeros(m.shape, bool)
    for i, nm, grund, fak, s_, r in ml.marken:
        alle |= s_
    haut = weich_je_teil(ml, g, STOFF_KARTE, maske=m & ~alle)
    # Markentexel: in der Haut-Karte mit der Umgebung fuellen (sonst bleibt dort G), dann Zeichnung weich darueber
    haut_voll = weich_je_teil(ml, np.where(alle[..., None], haut, haut), STOFF_KARTE)
    zeich = weich_je_teil(ml, g, 0.7, maske=alle)
    a = cv2.GaussianBlur(alle.astype(np.float32), (0, 0), 1.0)
    a = np.clip(a * 1.6, 0, 1) * m
    out = haut_voll * (1 - a[..., None]) + np.where(alle[..., None], zeich, g) * a[..., None]
    return out, alle


def _hochaufloesend(ml, f=4):
    """Bindlage je Unter-Texel (f-fach), aus der Texelkarte bilinear, aussen mit dem naechsten Texel aufgefuellt."""
    m = ml.m
    pb = np.zeros(m.shape + (3,), np.float32)
    pb[m] = ml.pb.astype(np.float32)
    iy, ix = ml.naechst
    pb[~m] = pb[iy[~m], ix[~m]]
    H, W = m.shape
    big = cv2.resize(pb, (W * f, H * f), interpolation=cv2.INTER_LINEAR)
    dd = np.zeros(m.shape + (3,), np.float32)
    dd[m] = ml.dd.astype(np.float32)
    dd[~m] = dd[iy[~m], ix[~m]]
    dbig = cv2.resize(dd, (W * f, H * f), interpolation=cv2.INTER_NEAREST)
    return big, dbig


def stoff_h1(ml, f=4):
    P_, D_ = _hochaufloesend(ml, f)
    H, W = ml.m.shape
    p = P_.reshape(-1, 3).astype(np.float64)
    d = D_.reshape(-1, 3).astype(np.float64)
    korn = (0.40 * vm_mal.rauschen(p / 6.0, 81) + 0.32 * vm_mal.rauschen(p / 13.0, 83) + 0.28 * vm_mal.rauschen(p / 28.0, 85))
    poren = vm_mal.glatt_stufe(vm_mal.rauschen(p / 4.5, 87), 0.5, 0.8)
    la = (p * d).sum(1, keepdims=True)
    q = (p - la * d + la * d / 1.6) / 75.0
    f1, f2 = vm_mal.zellen(q, 89)
    knoten = np.clip(1 - f1 / 0.7, 0, 1) ** 1.5 - 0.6 * (1 - vm_mal.glatt_stufe(f2 - f1, 0.0, 0.12))
    quer = p - la * d
    falte = np.abs(vm_mal.rauschen(quer / 22.0 + d * (la / 160.0), 91))
    falten = 1 - vm_mal.glatt_stufe(falte, 0.0, 0.12)
    L = 0.55 * korn - 0.45 * poren + 0.35 * knoten - 0.40 * falten
    L = L.reshape(H * f, W * f).astype(np.float32)
    L = cv2.resize(L, (W, H), interpolation=cv2.INTER_AREA)     # wie heruntergerechnet
    return L


def re2_material():
    """Feinstruktur (Y / Gauss(Y, 3)) aus Haut und Fleisch der RE2-Birkin-Texturen, Liste von Ausschnitten 24 x 24 mit >= 96 % gueltig."""
    import vm_gezeichnet as VG
    stuecke = []
    for kind in (0x31, 0x33, 0x34):
        mo, _t = VG.re2(kind)
        rgb, b = VG.textur_von(mo)
        a = rgb.astype(np.float32)
        Y = a @ YK
        wunde = (a[..., 0] > 90) & (a[..., 0] > 1.6 * a[..., 1])
        ok = b & ~wunde & (Y > 15)
        mm = ok.astype(np.float32)
        bl = cv2.GaussianBlur(Y * mm, (0, 0), 3.0) / np.maximum(cv2.GaussianBlur(mm, (0, 0), 3.0), 1e-3)
        hp = np.where(ok, Y / np.maximum(bl, 4.0), 1.0)
        H, W = Y.shape
        for y in range(0, H - 24, 12):
            for x in range(0, W - 24, 12):
                if ok[y:y + 24, x:x + 24].mean() >= 0.96:
                    stuecke.append(np.log(np.clip(hp[y:y + 24, x:x + 24], 0.3, 3.0)))
    return stuecke


def stoff_h2(ml):
    st = re2_material()
    H, W = ml.m.shape
    L = np.zeros((H, W), np.float32)
    rng = np.random.RandomState(5)
    for i, p in enumerate(ml.teile):
        x, y, w, h = ml.lage[i]
        if w <= 0 or h <= 0:
            continue
        s = st[rng.randint(len(st))]
        reps_y, reps_x = h // 24 + 2, w // 24 + 2
        kach = np.block([[s if (a + b) % 2 == 0 else s[::-1, ::-1] for b in range(reps_x)] for a in range(reps_y)])
        oy, ox = rng.randint(24), rng.randint(24)
        L[y:y + h, x:x + w] = kach[oy:oy + h, ox:ox + w]
    return L


def backen(form, ml, art, g_textur):
    g = cv2.imread(g_textur)[:, :, ::-1].astype(np.float32)
    karte, alle = farbkarte(ml, g)
    m = ml.m
    L = stoff_h1(ml) if art == 'h1' else stoff_h2(ml)
    Lm = L[m]
    L = (L - float(np.median(Lm))) / max(float(Lm.std()), 1e-6)
    k = STAERKE[art] * (1.0 if art == 'h1' else 1.0)
    fak = np.exp(k * 0.75 * L) if art == 'h1' else np.exp(k * L * float(Lm.std()))
    fak = fak / float(fak[m].mean())
    # in Zeichnungen halb so stark (Farbe der Stelle bleibt erkennbar), gemaltes Licht leise dazu
    fak = np.where(alle, 1 + 0.5 * (fak - 1), fak)
    lk = np.ones(m.shape, np.float32)
    lk[m] = 1 + 0.12 * ml.licht_g
    out = karte * (fak * lk)[..., None]
    # Z5-Rand wie G (Pruefpunkte kappe_*): diese Texel aus G
    z5 = np.zeros(m.shape, bool)
    for i, nm, grund, fk, s_, r in ml.marken:
        if i == 'Z5' or i.endswith('_z5'):
            z5 |= s_
    if z5.any():
        qy = np.nan_to_num(ml.bk.q[1][..., 1], nan=999.0)
        qx = np.nan_to_num(ml.bk.q[1][..., 0], nan=-999.0)
        rand_ = z5 & (cv2.distanceTransform(z5.astype(np.uint8), cv2.DIST_L2, 3) < 1.5) & ((qy < 289) | (np.abs(qx - 398) <= 6))
        out[rand_] = g[rand_]
    for s_ in ml.n12:
        Y = out @ YK
        zu = s_ & (Y > vm_mal.Y_N)
        out[zu] *= (vm_mal.Y_N / Y[zu])[:, None]
    import vm_bild
    out = vm_bild.naht(ml, out)
    iy, ix = ml.naechst
    out[~m] = out[iy[~m], ix[~m]]
    return np.clip(np.round(out), 0, 255).astype(np.uint8), dict(art=art, staerke=k)
