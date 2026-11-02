#!/usr/bin/env python3
"""Varianten, Pruefung je Zeichnung (E14): Nahvergleich und Farbton-Abgleich. python vm_stellen.py AUS_PREFIX "Name=form.json" ["Ref=form.json"]
Blatt AUS_PREFIX.png: je Stelle Screenshot | Name sauber | Name gefiltert | Ref sauber, 6-fach naechster Nachbar, Ausschnitt um die Bildmaske (rf_marken).
Mass AUS_PREFIX.json je Stelle (Bildmaske ohne Logo):
 - fremd: Anteil der Modellpixel (gefiltert, ohne Korn), deren Farbton im Screenshot an der Stelle nicht vorkommt. Farbton = Lab (a*, b*) in Feldern
   von 6; Pixel mit Buntheit unter 8 zaehlen als ein Feld "neutral". Vorkommen = mindestens 1 % der Screenshotpixel der Stelle, Nachbarfelder dazu.
 - rot: Anteil roetlicher Pixel (Lab a* > 15, Farbton -35..45 Grad) im Screenshot, im Modell sauber, im Modell gefiltert.
Kein RE-Wert."""
import json
import sys

import cv2
import numpy as np
from PIL import Image, ImageDraw
from skimage.color import rgb2lab

from rf_kern import TOOLS
sys.path.insert(0, TOOLS)
import form_pruefung as P      # noqa: E402
import form_video as FV        # noqa: E402
import emd_ansichtsblatt as EA  # noqa: E402
import fb_mal as FM            # noqa: E402

S = 6
RAND = 6
ROT = 40


def felder(rgb):
    lab = rgb2lab(rgb.astype(np.float32) / 255.0)
    C = np.hypot(lab[..., 1], lab[..., 2])
    fa = np.floor(lab[..., 1] / 6).astype(int) + 30
    fb = np.floor(lab[..., 2] / 6).astype(int) + 30
    f = fa * 64 + fb
    return np.where(C < 8, -1, f)


def fremd(model, shot, q):
    fs, fm = felder(shot)[q], felder(model)[q]
    if fs.size == 0:
        return None
    u, n = np.unique(fs, return_counts=True)
    da = set(int(x) for x, c in zip(u, n) if c >= 0.01 * fs.size)
    erlaubt = set()
    for f in da:
        if f < 0:
            erlaubt.add(-1)
            continue
        for dx in (-1, 0, 1):
            for dy in (-1, 0, 1):
                erlaubt.add(f + dx * 64 + dy)
    return float(np.mean([int(x) not in erlaubt for x in fm]))


def rot(a, q):
    """Anteil roetlicher Pixel: Lab a* > 15 und Farbtonwinkel zwischen -35 und 45 Grad (Rot bis Rotorange, auch dunkles Rot; R-G > 40 verfehlte
    die dunkelroten Flecken in Z5 von F)."""
    if not q.any():
        return 0.0
    lab = rgb2lab(a.astype(np.float32) / 255.0)
    h = np.degrees(np.arctan2(lab[..., 2], lab[..., 1]))
    r_ = (lab[..., 1] > 15) & (h > -35) & (h < 45)
    return float(r_[q].mean())


def main():
    pre = sys.argv[1]
    ks = [(z.split('=', 1)[0], P.Kandidat(z.split('=', 1)[1])) for z in sys.argv[2:]]
    logo = P.logo_maske()
    r = {}
    for nm, k in ks:
        r[nm] = {}
        for n in (1, 2):
            s, _a, _m, _ = k.rendern(n)
            r[nm][n] = (s, FV.video_filter(s, bild=n, entzerren=False, rauschen=False))
    shot = {n: P.original(n) for n in (1, 2)}
    zeilen, mass = [], {}
    name0 = ks[0][0]
    for i, n, mk, _f in FM.liste():
        q = (mk > 0) & ~logo
        if q.sum() < 4:
            continue
        ys, xs = np.nonzero(q)
        cx, cy = int(xs.mean()), int(ys.mean())
        hw = max(20, (xs.max() - xs.min()) // 2 + RAND, (ys.max() - ys.min()) // 2 + RAND)
        hw = min(hw, 45)
        x0, y0, x1, y1 = max(cx - hw, 0), max(cy - hw, 0), min(cx + hw, P.BREITE), min(cy + hw, P.HOEHE)
        e = {}
        for nm, _k in ks:
            s, g = r[nm][n]
            e[nm] = dict(fremd=round(fremd(g, shot[n], q), 3), rot_sauber=round(rot(s, q), 3), rot_gefiltert=round(rot(g, q), 3))
        e['rot_screenshot'] = round(rot(shot[n], q), 3)
        mass[i] = e
        gr = lambda a: cv2.resize(a[y0:y1, x0:x1], (2 * hw * S, 2 * hw * S), interpolation=cv2.INTER_NEAREST)   # noqa: E731
        kont = (cv2.dilate(q.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0) & ~q
        tafeln = [('Screenshot', shot[n]), ('%s sauber' % name0, r[name0][n][0]), ('%s gefiltert' % name0, r[name0][n][1])]
        if len(ks) > 1:
            tafeln.append(('%s sauber' % ks[1][0], r[ks[1][0]][n][0]))
        bilder = []
        for t, a in tafeln:
            b = gr(a).copy()
            km = gr(np.dstack([kont] * 3).astype(np.uint8) * 255)[..., 0] > 0
            b[km] = (255, 230, 60)
            bilder.append((t, cv2.resize(b, (240, 240), interpolation=cv2.INTER_NEAREST)))
        txt = '%s (Bild %d): fremd %s %.2f' % (i, n, name0, e[name0]['fremd'])
        if len(ks) > 1:
            txt += ', %s %.2f' % (ks[1][0], e[ks[1][0]]['fremd'])
        txt += ' | rot Shot %.2f, %s sauber %.2f gef %.2f' % (e['rot_screenshot'], name0, e[name0]['rot_sauber'], e[name0]['rot_gefiltert'])
        if len(ks) > 1:
            txt += ', %s sauber %.2f' % (ks[1][0], e[ks[1][0]]['rot_sauber'])
        zeilen.append((txt, bilder))
    json.dump(mass, open(pre + '.json', 'w', encoding='utf-8'), indent=1)
    # Blatt: zwei Stellen je Reihe
    bw = 240
    breite = 2 * (len(zeilen[0][1]) * (bw + 6) + 20) + 20
    hoehe = 50 + ((len(zeilen) + 1) // 2) * (bw + 50)
    b = Image.new('RGB', (breite, hoehe), (18, 19, 22))
    d = ImageDraw.Draw(b)
    d.text((10, 8), 'Je Zeichnung: Screenshot | %s sauber | %s gefiltert%s; gelb = Bildmaske der Stelle; fremd = Anteil gefilterter Pixel mit Farbton, den der Screenshot dort nicht hat'
           % (name0, name0, (' | %s sauber' % ks[1][0]) if len(ks) > 1 else ''), font=EA.font(15, True), fill=(240, 200, 90))
    for k, (t, bl) in enumerate(zeilen):
        x = 10 + (k % 2) * (len(bl) * (bw + 6) + 20)
        y = 40 + (k // 2) * (bw + 50)
        d.text((x, y), t, font=EA.font(12, True), fill=(214, 138, 74))
        for j, (tt, a) in enumerate(bl):
            b.paste(Image.fromarray(a), (x + j * (bw + 6), y + 22))
    b.save(pre + '.png')
    for i, e in mass.items():
        print('%-5s rot Shot %.2f | ' % (i, e['rot_screenshot']) + ' | '.join('%s fremd %.2f rot sauber %.2f gef %.2f' % (nm, e[nm]['fremd'], e[nm]['rot_sauber'], e[nm]['rot_gefiltert']) for nm, _k in ks))


if __name__ == '__main__':
    main()
