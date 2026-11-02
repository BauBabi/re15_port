#!/usr/bin/env python3
"""Varianten, Uebersicht und Farbmessung. python vm_matrix.py --aus DIR "Beschriftung=pfad/form.json" ...
Je Bild ein Blatt uebersicht_bild{1,2}.png: Zeilen = Varianten, Spalten = Screenshot | Modell sauber | Modell durch form_video.video_filter |
Schwanz vergroessert (Bild 1: Pose 1 wie im Bild, aufgehellt Gamma 0,5; Bild 2: Ruhepose seitlich, Blattlicht) | Schwanz Ruhepose von oben hinten.
Ausschnitt und Kamera wie pose_bild1/2 (form_pruefung.ausschnitt_box), 2-fach, naechster Nachbar.
Farbabstand (matrix.json): in der Figurmaske (ziel2_bild*.png weiss, ohne Logo), Render durch video_filter ohne Korn gegen das Standbild:
  dE_mittel = CIE76 zwischen den Mittelfarben (Lab), dE_feld = mittleres CIE76 je Pixel nach Gauss 3 px auf beiden, dazu Mittel RGB und Lab.
Kein RE-Wert."""
import argparse
import copy
import json
import os
import sys

import cv2
import numpy as np
from PIL import Image, ImageDraw
from skimage.color import rgb2lab

from rf_kern import TOOLS
sys.path.insert(0, TOOLS)
import form_pruefung as P      # noqa: E402
import form_video as FV        # noqa: E402
import form_buehne as FB       # noqa: E402
import emd_ansichtsblatt as EA  # noqa: E402

S = 2
SCHWANZ_BOX1 = (205, 182, 337, 482)      # Bild 1: Nutzerteil tail (teile_bild1.png), Rand 4 px
SCHWANZ_S = 3


def figurmaske(n):
    z = P.ziel_laden(n)[1]
    return (z == 255) & ~P.logo_maske()


def farbmass(gef, orig, maske):
    a = rgb2lab(gef.astype(np.float32) / 255.0)
    b = rgb2lab(orig.astype(np.float32) / 255.0)
    ma, mb = a[maske].mean(0), b[maske].mean(0)
    ga = cv2.GaussianBlur(a, (0, 0), 3.0)
    gb = cv2.GaussianBlur(b, (0, 0), 3.0)
    feld = np.linalg.norm(ga - gb, axis=2)[maske].mean()
    return dict(dE_mittel=round(float(np.linalg.norm(ma - mb)), 1), dE_feld=round(float(feld), 1),
                rgb_render=[round(float(x), 1) for x in gef[maske].mean(0)], rgb_bild=[round(float(x), 1) for x in orig[maske].mean(0)],
                lab_render=[round(float(x), 1) for x in ma], lab_bild=[round(float(x), 1) for x in mb])


def hell(a, m=None):
    h = np.clip(np.power(a / 255.0, 0.5) * 255, 0, 255).astype(np.uint8)
    if m is not None:
        out = a.copy()
        out[m] = h[m]
        return out
    return h


def nur_knochen(mo, knochen):
    """Flache Kopie des Modells, Meshes ausserhalb `knochen` leer."""
    q = copy.copy(mo)
    q.meshes = []
    for b, m in enumerate(mo.meshes):
        if b in knochen:
            q.meshes.append(m)
        else:
            e = copy.copy(m)
            e.tv, e.tn, e.tf, e.tuv, e.qv, e.qn, e.qf, e.quv = [], [], [], [], [], [], [], []
            q.meshes.append(e)
    return q


def schwanz_ruhe(k, az, el, gross):
    r = k.j.get('ruhe', {})
    kf = k.kf(int(r.get('clip', 2)), int(r.get('bild', 0)))
    mo = nur_knochen(k.mo, set(k.j['teile']['tail']))
    a = np.full((gross, gross, 3), 92, np.uint8)
    FB.rastern(P.rundum_dreiecke(mo, kf, az, el, gross), k.tex, a, dict(ax=1.0, bx=0.0, ay=1.0, by=0.0))
    return a


def zelle(a, s, interp=cv2.INTER_NEAREST):
    return cv2.resize(a, None, fx=s, fy=s, interpolation=interp)


def blatt(zeilen, kopf, spalten_namen, ziel):
    """zeilen: [(Beschriftung, [Bild...])], alle Bilder einer Spalte gleich gross."""
    pad, links, oben = 10, 300, 56
    breiten = [max(z[1][i].shape[1] for z in zeilen) for i in range(len(zeilen[0][1]))]
    hoehe = max(max(b.shape[0] for b in z[1]) for z in zeilen)
    W = links + sum(breiten) + pad * (len(breiten) + 1)
    H = oben + len(zeilen) * (hoehe + pad) + pad
    b = Image.new('RGB', (W, H), (18, 19, 22))
    d = ImageDraw.Draw(b)
    d.text((pad, 8), kopf, font=EA.font(18, True), fill=(220, 220, 225))
    x = links + pad
    for i, nm in enumerate(spalten_namen):
        d.text((x, 32), nm, font=EA.font(16, True), fill=(214, 138, 74))
        x += breiten[i] + pad
    for r, (text, bilder) in enumerate(zeilen):
        y = oben + r * (hoehe + pad)
        yy = y + 4
        for j, t in enumerate(text.split('\n')):
            d.text((pad, yy), t, font=EA.font(17 if j == 0 else 14, j == 0), fill=(240, 200, 90) if j == 0 else (190, 192, 198))
            yy += 24 if j == 0 else 19
        x = links + pad
        for i, a in enumerate(bilder):
            b.paste(Image.fromarray(a), (x, y))
            x += breiten[i] + pad
    b.save(ziel)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--aus', required=True)
    ap.add_argument('zeilen', nargs='+')
    a = ap.parse_args()
    os.makedirs(a.aus, exist_ok=True)
    R = {}
    reihen = {1: [], 2: []}
    for z in a.zeilen:
        text, pfad = z.split('=', 1)
        text = text.replace('|', '\n')
        k = P.Kandidat(pfad)
        name = text.split('\n')[0]
        R[name] = {}
        for n in (1, 2):
            sauber, alb, m, _ = k.rendern(n)
            gef = FV.video_filter(sauber, bild=n, entzerren=False)
            gef_mess = FV.video_filter(sauber, bild=n, entzerren=False, rauschen=False)
            orig = P.original(n)
            fm = farbmass(gef_mess, orig, figurmaske(n))
            fs_ = farbmass(sauber, orig, figurmaske(n))
            fm['sauber'] = dict(dE_mittel=fs_['dE_mittel'], dE_feld=fs_['dE_feld'], rgb_render=fs_['rgb_render'])
            R[name]['bild%d' % n] = fm
            box = P.ausschnitt_box(n)
            x0, y0, x1, y1 = box
            bilder = [zelle(orig[y0:y1, x0:x1], S), zelle(sauber[y0:y1, x0:x1], S), zelle(gef[y0:y1, x0:x1], S)]
            if n == 1:
                sx0, sy0, sx1, sy1 = SCHWANZ_BOX1
                bilder.append(cv2.resize(hell(sauber, m)[sy0:sy1, sx0:sx1], None, fx=SCHWANZ_S * 0.64, fy=SCHWANZ_S * 0.64, interpolation=cv2.INTER_NEAREST))
                bilder.append(schwanz_ruhe(k, 0, 10, 560))
            else:
                bilder.append(schwanz_ruhe(k, 180, 30, 560))
            t_ = '%s|gefiltert: dE Mittel %.1f, Feld %.1f|  RGB %s|sauber: dE Mittel %.1f, Feld %.1f|  RGB %s|Screenshot RGB %s' % (
                text, fm['dE_mittel'], fm['dE_feld'], tuple(int(round(v)) for v in fm['rgb_render']), fs_['dE_mittel'], fs_['dE_feld'],
                tuple(int(round(v)) for v in fs_['rgb_render']), tuple(int(round(v)) for v in fm['rgb_bild']))
            reihen[n].append((t_.replace('|', '\n'), bilder))
    json.dump(R, open(os.path.join(a.aus, 'matrix.json'), 'w', encoding='utf-8'), indent=1)
    blatt(reihen[1], 'Bild 1 (ROOM5080, Pose 1): Zeilen = Varianten. Farbabstand dE (CIE76) in der Figurmaske gegen das Standbild: gefiltert (ohne Korn) und sauber',
          ['Screenshot 1', 'Modell sauber', 'Modell durch video_filter', 'Schwanz wie Bild 1, aufgehellt', 'Schwanz Ruhepose seitlich'],
          os.path.join(a.aus, 'uebersicht_bild1.png'))
    blatt(reihen[2], 'Bild 2 (ROOM5090 Schnitt 11, Pose 2): Zeilen = Varianten. Farbabstand dE (CIE76) in der Figurmaske gegen das Standbild: gefiltert (ohne Korn) und sauber',
          ['Screenshot 2', 'Modell sauber', 'Modell durch video_filter', 'Schwanz Ruhepose von oben hinten'],
          os.path.join(a.aus, 'uebersicht_bild2.png'))
    for nm, v in R.items():
        print('%-28s gefiltert Bild1 dE %5.1f / %5.1f  Bild2 %5.1f / %5.1f | sauber Bild1 %5.1f / %5.1f  Bild2 %5.1f / %5.1f' % (
            nm, v['bild1']['dE_mittel'], v['bild1']['dE_feld'], v['bild2']['dE_mittel'], v['bild2']['dE_feld'],
            v['bild1']['sauber']['dE_mittel'], v['bild1']['sauber']['dE_feld'], v['bild2']['sauber']['dE_mittel'], v['bild2']['sauber']['dE_feld']))


if __name__ == '__main__':
    main()
